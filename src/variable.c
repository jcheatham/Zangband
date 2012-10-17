/* File: variable.c */

/* Purpose: Angband variables */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"


/*
 * Hack -- Link a copyright message into the executable
 */
cptr copyright[5] =
{
	"Copyright (c) 1989 James E. Wilson, Robert A. Keoneke",
	"",
	"This software may be copied and distributed for educational, research,",
	"and not for profit purposes provided that this copyright and statement",
	"are included in all such copies."
};


/*
 * Executable version
 */
byte version_major = VERSION_MAJOR;
byte version_minor = VERSION_MINOR;
byte version_patch = VERSION_PATCH;
byte version_extra = VERSION_EXTRA;

/*
 * Savefile version
 */
byte sf_major;			/* Savefile's "version_major" */
byte sf_minor;			/* Savefile's "version_minor" */
byte sf_patch;			/* Savefile's "version_patch" */
byte sf_extra;			/* Savefile's "version_extra" */
u32b sf_version;		/* Savefile's "version" */

byte z_major;           /* Savefile version for Zangband */
byte z_minor;
byte z_patch;

/*
 * Savefile information
 */
u32b sf_xtra;			/* Operating system info */
u32b sf_when;			/* Time when savefile created */
u16b sf_lives;			/* Number of past "lives" with this file */
u16b sf_saves;			/* Number of "saves" during this life */

/*
 * Run-time arguments
 */
bool arg_fiddle;			/* Command arg -- Request fiddle mode */
bool arg_wizard;			/* Command arg -- Request wizard mode */
bool arg_sound;				/* Command arg -- Request special sounds */
byte arg_graphics;			/* Command arg -- Request graphics mode */
bool arg_monochrome;		/* Command arg -- Request monochrome mode */
bool arg_force_original;	/* Command arg -- Request original keyset */
bool arg_force_roguelike;	/* Command arg -- Request roguelike keyset */

/*
 * Various things
 */
bool character_generated;	/* The character exists */
bool character_dungeon;		/* The character has a dungeon */
bool character_loaded;		/* The character was loaded from a savefile */
bool character_saved;		/* The character was just saved to a savefile */

bool character_icky;		/* The game is in an icky full screen mode */
bool character_xtra;		/* The game is in an icky startup mode */

u32b seed_flavor;		/* Hack -- consistent object colors */
u32b seed_town;			/* Hack -- consistent town layout */

s16b command_cmd;		/* Current "Angband Command" */

s16b command_arg;		/* Gives argument of current command */
s16b command_rep;		/* Gives repetition of current command */
s16b command_dir;		/* Gives direction of current command */

s16b command_see;		/* See "cmd1.c" */
s16b command_wrk;		/* See "cmd1.c" */

s16b command_gap = 50;	/* See "cmd1.c" */

s16b command_new;		/* Command chaining from inven/equip view */

s16b energy_use;		/* Energy use this turn */

bool create_up_stair;	/* Auto-create "up stairs" */
bool create_down_stair;	/* Auto-create "down stairs" */

bool msg_flag;			/* Used in msg_print() for "buffering" */

bool alive;				/* True if game is running */

bool death;				/* True if player has died */

s16b running;			/* Current counter for running, if any */
s16b resting;			/* Current counter for resting, if any */

s16b min_hgt;			/* Current y bounds of area() */
s16b max_hgt;
s16b min_wid;			/* Current x bounds of area() */
s16b max_wid;

s16b dun_level;			/* Current dungeon level */
s16b num_repro;			/* Current reproducer count */

bool mon_fight;			/* Monster fighting indicator */

s16b object_level;		/* Current object creation level */
s16b monster_level;		/* Current monster creation level */
s16b base_level;        /* Base dungeon level */

s32b turn;				/* Current game turn */
s32b old_turn;			/* Turn when level began (feelings) */

bool wizard;			/* Is the player currently in Wizard mode? */

bool use_sound;			/* The "sound" mode is enabled */
bool use_graphics;		/* The "graphics" mode is enabled */

u16b total_winner;		/* Semi-Hack -- Game has been won */

