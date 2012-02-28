#include "vehicle.h"
#include "map.h"
#include "output.h"
#include "game.h"
#include "item.h"
#include <sstream>
#include <stdlib.h>
#if (defined _WIN32 || defined WINDOWS)
    #include "catacurse.h"
#else
    #include <curses.h>
#endif

vehicle::vehicle()
{
    type = veh_null;
    posx = 0;
    posy = 0;
    velocity = 0;
    turn_dir = 0;
    moves = 0;
    fuel = 0;
    cruise_velocity = 0;
    smoking_turns = 0;
    malfunction = false;
    driven = false;
    skidding = false;
    cruise_on = true;
}

vehicle::vehicle (vhtype_id type_id, int px, int py, int d, int vel):
    posx (px), posy (py), velocity (vel)
{
    face.init (d);
    turn_dir = 0;
    moves = 0;
    fuel = 0;
    cruise_velocity = 0;
    smoking_turns = 0;
    malfunction = false;
    driven = false;
    skidding = false;
    cruise_on = true;
    make (type_id);
}

vehicle::~vehicle()
{
}

void vehicle::load (std::ifstream &stin, game *g)
{
    int t;
    int fdir, mdir, fl, mf, drv, skd, cargo_parts, cr_on;
    stin >>
        t >>
        posx >>
        posy >>
        fdir >>
        mdir >>
        turn_dir >>
        velocity >>
        cruise_velocity >>
        fl >>
        drv >>
        mf >>
        skd >>
        smoking_turns >>
        moves >>
        cr_on >>
        cargo_parts;
    make ((vhtype_id) t);
    face.init (fdir);
    move.init (mdir);
    fuel = fl;
    malfunction = mf != 0;
    driven = drv != 0;
    skidding = skd != 0;
    cruise_on = cr_on != 0;
    std::string databuff;
    getline(stin, databuff); // Clear EoL
//    debugmsg ("vehicle load cargo parts=%d", cargo_parts);
    for (int i = 0; i < cargo_parts; i++)
    {
        int p, num_it;
        stin >> p >> num_it;  // Number of part and number of items inside it
//        debugmsg ("[%d] part=%d num_it=%d", i, p, num_it);
        getline(stin, databuff); // Clear EoL
        for (int j = 0; j < num_it; j++)
        {
            getline(stin, databuff);
            item itm;
            itm.load_info (databuff, g);
            if (p >= 0 && p < parts.size())
                parts[p].items.push_back (itm);
            int ncont;
            stin >> ncont; // how many items inside container
            getline(stin, databuff); // Clear EoL
//            debugmsg ("[%d] p[%d] it[%d] num_cont=%d", i, p, j, ncont);
            for (int k = 0; k < ncont; k++)
            {
                getline(stin, databuff);
                item citm;
                citm.load_info (databuff, g);
                if (p >= 0 && p < parts.size())
                    parts[p].items[parts[p].items.size()-1].put_in (citm);
            }
        }
    }
}

void vehicle::save (std::ofstream &stout)
{
    std::vector<int> cargo_parts;
    for (int p = 0; p < parts.size(); p++)
        if (parts[p].flags & VHP_CARGO)
            cargo_parts.push_back (p);
    stout <<
        int(type) << " " <<
        posx << " " <<
        posy << " " <<
        face.dir() << " " <<
        move.dir() << " " <<
        turn_dir << " " <<
        velocity << " " <<
        cruise_velocity << " " <<
        fuel << " " <<
        (driven? 1 : 0) << " " <<
        (malfunction? 1 : 0) << " " <<
        (skidding? 1 : 0) << " " <<
        smoking_turns << " " <<
        moves << " " <<
        (cruise_on? 1 : 0) << " " <<
        cargo_parts.size() << std::endl;

    for (int i = 0; i < cargo_parts.size(); i++)
    {
        int p = cargo_parts[i];
        stout << p << " " << parts[p].items.size() << std::endl; // number of part and how many items in each cargo part
        for (int i = 0; i < parts[p].items.size(); i++)
        {
            stout << parts[p].items[i].save_info() << std::endl;     // item info
            stout << parts[p].items[i].contents.size() << std::endl; // how many items inside this item
            for (int l = 0; l < parts[p].items[i].contents.size(); l++)
                stout << parts[p].items[i].contents[l].save_info() << std::endl; // contents info
        }
    }
//    debugmsg ("vehicle saved. parts=%d", cargo_parts.size());
}

