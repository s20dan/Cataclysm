#include "vehicle.h"
#include "game.h"
#include <stdlib.h>

void vehicle::make (vhtype_id type_id)
{
    parts.clear();
    type = type_id;
    vehicle_part tmp_part;

    // mount coords:
    // x + 1 -- in front of driver
    // x - 1 -- behind driver
    // y - 1 -- left of driver
    // y + 1 -- right of driver
    // assign symbols as they will look when vehicle face north!

    switch (type)
    {
    case veh_motorcycle:
        name = "motorcycle";
        max_velocity = 80 * 100;  // max mph
        max_rear_velocity = 15 * 100;  // max mph
        acceleration = 5 * 100;   // how much more mph per thrust
        brake = 20;         // hom much less percent of speed per brake
        noise = 70;
        fuel_type = AT_GAS; // AT_GAS, AT_BATT, AT_PLUT, AT_PLASMA
        max_fuel = 2000;
        mass = 150;         // kg
        seatbelt = 0;       // bonus to str when calculating driver throw due to collision

        tmp_part.name = std::string("seat");
        tmp_part.sym = '#';
        tmp_part.color = c_red;
        tmp_part.mount_dx = 0;
        tmp_part.mount_dy = 0;
        tmp_part.dmg_mod = 70;
        tmp_part.flags = VHP_SEAT | VHP_ENGINE;
        parts.push_back (tmp_part);

        tmp_part.name = std::string("forward wheel");
        tmp_part.sym = '0';
        tmp_part.color = c_dkgray;
        tmp_part.mount_dx = 2;
        tmp_part.mount_dy = 0;
        tmp_part.dmg_mod = 50;
        tmp_part.flags = VHP_GROUND;
        parts.push_back (tmp_part);

        tmp_part.name = std::string("handle");
        tmp_part.sym = '^';
        tmp_part.color = c_ltcyan;
        tmp_part.mount_dx = 1;
        tmp_part.mount_dy = 0;
        tmp_part.dmg_mod = 40;
        tmp_part.flags = VHP_FUEL_TANK;
        parts.push_back (tmp_part);

        tmp_part.name = std::string("back wheel");
        tmp_part.sym = 'o';
        tmp_part.color = c_dkgray;
        tmp_part.mount_dx = -1;
        tmp_part.mount_dy = 0;
        tmp_part.dmg_mod = 50;
        tmp_part.flags = VHP_GROUND | VHP_CARGO | VHP_EXHAUST | VHP_BACK;
        parts.push_back (tmp_part);

        break;
    case veh_sandbike:
        name = "sand bike";
        max_velocity = 60 * 100;  // max mph
        max_rear_velocity = 30 * 100;  // max mph
        acceleration = 10 * 100;   // how much more mph per thrust
        brake = 30;         // hom much less percent of speed per brake
        noise = 40;
        fuel_type = AT_BATT; // AT_GAS, AT_BATT, AT_PLUT, AT_PLASMA
        max_fuel = 2000;
        mass = 250;
        seatbelt = 5;

        tmp_part.name = std::string("seat");
        tmp_part.sym = '#';
        tmp_part.color = c_blue;
        tmp_part.mount_dx = 0;
        tmp_part.mount_dy = 0;
        tmp_part.dmg_mod = 70;
        tmp_part.flags = VHP_SEAT;
        parts.push_back (tmp_part);

        tmp_part.name = std::string("forward wheel");
        tmp_part.sym = '0';
        tmp_part.color = c_dkgray;
        tmp_part.mount_dx = 1;
        tmp_part.mount_dy = -1;
        tmp_part.dmg_mod = 50;
        tmp_part.flags = VHP_GROUND;
        parts.push_back (tmp_part);
        tmp_part.mount_dy = 1;
        parts.push_back (tmp_part);

        tmp_part.name = std::string("engine");
        tmp_part.sym = '^';
        tmp_part.color = c_brown;
        tmp_part.mount_dx = 1;
        tmp_part.mount_dy = 0;
        tmp_part.dmg_mod = 70;
        tmp_part.flags = VHP_FUEL_TANK | VHP_ENGINE;
        parts.push_back (tmp_part);

        tmp_part.name = std::string("back wheel");
        tmp_part.sym = 'o';
        tmp_part.color = c_dkgray;
        tmp_part.mount_dx = -1;
        tmp_part.mount_dy = -1;
        tmp_part.dmg_mod = 50;
        tmp_part.flags = VHP_GROUND;
        parts.push_back (tmp_part);
        tmp_part.mount_dy = 1;
        parts.push_back (tmp_part);

        tmp_part.name = std::string("trunk");
        tmp_part.sym = 'H';
        tmp_part.color = c_brown;
        tmp_part.mount_dx = -1;
        tmp_part.mount_dy = 0;
        tmp_part.dmg_mod = 40;
        tmp_part.flags = VHP_CARGO | VHP_EXHAUST | VHP_BACK;
        parts.push_back (tmp_part);

        break;
    case veh_nukecar:
        name = "nuclear car";
        max_velocity = 120 * 100;  // max mph
        max_rear_velocity = 120 * 100;  // max mph
        acceleration = 24 * 100;   // how much more mph per thrust
        brake = 50;            // hom much less percent of speed per brake
        noise = 10;
        fuel_type = AT_PLUT; // AT_GAS, AT_BATT, AT_PLUT, AT_PLASMA
        max_fuel = 5000;
        mass = 1000;
        seatbelt = 30;

        // Seat
        tmp_part.name = std::string("seat");
        tmp_part.sym = '#';
        tmp_part.color = c_brown;
        tmp_part.mount_dx = 0;
        tmp_part.mount_dy = 0;
        tmp_part.dmg_mod = 70;
        tmp_part.flags = VHP_SEAT;
        parts.push_back (tmp_part);

        tmp_part.name = std::string("wheel blade");
        tmp_part.sym = '-';
        tmp_part.color = c_white;
        tmp_part.mount_dx = 1;
        tmp_part.mount_dy = -2;
        tmp_part.dmg_mod = 400;
        tmp_part.flags = VHP_SHARP;
        parts.push_back (tmp_part);
        tmp_part.mount_dy = 2;
        parts.push_back (tmp_part);
        tmp_part.mount_dx = -2;
        parts.push_back (tmp_part);
        tmp_part.mount_dy = -2;
        parts.push_back (tmp_part);

        tmp_part.name = std::string("spike");
        tmp_part.sym = '.';
        tmp_part.color = c_white;
        tmp_part.mount_dx = 2;
        tmp_part.mount_dy = -1;
        tmp_part.dmg_mod = 300;
        tmp_part.flags = VHP_SHARP;
        parts.push_back (tmp_part);
        tmp_part.mount_dy = 1;
        parts.push_back (tmp_part);
        tmp_part.mount_dy = 0;
        parts.push_back (tmp_part);

        tmp_part.name = std::string("forward wheel");
        tmp_part.sym = '0';
        tmp_part.color = c_dkgray;
        tmp_part.mount_dx = 1;
        tmp_part.mount_dy = -1;
        tmp_part.dmg_mod = 50;
        tmp_part.flags = VHP_GROUND;
        parts.push_back (tmp_part);
        tmp_part.mount_dy = 1;
        parts.push_back (tmp_part);

        tmp_part.name = std::string("back wheel");
        tmp_part.sym = '0';
        tmp_part.color = c_dkgray;
        tmp_part.mount_dx = -2;
        tmp_part.mount_dy = -1;
        tmp_part.dmg_mod = 50;
        tmp_part.flags = VHP_GROUND;
        parts.push_back (tmp_part);
        tmp_part.mount_dy = 1;
        parts.push_back (tmp_part);

        tmp_part.name = std::string("engine");
        tmp_part.sym = '^';
        tmp_part.color = c_green;
        tmp_part.mount_dx = 1;
        tmp_part.mount_dy = 0;
        tmp_part.dmg_mod = 100;
        tmp_part.flags = VHP_FUEL_TANK | VHP_ENGINE;
        parts.push_back (tmp_part);

        tmp_part.name = std::string("frame");
        tmp_part.sym = '|';
        tmp_part.color = c_green;
        tmp_part.mount_dx = 0;
        tmp_part.mount_dy = -1;
        tmp_part.dmg_mod = 70;
        tmp_part.flags = 0;
        parts.push_back (tmp_part);
        tmp_part.mount_dy = 1;
        parts.push_back (tmp_part);

        tmp_part.name = std::string("trunk");
        tmp_part.sym = 'H';
        tmp_part.color = c_ltgreen;
        tmp_part.mount_dx = -1;
        tmp_part.mount_dy = -1;
        tmp_part.dmg_mod = 40;
        tmp_part.flags = VHP_CARGO;
        parts.push_back (tmp_part);
        tmp_part.mount_dy = 0;
        parts.push_back (tmp_part);
        tmp_part.mount_dy = 1;
        parts.push_back (tmp_part);

        tmp_part.name = std::string("frame");
        tmp_part.sym = '-';
        tmp_part.color = c_green;
        tmp_part.mount_dx = -2;
        tmp_part.mount_dy = 0;
        tmp_part.dmg_mod = 60;
        tmp_part.flags = VHP_EXHAUST | VHP_BACK;
        parts.push_back (tmp_part);

        break;
    default:;
        name = "invalid vehicle";
    }
    fuel = rng (max_fuel / 10, max_fuel * 3 / 4);
}