u16b panic_save;		/* Track some special "conditions" */
u16b noscore;			/* Track various "cheating" conditions */

s16b signal_count;		/* Hack -- Count interupts */

bool inkey_base;		/* See the "inkey()" function */
bool inkey_xtra;		/* See the "inkey()" function */
bool inkey_scan;		/* See the "inkey()" function */
bool inkey_flag;		/* See the "inkey()" function */

s16b coin_type;			/* Hack -- force coin type */

bool opening_chest;		/* Hack -- prevent chest generation */

bool shimmer_monsters;	/* Hack -- optimize multi-hued monsters */

bool repair_monsters;	/* Hack -- optimize detect monsters */

s16b inven_nxt;			/* Hack -- unused */
bool hack_mind;
bool hack_mutation;
int artifact_bias;

s16b inven_cnt;			/* Number of items in inventory */
s16b equip_cnt;			/* Number of items in equipment */

s16b o_max = 1;			/* Number of allocated objects */
s16b o_cnt = 0;			/* Number of live objects */

s16b m_max = 1;			/* Number of allocated monsters */
s16b m_cnt = 0;			/* Number of live monsters */

s16b fld_max = 1;			/* Number of allocated fields */
s16b fld_cnt = 0;			/* Number of live fields */

s16b hack_m_idx = 0;	/* Hack -- see "process_monsters()" */
s16b hack_m_idx_ii = 0;
s16b *hack_fld_ptr = NULL; /* Hack -- see "fields.c" */
bool multi_rew = FALSE;
char summon_kin_type;   /* Hack, by Julian Lighton: summon 'relatives' */

int total_friends = 0;
s32b total_friend_levels = 0;
s32b friend_align = 0;

int leaving_quest = 0;

s16b store_cache_num = 0;	/* Number of stores with stock */
store_type **store_cache;	/* The cache of store stocks */

/*
 * Software options (set via the '=' command).  See "tables.c"
 * Needs to be rearranged to fit the data in tables.c
 */


/* Option Set 1 -- User Interface */

bool rogue_like_commands;	/* Rogue-like commands */
bool quick_messages;		/* Activate quick messages */
bool other_query_flag;		/* Prompt for various information */
bool carry_query_flag;		/* Prompt before picking things up */
bool use_old_target;		/* Use old target by default */
bool always_pickup;			/* Pick things up by default */
bool always_repeat;			/* Repeat obvious commands */
bool depth_in_feet;			/* Show dungeon level in feet */

bool stack_force_notes;		/* Merge inscriptions when stacking */
bool stack_force_costs;		/* Merge discounts when stacking */

bool show_labels;			/* Show labels in object listings */
bool show_weights;			/* Show weights in object listings */

bool ring_bell;				/* Ring the bell (on errors, etc) */
bool use_color;				/* Use color if possible (slow) */


/* Option Set 2 -- Disturbance */

bool find_ignore_stairs;	/* Run past stairs */
bool find_ignore_doors;		/* Run through open doors */
bool find_cut;				/* Run past known corners */
bool find_examine;			/* Run into potential corners */

bool disturb_move;			/* Disturb whenever any monster moves */
bool disturb_near;			/* Disturb whenever viewable monster moves */
bool disturb_panel;			/* Disturb whenever map panel changes */
bool disturb_state;			/* Disturn whenever player state changes */
bool disturb_minor;			/* Disturb whenever boring things happen */
bool disturb_other;			/* Disturb whenever various things happen */
bool disturb_traps;			/* Disturb whenever you move out of detect radius */

bool alert_failure;		/* Alert user to various failures */
bool last_words;		/* Get last words upon dying */
bool speak_unique;		/* Speaking uniques */
bool small_levels;		/* Allow unusually small dungeon levels */
bool empty_levels;		/* Allow empty 'arena' levels */
bool plain_descriptions;	/* Plain object descriptions */
bool stupid_monsters;		/* Monsters use old AI */
bool silly_monsters;		/* Allow the silly monsters to be generated */
bool auto_destroy;		/* Known worthless items are destroyed without confirmation */
bool confirm_stairs;		/* Prompt before staircases... */
bool wear_confirm;		/* Confirm before putting on known cursed items */