std::string vehicle::fuel_name()
{
    switch (fuel_type)
    {
    case AT_GAS:
        return std::string("gasoline");
    case AT_BATT:
        return std::string("batteries");
    case AT_PLUT:
        return std::string("plutonium cells");
    case AT_PLASMA:
        return std::string("hydrogen");
    default:
        return std::string("INVALID FUEL (BUG)");
    }
}

std::string vehicle::part_desc (int part)
{
    std::string res;
    if (part < 0 || part >= parts.size())
        return res;

    if (parts[part].flags & VHP_ENGINE)
    {
        if (malfunction)
            res += "Engine (broken). ";
        else
            res += "Engine. ";
    }
    if (parts[part].flags & VHP_FUEL_TANK)
    {
        res += "Fuel tank. ";
    }
    if (parts[part].flags & VHP_CARGO)
    {
        res += "Cargo. ";
    }
    if (parts[part].flags & VHP_SHARP)
    {
        res += "Sharp. ";
    }
    return res;
}


void vehicle::coord_translate (int reldx, int reldy, int &dx, int &dy)
{
    tileray tdir (face.dir());
    tdir.advance (reldx);
    dx = tdir.dx() + tdir.ortho_dx(reldy);
    dy = tdir.dy() + tdir.ortho_dy(reldy);
}

void vehicle::coord_translate (int dir, int reldx, int reldy, int &dx, int &dy)
{
    tileray tdir (dir);
    tdir.advance (reldx);
    dx = tdir.dx() + tdir.ortho_dx(reldy);
    dy = tdir.dy() + tdir.ortho_dy(reldy);
}

int vehicle::part_at(int dx, int dy)
{
    for (int i = 0; i < parts.size(); i++)
    {
        int pdx, pdy;
        coord_translate (parts[i].mount_dx, parts[i].mount_dy, pdx, pdy);
        if (pdx == dx && pdy == dy)
            return i;
    }
    return -1;
}

player *vehicle::get_driver(game *g)
{
    // TODO: NPC drivers
    if (driven)
        return &g->u;
    else
        return 0;
}

void vehicle::global_pos (int smx, int smy, int &x, int &y)
{
    x = smx * SEEX + posx;
    y = smy * SEEY + posy;
}

