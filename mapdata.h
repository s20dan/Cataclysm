#ifndef _MAPDATA_H_
#define _MAPDATA_H_

#include <vector>
#include <string>
#include "color.h"
#include "item.h"
#include "trap.h"
#include "monster.h"
#include "enums.h"
#include "computer.h"
#include "vehicle.h"

class game;
class monster;

#define MAP_EXTRA_CHANCE 40

#ifndef SEEX 	// SEEX is how far the player can see in the X direction (at
#define SEEX 12	// least, without scrolling).  All map segments will need to be
#endif		// at least this wide. The map therefore needs to be 3x as wide.

#ifndef SEEY	// Same as SEEX
#define SEEY 12 // Requires 2*SEEY+1= 25 vertical squares
#endif	        // Nuts to 80x24 terms. Mostly exists in graphical clients, and
	        // those fatcats can resize.

// mfb(t_flag) converts a flag to a bit for insertion into a bitfield
#ifndef mfb
#define mfb(n) long(1 << (n))
#endif

enum t_flag {
 transparent = 0,// Player & monsters can see through/past it
 bashable,     // Player & monsters can bash this & make it the next in the list
 container,    // Items on this square are hidden until looted by the player
 door,         // Can be opened--used for NPC pathfinding.
 flammable,    // May be lit on fire, burns to rubble.
 inflammable,  // May be lit on fire, burns to ash.
 meltable,     // Burns into "twisted metal" under enough heat.
 explodes,     // Explodes when on fire
 diggable,     // Digging monsters, seeding monsters, digging w/ shovel, etc.
 swimmable,    // You (and monsters) swim here
 sharp,	       // May do minor damage to players/monsters passing it
 razor,        // Sharp, turned up to 11
 electric,     // Same damage as razor, and locks player in place for 8 turns.
 rough,        // May hurt the player's feet
 sealed,       // Can't 'e' to retrieve items here
 noitem,       // Items "fall off" this space
 goes_down,    // Can '>' to go down a level
 goes_up,      // Can '<' to go up a level
 console,      // Used as a computer
 alarmed,      // Sets off an alarm if smashed
 supports_roof,// Used as a boundary for roof construction
 thin_obstacle,// obstacles that can be passed by player and monsters, and can't by vehicles
 num_t_flags   // MUST be last
};

struct ter_t {
 std::string name;
 char sym;
 nc_color color;
 unsigned char movecost;
 unsigned long flags;// : num_t_flags;
};

enum ter_id {
t_null = 0,
t_hole,	// Real nothingness; makes you fall a z-level
// Smithing
t_forge, t_grindstone, t_watertub, t_crucible, t_anvil,
// Ground
t_dirt, t_bog, t_dbog, t_claydirt, t_dirtmound,
t_pit_shallow, t_pit, t_pit_spiked, t_pit_bridge, t_s_pit_bridge,
t_rock_floor, t_rubble, t_ash, t_wreckage, t_b_metal,
t_grass, t_tent, t_flap_c, t_flap_o, t_groundsheet, t_campfire, t_awnsheet,
t_awnfloor, t_support, t_fence_electric,
t_metal_floor,
t_pavement, t_pavement_y, t_sidewalk,
t_floor,
t_grate,
t_slime,
t_bridge,
// Walls & doors
t_wall_half, t_wall_wood,
t_wall_v, t_wall_h,
t_wall_metal_v, t_wall_metal_h,
t_wall_glass_v, t_wall_glass_h,
t_wall_glass_v_alarm, t_wall_glass_h_alarm,
t_reinforced_glass_v, t_reinforced_glass_h,
t_bars, t_spikebar, t_palisade, t_palgate_c, t_palgate_o,
t_door_c, t_door_b, t_door_o, t_door_locked, t_door_locked_alarm, t_door_frame,
 t_door_boarded,
t_door_metal_c, t_door_metal_o, t_door_metal_locked,
t_door_glass_c, t_door_glass_o,
t_bulletin,
t_portcullis,
t_window, t_window_alarm, t_window_empty, t_window_frame, t_window_boarded,
t_rock, t_fault,
t_paper,
// Tree
t_tree, t_tree_young, t_underbrush, t_stump, t_fallentree,
t_root_wall,
t_wax, t_floor_wax,
t_fence_v, t_fence_h,
t_railing_v, t_railing_h,
// Nether
t_marloss, t_fungus, t_tree_fungal,
// Water, lava, etc.
t_water_sh, t_water_dp, t_sewage,
t_lava,
// Embellishments
t_bed, t_cot, t_pallet, t_toilet,
t_sandbox, t_slide, t_monkey_bars, t_backboard,
t_bench, t_table, t_pool_table,
t_gas_pump, t_gas_pump_smashed, t_gas_pump_depleted,
t_missile, t_missile_exploded,
t_counter,
t_radio_tower, t_radio_controls,
t_console_broken, t_console,
t_sewage_pipe, t_sewage_pump,
t_centrifuge,
// Containers
t_fridge, t_dresser,
t_rack, t_bookcase,
t_dumpster,
t_vat, t_crate_c, t_crate_o,
// Staircases etc.
t_stairs_down, t_stairs_up, t_manhole, t_ladder_up, t_ladder_down, t_slope_down,
 t_slope_up, t_rope_up,
t_manhole_cover,
// Special
t_card_science, t_card_military, t_card_reader_broken, t_slot_machine,
 t_elevator_control, t_elevator_control_off, t_elevator, t_pedestal_wyrm,
 t_pedestal_temple,
// Temple tiles
t_rock_red, t_rock_green, t_rock_blue, t_floor_red, t_floor_green, t_floor_blue,
 t_switch_rg, t_switch_gb, t_switch_rb, t_switch_even,
num_terrain_types
};