bool disturb_pets;		/* Pets moving nearby disturb us */





/* Option Set 3 -- Game-Play */

bool auto_haggle;			/* Auto-haggle in stores */

bool auto_scum;				/* Auto-scum for good levels */

bool stack_allow_items;		/* Allow weapons and armor to stack */
bool stack_allow_wands;		/* Allow wands/staffs/rods to stack */

bool expand_look;			/* Expand the power of the look command */
bool expand_list;			/* Expand the power of the list commands */

bool view_perma_grids;		/* Map remembers all perma-lit grids */
bool view_torch_grids;		/* Map remembers all torch-lit grids */

bool dungeon_align;			/* Generate dungeons with aligned rooms */
bool dungeon_stair;			/* Generate dungeons with connected stairs */

bool flow_by_sound;			/* Monsters track new player location */
bool flow_by_smell;			/* Monsters track old player location */

bool track_follow;			/* Monsters follow the player */
bool track_target;			/* Monsters target the player */

bool smart_learn;			/* Monsters learn from their mistakes */
bool smart_cheat;			/* Monsters exploit player weaknesses */

bool take_notes;                        /* Allow notes to be added to a file */
bool auto_notes;                        /* Automatically take notes */

bool point_based;                       /* Point-based generation */

/* Option Set 4 -- Efficiency */

bool view_reduce_lite;		/* Reduce lite-radius when running */
bool view_reduce_view;		/* Reduce view-radius in town */

bool avoid_abort;			/* Avoid checking for user abort */
bool avoid_other;			/* Avoid processing special colors */

bool flush_failure;			/* Flush input on any failure */
bool flush_disturb;			/* Flush input on disturbance */
bool flush_command;			/* Flush input before every command */

bool fresh_before;			/* Flush output before normal commands */
bool fresh_after;			/* Flush output after normal commands */
bool fresh_message;			/* Flush output after all messages */

bool compress_savefile;		/* Compress messages in savefiles */

bool hilite_player;			/* Hilite the player with the cursor */

bool view_yellow_lite;		/* Use special colors for torch-lit grids */
bool view_bright_lite;		/* Use special colors for 'viewable' grids */

bool view_granite_lite;		/* Use special colors for wall grids (slow) */
bool view_special_lite;		/* Use special colors for floor grids (slow) */

/* Option set 5 -- Testing */

bool testing_stack;			/* Test the stacking code */

bool testing_carry;			/* Test the carrying code */


/* Cheating options */

bool cheat_peek;		/* Peek into object creation */
bool cheat_hear;		/* Peek into monster creation */
bool cheat_room;		/* Peek into dungeon creation */
bool cheat_xtra;		/* Peek into something else */
bool cheat_know;		/* Know complete monster info */
bool cheat_live;		/* Allow player to avoid death */


/* Special options */

byte hitpoint_warn;		/* Hitpoint warning (0 to 9) */

byte delay_factor;		/* Delay factor (0 to 9) */

byte autosave_l;        /* Autosave before entering new levels */
byte autosave_t;        /* Timed autosave */
s16b autosave_freq;     /* Autosave frequency */


/*
 * Dungeon variables
 */

byte feeling;			/* Most recent feeling */
s16b rating;			/* Level's current rating */

bool good_item_flag;		/* True if "Artifact" on this level */

bool closing_flag;		/* Dungeon is closing */

bool fake_monochrome;	/* Use fake monochrome for effects */


/*
 * Dungeon size info
 */

s16b max_panel_rows, max_panel_cols;
s16b panel_row_min, panel_row_max;
s16b panel_col_min, panel_col_max;
s16b panel_col_prt, panel_row_prt;

byte *mp_a = NULL;
char *mp_c = NULL;
	
#ifdef USE_TRANSPARENCY
byte *mp_ta = NULL;
char *mp_tc = NULL;
#endif /* USE_TRANSPARENCY */


/*
 * Player location in dungeon
 */
s16b py;
s16b px;

/*
 * Targeting variables
 */
s16b target_who;
s16b target_col;
s16b target_row;


/*
 * User info
 */
int player_uid;
int player_euid;
int player_egid;