void vehicle::thrust (game *g, int thd, int x, int y)
{
    if (velocity == 0)
    {
        turn_dir = face.dir();
        move = face;
        moves = 0;
        last_turn = 0;
        skidding = false;
    }

    if (!thd)
        return;

    if (malfunction)
    {
        if (get_driver(g) == &g->u)
            g->add_msg ("The %s is broken.", name.c_str());
        cruise_on = false;
        stop();
        return;
    }

    int sgn = velocity < 0? -1 : (velocity > 0? 1 : 0);
    bool thrust = sgn == thd;
    if (fuel < 1)
    {
        if (thrust)
        {
            g->add_msg ("The %s's engine emits sneezing sound.", name.c_str());
            cruise_velocity = 0;
        }
        return;
    }

    if (thrust)
    {
        fuel--;
        // add sound and smoke
        if (fuel_type == AT_GAS)
            for (int i = 0; i < parts.size(); i++)
            {
                if (!(parts[i].flags & VHP_EXHAUST))
                    continue;
                int fd = (int) (parts[i].flags & VHP_DIR_MASK);
                int sdx = parts[i].mount_dx + (fd == VHP_BACK? -1 : (fd == VHP_FORWARD? 1 : 0));
                int sdy = parts[i].mount_dy + (fd == VHP_LEFT? -1 : (fd == VHP_RIGHT? 1 : 0));
                int rdx, rdy;
                coord_translate (sdx, sdy, rdx, rdy);
                g->m.add_field(g, x + rdx, y + rdy, fd_smoke, 1);
            }

        if (noise > 0)
            g->sound(x, y, noise, "");
    }

    if (skidding)
        return;

    int brk = abs(velocity) * brake / 100;
    if (brk < acceleration)
        brk = acceleration;
    if (brk < 10 * 100)
        brk = 10 * 100;
    int vel_inc = (thrust? acceleration : brk) * thd;
    if (abs(velocity) <= max_velocity / 5)
        vel_inc /= 2;
    if ((velocity > 0 && velocity + vel_inc < 0) ||
        (velocity < 0 && velocity + vel_inc > 0))
        stop ();
    else
    {
        velocity += vel_inc;
        if (velocity > max_velocity)
            velocity = max_velocity;
        else
        if (velocity < -max_rear_velocity)
            velocity = -max_rear_velocity;
    }
}

void vehicle::cruise_thrust (int amount)
{
    if (!amount)
        return;
    cruise_velocity += amount;
    cruise_velocity = cruise_velocity / abs(amount) * abs(amount);
    if (cruise_velocity > max_velocity)
        cruise_velocity = max_velocity;
    else
    if (-cruise_velocity > max_rear_velocity)
        cruise_velocity = -max_rear_velocity;
}

void vehicle::turn (game *g, int deg)
{
    if (deg == 0)
        return;
    if (velocity < 0)
        deg = -deg;
    last_turn = deg;
    turn_dir += deg;
    if (turn_dir < 0)
        turn_dir += 360;
    if (turn_dir >= 360)
        turn_dir -= 360;
}

void vehicle::stop ()
{
    velocity = 0;
    skidding = false;
    move = face;
    last_turn = 0;
    moves = 0;
}

void vehicle::wreck ()
{
    smoking_turns += 200;
    malfunction = true;
}

void vehicle::explode (game *g, int vx, int vy)
{
    malfunction = true;
    fuel = 0;
    int p;
    for (p = 0; p < parts.size(); p++)
        if (parts[p].flags & VHP_FUEL_TANK)
            break;
    if (p >= parts.size())
        p = 0;
    int rdx, rdy;
    coord_translate (parts[p].mount_dx, parts[p].mount_dy, rdx, rdy);
    g->explosion (vx + rdx, vy + rdy, 20, 10, fuel_type == AT_GAS);
    if (fuel_type == AT_GAS)
        for (int x = -2; x <= 2; x++)
            for (int y = -2; y <= 2; y++)
            {
                if (!one_in(3))
                    continue;
                g->m.add_item(vx + x + rdx, vy + y + rdy, g->itypes[itm_gasoline], 0);
            }
}