const ter_t terlist[num_terrain_types] = {  // MUST match enum ter_id above!
{"nothing",	     ' ', c_white,   2,
	mfb(transparent)|mfb(diggable)},
{"empty space",      '#', c_black,   2,
	mfb(transparent)},
{"forge",            '&', c_dkgray,   0,
	mfb(transparent)|mfb(bashable)},
{"grindstone",       '&', c_ltgray,   0,
        mfb(transparent)|mfb(bashable)},
{"water tub",        '~', c_ltblue,   0,
	mfb(transparent)|mfb(bashable)},
{"crucible",         'O', c_dkgray,   0,
        mfb(transparent)|mfb(bashable)},
{"anvil",            'z', c_dkgray,   0,
        mfb(transparent)|mfb(bashable)},
{"dirt",	     '.', c_brown,   2,
	mfb(transparent)|mfb(diggable)},
{"bog",	             '.', c_ltblue,   2,
	mfb(transparent)|mfb(diggable)},
{"bog",   	     '.', c_ltblue,   2,
	mfb(transparent)|mfb(diggable)},
{"dirt",	     '.', c_brown,   2,
	mfb(transparent)|mfb(diggable)},
{"mound of dirt",    '#', c_brown,   3,
	mfb(transparent)|mfb(diggable)},
{"shallow pit",	     '0', c_yellow,  8,
	mfb(transparent)},
{"pit",              '0', c_brown,  10,
	mfb(transparent)},
{"spiked pit",       '0', c_ltred,  10,
	mfb(transparent)},
{"covered pit",      '#', c_brown,   4,
        mfb(transparent)},
{"covered pit",      '#', c_brown,   5,
        mfb(transparent)},
{"rock floor",       '.', c_ltgray,  2,
	mfb(transparent)},
{"pile of rubble",   '#', c_ltgray,  4,
	mfb(transparent)|mfb(rough)|mfb(diggable)},
{"ashes",            '#', c_dkgray,  2,
        mfb(transparent)|mfb(diggable)},
{"metal wreckage",   '#', c_cyan,    5,
	mfb(transparent)|mfb(rough)|mfb(sharp)|mfb(container)},
{"twisted metal",    '#', c_ltcyan,  4,
        mfb(transparent)|mfb(sharp)|mfb(container)},
{"grass",	     '.', c_green,   2,
	mfb(transparent)|mfb(diggable)},
{"tent wall",        'O', c_green,   0,
        mfb(inflammable)},
{"tent flap",        '+', c_green,   0,
        mfb(inflammable)},
{"tent flap",        '0', c_green,   2,
        mfb(inflammable)|mfb(door)|mfb(transparent)},
{"groundsheet",      '.', c_green,   2,
        mfb(inflammable)|mfb(transparent)},
{"firepit",         '0', c_ltgray,  2,
        mfb(container)|mfb(transparent)},
{"canvas floor",     ',', c_green,   2,
        mfb(inflammable)|mfb(transparent)},
{"canvas floor",     '.', c_green,   2,
        mfb(inflammable)|mfb(transparent)},
{"support",          'o', c_green,   2,
        mfb(inflammable)|mfb(transparent)},
{"electric fence",   '^', c_cyan,    4,
        mfb(electric)|mfb(transparent)}, 
{"metal floor",      '.', c_ltcyan,  2,
	mfb(transparent)},
{"pavement",	     '.', c_dkgray,  2,
	mfb(transparent)},
{"yellow pavement",  '.', c_yellow,  2,
	mfb(transparent)},
{"sidewalk",         '.', c_ltgray,  2,
	mfb(transparent)},
{"floor",	     '.', c_cyan,    2,
	mfb(transparent)|mfb(inflammable)},
{"metal grate",      '#', c_dkgray,  2,
	mfb(transparent)},
{"slime",            '~', c_green,   6,
	mfb(transparent)|mfb(container)|mfb(inflammable)},
{"walkway",          '#', c_yellow,  2,
	mfb(transparent)},
{"half-built wall",  '#', c_brown,   4,
	mfb(transparent)|mfb(bashable)|mfb(inflammable)|mfb(noitem)},
{"wooden wall",      '#', c_brown,   0,
	mfb(bashable)|mfb(inflammable)|mfb(noitem)|mfb(supports_roof)},
{"wall",             '|', c_ltgray,  0,
	mfb(flammable)|mfb(noitem)|mfb(supports_roof)},
{"wall",             '-', c_ltgray,  0,
	mfb(flammable)|mfb(noitem)|mfb(supports_roof)},
{"metal wall",       '|', c_cyan,    0,
	mfb(noitem)|mfb(noitem)|mfb(supports_roof)},
{"metal wall",       '-', c_cyan,    0,
	mfb(noitem)|mfb(noitem)|mfb(supports_roof)},
{"glass wall",       '|', c_ltcyan,  0,
	mfb(transparent)|mfb(bashable)|mfb(noitem)|mfb(supports_roof)},
{"glass wall",       '-', c_ltcyan,  0,
	mfb(transparent)|mfb(bashable)|mfb(noitem)|mfb(supports_roof)},
{"glass wall",       '|', c_ltcyan,  0, // Alarmed
	mfb(transparent)|mfb(bashable)|mfb(alarmed)|mfb(noitem)|
        mfb(supports_roof)},
{"glass wall",       '-', c_ltcyan,  0, // Alarmed
	mfb(transparent)|mfb(bashable)|mfb(alarmed)|mfb(noitem)|
        mfb(supports_roof)},
{"reinforced glass", '|', c_ltcyan,  0,
	mfb(transparent)|mfb(bashable)|mfb(noitem)|mfb(supports_roof)},
{"reinforced glass", '-', c_ltcyan,  0,
	mfb(transparent)|mfb(bashable)|mfb(noitem)|mfb(supports_roof)},
{"metal bars",       '"', c_ltgray,  0,
	mfb(transparent)|mfb(noitem)},
{"spiked barricade", '^', c_brown,   8,
        mfb(noitem)|mfb(inflammable)|mfb(razor)|mfb(transparent)},
{"wooden palisade",  '#', c_brown,   0,
        mfb(noitem)|mfb(inflammable)|mfb(transparent)},
{"palisade gate",    '#', c_ltgray,  0,
        mfb(noitem)|mfb(inflammable)|mfb(transparent)},
{"raised gate",      '.', c_ltgray,  2,
        mfb(noitem)|mfb(inflammable)|mfb(transparent)},
{"closed wood door", '+', c_brown,   0,
	mfb(bashable)|mfb(inflammable)|mfb(door)|mfb(noitem)|mfb(supports_roof)},
{"damaged wood door",'&', c_brown,   0,
	mfb(transparent)|mfb(bashable)|mfb(inflammable)|mfb(noitem)|
        mfb(supports_roof)},
{"open wood door",  '\'', c_brown,   2,
	mfb(transparent)|mfb(supports_roof)|mfb(inflammable)},
{"closed wood door", '+', c_brown,   0,	// Actually locked
	mfb(bashable)|mfb(inflammable)|mfb(noitem)|mfb(supports_roof)},
{"closed wood door", '+', c_brown,   0, // Locked and alarmed
	mfb(bashable)|mfb(inflammable)|mfb(alarmed)|mfb(noitem)|
        mfb(supports_roof)},
{"empty door frame", '.', c_brown,   2,
	mfb(transparent)|mfb(supports_roof)|mfb(inflammable)},
{"boarded up door",  '#', c_brown,   0,
	mfb(bashable)|mfb(inflammable)|mfb(noitem)|mfb(supports_roof)},
{"closed metal door",'+', c_cyan,    0,
	mfb(noitem)|mfb(supports_roof)},
{"open metal door", '\'', c_cyan,    2,
	mfb(transparent)|mfb(supports_roof)},
{"closed metal door",'+', c_cyan,    0, // Actually locked
	mfb(noitem)|mfb(supports_roof)},
{"closed glass door",'+', c_ltcyan,  0,
	mfb(transparent)|mfb(bashable)|mfb(door)|mfb(noitem)|mfb(supports_roof)},
{"open glass door", '\'', c_ltcyan,  2,
	mfb(transparent)|mfb(supports_roof)},
{"bulletin board",   '6', c_blue,    0,
	mfb(noitem)},
{"makeshift portcullis", '&', c_cyan, 0,
	mfb(noitem)},
{"window",	     '"', c_ltcyan,  0,
	mfb(transparent)|mfb(bashable)|mfb(flammable)|mfb(noitem)|
        mfb(supports_roof)},
{"window",	     '"', c_ltcyan,  0, // Actually alarmed
	mfb(transparent)|mfb(bashable)|mfb(flammable)|mfb(alarmed)|mfb(noitem)|
        mfb(supports_roof)},
{"empty window",     '0', c_yellow,  8,
	mfb(transparent)|mfb(flammable)|mfb(noitem)|mfb(supports_roof)},
{"window frame",     '0', c_ltcyan,  8,
	mfb(transparent)|mfb(sharp)|mfb(flammable)|mfb(noitem)|
        mfb(supports_roof)},
{"boarded up window",'#', c_brown,   0,
	mfb(bashable)|mfb(flammable)|mfb(noitem)|mfb(supports_roof)},
{"solid rock",       '#', c_white,   0,
	mfb(noitem)|mfb(supports_roof)},
{"odd fault",        '#', c_magenta, 0,
	mfb(noitem)|mfb(supports_roof)},
{"paper wall",       '#', c_white,   0,
	mfb(bashable)|mfb(inflammable)|mfb(noitem)},
{"tree",	     '7', c_green,   4,
	mfb(inflammable)|mfb(noitem)},
{"young tree",       '1', c_green,   3,
	mfb(transparent)|mfb(bashable)|mfb(inflammable)|mfb(noitem)},
{"underbrush",       '#', c_green,   6,
	mfb(transparent)|mfb(bashable)|mfb(diggable)|mfb(container)|mfb(rough)|
	mfb(inflammable)},
{"tree stump",       '0', c_ltgray,  3,
        mfb(transparent)|mfb(inflammable)},
{"fallen tree",      '#', c_ltgray,  4,
        mfb(inflammable)|mfb(noitem)},
{"root wall",        '#', c_brown,   0,
	mfb(noitem)|mfb(supports_roof)},
{"wax wall",         '#', c_yellow,  0,
	mfb(container)|mfb(inflammable)|mfb(noitem)|mfb(supports_roof)},
{"wax floor",        '.', c_yellow,  2,
	mfb(transparent)|mfb(inflammable)},
{"picket fence",     '|', c_brown,   3,
	mfb(transparent)|mfb(diggable)|mfb(flammable)|mfb(noitem)|mfb(thin_obstacle)},
{"picket fence",     '-', c_brown,   3,
	mfb(transparent)|mfb(diggable)|mfb(flammable)|mfb(noitem)|mfb(thin_obstacle)},
{"railing",          '|', c_yellow,  3,
	mfb(transparent)|mfb(noitem)|mfb(thin_obstacle)},
{"railing",          '-', c_yellow,  3,
	mfb(transparent)|mfb(noitem)|mfb(thin_obstacle)},
{"marloss bush",     '1', c_dkgray,  0,
	mfb(transparent)|mfb(bashable)|mfb(inflammable)},
{"fungal bed",       '#', c_dkgray,  3,
	mfb(transparent)|mfb(inflammable)|mfb(diggable)},
{"fungal tree",      '7', c_dkgray,  0,
	mfb(inflammable)|mfb(noitem)},
{"shallow water",    '~', c_ltblue,  5,
	mfb(transparent)|mfb(swimmable)},
{"deep water",       '~', c_blue,    0,
	mfb(transparent)|mfb(swimmable)},
{"sewage",           '~', c_ltgreen, 6,
	mfb(transparent)|mfb(swimmable)},
{"lava",             '~', c_red,     0,
	mfb(transparent)},
{"bed",              '#', c_magenta, 5,
	mfb(transparent)|mfb(container)|mfb(inflammable)},
{"cot",              '#', c_green,   2,
	mfb(transparent)|mfb(container)|mfb(inflammable)},
{"sleeping pallet",  '#', c_magenta, 3,
	mfb(transparent)|mfb(container)|mfb(inflammable)},
{"toilet",           '&', c_white,   0,
	mfb(transparent)|mfb(bashable)},
{"sandbox",          '#', c_yellow,  3,
	mfb(transparent)},
{"slide",            '#', c_ltcyan,  4,
	mfb(transparent)},
{"monkey bars",      '#', c_cyan,    4,
	mfb(transparent)},
{"backboard",        '7', c_red,     0,
	mfb(transparent)},
{"bench",            '#', c_brown,   3,
	mfb(transparent)|mfb(inflammable)},
{"table",            '#', c_red,     4,
	mfb(transparent)|mfb(inflammable)},
{"pool table",       '#', c_green,   4,
	mfb(transparent)|mfb(inflammable)},
{"gasoline pump",    '&', c_red,     0,
	mfb(transparent)|mfb(explodes)|mfb(noitem)},
{"smashed gas pump", '&', c_ltred,   0,
	mfb(transparent)|mfb(noitem)},
{"depleted gas pump",'&', c_ltred,   0,
        mfb(transparent)|mfb(noitem)},
{"missile",          '#', c_ltblue,  0,
	mfb(explodes)|mfb(noitem)},
{"blown-out missile",'#', c_ltgray,  0,
	mfb(noitem)},
{"counter",	     '#', c_blue,    4,
	mfb(transparent)|mfb(inflammable)},
{"radio tower",      '&', c_ltgray,  0,
	mfb(noitem)},
{"radio controls",   '6', c_green,   0,
	mfb(transparent)|mfb(bashable)|mfb(noitem)},
{"broken console",   '6', c_ltgray,  0,
	mfb(transparent)|mfb(noitem)},
{"computer console", '6', c_blue,    0,
	mfb(transparent)|mfb(console)|mfb(noitem)},
{"sewage pipe",      '1', c_ltgray,  0,
	mfb(transparent)},
{"sewage pump",      '&', c_ltgray,  0,
	mfb(noitem)},
{"centrifuge",       '{', c_magenta, 0,
	mfb(transparent)},
{"refrigerator",     '{', c_ltcyan,  0,
	mfb(container)|mfb(meltable)},
{"dresser",          '{', c_brown,   0,
	mfb(transparent)|mfb(container)|mfb(inflammable)},
{"display rack",     '{', c_ltgray,  0,
        mfb(transparent)|mfb(container)|mfb(meltable)},
{"book case",        '{', c_brown,   0,
	mfb(container)|mfb(inflammable)},
{"dumpster",	     '{', c_green,   0,
	mfb(container)},
{"cloning vat",      '0', c_ltcyan,  0,
	mfb(transparent)|mfb(bashable)|mfb(container)|mfb(sealed)},
{"crate",            '{', c_brown,   0,
        mfb(transparent)|mfb(bashable)|mfb(container)|mfb(sealed)|
        mfb(inflammable)},
{"open crate",       '{', c_brown,   0,
        mfb(transparent)|mfb(bashable)|mfb(container)|mfb(inflammable)},
{"stairs down",      '>', c_yellow,  2,
	mfb(transparent)|mfb(goes_down)|mfb(container)},
{"stairs up",        '<', c_yellow,  2,
	mfb(transparent)|mfb(goes_up)|mfb(container)},
{"manhole",          '>', c_dkgray,  2,
	mfb(transparent)|mfb(goes_down)|mfb(container)},
{"ladder",           '<', c_dkgray,  2,
	mfb(transparent)|mfb(goes_up)|mfb(container)},
{"ladder",           '>', c_dkgray,  2,
	mfb(transparent)|mfb(goes_down)|mfb(container)},
{"downward slope",   '>', c_brown,   2,
	mfb(transparent)|mfb(goes_down)|mfb(container)},
{"upward slope",     '<', c_brown,   2,
	mfb(transparent)|mfb(goes_up)|mfb(container)},
{"rope leading up",  '<', c_white,   2,
	mfb(transparent)|mfb(goes_up)},
{"manhole cover",    '0', c_dkgray,  2,
	mfb(transparent)},
{"card reader",	     '6', c_pink,    0,	// Science
	mfb(noitem)},
{"card reader",	     '6', c_pink,    0,	// Military
	mfb(noitem)},
{"broken card reader",'6',c_ltgray,  0,
	mfb(noitem)},
{"slot machine",     '6', c_green,   0,
	mfb(bashable)|mfb(noitem)},
{"elevator controls",'6', c_ltblue,  0,
	mfb(noitem)},
{"powerless controls",'6',c_ltgray,  0,
	mfb(noitem)},
{"elevator",         '.', c_magenta, 2,
	0},
{"dark pedestal",    '&', c_dkgray,  0,
	mfb(transparent)},
{"light pedestal",   '&', c_white,   0,
	mfb(transparent)},
{"red stone",        '#', c_red,     0,
	0},
{"green stone",      '#', c_green,   0,
	0},
{"blue stone",       '#', c_blue,    0,
	0},
{"red floor",        '.', c_red,     2,
	mfb(transparent)},
{"green floor",      '.', c_green,   2,
	mfb(transparent)},
{"blue floor",       '.', c_blue,    2,
	mfb(transparent)},
{"yellow switch",    '6', c_yellow,  0,
	mfb(transparent)},
{"cyan switch",      '6', c_cyan,    0,
	mfb(transparent)},
{"purple switch",    '6', c_magenta, 0,
	mfb(transparent)},
{"checkered switch", '6', c_white,   0,
	mfb(transparent)}

};