/*
 * Current player's character name
 */
char player_name[32];

/*
 * Stripped version of "player_name"
 */
char player_base[32];

/*
 * What killed the player
 */
char died_from[80];

/*
 * Hack -- Textual "history" for the Player
 */
char history[4][60];

/*
 * Buffer to hold the current savefile name
 */
char savefile[1024];



/*
 * Array of grids viewable to the player (see "cave.c")
 */
s16b view_n;
s16b view_y[VIEW_MAX];
s16b view_x[VIEW_MAX];

/*
 * Array of grids for use by various functions (see "cave.c")
 */
s16b temp_n;
s16b temp_y[TEMP_MAX];
s16b temp_x[TEMP_MAX];


/*
 * Array of grids for use in monster lighting effects (see "cave.c")
 */
s16b lite_n = 0;
s16b lite_y[LITE_MAX];
s16b lite_x[LITE_MAX];


/*
 * Number of active macros.
 */
s16b macro__num;

/*
 * Array of macro patterns [MACRO_MAX]
 */
cptr *macro__pat;

/*
 * Array of macro actions [MACRO_MAX]
 */
cptr *macro__act;

/*
 * Array of macro types [MACRO_MAX]
 */
bool *macro__cmd;

/*
 * Current macro action [1024]
 */
char *macro__buf;


/*
 * The number of quarks
 */
s16b quark__num;

/*
 * The pointers to the quarks [QUARK_MAX]
 */
cptr *quark__str;


/*
 * The next "free" index to use
 */
u16b message__next;

/*
 * The index of the oldest message (none yet)
 */
u16b message__last;

/*
 * The next "free" offset
 */
u16b message__head;

/*
 * The offset to the oldest used char (none yet)
 */
u16b message__tail;

/*
 * The array of offsets, by index [MESSAGE_MAX]
 */
u16b *message__ptr;

/*
 * The array of chars, by offset [MESSAGE_BUF]
 */
char *message__buf;

/*
 * The array[MESSAGE_MAX] of bytes for the colors of messages
 */
byte *message__color;



/*
 * The array of normal options
 */
u32b option_flag[8];
u32b option_mask[8];


/*
 * The array of window options
 */
u32b window_flag[8];
u32b window_mask[8];


/*
 * The array of window pointers
 */
term *angband_term[8];


/*
 * Standard window names
 */
char angband_term_name[8][16] =
{
	"Zangband",
	"Term-1",
	"Term-2",
	"Term-3",
	"Term-4",
	"Term-5",
	"Term-6",
	"Term-7"
};


/*
 * Global table of color definitions
 */
byte angband_color_table[256][4] =
{
	{0x00, 0x00, 0x00, 0x00},	/* TERM_DARK */
	{0x00, 0xFF, 0xFF, 0xFF},	/* TERM_WHITE */
	{0x00, 0x80, 0x80, 0x80},	/* TERM_SLATE */
	{0x00, 0xFF, 0x80, 0x00},	/* TERM_ORANGE */
	{0x00, 0xC0, 0x00, 0x00},	/* TERM_RED */
	{0x00, 0x00, 0x80, 0x40},	/* TERM_GREEN */
	{0x00, 0x00, 0x00, 0xFF},	/* TERM_BLUE */
	{0x00, 0x80, 0x40, 0x00},	/* TERM_UMBER */
	{0x00, 0x40, 0x40, 0x40},	/* TERM_L_DARK */
	{0x00, 0xC0, 0xC0, 0xC0},	/* TERM_L_WHITE */
	{0x00, 0xFF, 0x00, 0xFF},	/* TERM_VIOLET */
	{0x00, 0xFF, 0xFF, 0x00},	/* TERM_YELLOW */
	{0x00, 0xFF, 0x00, 0x00},	/* TERM_L_RED */
	{0x00, 0x00, 0xFF, 0x00},	/* TERM_L_GREEN */
	{0x00, 0x00, 0xFF, 0xFF},	/* TERM_L_BLUE */
	{0x00, 0xC0, 0x80, 0x40}	/* TERM_L_UMBER */
};


/*
 * Standard sound names
 */
