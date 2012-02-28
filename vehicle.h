#ifndef _VEHICLE_H_
#define _VEHICLE_H_

#include "tileray.h"
#include "color.h"
#include "item.h"
#include <vector>
#include <string>
#include <fstream>

enum vhtype_id
{
    veh_null = 0,
    veh_motorcycle,
    veh_sandbike,
    veh_nukecar,

    num_vehicles
};

enum vhpart_flag {
    VHP_BACK        = 0x0,      // flags for exhaust direction, relative
    VHP_FORWARD     = 0x1,      // to corresponding part
    VHP_LEFT        = 0x2,      // i.e. motorcycle back wheel might have
    VHP_RIGHT       = 0x3,      // VHP_CARGO | VHP_EXHAUST | VHP_BACK
    VHP_DIR_MASK    = 0x3,

    VHP_SEAT        = 0x4,      // this part contains seat (you can't board it without a seat. also seat should be mounted at 0,0)
    VHP_GROUND      = 0x8,      // this part have contact with ground
    VHP_ENGINE      = 0x10,     // this part contains engine
    VHP_FUEL_TANK   = 0x20,     // this part contains fuel tank
    VHP_CARGO       = 0x40,     // this part contains cargo trunk
    VHP_EXHAUST     = 0x80,     // this part produces exhaust to direction VHP_BACK or VHP_FORWARD or VHP_LEFT or VHP_RIGHT
    VHP_SHARP       = 0x100     // this part does cutting damage instead of bashing
};

class map;
class player;
class game;

// Structure, describing vehicle part (ie, wheel, seat)
struct vehicle_part
{
    std::string name;
    char sym;           // symbol of part as if it's looking north
    nc_color color;
    int mount_dx;
    int mount_dy;
    int dmg_mod;
    unsigned long flags;
    std::vector<item> items;
};

// Facts you need to know about implementation:
// - Vehicles belong to map. There's std::vector<vehicle>
//   for each submap in grid. When requesting a reference
//   to vehicle, keep in mind it can be invalidated
//   by functions such as map::displace_vehicle.
// - To check if there's any vehicle at given map tile,
//   call map::veh_at, and check vehicle type (veh_null
//   means there's no vehicle there).
// - Driver doesn't know what vehicle he drives.
//   There's only player::drive_mode flag which
//   indicates that he drives something. To figure
//   out what, you need to ask a map if there's a vehicle
//   at driver position.
// - There's also vehicle::driven flag, that means someone is
//   driving this vehicle. Right now it can be only player.
//   To add NPC drivers you need to add some property to
//   vehicle class (i.e. index in active NPCs array, with -2 means
//   game::u) and update veh::get_driver function accordingly.
// - To keep those flags consistent always use
//   map::board_vehicle and map::unboard_vehicle for
//   boarding/unboarding player.
// - To add new vehicle, assign new value for vhtype_id enum
//   and add new case in vehicle::make function similar to
//   already implemented.
class vehicle
{
public:
    vehicle ();
    vehicle (vhtype_id type_id, int px, int py, int d, int vel);
    ~vehicle ();

    std::string fuel_name();   // vehicle's fuel type name

// Constuct a vehicle of type type_id
    void make (vhtype_id type_id);

// load and init vehicle data from stream. This implies valid save data!
    void load (std::ifstream &stin, game *g);

// Save vehicle data to stream
    void save (std::ofstream &stout);

// Vehicle part features description
    std::string part_desc (int part);

// Translate seat-relative mount coords into tile coords
    void coord_translate (int reldx, int reldy, int &dx, int &dy);

// Translate seat-relative mount coords into tile coords using given face direction
    void coord_translate (int dir, int reldx, int reldy, int &dx, int &dy);

// Seek a vehicle part which obstructs tile with given coords relative to vehicle position
    int part_at (int dx, int dy);

// Get the driver of this vehicle
    player *get_driver(game *g);

// get global coords for vehicle given submap coords
    void global_pos (int smx, int smy, int &x, int &y);

// thrust (1) or brake (-1) vehicle
    void thrust (game *g, int thd, int x, int y);

// cruise control
    void cruise_thrust (int amount);

// turn vehicle left (negative) or right (positive), degrees
    void turn (game *g, int deg);

// Check if we hit other vehicle, monster/NPC/player or terrain obstacle. Return true if we continue to move
    bool handle_collision (game *g, int vx, int vy, int part, int x, int y);

// Process the trap beneath
    void handle_trap (game *g, int x, int y, int part);

// add item to part's cargo. if false, then there's no cargo at this part or cargo is full
    bool add_item (int part, item itm);

// remove item from part's cargo
    void remove_item (int part, int itemdex);

// reduces velocity to 0
    void stop ();

// engine/whatever damaged, emit smoke
    void wreck ();

    void explode (game *g, int vx, int vy);

    std::string name;   // vehicle name
    int type;           // vehicle type
    std::vector<vehicle_part> parts;   // Parts which occupy different tiles
    int max_velocity;   // maximum velocity, mph
    int max_rear_velocity; // maximum rear velocity, mph
    int acceleration;   // velocity increase per one thrust, mph
    int brake;          // velocity decrease per one brake, percent
    int noise;
    int fuel_type;      // AT_GAS, AT_BATT, AT_PLUT, AT_PLASMA
    int max_fuel;
    int mass;           // mass in kg. 100=motorbike, 1000=car, 3000=truck. heavier vehicles lose less speed on collisions
    int seatbelt;       // bonus to str when calculating driver throw due to collision

// TEMP VALUES
    bool driven;        // Means the player is driving this vehicle
                        // if there will be NPCs drivig vehicles, it need to be converted into
                        // index in active NPCs list (with special value for player) or something
    int posx, posy;
    tileray face;       // frame direction
    tileray move;       // direction we are moving
    int velocity;       // vehicle current velocity, mph * 100
    int cruise_velocity; // velocity vehicle's cruise control trying to acheive
    bool cruise_on;     // cruise control on/off
    int fuel;           // amount of fuel in tank
    bool malfunction;
    int smoking_turns;  // amount of turns to smoke (due to malfunction)
    int turn_dir;       // direction, to wich vehicle is turning (player control). will rotate frame on next move
    bool skidding;      // skidding mode
    int last_turn;      // amount of last turning (for calculate skidding due to handbrake)
    int moves;
// std::vector<item> inv; // Inventory

};

#endif