bool vehicle::handle_collision (game *g, int vx, int vy, int part, int x, int y)
{
    player *p = get_driver (g);
    int mondex = g->mon_at(x, y);
    int npcind = g->npc_at(x, y);
    bool u_here = x == g->u.posx && y == g->u.posy && !g->u.drive_mode;
    monster *z = mondex >= 0? &g->z[mondex] : 0;
    player *ph = (npcind >= 0? &g->active_npc[npcind] :
                  (u_here? &g->u : 0));
    vehicle &oveh = g->m.veh_at (x, y);
    bool veh_collision = oveh.type != veh_null && (oveh.posx != posx || oveh.posy != posy);
    bool body_collision = (g->u.posx == x && g->u.posy == y && !g->u.drive_mode) ||
                           mondex >= 0 || npcind >= 0;
    bool is_u = p == &g->u;
    int dummy;
    bool can_see = is_u || g->u_see(x, y, dummy);

    // 0 - nothing, 1 - monster/player/npc, 2 - vehicle,
    // 3 - thin_obstacle, 4 - bashable, 5 - destructible, 6 - other
    int collision_type = 0;
    std::string obs_name = g->m.tername(x, y).c_str();

    // let's calculate type of collision & mass of object we hit
    int mass2;
    if (veh_collision)
    { // first, check if we collide with another vehicle (there shouldn't be impassable terrain below)
        collision_type = 2; // vehicle
        mass2 = oveh.mass;
        body_collision = false;
        obs_name = oveh.name.c_str();
    }
    else
    if (body_collision)
    { // then, check any monster/NPC/player on the way
        collision_type = 1; // body
        if (z)
            switch (z->type->size)
            {
            case MS_TINY:    // Rodent
                mass2 = 15;
                break;
            case MS_SMALL:   // Half human
                mass2 = 40;
                break;
            default:
            case MS_MEDIUM:  // Human
                mass2 = 80;
                break;
            case MS_LARGE:   // Cow
                mass2 = 200;
                break;
            case MS_HUGE:     // TAAAANK
                mass2 = 800;
                break;
            }
        else
            mass2 = 80;// player or NPC
    }
    else // if all above fails, go for terrain which might obstruct moving
    if (g->m.has_flag (thin_obstacle, x, y))
    {
        collision_type = 3; // some fence
        mass2 = 20;
    }
    else
    if (g->m.has_flag(bashable, x, y))
    {
        collision_type = 4; // bashable (door, window)
        mass2 = 50;    // special case: instead of calculating absorb based on mass of obstacle later, we let
                       // map::bash function deside, how much absorb is
    }
    else
    if (g->m.is_destructable(x, y))
    {
        collision_type = 5; // destructible (wall)
        mass2 = 200;
    }
    else
    if (g->m.move_cost_ter_only(x, y) == 0 && !g->m.has_flag(swimmable, x, y))
    {
        collision_type = 6; // not destructible
        mass2 = 1000;
    }
    if (!collision_type)  // hit nothing
        return true;

    if (is_u && !body_collision)
        g->add_msg ("Your %s's %s rams into %s.", name.c_str(), parts[part].name.c_str(), obs_name.c_str());

    int degree = rng (70, 100);
    int imp = abs(velocity) * mass / 100 / 100;
    int imp2 = imp * mass2 / (mass + mass2) * degree / 100;
    bool smashed = true;
    if (collision_type == 4) // something bashable -- use map::bash to determine outcome
    {
        int absorb = -1;
        std::string snd;
        g->m.bash(x, y, imp * parts[part].dmg_mod / 100, snd, &absorb);
        if (snd.length() > 0)
            g->add_msg ("You hear %s", snd.c_str());
        if (absorb != -1)
            imp2 = absorb;
        smashed = imp * parts[part].dmg_mod / 100 > absorb;
    }
    else
    if (collision_type >= 3) // some other terrain
    {
        smashed = imp * rng (80, 120) / 100 > mass2;
        if (smashed)
            switch (collision_type) // destroy obstacle
            {
            case 3:
                g->m.ter (x, y) = t_dirt;
                break;
            case 5:
                g->m.ter(x, y) = t_rubble;
                break;
            case 6:
                smashed = false;
                break;
            default:;
            }
    }
    int imp1 = imp - imp2;
    int vel1 = imp1 * 100 * 100 / mass;
    int vel2 = imp2 * 100 * 100 / mass2;

//     g->add_msg ("Col t=%s i=%d i1=%d i2=%d v=%d v1=%d v2=%d m1=%d m2=%d",
//                 obs_name.c_str(), imp, imp1, imp2, abs(velocity), vel1, vel2, mass, mass2);
// 
    if (collision_type == 1)
    {
        int dam = imp * parts[part].dmg_mod / 100;
//        g->add_msg("dam=%d imp=%d dm=%d", dam, imp, parts[part].dmg_mod);
        if (z)
        {
            int z_armor = parts[part].flags & VHP_SHARP? z->type->armor_cut : z->type->armor_bash;
            if (z_armor < 0)
                z_armor = 0;
            if (z)
                dam -= z_armor;
        }
        if (dam < 0)
            dam = 0;

        int turns_stunned = rng (0, dam) > 10? rng (1, 2) + (dam > 40? rng (1, 2) : 0) : 0;
        if (parts[part].flags & VHP_SHARP)
            turns_stunned = 0;
        if (turns_stunned > 6)
            turns_stunned = 6;
        if (turns_stunned > 0 && z)
            z->add_effect(ME_STUNNED, turns_stunned);

        std::string dname;
        if (z)
            dname = z->name().c_str();
        else
            dname = ph->name;
        if (is_u)
            g->add_msg ("Your %s's %s rams into %s, inflicting %d damage%s!",
                    name.c_str(), parts[part].name.c_str(), dname.c_str(), dam,
                    turns_stunned > 0 && z? " and stunning it" : "");

        int angle = (100 - degree) * 2 * (one_in(2)? 1 : -1);
        if (z)
        {
            z->hurt(dam);
            if (imp2 > rng (10, 40))
                g->fling_player_or_monster (0, z, move.dir() + angle, imp2);
            if (z->hp < 1)
                g->kill_mon (mondex);
        }
        else
        {
            ph->hitall (g, dam, 40);
            if (imp2 > rng (10, 40))
                g->fling_player_or_monster (ph, 0, move.dir() + angle, imp2);
        }

        if (parts[part].flags & VHP_SHARP)
        {
            if (g->m.field_at(x, y).type == fd_blood &&
                g->m.field_at(x, y).density < 2)
                g->m.field_at(x, y).density++;
            else
                g->m.add_field(g, x, y, fd_blood, 1);
        }
    }

    bool throw_driver = false;
    if (p)
    {
        int throw_roll = rng (imp2 / 2, imp2 * 2);
        throw_driver = throw_roll > (p->str_cur + seatbelt) * 3;
//         debugmsg ("Hurl tr=%d str=%d sb=%d", throw_roll, p->str_cur, seatbelt);

        if (throw_driver)
        {
            if (is_u)
                g->add_msg ("You was hurled from %s's seat by the power of impact!", name.c_str());
            g->m.unboard_vehicle (g, vx, vy);
            // displace driver
            g->fling_player_or_monster (p, 0, face.dir() + rng(0, 120) - 60, (imp2 - seatbelt < 10? 10 : imp2 - seatbelt));
        }
    }

    if (!smashed)
    {
        if (collision_type >= 3)
        {
            if (rng (0, imp2 * 100 / mass) > 20)
            {
                wreck();
                if (one_in (5))
                    explode(g, vx, vy);
            }
            cruise_on = false;
            stop();
        }
        else
        if (collision_type == 2)
        {
            if (rng (0, imp1 * 100 / mass) > 20)
            {
                oveh.wreck();
                oveh.cruise_on = false;
                if (one_in (5))
                    oveh.explode(g, x, y);
            }
        }
    }
    else
    {
        if (vel1 < 500)
            stop();
        else
        {
            if (velocity < 0)
                velocity = -vel1;
            else
                velocity = vel1;

            int turn_roll = rng (0, 100);
            int turn_amount = rng (1, 3) * sqrt (imp2);
            turn_amount /= 15;
            if (turn_amount < 1)
                turn_amount = 1;
            turn_amount *= 15;
            if (turn_amount > 120)
                turn_amount = 120;
            bool turn_veh = turn_roll < (abs(velocity) - vel1) / 100;
            if (p && !throw_driver)
            {
                int lose_ctrl_roll = rng (0, imp2);
                if (turn_veh && lose_ctrl_roll > p->dex_cur * 2 + p->sklevel[sk_driving] * 3)
                {
                    if (is_u)
                        g->add_msg ("You lose control of %s.", name.c_str());
                }
                else
                    turn_veh = false;
            }
            if (turn_veh)
            {
                skidding = true;
                turn (g, one_in (2)? turn_amount : -turn_amount);
            }

        }
    }
    return velocity != 0;
}