char angband_sound_name[SOUND_MAX][16] =
{
	"",
	"hit",
	"miss",
	"flee",
	"drop",
	"kill",
	"level",
	"death",
	"study",
	"teleport",
	"shoot",
	"quaff",
	"zap",
	"walk",
	"tpother",
	"hitwall",
	"eat",
	"store1",
	"store2",
	"store3",
	"store4",
	"dig",
	"opendoor",
	"shutdoor",
	"tplevel",
	"scroll",
	"buy",
	"sell",
	"warn",
	"rocket",
	"n_kill",
	"u_kill",
	"quest",
	"heal",
	"x_heal",
	"bite",
	"claw",
	"m_spell",
	"summon",
	"breath",
	"ball",
	"m_heal",
	"atkspell",
	"evil",
	"touch",
	"sting",
	"crush",
	"slime",
	"wail",
	"winner",
	"fire",
	"acid",
	"elec",
	"cold",
	"illegal",
	"fail",
	"wakeup",
	"invuln",
	"fall",
	"pain",
	"destitem",
	"moan",
	"show",
	"unused",
	"explode",
};


/*
 * The array of "cave grids" [MAX_WID][MAX_HGT].
 * Not completely allocated, that would be inefficient
 * Not completely hardcoded, that would overflow memory
 */
cave_type *cave[MAX_HGT];

/*
 * The function pointer that is used to access the dungeon / wilderness.
 * It points to a simple function when in the dungeon, that evaluates
 * cave[y][x]
 * In the wilderness, things are more complicated.
 */

cave_type *(*area)(int, int);

/*
 * Variables used to access the scrollable wilderness.
 * This is designed to be as fast as possible - whilst using as little
 * RAM as possible to store a massive wilderness.
 *
 * The wilderness is generated "on the fly" as the player moves around it.
 * To save time - blocks of 16x16 squares are saved in a cache so they
 * don't need to be redone if the player moves back and forth.
 */

/* wilderness block - array of 16x16 cave grids. */
/* cave_type *block[WILD_BLOCK_SIZE]; */

/* block used to generate plasma fractal for random wilderness */
u16b *temp_block[WILD_BLOCK_SIZE+1];

/* cache of blocks near the player */
cave_type **wild_cache[WILD_BLOCKS];

/* grid of blocks around the player */
wild_grid_type wild_grid;

/* The wilderness itself */
wild_type **wild;

/* Description of wilderness block types */
wild_gen_data_type *wild_gen_data;

/* The decision tree for working out what block type to pick */
wild_choice_tree_type *wild_choice_tree;

byte *wild_temp_dist;

/*
 * The array of dungeon items [max_o_idx]
 */
object_type *o_list;

/*
 * The array of dungeon monsters [max_m_idx]
 */
monster_type *m_list;

/*
 * The array of fields [max_fld_idx]
 */
field_type *fld_list;



/*
 * Maximum number of towns
 */
u16b max_towns;

/*
 * Number of towns used.
 */
u16b town_count;

/*
 * The towns [max_towns]
 */
town_type *town;


/*
 * The player's inventory [INVEN_TOTAL]
 */
object_type *inventory;


/*
 * The size of "alloc_kind_table" (at most max_k_idx * 4)
 */
s16b alloc_kind_size;

/*
 * The entries in the "kind allocator table"
 */
alloc_entry *alloc_kind_table;


/*
 * The size of "alloc_race_table" (at most max_r_idx)
 */
s16b alloc_race_size;

/*
 * The entries in the "race allocator table"
 */
alloc_entry *alloc_race_table;


/*
 * Specify attr/char pairs for visual special effects
 * Be sure to use "index & 0x7F" to avoid illegal access
 */
byte misc_to_attr[256];
char misc_to_char[256];


/*
 * Specify attr/char pairs for inventory items (by tval)
 * Be sure to use "index & 0x7F" to avoid illegal access
 */
byte tval_to_attr[128];
char tval_to_char[128];


/*
 * Keymaps for each "mode" associated with each keypress.
 */
cptr keymap_act[KEYMAP_MODES][256];



/*** Player information ***/

/*
 * Static player info record
 */