enum map_extra {
 mx_null = 0,
 mx_helicopter,
 mx_military,
 mx_science,
 mx_stash,
 mx_drugdeal,
 mx_supplydrop,
 mx_portal,
 mx_minefield,
 mx_wolfpack,
 mx_puddle,
 mx_crater,
 mx_fumarole,
 mx_portal_in,
 num_map_extras
};

// Chances are relative to eachother; e.g. a 200 chance is twice as likely
// as a 100 chance to appear.
const int map_extra_chance[num_map_extras + 1] = {
  0,	// Null - 0 chance
 40,	// Helicopter
 50,	// Military
120,	// Science
200,	// Stash
 20,	// Drug deal
 10,    // Supply drop
  5,	// Portal
 70,	// Minefield
 30,	// Wolf pack
250,	// Puddle
 10,	// Crater
  8,	// Fumarole
  7,	// One-way portal into this world
  0	// Just a cap value; leave this as the last one
};

struct map_extras {
 unsigned int chance;
 int chances[num_map_extras + 1];
 map_extras(unsigned int embellished, int helicopter = 0, int mili = 0,
            int sci = 0, int stash = 0, int drug = 0, int supply = 0,
            int portal = 0, int minefield = 0, int wolves = 0, int puddle = 0, 
            int crater = 0, int lava = 0, int marloss = 0)
            : chance(embellished)
 {
  chances[ 0] = 0;
  chances[ 1] = helicopter;
  chances[ 2] = mili;
  chances[ 3] = sci;
  chances[ 4] = stash;
  chances[ 5] = drug;
  chances[ 6] = supply;
  chances[ 7] = portal;
  chances[ 8] = minefield;
  chances[ 9] = wolves;
  chances[10] = puddle;
  chances[11] = crater;
  chances[12] = lava;
  chances[13] = marloss;
  chances[14] = 0;
 }
};