void vehicle::handle_trap (game *g, int x, int y, int part)
{
    if (!(parts[part].flags & VHP_GROUND))
        return;
    trap_id t = g->m.tr_at(x, y);
    if (t == tr_null)
        return;
    int noise = 0;
    int chance = 100;
    int expl = 0;
    int shrap = 0;
    bool wreckit = false;
    std::string msg ("The %s's %s runs over %s.");
    std::string snd;
    switch (t)
    {
        case tr_bubblewrap:
            noise = 18;
            snd = "Pop!";
            break;
        case tr_beartrap:
        case tr_beartrap_buried:
            noise = 8;
            snd = "SNAP!";
            wreckit = true;
            g->m.tr_at(x, y) = tr_null;
            g->m.add_item(x, y, g->itypes[itm_beartrap], 0);
            break;
        case tr_nailboard:
            wreckit = true;
            break;
        case tr_blade:
            noise = 1;
            snd = "Swinnng!";
            wreckit = true;
            break;
        case tr_crossbow:
            chance = 30;
            noise = 1;
            snd = "Clank!";
            wreckit = true;
            g->m.tr_at(x, y) = tr_null;
            g->m.add_item(x, y, g->itypes[itm_crossbow], 0);
            g->m.add_item(x, y, g->itypes[itm_string_6], 0);
            if (!one_in(10))
                g->m.add_item(x, y, g->itypes[itm_bolt_steel], 0);
            break;
        case tr_shotgun_2:
        case tr_shotgun_1:
            noise = 60;
            snd = "Bang!";
            chance = 70;
            wreckit = true;
            if (t == tr_shotgun_2)
                g->m.tr_at(x, y) = tr_shotgun_1;
            else
            {
                g->m.tr_at(x, y) = tr_null;
                g->m.add_item(x, y, g->itypes[itm_shotgun_sawn], 0);
                g->m.add_item(x, y, g->itypes[itm_string_6], 0);
            }
            break;
        case tr_landmine:
            expl = 10;
            shrap = 8;
            break;
        case tr_boobytrap:
            expl = 18;
            shrap = 12;
            break;
        case tr_dissector:
            noise = 10;
            snd = "BRZZZAP!";
            wreckit = true;
            break;
        case tr_sinkhole:
        case tr_pit:
        case tr_spike_pit:
        case tr_ledge:
            wreckit = true;
            break;
        case tr_goo:
        case tr_portal:
        case tr_telepad:
        case tr_temple_flood:
        case tr_temple_toggle:
            msg.clear();
        default:;
    }
    int dummy;
    if (msg.size() > 0 && g->u_see(x, y, dummy))
        g->add_msg (msg.c_str(), name.c_str(), parts[part].name.c_str(), g->traps[t]->name.c_str());
    if (noise > 0)
        g->sound (x, y, noise, snd);
    if (wreckit && chance >= rng (1, 100))
    {
        wreck ();
        cruise_on = false;
        stop ();
    }
    if (expl > 0)
        g->explosion(x, y, expl, shrap, false);
}

bool vehicle::add_item (int part, item itm)
{
    if (!(parts[part].flags & VHP_CARGO) || parts[part].items.size() >= 26)
        return false;
    parts[part].items.push_back (itm);
    return true;
}

void vehicle::remove_item (int part, int itemdex)
{
    if (itemdex < 0 || itemdex >= parts[part].items.size())
        return;
    parts[part].items.erase (parts[part].items.begin() + itemdex);
}