player_type p_body;

/*
 * Pointer to the player info
 */
player_type *p_ptr = &p_body;

/*
 * Pointer to the player tables
 * (sex, race, class, magic)
 */
player_sex *sp_ptr;
player_race *rp_ptr;
player_class *cp_ptr;
player_magic *mp_ptr;


/*
 * More spell info
 */
u32b spell_learned1;	/* bit mask of spells learned */
u32b spell_learned2;	/* bit mask of spells learned */
u32b spell_worked1;	/* bit mask of spells tried and worked */
u32b spell_worked2;	/* bit mask of spells tried and worked */
u32b spell_forgotten1;	/* bit mask of spells learned but forgotten */
u32b spell_forgotten2;	/* bit mask of spells learned but forgotten */
byte spell_order[64];	/* order spells learned/remembered/forgotten */


/*
 * Calculated base hp values for player at each level,
 * store them so that drain life + restore life does not
 * affect hit points.  Also prevents shameless use of backup
 * savefiles for hitpoint acquirement.
 */
s16b player_hp[PY_MAX_LEVEL];


/*
 * The vault generation arrays
 */
header *v_head;
vault_type *v_info;
char *v_name;
char *v_text;

/*
 * The terrain feature arrays
 */
header *f_head;
feature_type *f_info;
char *f_name;
char *f_text;

/*
 * The object kind arrays
 */
header *k_head;
object_kind *k_info;
char *k_name;
char *k_text;

/*
 * The artifact arrays
 */
header *a_head;
artifact_type *a_info;
char *a_name;
char *a_text;

/*
 * The ego-item arrays
 */
header *e_head;
ego_item_type *e_info;
char *e_name;
char *e_text;


/*
 * The monster race arrays
 */
header *r_head;
monster_race *r_info;
char *r_name;
char *r_text;

/*
 * The field thaumatergical array
 */
field_thaum *t_info;

/*
 * Hack -- The special Angband "System Suffix"
 * This variable is used to choose an appropriate "pref-xxx" file
 */
cptr ANGBAND_SYS = "xxx";

/*
 * Hack -- The special Angband "Graphics Suffix"
 * This variable is used to choose an appropriate "graf-xxx" file
 */
cptr ANGBAND_GRAF = "old";

/*
 * Path name: The main "lib" directory
 * This variable is not actually used anywhere in the code
 */
cptr ANGBAND_DIR;

/*
 * High score files (binary)
 * These files may be portable between platforms
 */
cptr ANGBAND_DIR_APEX;

/*
 * Bone files for player ghosts (ascii)
 * These files are portable between platforms
 */
cptr ANGBAND_DIR_BONE;

/*
 * Binary image files for the "*_info" arrays (binary)
 * These files are not portable between platforms
 */
cptr ANGBAND_DIR_DATA;

/*
 * Textual template files for the "*_info" arrays (ascii)
 * These files are portable between platforms
 */
cptr ANGBAND_DIR_EDIT;

/*
 * Script files
 * These files are portable between platforms.
 */
cptr ANGBAND_DIR_SCRIPT;

/*
 * Various extra files (ascii)
 * These files may be portable between platforms
 */
cptr ANGBAND_DIR_FILE;

/*
 * Help files (normal) for the online help (ascii)
 * These files are portable between platforms
 */
cptr ANGBAND_DIR_HELP;

/*
 * Help files (spoilers) for the online help (ascii)
 * These files are portable between platforms
 */
cptr ANGBAND_DIR_INFO;

/*
 * Savefiles for current characters (binary)
 * These files are portable between platforms
 */
cptr ANGBAND_DIR_SAVE;

/*
 * User "preference" files (ascii)
 * These files are rarely portable between platforms
 */
cptr ANGBAND_DIR_USER;

/*
 * Various extra files (binary)
 * These files are rarely portable between platforms
 */
cptr ANGBAND_DIR_XTRA;


/*
 * Total Hack -- allow all items to be listed (even empty ones)
 * This is only used by "do_cmd_inven_e()" and is cleared there.
 */
bool item_tester_full;


