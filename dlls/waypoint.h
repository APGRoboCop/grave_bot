//
// HPB_bot - botman's High Ping Bastard bot
//
// (http://planethalflife.com/botman/)
//
// waypoint.h
//

#ifndef WAYPOINT_H
#define WAYPOINT_H

#include <climits>

#define MAX_WAYPOINTS 1024

#define REACHABLE_RANGE 400.0f

// defines for waypoint flags field (32 bits are available)
#define W_FL_TEAM        ((1<<0) + (1<<1))  /* allow for 4 teams (0-3) */
#define W_FL_TEAM_SPECIFIC (1<<2)  /* waypoint only for specified team */
#define W_FL_CROUCH      (1<<3)  /* must crouch to reach this waypoint */
#define W_FL_LADDER      (1<<4)  /* waypoint on a ladder */
#define W_FL_LIFT        (1<<5)  /* wait for lift to be down before approaching this waypoint */
#define W_FL_DOOR        (1<<6)  /* wait for door to open */
#define W_FL_HEALTH      (1<<7)  /* health kit (or wall mounted) location */
#define W_FL_ARMOR       (1<<8)  /* armor (or HEV) location */
#define W_FL_AMMO        (1<<9)  /* ammo location */
#define W_FL_SNIPER      (1<<10) /* sniper waypoint (a good sniper spot) */

#define W_FL_FLAG   (1<<11) /* scientist */

#define W_FL_FLAG_GOAL (1<<12) /* administrator */

#define W_FL_PRONE       (1<<13) /* go prone (laying down) */
#define W_FL_AIMING      (1<<14) /* aiming waypoint */

#define W_FL_WEAPON      (1<<17) /* weapon_ entity location */
#define W_FL_JUMP        (1<<18) /* jump waypoint */

#define W_FL_ITEM        (1<<19) /* some other item */

#define W_FL_DUCKJUMP        (1<<20) /* jump-duck waypoint */

#define W_FL_DEFEND        (1<<21) /* bot will patrol this waypoint */

#define W_FL_DELETED     (1<<31) /* used by waypoint allocation code */


#define WAYPOINT_VERSION 4

// define the waypoint file header structure...
typedef struct {
   char filetype[8];  // should be "HPB_bot\0"
   int  waypoint_file_version;
   int  waypoint_file_flags;  // not currently used
   int  number_of_waypoints;
   char mapname[32];  // name of map for these waypoints
} WAYPOINT_HDR;


// define the structure for waypoints...
typedef struct {
   int    flags;    // button, lift, flag, health, ammo, etc.
   Vector origin;   // location
} OLD_WAYPOINT;

// define the structure for waypoints...
typedef struct {
   int    flags;    // button, lift, flag, health, ammo, etc.
   Vector origin;   // location
   char   item[19]; // item_healthkit, weapon_9mmAR, etc. (19 is weapon_handgrenade\0 and func_healthcharger\0)
} WAYPOINT;



#define WAYPOINT_UNREACHABLE   USHRT_MAX
#define WAYPOINT_MAX_DISTANCE (USHRT_MAX-1)

#define MAX_PATH_INDEX 4

// define the structure for waypoint paths (paths are connections between
// two waypoint nodes that indicates the bot can get from point A to point B.
// note that paths DON'T have to be two-way.  sometimes they are just one-way
// connections between two points.  There is an array called "paths" that
// contains head pointers to these structures for each waypoint index.
typedef struct path {
   short int index[MAX_PATH_INDEX];  // indexes of waypoints (index -1 means not used)
   struct path *next;   // link to next structure
} PATH;


// waypoint function prototypes...
void WaypointInit();
int  WaypointFindPath(PATH **pPath, int *path_index, int waypoint_index, int team);
int  WaypointFindNearest(edict_t *pEntity, float distance, int team);
int  WaypointFindNearest(Vector v_src, edict_t *pEntity, float range, int team);
int  WaypointFindNearest(edict_t *pEntity, float distance, int team, int exclude[]);
int  WaypointFindNearest(Vector v_src, edict_t *pEntity, float range, int team, int exclude[]);
int  WaypointFindNearestGoal(edict_t *pEntity, int src, int team, int flags);
int  WaypointFindNearestGoal(edict_t *pEntity, int src, int team, int flags, int exclude[]);
int  WaypointFindNearestGoal(edict_t *pEntity, int src, int team, int flags, char item[]);
int  WaypointFindNearestGoal(Vector v_src, edict_t *pEntity, float range, int team, int flags);
int  WaypointFindNearestGoal(Vector v_src, edict_t *pEntity, float range, int team, int flags, int exclude[]);
int  WaypointFindRandomGoal(edict_t *pEntity, int team, int flags);
int  WaypointFindRandomGoal(edict_t *pEntity, int team, int flags, int exclude[]);
int  WaypointFindRandomGoal(edict_t *pEntity, int team, int flags, char item[]);
int  WaypointFindRandomGoal(Vector v_src, edict_t *pEntity, float range, int team, int flags);
int  WaypointFindNearestWeapon(edict_t *pEntity, int src, int team, int flags, int weapon);
int  WaypointFindNearestAiming(Vector v_origin);
void WaypointDrawBeam(edict_t *pEntity, Vector start, Vector end, int width, int noise, int red, int green, int blue, int brightness, int speed);
void WaypointSearchItems(edict_t *pEntity, Vector origin, int wpt_index);
edict_t *WaypointFindItem( int wpt_index );
void WaypointAdd(edict_t *pEntity);
void WaypointAddAiming(edict_t *pEntity);
void WaypointDelete(edict_t *pEntity);
void WaypointUpdate(edict_t *pEntity);
void WaypointCreatePath(edict_t *pEntity, int cmd);
void WaypointRemovePath(edict_t *pEntity, int cmd);
bool WaypointLoad(edict_t *pEntity, char *ext);
void WaypointSave();
bool WaypointReachable(Vector v_srv, Vector v_dest, edict_t *pEntity);
int  WaypointFindReachable(edict_t *pEntity, float range, int team);
void WaypointPrintInfo(edict_t *pEntity);
void WaypointThink(edict_t *pEntity);
void WaypointFloyds(short *shortest_path, short *from_to);
void WaypointRouteInit();
unsigned short WaypointRouteFromTo(int src, int dest, int team);
int  WaypointDistanceFromTo(int src, int dest, int team);

#endif // WAYPOINT_H