struct field_t {
 std::string name[3];
 char sym;
 nc_color color[3];
 bool transparent[3];
 bool dangerous[3];
 int halflife;	// In turns
};

enum field_id {
 fd_null = 0,
 fd_blood,
 fd_bile,
 fd_web,
 fd_slime,
 fd_acid,
 fd_sap,
 fd_fire,
 fd_smoke,
 fd_toxic_gas,
 fd_tear_gas,
 fd_nuke_gas,
 fd_gas_vent,
 fd_fire_vent,
 fd_flame_burst,
 fd_electricity,
 fd_fatigue,
 num_fields
};

const field_t fieldlist[] = {
{{"",	"",	""},					'%',
 {c_white, c_white, c_white},	{true, true, true}, {false, false, false},   0},

{{"blood splatter", "blood stain", "puddle of blood"},	'%',
 {c_red, c_red, c_red},		{true, true, true}, {false, false, false},2500},

{{"bile splatter", "bile stain", "puddle of bile"},	'%',
 {c_pink, c_pink, c_pink},	{true, true, true}, {false, false, false},2500},

{{"cobwebs","webs", "thick webs"},			'}',
 {c_white, c_white, c_white},	{true, true, false},{false, false, false},   0},

{{"slime trail", "slime stain", "puddle of slime"},	'%',
 {c_ltgreen, c_ltgreen, c_green},{true, true, true},{false, false, false},2500},

{{"acid splatter", "acid streak", "pool of acid"},	'5',
 {c_ltgreen, c_green, c_green},	{true, true, true}, {true, true, true},	    10},

{{"sap splatter", "glob of sap", "pool of sap"},	'5',
 {c_yellow, c_brown, c_brown},	{true, true, true}, {true, true, true},     20},

{{"small fire",	"fire",	"raging fire"},			'4',
 {c_yellow, c_ltred, c_red},	{true, true, true}, {true, true, true},	   800},

{{"thin smoke",	"smoke", "thick smoke"},		'8',
 {c_white, c_ltgray, c_dkgray},	{true, false, false},{false, true, true},  300},

{{"hazy cloud","toxic gas","thick toxic gas"},		'8',
 {c_white, c_ltgreen, c_green}, {true, false, false},{false, true, true},  900},

{{"hazy cloud","tear gas","thick tear gas"},		'8',
 {c_white, c_yellow, c_brown},	{true, false, false},{true, true, true},   600},

{{"hazy cloud","radioactive gas", "thick radioactive gas"}, '8',
 {c_white, c_ltgreen, c_green},	{true, true, false}, {true, true, true},  1000},

{{"gas vent", "gas vent", "gas vent"}, '%',
 {c_white, c_white, c_white}, {true, true, true}, {false, false, false}, 0},

{{"", "", ""}, '%', // fire vents
 {c_white, c_white, c_white}, {true, true, true}, {false, false, false}, 0},

{{"fire", "fire", "fire"}, '5',
 {c_red, c_red, c_red}, {true, true, true}, {true, true, true}, 0},

{{"sparks", "electric crackle", "electric cloud"},	'9',
 {c_white, c_cyan, c_blue},	{true, true, true}, {true, true, true},	     2},

{{"odd ripple", "swirling air", "tear in reality"},	'*',
 {c_ltgray, c_dkgray, c_magenta},{true, true, false},{false, false, false},  0}
};