/*
 * Here is a "pseudo-hook" used during calls to "get_item()" and
 * "show_inven()" and "show_equip()", and the choice window routines.
 */
byte item_tester_tval;


/*
 * Here is a "hook" used during calls to "get_item()" and
 * "show_inven()" and "show_equip()", and the choice window routines.
 */
bool (*item_tester_hook)(object_type*);



/*
 * Current "comp" function for ang_sort()
 */
bool (*ang_sort_comp)(vptr u, vptr v, int a, int b);


/*
 * Current "swap" function for ang_sort()
 */
void (*ang_sort_swap)(vptr u, vptr v, int a, int b);



/*
 * Hack -- function hooks to restrict "get_mon_num_prep()" function
 */
monster_hook_type get_mon_num_hook;
monster_hook_type get_mon_num2_hook;


/*
 * Hack -- function hook to restrict "get_obj_num_prep()" function
 */
bool (*get_obj_num_hook)(int k_idx);


/* Hack, monk armour */
bool monk_armour_aux;
bool monk_notify_aux;


/* Easy patch flags */
bool easy_open;
bool easy_disarm;
bool easy_floor;

bool use_command;
bool center_player;
bool avoid_center;

/* Auto-destruction options */
bool destroy_worthless;

/* Monster lighting effects */
bool monster_light;

/*
 * Buildings
 */
building_type building[MAX_BLDG];


/*
 * Maximum number of quests
 */
u16b max_quests;

/*
 * Maximum number of monsters in r_info.txt
 */
u16b max_r_idx;

/*
 * Maximum number of items in k_info.txt
 */
u16b max_k_idx;

/*
 * Maximum number of vaults in v_info.txt
 */
u16b max_v_idx;

/*
 * Maximum number of terrain features in f_info.txt
 */
u16b max_f_idx;

/*
 * Maximum number of artifacts in a_info.txt
 */
u16b max_a_idx;

/*
 * Maximum number of ego-items in e_info.txt
 */
u16b max_e_idx;

/*
 * Maximum number of objects in the level
 */
u16b max_o_idx;

/*
 * Maximum number of monsters in the level
 */
u16b max_m_idx;

/*
 * Maximum number of fields on the level
 */
u16b max_fld_idx;

/*
 * Maximum number of field types
 */
u16b max_t_idx;


/*
 * Maximum size of the wilderness
 */
s32b max_wild_size;

/*
 * Current size of the wilderness
 */
s32b max_wild;

/*
 * Maximum number of nodes in the wilderness decision tree
 */
u16b max_w_node;

/*
 * Maximum number of types of wilderness block.
 */
u16b max_w_block;

/*
 * Quest info
 */
quest_type *quest;

/*
 * Quest text
 */
char quest_text[10][80];

/*
 * Current line of the quest text
 */
int quest_text_line;

/*
 * Default spell color table (quark index)
 */
s16b gf_color[MAX_GF];

/*
 * Flags for initialization
 */
int init_flags;


/*
 * The "highscore" file descriptor, if available.
 */
int highscore_fd = -1;

/*
 * Should the monster allocation fail with inappropriate terrain?
 */
bool monster_terrain_sensitive = TRUE;

int mutant_regenerate_mod = 100;

/*
 * Startup options
 */
bool vanilla_town;            /* Use "vanilla" town without set quests */
bool ironman_shops;           /* Stores are permanently closed */
bool ironman_small_levels;    /* Always create unusually small dungeon levels */
bool ironman_downward;        /* Don't allow climbing upwards/recalling */
bool ironman_autoscum;        /* Permanently enable the autoscummer */
bool ironman_hard_quests;     /* Quest monsters get reinforcements */
bool ironman_empty_levels;    /* Always create empty 'arena' levels */
bool terrain_streams;         /* Create terrain 'streamers' in the dungeon */
bool munchkin_death;          /* Ask for saving death */
bool ironman_rooms;           /* Always generate very unusual rooms */
bool ironman_nightmare;			/* Play the game in Nightmare mode */
bool maximize_mode;
bool preserve_mode;
bool autoroller;


bool use_transparency = FALSE; /* Use transparent tiles */

bool can_save = TRUE;         /* Game can be saved */