struct field {
 field_id type;
 signed char density;
 int age;
 field() { type = fd_null; density = 1; age = 0; };
 field(field_id t, unsigned char d, unsigned int a) {
  type = t;
  density = d;
  age = a;
 }

 bool is_null()
 {
  return (type == fd_null || type == fd_blood || type == fd_bile ||
          type == fd_slime);
 }

 bool is_dangerous()
 {
  return fieldlist[type].dangerous[density - 1];
 }

 std::string name()
 {
  return fieldlist[type].name[density - 1];
 }

};

struct spawn_point {
 int posx, posy;
 int count;
 mon_id type;
 int faction_id;
 int mission_id;
 bool friendly;
 std::string name;
 spawn_point(mon_id T = mon_null, int C = 0, int X = -1, int Y = -1,
             int FAC = -1, int MIS = -1, bool F = false,
             std::string N = "NONE") :
             posx (X), posy (Y), count (C), type (T), faction_id (FAC),
             mission_id (MIS), friendly (F), name (N) {}
};

struct submap {
 ter_id			ter[SEEX][SEEY]; // Terrain on each square
 std::vector<item>	itm[SEEX][SEEY]; // Items on each square
 trap_id		trp[SEEX][SEEY]; // Trap on each square
 field			fld[SEEX][SEEY]; // Field on each square
 int			rad[SEEX][SEEY]; // Irradiation of each square
 int active_item_count;
 int field_count;
 std::vector<spawn_point> spawns;
 std::vector<vehicle> vehicles;
 computer comp;
};

#endif
