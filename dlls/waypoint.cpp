// HPB bot - botman's High Ping Bastard bot
//
// (http://planethalflife.com/botman/)
//
// waypoint.cpp
//

#ifndef __linux__
#include <io.h>
#endif

#include <fcntl.h>

#ifndef __linux__
#include <sys\stat.h>
#else
#include <string.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <sstream>
#endif

#ifdef _WIN32
#define access _access
#define open _open
#define close _close
#define unlink _unlink
#endif

#ifndef METAMOD_BUILD
   #include "extdll.h"
   #include "enginecallback.h"
   #include "util.h"
   #include "cbase.h"
   #include "entity_state.h"
#else
   #include <extdll.h>
   #include <dllapi.h>
   #include <h_export.h>
   #include <meta_api.h>
#endif

#include "bot.h"
#include "bot_weapons.h"
#include "waypoint.h"


extern int mod_id;
extern int m_spriteTexture;

// backward compatible
OLD_WAYPOINT old_waypoint;
// waypoints with information bits (flags)
WAYPOINT waypoints[MAX_WAYPOINTS];

// number of waypoints currently in use
int num_waypoints;
// number of waypoints that are defend waypoints
int num_def_waypoints;

// declare the array of head pointers to the path structures...
PATH *paths[MAX_WAYPOINTS];

// time that this waypoint was displayed (while editing)
float wp_display_time[MAX_WAYPOINTS];

bool g_waypoint_paths = FALSE;  // have any paths been allocated?
bool g_waypoint_on = FALSE;
bool g_auto_waypoint = FALSE;
bool g_path_waypoint = FALSE;
Vector last_waypoint;
float f_path_time = 0.0;

unsigned int route_num_waypoints;
unsigned short *shortest_path[4] = {nullptr, nullptr, nullptr, nullptr};
unsigned short *from_to[4] = {nullptr, nullptr, nullptr, nullptr};

static FILE *fp;


void WaypointDebug()
{
	int y = 1, x = 1;
	
	fp=fopen("bot.txt","a");
	fprintf(fp,"WaypointDebug: LINKED LIST ERROR!!!\n");
	fclose(fp);
	
	x = x - 1;  // x is zero
	y = y / x;  // cause an divide by zero exception
	
	return;
}


// free the linked list of waypoint path nodes...
void WaypointFree()
{
	for (int i=0; i < MAX_WAYPOINTS; i++)
	{
		int count = 0;
		
		if (paths[i])
		{
			PATH *p = paths[i];
			PATH *p_next;
			
			while (p)  // free the linked list
			{
				p_next = p->next;  // save the link to next
				free(p);
				p = p_next;
				
#ifdef _DEBUG
				count++;
				if (count > 1000) WaypointDebug();
#endif
			}
			
			paths[i] = nullptr;
		}
	}
}


// initialize the waypoint structures...
void WaypointInit()
{
	int i;
	
	// have any waypoint path nodes been allocated yet?
	if (g_waypoint_paths)
		WaypointFree();  // must free previously allocated path memory
	
	for (i=0; i < 4; i++)
	{
		if (shortest_path[i] != nullptr)
			free(shortest_path[i]);
		
		if (from_to[i] != nullptr)
			free(from_to[i]);
	}
	
	for (i=0; i < MAX_WAYPOINTS; i++)
	{
		waypoints[i].flags = 0;
		waypoints[i].origin = Vector(0,0,0);
		
		wp_display_time[i] = 0.0;
		
		paths[i] = nullptr;  // no paths allocated yet
	}
	
	f_path_time = 0.0;  // reset waypoint path display time
	
	num_waypoints = 0;
	
	last_waypoint = Vector(0,0,0);
	
	for (i=0; i < 4; i++)
	{
		shortest_path[i] = nullptr;
		from_to[i] = nullptr;
	}
}


// add a path from one waypoint (add_index) to another (path_index)...
void WaypointAddPath(short int add_index, short int path_index)
{
	PATH *p, *prev;
	int i;
	int count = 0;
	
	p = paths[add_index];
	prev = nullptr;
	
	// find an empty slot for new path_index...
	while (p != nullptr)
	{
		i = 0;
		
		while (i < MAX_PATH_INDEX)
		{
			if (p->index[i] == -1)
			{
				p->index[i] = path_index;
				
				return;
			}
			
			i++;
		}
		
		prev = p;     // save the previous node in linked list
		p = p->next;  // go to next node in linked list
		
#ifdef _DEBUG
		count++;
		if (count > 100) WaypointDebug();
#endif
	}
	
	p = static_cast<PATH *>(malloc(sizeof(PATH)));
	
	if (p == nullptr)
	{
		ALERT(at_error, "Grave Bot - Error allocating memory for path!");
	}
	
	p->index[0] = path_index;
	p->index[1] = -1;
	p->index[2] = -1;
	p->index[3] = -1;
	p->next = nullptr;
	
	if (prev != nullptr)
		prev->next = p;  // link new node into existing list
	
	if (paths[add_index] == nullptr)
		paths[add_index] = p;  // save head point if necessary
}


// delete all paths to this waypoint index...
void WaypointDeletePath(short int del_index)
{
	PATH *p;
	int index, i;
	
	// search all paths for this index...
	for (index=0; index < num_waypoints; index++)
	{
		p = paths[index];
		
		int count = 0;
		
		// search linked list for del_index...
		while (p != nullptr)
		{
			i = 0;
			
			while (i < MAX_PATH_INDEX)
			{
				if (p->index[i] == del_index)
				{
					p->index[i] = -1;  // unassign this path
				}
				
				i++;
			}
			
			p = p->next;  // go to next node in linked list
			
#ifdef _DEBUG
			count++;
			if (count > 100) WaypointDebug();
#endif
		}
	}
}


// delete a path from a waypoint (path_index) to another waypoint
// (del_index)...
void WaypointDeletePath(short int path_index, short int del_index)
{
	PATH *p;
	int i;
	int count = 0;
	
	p = paths[path_index];
	
	// search linked list for del_index...
	while (p != nullptr)
	{
		i = 0;
		
		while (i < MAX_PATH_INDEX)
		{
			if (p->index[i] == del_index)
			{
				p->index[i] = -1;  // unassign this path
			}
			
			i++;
		}
		
		p = p->next;  // go to next node in linked list
		
#ifdef _DEBUG
		count++;
		if (count > 100) WaypointDebug();
#endif
	}
}


// find a path from the current waypoint. (pPath MUST be NULL on the
// initial call. subsequent calls will return other paths if they exist.)
int WaypointFindPath(PATH **pPath, int *path_index, int waypoint_index, int team)
{
	int index;
	int count = 0;
	
	if (*pPath == nullptr)
	{
		*pPath = paths[waypoint_index];
		*path_index = 0;
	}
	
	if (*path_index == MAX_PATH_INDEX)
	{
		*path_index = 0;
		
		*pPath = (*pPath)->next;  // go to next node in linked list
	}
	
	while (*pPath != nullptr)
	{
		while (*path_index < MAX_PATH_INDEX)
		{
			if ((*pPath)->index[*path_index] != -1)  // found a path?
			{
				// save the return value
				index = (*pPath)->index[*path_index];
				
				// skip this path if next waypoint is team specific and NOT this team
				if ((team != -1) && (waypoints[index].flags & W_FL_TEAM_SPECIFIC) &&
					((waypoints[index].flags & W_FL_TEAM) != team))
				{
					(*path_index)++;
					continue;
				}
				
				// set up stuff for subsequent calls...
				(*path_index)++;
				
				return index;
			}
			
			(*path_index)++;
		}
		
		*path_index = 0;
		
		*pPath = (*pPath)->next;  // go to next node in linked list
		
#ifdef _DEBUG
		count++;
		if (count > 100) WaypointDebug();
#endif
	}
	
	return -1;
}


// find the nearest waypoint to the player and return the index
// (-1 if not found)...
int WaypointFindNearest(edict_t *pEntity, float range, int team)
{
	int i, min_index;
	float distance;
	float min_distance;
	TraceResult tr;
	Vector vecOrigin;

	if (num_waypoints < 1 || pEntity == nullptr)
		return -1;
	
	vecOrigin = UTIL_GetOrigin(pEntity);

	// find the nearest waypoint...
	
	min_index = -1;
	min_distance = 9999.0;
	
	for (i=0; i < num_waypoints; i++)
	{
		if (waypoints[i].flags & W_FL_DELETED)
			continue;  // skip any deleted waypoints
		
		if (waypoints[i].flags & W_FL_AIMING)
			continue;  // skip any aiming waypoints
		
		// skip this waypoint if it's team specific and teams don't match...
		if ((team != -1) && (waypoints[i].flags & W_FL_TEAM_SPECIFIC) &&
			((waypoints[i].flags & W_FL_TEAM) != team))
			continue;
		
		distance = (waypoints[i].origin - vecOrigin).Length();
		
		if ((distance < min_distance) && (distance < range))
		{
			// if waypoint is visible from current position (even behind head)...
			UTIL_TraceLine( vecOrigin + pEntity->v.view_ofs, waypoints[i].origin,
				ignore_monsters, pEntity->v.pContainingEntity, &tr );
			
			if (tr.flFraction >= 1.0)
			{
				min_index = i;
				min_distance = distance;
			}
		}
	}
	
	return min_index;
}


// find the nearest waypoint to the source postition and return the index
// of that waypoint...
int WaypointFindNearest(Vector v_src, edict_t *pEntity, float range, int team)
{
	int index, min_index;
	float distance;
	float min_distance;
	TraceResult tr;
	
	if (num_waypoints < 1)
		return -1;
	
	// find the nearest waypoint...
	
	min_index = -1;
	min_distance = 9999.0;
	
	for (index=0; index < num_waypoints; index++)
	{
		if (waypoints[index].flags & W_FL_DELETED)
			continue;  // skip any deleted waypoints
		
		if (waypoints[index].flags & W_FL_AIMING)
			continue;  // skip any aiming waypoints
		
		// skip this waypoint if it's team specific and teams don't match...
		if ((team != -1) && (waypoints[index].flags & W_FL_TEAM_SPECIFIC) &&
			((waypoints[index].flags & W_FL_TEAM) != team))
			continue;
		
		distance = (waypoints[index].origin - v_src).Length();
		
		if ((distance < min_distance) && (distance < range))
		{
			// if waypoint is visible from source position...
			UTIL_TraceLine( v_src, waypoints[index].origin, ignore_monsters,
				pEntity->v.pContainingEntity, &tr );
			
			if (tr.flFraction >= 1.0)
			{
				min_index = index;
				min_distance = distance;
			}
		}
	}
	
	return min_index;
}


// find the nearest waypoint to the player and return the index
// (-1 if not found)...
int WaypointFindNearest(edict_t *pEntity, float range, int team, int exclude[])
{
	int i, min_index, exclude_index;
	float distance;
	float min_distance;
	TraceResult tr;
	Vector vecOrigin;

	if (num_waypoints < 1 || pEntity == nullptr)
		return -1;
	
	vecOrigin = UTIL_GetOrigin(pEntity);

	// find the nearest waypoint...
	
	min_index = -1;
	min_distance = 9999.0;
	
	for (i=0; i < num_waypoints; i++)
	{
		if (waypoints[i].flags & W_FL_DELETED)
			continue;  // skip any deleted waypoints
		
		if (waypoints[i].flags & W_FL_AIMING)
			continue;  // skip any aiming waypoints
		
		// skip this waypoint if it's team specific and teams don't match...
		if ((team != -1) && (waypoints[i].flags & W_FL_TEAM_SPECIFIC) &&
			((waypoints[i].flags & W_FL_TEAM) != team))
			continue;
		
		exclude_index = 0;
		while (exclude[exclude_index])
		{
			if (i == exclude[exclude_index])
				break;  // found a match, break out of while loop
			
			exclude_index++;
		}
		
		if (i == exclude[exclude_index])
			continue;  // skip any index that matches exclude list

		distance = (waypoints[i].origin - vecOrigin).Length();
		
		if ((distance < min_distance) && (distance < range))
		{
			// if waypoint is visible from current position (even behind head)...
			UTIL_TraceLine( vecOrigin + pEntity->v.view_ofs, waypoints[i].origin,
				ignore_monsters, pEntity->v.pContainingEntity, &tr );
			
			if (tr.flFraction >= 1.0)
			{
				min_index = i;
				min_distance = distance;
			}
		}
	}
	
	return min_index;
}


// find the nearest waypoint to the source postition and return the index
// of that waypoint...
int WaypointFindNearest(Vector v_src, edict_t *pEntity, float range, int team, int exclude[])
{
	int index, min_index, exclude_index;
	float distance;
	float min_distance;
	TraceResult tr;
	
	if (num_waypoints < 1)
		return -1;
	
	// find the nearest waypoint...
	
	min_index = -1;
	min_distance = 9999.0;
	
	for (index=0; index < num_waypoints; index++)
	{
		if (waypoints[index].flags & W_FL_DELETED)
			continue;  // skip any deleted waypoints
		
		if (waypoints[index].flags & W_FL_AIMING)
			continue;  // skip any aiming waypoints
		
		// skip this waypoint if it's team specific and teams don't match...
		if ((team != -1) && (waypoints[index].flags & W_FL_TEAM_SPECIFIC) &&
			((waypoints[index].flags & W_FL_TEAM) != team))
			continue;
		
		exclude_index = 0;
		while (exclude[exclude_index])
		{
			if (index == exclude[exclude_index])
				break;  // found a match, break out of while loop
			
			exclude_index++;
		}
		
		if (index == exclude[exclude_index])
			continue;  // skip any index that matches exclude list

		distance = (waypoints[index].origin - v_src).Length();
		
		if ((distance < min_distance) && (distance < range))
		{
			// if waypoint is visible from source position...
			UTIL_TraceLine( v_src, waypoints[index].origin, ignore_monsters,
				pEntity->v.pContainingEntity, &tr );
			
			if (tr.flFraction >= 1.0)
			{
				min_index = index;
				min_distance = distance;
			}
		}
	}
	
	return min_index;
}

// find the goal nearest to the player matching the "flags" bits and return
// the index of that waypoint...
int WaypointFindNearestGoal(edict_t *pEntity, int src, int team, int flags)
{
	int index, min_index;
	int distance, min_distance;
	edict_t *wpt_item;
	Vector vecOrigin;

	if (num_waypoints < 1 || pEntity == nullptr)
		return -1;
	
	vecOrigin = UTIL_GetOrigin(pEntity);
	
	// find the nearest waypoint with the matching flags...
	
	min_index = -1;
	min_distance = 99999;
	
	for (index=0; index < num_waypoints; index++)
	{
		if (index == src)
			continue;  // skip the source waypoint
		
		if (waypoints[index].flags & W_FL_DELETED)
			continue;  // skip any deleted waypoints
		
		if (waypoints[index].flags & W_FL_AIMING)
			continue;  // skip any aiming waypoints
		
		// skip this waypoint if it's team specific and teams don't match...
		if ((team != -1) && (waypoints[index].flags & W_FL_TEAM_SPECIFIC) &&
			((waypoints[index].flags & W_FL_TEAM) != team))
			continue;
		
		if ((waypoints[index].flags & flags) != flags)
			continue;  // skip this waypoint if the flags don't match
		
		if (waypoints[index].item[0])
		{
			wpt_item = WaypointFindItem(index);	
			if ((wpt_item == nullptr) || (wpt_item->v.effects & EF_NODRAW) || (wpt_item->v.frame > 0) ||
				((team != -1) && (UTIL_GetTeam(wpt_item) != team)))
				continue;
		}

		distance = WaypointDistanceFromTo(src, index, team);
		
		if (distance < min_distance)
		{
			min_index = index;
			min_distance = distance;
		}
	}
	
	return min_index;
}

// find the goal nearest to the source position (v_src) matching the "flags"
// bits and return the index of that waypoint...
int WaypointFindNearestGoal(edict_t *pEntity, int src, int team, int flags, int exclude[])
{
	int index, min_index;
	int distance, min_distance;
	int exclude_index;
	edict_t *wpt_item;

	if (num_waypoints < 1)
		return -1;
	
	// find the nearest waypoint with the matching flags...
	
	min_index = -1;
	min_distance = 9999;
	
	for (index=0; index < num_waypoints; index++)
	{
		if (index == src)
			continue;  // skip the source waypoint
		
		if (waypoints[index].flags & W_FL_DELETED)
			continue;  // skip any deleted waypoints
		
		if (waypoints[index].flags & W_FL_AIMING)
			continue;  // skip any aiming waypoints
		
		// skip this waypoint if it's team specific and teams don't match...
		if ((team != -1) && (waypoints[index].flags & W_FL_TEAM_SPECIFIC) &&
			((waypoints[index].flags & W_FL_TEAM) != team))
			continue;
		
		if ((waypoints[index].flags & flags) != flags)
			continue;  // skip this waypoint if the flags don't match
		
		exclude_index = 0;
		while (exclude[exclude_index])
		{
			if (index == exclude[exclude_index])
				break;  // found a match, break out of while loop
			
			exclude_index++;
		}
		
		if (index == exclude[exclude_index])
			continue;  // skip any index that matches exclude list
	
		if (waypoints[index].item[0])
		{
			wpt_item = WaypointFindItem(index);	
			if ((wpt_item == nullptr) || (wpt_item->v.effects & EF_NODRAW) || (wpt_item->v.frame > 0) ||
				((team != -1) && (UTIL_GetTeam(wpt_item) != team)))
				continue;
		}

		distance = WaypointDistanceFromTo(src, index, team);
		
		if (distance < min_distance)
		{
			min_index = index;
			min_distance = distance;
		}
	}
	
	return min_index;
}


// find the goal nearest to the player matching the "flags" bits and return
// the index of that waypoint...
int WaypointFindNearestGoal(edict_t *pEntity, int src, int team, int flags, char item[])
{
	int index, min_index;
	int distance, min_distance;
	edict_t *wpt_item;
	
	if (num_waypoints < 1)
		return -1;
	
	// find the nearest waypoint with the matching flags...
	
	min_index = -1;
	min_distance = 99999;
	
	for (index=0; index < num_waypoints; index++)
	{
		if (index == src)
			continue;  // skip the source waypoint
		
		if (waypoints[index].flags & W_FL_DELETED)
			continue;  // skip any deleted waypoints
		
		if (waypoints[index].flags & W_FL_AIMING)
			continue;  // skip any aiming waypoints
		
		// skip this waypoint if it's team specific and teams don't match...
		if ((team != -1) && (waypoints[index].flags & W_FL_TEAM_SPECIFIC) &&
			((waypoints[index].flags & W_FL_TEAM) != team))
			continue;
		
		if ((waypoints[index].flags & flags) != flags)
			continue;  // skip this waypoint if the flags don't match
	
		// skip this waypoint if not the right item
		if (strcmp(waypoints[index].item, item) != 0)
			continue;

		if (waypoints[index].item[0])
		{
			wpt_item = WaypointFindItem(index);	
			if ((wpt_item == nullptr) || (wpt_item->v.effects & EF_NODRAW) || (wpt_item->v.frame > 0) ||
				((team != -1) && (UTIL_GetTeam(wpt_item) != team)))
				continue;
		}

		distance = WaypointDistanceFromTo(src, index, team);
		
		if (distance < min_distance)
		{
			min_index = index;
			min_distance = distance;
		}
	}
	
	return min_index;
}

// find the goal nearest to the source position (v_src) matching the "flags"
// bits and return the index of that waypoint...
int WaypointFindNearestGoal(Vector v_src, edict_t *pEntity, float range, int team, int flags)
{
	int index, min_index;
	int distance, min_distance;
	edict_t *wpt_item;
	
	if (num_waypoints < 1)
		return -1;
	
	// find the nearest waypoint with the matching flags...
	
	min_index = -1;
	min_distance = 99999;
	
	for (index=0; index < num_waypoints; index++)
	{
		if (waypoints[index].flags & W_FL_DELETED)
			continue;  // skip any deleted waypoints
		
		if (waypoints[index].flags & W_FL_AIMING)
			continue;  // skip any aiming waypoints
		
		// skip this waypoint if it's team specific and teams don't match...
		if ((team != -1) && (waypoints[index].flags & W_FL_TEAM_SPECIFIC) &&
			((waypoints[index].flags & W_FL_TEAM) != team))
			continue;
		
		if ((waypoints[index].flags & flags) != flags)
			continue;  // skip this waypoint if the flags don't match
		
		if (waypoints[index].item[0])
		{
			wpt_item = WaypointFindItem(index);	
			if ((wpt_item == nullptr) || (wpt_item->v.effects & EF_NODRAW) || (wpt_item->v.frame > 0) ||
				((team != -1) && (UTIL_GetTeam(wpt_item) != team)))
				continue;
		}

		distance = (waypoints[index].origin - v_src).Length();
		
		if ((distance < range) && (distance < min_distance))
		{
			min_index = index;
			min_distance = distance;
		}
	}
	
	return min_index;
}

// find the goal nearest to the source position (v_src) matching the "flags"
// bits and return the index of that waypoint...
int WaypointFindNearestGoal(Vector v_src, edict_t *pEntity, float range, int team, int flags, int exclude[])
{
	int index, min_index, exclude_index;
	int distance, min_distance;
	edict_t *wpt_item;
	
	if (num_waypoints < 1)
		return -1;
	
	// find the nearest waypoint with the matching flags...
	
	min_index = -1;
	min_distance = 99999;
	
	for (index=0; index < num_waypoints; index++)
	{
		if (waypoints[index].flags & W_FL_DELETED)
			continue;  // skip any deleted waypoints
		
		if (waypoints[index].flags & W_FL_AIMING)
			continue;  // skip any aiming waypoints
		
		// skip this waypoint if it's team specific and teams don't match...
		if ((team != -1) && (waypoints[index].flags & W_FL_TEAM_SPECIFIC) &&
			((waypoints[index].flags & W_FL_TEAM) != team))
			continue;
		
		if ((waypoints[index].flags & flags) != flags)
			continue;  // skip this waypoint if the flags don't match
		
		exclude_index = 0;
		while (exclude[exclude_index])
		{
			if (index == exclude[exclude_index])
				break;  // found a match, break out of while loop
			
			exclude_index++;
		}
		
		if (index == exclude[exclude_index])
			continue;  // skip any index that matches exclude list

		if (waypoints[index].item[0])
		{
			wpt_item = WaypointFindItem(index);	
			if ((wpt_item == nullptr) || (wpt_item->v.effects & EF_NODRAW) || (wpt_item->v.frame > 0) ||
				((team != -1) && (UTIL_GetTeam(wpt_item) != team)))
				continue;
		}

		distance = (waypoints[index].origin - v_src).Length();
		
		if ((distance < range) && (distance < min_distance))
		{
			min_index = index;
			min_distance = distance;
		}
	}
	
	return min_index;
}

// find a random goal matching the "flags" bits and return the index of
// that waypoint...
int WaypointFindRandomGoal(edict_t *pEntity, int team, int flags)
{
	int index;
	int indexes[50];
	int count = 0;
	edict_t *wpt_item;
	
	if (num_waypoints < 1)
		return -1;
	
	// find all the waypoints with the matching flags...
	
	for (index=0; index < num_waypoints; index++)
	{
		if (waypoints[index].flags & W_FL_DELETED)
			continue;  // skip any deleted waypoints
		
		if (waypoints[index].flags & W_FL_AIMING)
			continue;  // skip any aiming waypoints
		
		// skip this waypoint if it's team specific and teams don't match...
		if ((team != -1) && (waypoints[index].flags & W_FL_TEAM_SPECIFIC) &&
			((waypoints[index].flags & W_FL_TEAM) != team))
			continue;
		
		if ((waypoints[index].flags & flags) != flags)
			continue;  // skip this waypoint if the flags don't match
		
		if (waypoints[index].item[0])
		{
			wpt_item = WaypointFindItem(index);	
			if ((wpt_item == nullptr) || (wpt_item->v.effects & EF_NODRAW) || (wpt_item->v.frame > 0) ||
				((team != -1) && (UTIL_GetTeam(wpt_item) != team)))
				continue;
		}

		if (count < 50)
		{
			indexes[count] = index;
			
			count++;
		}
	}
	
	if (count == 0)  // no matching waypoints found
		return -1;
	
	index = RANDOM_LONG(1, count) - 1;
	
	return indexes[index];
}

// find a random goal within a range of a position (v_src) matching the
// "flags" bits and return the index of that waypoint...
int WaypointFindRandomGoal(edict_t *pEntity, int team, int flags, int exclude[])
{
	int index;
	int indexes[200];
	int count = 0;
	int exclude_index;
	edict_t *wpt_item;
	
	if (num_waypoints < 1)
		return -1;
	
	// find all the waypoints with the matching flags...
	
	for (index=0; index < num_waypoints; index++)
	{
		if (waypoints[index].flags & W_FL_DELETED)
			continue;  // skip any deleted waypoints
		
		if (waypoints[index].flags & W_FL_AIMING)
			continue;  // skip any aiming waypoints
		
		// skip this waypoint if it's team specific and teams don't match...
		if ((team != -1) && (waypoints[index].flags & W_FL_TEAM_SPECIFIC) &&
			((waypoints[index].flags & W_FL_TEAM) != team))
			continue;
		
		if ((waypoints[index].flags & flags) != flags)
			continue;  // skip this waypoint if the flags don't match
		
		exclude_index = 0;
		while (exclude[exclude_index])
		{
			if (index == exclude[exclude_index])
				break;  // found a match, break out of while loop
			
			exclude_index++;
		}
		
		if (index == exclude[exclude_index])
			continue;  // skip any index that matches exclude list
		
		if (waypoints[index].item[0])
		{
			wpt_item = WaypointFindItem(index);	
			if ((wpt_item == nullptr) || (wpt_item->v.effects & EF_NODRAW) || (wpt_item->v.frame > 0) ||
				((team != -1) && (UTIL_GetTeam(wpt_item) != team)))
				continue;
		}

		if (count < 200)
		{
			indexes[count] = index;
			
			count++;
		}
	}
	
	if (count == 0)  // no matching waypoints found
		return -1;
	
	index = RANDOM_LONG(1, count) - 1;
	
	return indexes[index];
}

// find a random goal matching the "flags" bits and return the index of
// that waypoint...
int WaypointFindRandomGoal(edict_t *pEntity, int team, int flags, char item[])
{
	int index;
	int indexes[50];
	int count = 0;
	edict_t *wpt_item;
	
	if (num_waypoints < 1)
		return -1;
	
	// find all the waypoints with the matching flags...
	
	for (index=0; index < num_waypoints; index++)
	{
		if (waypoints[index].flags & W_FL_DELETED)
			continue;  // skip any deleted waypoints
		
		if (waypoints[index].flags & W_FL_AIMING)
			continue;  // skip any aiming waypoints
		
		// skip this waypoint if it's team specific and teams don't match...
		if ((team != -1) && (waypoints[index].flags & W_FL_TEAM_SPECIFIC) &&
			((waypoints[index].flags & W_FL_TEAM) != team))
			continue;
		
		if ((waypoints[index].flags & flags) != flags)
			continue;  // skip this waypoint if the flags don't match
		
		// skip this waypoint if not the right item
		if (strcmp(waypoints[index].item, item) != 0)
			continue;

		if (waypoints[index].item[0])
		{
			wpt_item = WaypointFindItem(index);	
			if ((wpt_item == nullptr) || (wpt_item->v.effects & EF_NODRAW) || (wpt_item->v.frame > 0) ||
				((team != -1) && (UTIL_GetTeam(wpt_item) != team)))
				continue;
		}

		if (count < 50)
		{
			indexes[count] = index;
			
			count++;
		}
	}
	
	if (count == 0)  // no matching waypoints found
		return -1;
	
	index = RANDOM_LONG(1, count) - 1;
	
	return indexes[index];
}

// find a random goal within a range of a position (v_src) matching the
// "flags" bits and return the index of that waypoint...
int WaypointFindRandomGoal(Vector v_src, edict_t *pEntity, float range, int team, int flags)
{
	int index;
	int indexes[50];
	int count = 0;
	float distance;
	edict_t *wpt_item;
	
	if (num_waypoints < 1)
		return -1;
	
	// find all the waypoints with the matching flags...
	
	for (index=0; index < num_waypoints; index++)
	{
		if (waypoints[index].flags & W_FL_DELETED)
			continue;  // skip any deleted waypoints
		
		if (waypoints[index].flags & W_FL_AIMING)
			continue;  // skip any aiming waypoints
		
		// skip this waypoint if it's team specific and teams don't match...
		if ((team != -1) && (waypoints[index].flags & W_FL_TEAM_SPECIFIC) &&
			((waypoints[index].flags & W_FL_TEAM) != team))
			continue;
		
		if ((waypoints[index].flags & flags) != flags)
			continue;  // skip this waypoint if the flags don't match
		
		if (waypoints[index].item[0])
		{
			wpt_item = WaypointFindItem(index);	
			if ((wpt_item == nullptr) || (wpt_item->v.effects & EF_NODRAW) || (wpt_item->v.frame > 0) ||
				((team != -1) && (UTIL_GetTeam(wpt_item) != team)))
				continue;
		}

		distance = (waypoints[index].origin - v_src).Length();
		
		if ((distance < range) && (count < 50))
		{
			indexes[count] = index;
			
			count++;
		}
	}
	
	if (count == 0)  // no matching waypoints found
		return -1;
	
	index = RANDOM_LONG(1, count) - 1;
	
	return indexes[index];
}


// special waypoint finder for science and industry weapons
int WaypointFindNearestWeapon(edict_t *pEntity, int src, int team, int flags, int weapon)
{
	int index, min_index, select_index;
	int distance, min_distance;
	edict_t *wpt_item;
	bool found = false;
	// for weapon goals
	bot_weapon_select_t *pSelect = nullptr;
	pSelect = WeaponGetSelectPointer();
	
	if (num_waypoints < 1 || pSelect == nullptr)
		return -1;
	
	// find the nearest waypoint with the matching flags...
	
	min_index = -1;
	min_distance = 99999;
	
	for (index=0; index < num_waypoints; index++)
	{
		if (index == src)
			continue;  // skip the source waypoint
		
		if (waypoints[index].flags & W_FL_DELETED)
			continue;  // skip any deleted waypoints
		
		if (waypoints[index].flags & W_FL_AIMING)
			continue;  // skip any aiming waypoints
		
		// skip this waypoint if it's team specific and teams don't match...
		if ((team != -1) && (waypoints[index].flags & W_FL_TEAM_SPECIFIC) &&
			((waypoints[index].flags & W_FL_TEAM) != team))
			continue;
		
		if ((waypoints[index].flags & flags) != flags)
			continue;  // skip this waypoint if the flags don't match

		if (waypoints[index].item[0])
		{
			wpt_item = WaypointFindItem(index);	
			if ((wpt_item == nullptr) || (wpt_item->v.effects & EF_NODRAW) || (wpt_item->v.frame > 0 &&
				strncmp(STRING(wpt_item->v.classname), "func_", 5) == 0) ||
				((team != -1) && (UTIL_GetTeam(wpt_item) != team)))
				continue;

			found = false;
			select_index = 0;
			while (pSelect[select_index].iId)
			{
				if (pSelect[select_index].iId == weapon)
				{	// does the weapon match?
					if (strstr(STRING(wpt_item->v.model), pSelect[select_index].weapon_model) != nullptr)
					{
						found = true;
						break;
					}
				}

				select_index++;
			}

			if (!found)
				continue;
		}

		distance = WaypointDistanceFromTo(src, index, team);
		
		if (distance < min_distance)
		{
			min_index = index;
			min_distance = distance;
		}
	}
	
	return min_index;
}

// find the nearest "special" aiming waypoint (for sniper aiming)...
int WaypointFindNearestAiming(Vector v_origin)
{
	int index;
	int min_index = -1;
	int min_distance = 9999.0;
	float distance;
	
	if (num_waypoints < 1)
		return -1;
	
	// search for nearby aiming waypoint...
	for (index=0; index < num_waypoints; index++)
	{
		if (waypoints[index].flags & W_FL_DELETED)
			continue;  // skip any deleted waypoints
		
		if ((waypoints[index].flags & W_FL_AIMING) == 0)
			continue;  // skip any NON aiming waypoints
		
		distance = (v_origin - waypoints[index].origin).Length();
		
		if ((distance < min_distance) && (distance < 40))
		{
			min_index = index;
			min_distance = distance;
		}
	}
	
	return min_index;
}


void WaypointDrawBeam(edict_t *pEntity, Vector start, Vector end, int width,
					  int noise, int red, int green, int blue, int brightness, int speed)
{
	MESSAGE_BEGIN(MSG_ONE, SVC_TEMPENTITY, nullptr, pEntity);
	WRITE_BYTE( TE_BEAMPOINTS);
	WRITE_COORD(start.x);
	WRITE_COORD(start.y);
	WRITE_COORD(start.z);
	WRITE_COORD(end.x);
	WRITE_COORD(end.y);
	WRITE_COORD(end.z);
	WRITE_SHORT( m_spriteTexture );
	WRITE_BYTE( 1 ); // framestart
	WRITE_BYTE( 10 ); // framerate
	WRITE_BYTE( 10 ); // life in 0.1's
	WRITE_BYTE( width ); // width
	WRITE_BYTE( noise );  // noise
	
	WRITE_BYTE( red );   // r, g, b
	WRITE_BYTE( green );   // r, g, b
	WRITE_BYTE( blue );   // r, g, b
	
	WRITE_BYTE( brightness );   // brightness
	WRITE_BYTE( speed );    // speed
	MESSAGE_END();
}

void WaypointSearchItems(edict_t *pEntity, Vector origin, int wpt_index)
{
	edict_t *pent = nullptr;
	float radius = 40;
	TraceResult tr;
	float distance;
	float min_distance;
	char item_name[64];
	char nearest_name[64];
	char msg[80];
	edict_t *nearest_pent;
	
	nearest_name[0] = 0;      // null out nearest_name string
	nearest_pent = nullptr;
	
	min_distance = 9999.0;
	
	//********************************************************
	// look for the nearest health, armor, ammo, weapon, etc.
	//********************************************************
	
	while ((pent = UTIL_FindEntityInSphere( pent, origin, radius )) != nullptr)
	{
		if (pEntity)
			UTIL_TraceLine( origin, pent->v.origin, ignore_monsters,
			pEntity->v.pContainingEntity, &tr );
		else
			UTIL_TraceLine( origin, pent->v.origin, ignore_monsters, nullptr, &tr );
		
		// make sure entity is visible...
		if ((tr.flFraction >= 1.0) || (strcmp("func_healthcharger", STRING(pent->v.classname)) == 0) ||
			(strcmp("func_recharge", STRING(pent->v.classname)) == 0))
		{
			strcpy(item_name, STRING(pent->v.classname));
			
			if ((strcmp("item_healthkit", item_name) == 0) ||
				(strcmp("item_battery", item_name) == 0) ||
				(strcmp("func_healthcharger", item_name) == 0) ||
				(strcmp("func_recharge", item_name) == 0) ||
				(strcmp("item_longjump", item_name) == 0) ||
				(strncmp("ammo_", item_name, 5) == 0) ||
				((strncmp("weapon_", item_name, 7) == 0) &&
				(pent->v.owner == nullptr)))
			{
				distance = (pent->v.origin - origin).Length();
				
				if (distance < min_distance)
				{
					strcpy(nearest_name, item_name);
					
					nearest_pent = pent;
					
					min_distance = distance;
				}
			}
		}
	}
	
	if (nearest_name[0])  // found an entity name
	{	// store the name of this item
		strcpy(waypoints[wpt_index].item, nearest_name);

		if ((strcmp("item_healthkit", nearest_name) == 0) ||
			(strcmp("func_healthcharger", nearest_name) == 0))
		{
			if (pEntity)
			{
				sprintf(msg, "Found some health (%s) at waypoint %i!\n", nearest_name, wpt_index);
				SERVER_PRINT( msg);
			}
			waypoints[wpt_index].flags |= W_FL_HEALTH;
		}
		
		else if ((strcmp("item_battery", nearest_name) == 0) ||
			(strcmp("func_recharge", nearest_name) == 0))
		{
			if (pEntity)
			{
				sprintf(msg, "Found some armor (%s) at waypoint %i!\n", nearest_name, wpt_index);
				SERVER_PRINT( msg);
			}
			waypoints[wpt_index].flags |= W_FL_ARMOR;
		}
		
		else if (strncmp("ammo_", nearest_name, 5) == 0)
		{
			if (pEntity)
			{
				sprintf(msg, "Found some ammo (%s) at waypoint %i!\n", nearest_name, wpt_index);
				SERVER_PRINT( msg);
			}
			waypoints[wpt_index].flags |= W_FL_AMMO;
		}
		
		else if ((strncmp("weapon_", nearest_name, 7) == 0) &&
			(nearest_pent->v.owner == nullptr))
		{
			if (pEntity)
			{
				sprintf(msg, "Found a weapon (%s) at waypoint %i!\n", nearest_name, wpt_index);
				SERVER_PRINT( msg);
			}
			waypoints[wpt_index].flags |= W_FL_WEAPON;
		}

		else if (strcmp("item_longjump", nearest_name) == 0)
		{
			if (pEntity)
			{
				sprintf(msg, "Found a long jump module at waypoint %i!\n", wpt_index);
				SERVER_PRINT( msg);
			}
			waypoints[wpt_index].flags |= W_FL_ITEM;
		}
	}
}

edict_t *WaypointFindItem( int wpt_index )
{
	edict_t *pent = nullptr;
	float radius = 40;
	TraceResult tr;
	float distance;
	float min_distance;
	char item_name[64];
	char nearest_name[64];
	edict_t *nearest_pent = nullptr;
	
	nearest_name[0] = 0;      // null out nearest_name string
	nearest_pent = nullptr;
	
	min_distance = 9999.0;
	
	Vector origin = waypoints[wpt_index].origin;
	//********************************************************
	// look for the nearest health, armor, ammo, weapon, etc.
	//********************************************************
	while ((pent = UTIL_FindEntityInSphere( pent, origin, radius )) != nullptr)
	{
		UTIL_TraceLine( origin, pent->v.origin, ignore_monsters, nullptr, &tr );
		
		// make sure entity is visible...
		if ((tr.flFraction >= 1.0) || (tr.pHit == pent))
		{
			strcpy(item_name, STRING(pent->v.classname));
			
			if ((strcmp(waypoints[wpt_index].item, item_name) == 0) &&
				(pent->v.owner == nullptr))
			{
				distance = (pent->v.origin - origin).Length();
				
				if (distance < min_distance)
				{	
					nearest_pent = pent;
					min_distance = distance;
				}
			}
		}
	}

	return nearest_pent;
}

void WaypointAdd(edict_t *pEntity)
{
	int index;
	edict_t *pent = nullptr;
	float radius = 40;
	
	if (num_waypoints >= MAX_WAYPOINTS)
		return;
	
	index = 0;
	
	// find the next available slot for the new waypoint...
	while (index < num_waypoints)
	{
		if (waypoints[index].flags & W_FL_DELETED)
			break;
		
		index++;
	}
	
	waypoints[index].flags = 0;
	
	// store the origin (location) of this waypoint (use entity origin)
	waypoints[index].origin = pEntity->v.origin;
	
	// store the last used waypoint for the auto waypoint code...
	last_waypoint = pEntity->v.origin;
	
	// set the time that this waypoint was originally displayed...
	wp_display_time[index] = gpGlobals->time;
	
	
	Vector start, end;
	
	start = pEntity->v.origin - Vector(0, 0, 34);
	end = start + Vector(0, 0, 68);
	
	if ((pEntity->v.flags & FL_DUCKING) == FL_DUCKING)
	{
		waypoints[index].flags |= W_FL_CROUCH;  // crouching waypoint
		
		start = pEntity->v.origin - Vector(0, 0, 17);
		end = start + Vector(0, 0, 34);
	}
	
	if (pEntity->v.movetype == MOVETYPE_FLY)
		waypoints[index].flags |= W_FL_LADDER;  // waypoint on a ladder
	
	// search the area near the waypoint for items (HEALTH, AMMO, WEAPON, etc.)
	WaypointSearchItems(pEntity, waypoints[index].origin, index);
	
	// draw a white waypoint
	WaypointDrawBeam(pEntity, start, end, 30, 0, 255, 255, 255, 250, 5);
	
	EMIT_SOUND_DYN2(pEntity, CHAN_WEAPON, "weapons/xbow_hit1.wav", 1.0,
		ATTN_NORM, 0, 100);
	
	// increment total number of waypoints if adding at end of array...
	if (index == num_waypoints)
		num_waypoints++;
	
	// calculate all the paths to this new waypoint
	for (int i=0; i < num_waypoints; i++)
	{
		if (i == index)
			continue;  // skip the waypoint that was just added
		
		if (waypoints[i].flags & W_FL_AIMING)
			continue;  // skip any aiming waypoints
		
		// check if the waypoint is reachable from the new one (one-way)
		if ( WaypointReachable(pEntity->v.origin, waypoints[i].origin, pEntity) )
		{
			WaypointAddPath(index, i);
		}
		
		// check if the new one is reachable from the waypoint (other way)
		if ( WaypointReachable(waypoints[i].origin, pEntity->v.origin, pEntity) )
		{
			WaypointAddPath(i, index);
		}
	}
}


void WaypointAddAiming(edict_t *pEntity)
{
	int index;
	edict_t *pent = nullptr;
	
	if (num_waypoints >= MAX_WAYPOINTS)
		return;
	
	index = 0;
	
	// find the next available slot for the new waypoint...
	while (index < num_waypoints)
	{
		if (waypoints[index].flags & W_FL_DELETED)
			break;
		
		index++;
	}
	
	waypoints[index].flags = W_FL_AIMING;  // aiming waypoint
	
	Vector v_angle = pEntity->v.v_angle;
	
	v_angle.x = 0;  // reset pitch to horizontal
	v_angle.z = 0;  // reset roll to level
	
	MAKE_VECTORS(v_angle);
	
	// store the origin (location) of this waypoint (use entity origin)
	waypoints[index].origin = pEntity->v.origin + gpGlobals->v_forward * 25;
	
	// set the time that this waypoint was originally displayed...
	wp_display_time[index] = gpGlobals->time;
	
	
	Vector start, end;
	
	start = pEntity->v.origin - Vector(0, 0, 10);
	end = start + Vector(0, 0, 14);
	
	// draw a white waypoint
	WaypointDrawBeam(pEntity, start, end, 30, 0, 255, 255, 255, 250, 5);
	
	EMIT_SOUND_DYN2(pEntity, CHAN_WEAPON, "weapons/xbow_hit1.wav", 1.0,
		ATTN_NORM, 0, 100);
	
	// increment total number of waypoints if adding at end of array...
	if (index == num_waypoints)
		num_waypoints++;
}

void WaypointDelete(edict_t *pEntity)
{
	int index;
	int count = 0;
	
	if (num_waypoints < 1)
		return;
	
	index = WaypointFindNearest(pEntity, 50.0, -1);
	
	if (index == -1)
		return;
	
	if (waypoints[index].flags & W_FL_SNIPER)
	{
		int i;
		int min_index = -1;
		int min_distance = 9999.0;
		float distance;
		
		// search for nearby aiming waypoint and delete it also...
		for (i=0; i < num_waypoints; i++)
		{
			if (waypoints[i].flags & W_FL_DELETED)
				continue;  // skip any deleted waypoints
			
			if ((waypoints[i].flags & W_FL_AIMING) == 0)
				continue;  // skip any NON aiming waypoints
			
			distance = (waypoints[i].origin - waypoints[index].origin).Length();
			
			if ((distance < min_distance) && (distance < 40))
			{
				min_index = i;
				min_distance = distance;
			}
		}
		
		if (min_index != -1)
		{
			waypoints[min_index].flags = W_FL_DELETED;  // not being used
			waypoints[min_index].origin = Vector(0,0,0);
			
			wp_display_time[min_index] = 0.0;
		}
	}
	
	// delete any paths that lead to this index...
	WaypointDeletePath(index);
	
	// free the path for this index...
	
	if (paths[index] != nullptr)
	{
		PATH *p = paths[index];
		PATH *p_next;
		
		while (p)  // free the linked list
		{
			p_next = p->next;  // save the link to next
			free(p);
			p = p_next;
			
#ifdef _DEBUG
			count++;
			if (count > 100) WaypointDebug();
#endif
		}
		
		paths[index] = nullptr;
	}
	
	waypoints[index].flags = W_FL_DELETED;  // not being used
	waypoints[index].origin = Vector(0,0,0);
	
	wp_display_time[index] = 0.0;
	
	EMIT_SOUND_DYN2(pEntity, CHAN_WEAPON, "weapons/mine_activate.wav", 1.0,
		ATTN_NORM, 0, 100);
}

void WaypointUpdate(edict_t *pEntity)
{
	int index;
	int mask;
	
	mask = W_FL_HEALTH | W_FL_ARMOR | W_FL_AMMO | W_FL_WEAPON;
	
	for (index=0; index < num_waypoints; index++)
	{
		waypoints[index].flags &= ~mask;  // clear the mask bits
		
		WaypointSearchItems(nullptr, waypoints[index].origin, index);
	}
}

// allow player to manually create a path from one waypoint to another
void WaypointCreatePath(edict_t *pEntity, int cmd)
{
	static int waypoint1 = -1;  // initialized to unassigned
	static int waypoint2 = -1;  // initialized to unassigned
	
	if (cmd == 1)  // assign source of path
	{
		waypoint1 = WaypointFindNearest(pEntity, 50.0, -1);
		
		if (waypoint1 == -1)
		{
			// play "cancelled" sound...
			EMIT_SOUND_DYN2(pEntity, CHAN_WEAPON, "common/wpn_moveselect.wav", 1.0,
				ATTN_NORM, 0, 100);
			
			return;
		}
		
		// play "start" sound...
		EMIT_SOUND_DYN2(pEntity, CHAN_WEAPON, "common/wpn_hudoff.wav", 1.0,
			ATTN_NORM, 0, 100);
		
		return;
	}
	
	if (cmd == 2)  // assign dest of path and make path
	{
		waypoint2 = WaypointFindNearest(pEntity, 50.0, -1);
		
		if ((waypoint1 == -1) || (waypoint2 == -1))
		{
			// play "error" sound...
			EMIT_SOUND_DYN2(pEntity, CHAN_WEAPON, "common/wpn_denyselect.wav", 1.0,
				ATTN_NORM, 0, 100);
			
			return;
		}
		
		WaypointAddPath(waypoint1, waypoint2);
		
		// play "done" sound...
		EMIT_SOUND_DYN2(pEntity, CHAN_WEAPON, "common/wpn_hudon.wav", 1.0,
			ATTN_NORM, 0, 100);
	}
}


// allow player to manually remove a path from one waypoint to another
void WaypointRemovePath(edict_t *pEntity, int cmd)
{
	static int waypoint1 = -1;  // initialized to unassigned
	static int waypoint2 = -1;  // initialized to unassigned
	
	if (cmd == 1)  // assign source of path
	{
		waypoint1 = WaypointFindNearest(pEntity, 50.0, -1);
		
		if (waypoint1 == -1)
		{
			// play "cancelled" sound...
			EMIT_SOUND_DYN2(pEntity, CHAN_WEAPON, "common/wpn_moveselect.wav", 1.0,
				ATTN_NORM, 0, 100);
			
			return;
		}
		
		// play "start" sound...
		EMIT_SOUND_DYN2(pEntity, CHAN_WEAPON, "common/wpn_hudoff.wav", 1.0,
			ATTN_NORM, 0, 100);
		
		return;
	}
	
	if (cmd == 2)  // assign dest of path and make path
	{
		waypoint2 = WaypointFindNearest(pEntity, 50.0, -1);
		
		if ((waypoint1 == -1) || (waypoint2 == -1))
		{
			// play "error" sound...
			EMIT_SOUND_DYN2(pEntity, CHAN_WEAPON, "common/wpn_denyselect.wav", 1.0,
				ATTN_NORM, 0, 100);
			
			return;
		}
		
		WaypointDeletePath(waypoint1, waypoint2);
		
		// play "done" sound...
		EMIT_SOUND_DYN2(pEntity, CHAN_WEAPON, "common/wpn_hudon.wav", 1.0,
			ATTN_NORM, 0, 100);
	}
}


bool WaypointLoad(edict_t *pEntity, char *ext)
{
	char mapname[64];
	char filename[256];
	WAYPOINT_HDR header;
	char msg[80];
	int index, i;
	short int num;
	short int path_index;
	
	strcpy(mapname, STRING(gpGlobals->mapname));
	strcat(mapname, ext);
	
	UTIL_BuildFileName(filename, "maps", mapname);

   sprintf (msg, "loading waypoint file: %s\n", filename);
   SERVER_PRINT (msg);
	
	FILE *bfp = fopen(filename, "rb");
	
	// if file exists, read the waypoint structure from it
	if (bfp != nullptr)
	{
		fread(&header, sizeof(header), 1, bfp);
		
		header.filetype[7] = 0;
		if (strcmp(header.filetype, "HPB_bot") == 0)
		{
			if (header.waypoint_file_version != WAYPOINT_VERSION)
			{
				if (pEntity)
					SERVER_PRINT( "Incompatible Grave Bot waypoint file version!\nWaypoints not loaded!\n");
				
				fclose(bfp);
				return FALSE;
			}
			
			header.mapname[31] = 0;
			
			if (strcmp(header.mapname, STRING(gpGlobals->mapname)) == 0)
			{
				WaypointInit();  // remove any existing waypoints
				
				for (i=0; i < header.number_of_waypoints; i++)
				{	// pre new waypoins
					//fread(&waypoints[i], sizeof(waypoints[0]), 1, bfp);
					//num_waypoints++;
					fread(&waypoints[i], sizeof(old_waypoint), 1, bfp);
					for (int j = 0; j < 19; j++)
						waypoints[i].item[j] = 0;
					num_waypoints++;

					if (waypoints[i].flags & W_FL_DEFEND)
						num_def_waypoints++;
				}
				
				// read and add waypoint paths...
				for (index=0; index < num_waypoints; index++)
				{
					// read the number of paths from this node...
					fread(&num, sizeof(num), 1, bfp);
					
					for (i=0; i < num; i++)
					{
						fread(&path_index, sizeof(path_index), 1, bfp);
						
						WaypointAddPath(index, path_index);
					}
				}
				SERVER_PRINT( "HPB_Bot waypoints loaded...\n");
				g_waypoint_paths = TRUE;  // keep track so path can be freed
			}
			else
			{
				if (pEntity)
				{
					sprintf(msg, "%s Grave Bot waypoints are not for this map!\n", filename);
					SERVER_PRINT( msg);
				}
				
				fclose(bfp);
				return FALSE;
			}
		}
		else if (strcmp(header.filetype, "[Grave]") == 0 || strcmp(header.filetype, "Crabbed") == 0)
		{
			if (header.waypoint_file_version != WAYPOINT_VERSION)
			{
				if (pEntity)
					SERVER_PRINT( "Incompatible Grave Bot waypoint file version!\nWaypoints not loaded!\n");
				
				fclose(bfp);
				return FALSE;
			}
			
			header.mapname[31] = 0;
			
			if (strcmp(header.mapname, STRING(gpGlobals->mapname)) == 0)
			{
				WaypointInit();  // remove any existing waypoints
				
				for (i=0; i < header.number_of_waypoints; i++)
				{
					fread(&waypoints[i], sizeof(waypoints[0]), 1, bfp);
					num_waypoints++;

					if (waypoints[i].flags & W_FL_DEFEND)
						num_def_waypoints++;
				}
				
				// read and add waypoint paths...
				for (index=0; index < num_waypoints; index++)
				{
					// read the number of paths from this node...
					fread(&num, sizeof(num), 1, bfp);
					
					for (i=0; i < num; i++)
					{
						fread(&path_index, sizeof(path_index), 1, bfp);
						
						WaypointAddPath(index, path_index);
					}
				}
				SERVER_PRINT( "Grave Bot waypoints loaded...\n");
				g_waypoint_paths = TRUE;  // keep track so path can be freed
			}
			else
			{
				if (pEntity)
				{
					sprintf(msg, "%s Grave Bot waypoints are not for this map!\n", filename);
					SERVER_PRINT( msg);
				}
				
				fclose(bfp);
				return FALSE;
			}
		}
		else 
		{
			if (pEntity)
			{
				sprintf(msg, "%s is not a Grave Bot waypoint file!\n", filename);
				SERVER_PRINT( msg);
			}
			
			fclose(bfp);
			return FALSE;
		}
		
		fclose(bfp);
		
		WaypointRouteInit();
   }
   else
   {
      sprintf (msg, "waypoint file %s not found!\n", filename);
      SERVER_PRINT (msg);
	   
	   // try to load the .wpt file
	   if (FStrEq(ext, ".gbw"))
		   return WaypointLoad(pEntity, ".wpt");

	   return FALSE;
   }
   
   return TRUE;
}


void WaypointSave()
{
	char filename[256];
	char mapname[64];
	WAYPOINT_HDR header;
	int index, i;
	short int num;
	PATH *p;
	
	//strcpy(header.filetype, "HPB_bot");
	strcpy(header.filetype, "[Grave]");

	header.waypoint_file_version = WAYPOINT_VERSION;
	
	header.waypoint_file_flags = 0;  // not currently used
	
	header.number_of_waypoints = num_waypoints;
	
	memset(header.mapname, 0, sizeof(header.mapname));
	strncpy(header.mapname, STRING(gpGlobals->mapname), 31);
	header.mapname[31] = 0;
	
	strcpy(mapname, STRING(gpGlobals->mapname));
	strcat(mapname, ".gbw");
	
	UTIL_BuildFileName(filename, "maps", mapname);
	
	FILE *bfp = fopen(filename, "wb");
	
	// write the waypoint header to the file...
	fwrite(&header, sizeof(header), 1, bfp);
	
	// write the waypoint data to the file...
	for (index=0; index < num_waypoints; index++)
	{
		fwrite(&waypoints[index], sizeof(waypoints[0]), 1, bfp);
	}
	
	// save the waypoint paths...
	for (index=0; index < num_waypoints; index++)
	{
		// count the number of paths from this node...
		
		p = paths[index];
		num = 0;
		
		while (p != nullptr)
		{
			i = 0;
			
			while (i < MAX_PATH_INDEX)
			{
				if (p->index[i] != -1)
					num++;  // count path node if it's used
				
				i++;
			}
			
			p = p->next;  // go to next node in linked list
		}
		
		fwrite(&num, sizeof(num), 1, bfp);  // write the count
		
		// now write out each path index...
		
		p = paths[index];
		
		while (p != nullptr)
		{
			i = 0;
			
			while (i < MAX_PATH_INDEX)
			{
				if (p->index[i] != -1)  // save path node if it's used
					fwrite(&p->index[i], sizeof(p->index[0]), 1, bfp);
				
				i++;
			}
			
			p = p->next;  // go to next node in linked list
		}
	}
	
	fclose(bfp);
}


bool WaypointReachable(Vector v_src, Vector v_dest, edict_t *pEntity)
{
	TraceResult tr;
	float curr_height, last_height;
	
	float distance = (v_dest - v_src).Length();
	
	// is the destination close enough?
	if (distance < REACHABLE_RANGE)
	{
		// check if this waypoint is "visible"...
		
		UTIL_TraceLine( v_src, v_dest, ignore_monsters,
			pEntity->v.pContainingEntity, &tr );
		
		// if waypoint is visible from current position (even behind head)...
		if (tr.flFraction >= 1.0)
		{
			// check for special case of both waypoints being underwater...
			if ((POINT_CONTENTS( v_src ) == CONTENTS_WATER) &&
				(POINT_CONTENTS( v_dest ) == CONTENTS_WATER))
			{
				return TRUE;
			}
			
			// check for special case of waypoint being suspended in mid-air...
			
			// is dest waypoint higher than src? (45 is max jump height)
			if (v_dest.z > (v_src.z + 45.0))
			{
				Vector v_new_src = v_dest;
				Vector v_new_dest = v_dest;
				
				v_new_dest.z = v_new_dest.z - 50;  // straight down 50 units
				
				UTIL_TraceLine(v_new_src, v_new_dest, dont_ignore_monsters,
					pEntity->v.pContainingEntity, &tr);
				
				// check if we didn't hit anything, if not then it's in mid-air
				if (tr.flFraction >= 1.0)
				{
					return FALSE;  // can't reach this one
				}
			}
			
			// check if distance to ground increases more than jump height
			// at points between source and destination...
			
			Vector v_direction = (v_dest - v_src).Normalize();  // 1 unit long
			Vector v_check = v_src;
			Vector v_down = v_src;
			
			v_down.z = v_down.z - 1000.0;  // straight down 1000 units
			
			UTIL_TraceLine(v_check, v_down, ignore_monsters,
				pEntity->v.pContainingEntity, &tr);
			
			last_height = tr.flFraction * 1000.0;  // height from ground
			
			distance = (v_dest - v_check).Length();  // distance from goal
			
			while (distance > 10.0)
			{
				// move 10 units closer to the goal...
				v_check = v_check + (v_direction * 10.0);
				
				v_down = v_check;
				v_down.z = v_down.z - 1000.0;  // straight down 1000 units
				
				UTIL_TraceLine(v_check, v_down, ignore_monsters,
					pEntity->v.pContainingEntity, &tr);
				
				curr_height = tr.flFraction * 1000.0;  // height from ground
				
				// is the difference in the last height and the current height
				// higher that the jump height?
				if ((last_height - curr_height) > 45.0)
				{
					// can't get there from here...
					return FALSE;
				}
				
				last_height = curr_height;
				
				distance = (v_dest - v_check).Length();  // distance from goal
			}
			
			return TRUE;
		}
	}
	
	return FALSE;
}


// find the nearest reachable waypoint
int WaypointFindReachable(edict_t *pEntity, float range, int team)
{
	int i, min_index = 0;
	float distance;
	float min_distance;
	TraceResult tr;
	
	// find the nearest waypoint...
	
	min_distance = 9999.0;
	
	for (i=0; i < num_waypoints; i++)
	{
		if (waypoints[i].flags & W_FL_DELETED)
			continue;  // skip any deleted waypoints
		
		if (waypoints[i].flags & W_FL_AIMING)
			continue;  // skip any aiming waypoints
		
		// skip this waypoint if it's team specific and teams don't match...
		if ((team != -1) && (waypoints[i].flags & W_FL_TEAM_SPECIFIC) &&
			((waypoints[i].flags & W_FL_TEAM) != team))
			continue;
		
		distance = (waypoints[i].origin - pEntity->v.origin).Length();
		
		if (distance < min_distance)
		{
			// if waypoint is visible from current position (even behind head)...
			UTIL_TraceLine( pEntity->v.origin + pEntity->v.view_ofs, waypoints[i].origin,
				ignore_monsters, pEntity->v.pContainingEntity, &tr );
			
			if (tr.flFraction >= 1.0)
			{
				if (WaypointReachable(pEntity->v.origin, waypoints[i].origin, pEntity))
				{
					min_index = i;
					min_distance = distance;
				}
			}
		}
	}
	
	// if not close enough to a waypoint then just return
	if (min_distance > range)
		return -1;
	
	return min_index;
	
}


void WaypointPrintInfo(edict_t *pEntity)
{
	char msg[80];
	char item[19];
	int index;
	int flags;

	// find the nearest waypoint...
	index = WaypointFindNearest(pEntity, 50.0, -1);
	
	if (index == -1)
		return;
	
	sprintf(msg,"Waypoint %d of %d total\n", index, num_waypoints);
	SERVER_PRINT( msg);
	
	flags = waypoints[index].flags;
	strcpy(item, waypoints[index].item);

	if (flags & W_FL_TEAM_SPECIFIC)
	{
		{
			if ((flags & W_FL_TEAM) == 0)
				strcpy(msg, "Waypoint is for TEAM 1\n");
			else if ((flags & W_FL_TEAM) == 1)
				strcpy(msg, "Waypoint is for TEAM 2\n");
			else if ((flags & W_FL_TEAM) == 2)
				strcpy(msg, "Waypoint is for TEAM 3\n");
			else if ((flags & W_FL_TEAM) == 3)
				strcpy(msg, "Waypoint is for TEAM 4\n");
		}
		
		SERVER_PRINT( msg);
	}
	
	if (flags & W_FL_LIFT)
		SERVER_PRINT( "Bot will wait for lift before approaching\n");
	
	if (flags & W_FL_LADDER)
		SERVER_PRINT( "This waypoint is on a ladder\n");
	
	if (flags & W_FL_DOOR)
		SERVER_PRINT( "This is a door waypoint\n");
	
	if (flags & W_FL_HEALTH)
	{
		if (item[0])
			sprintf(msg, "There is health (%s) near this waypoint\n", item);
		else
			strcpy(msg, "There is health near this waypoint\n");

		SERVER_PRINT( msg);
	}

	if (flags & W_FL_ARMOR)
	{
		if (item[0])
			sprintf(msg, "There is armor (%s) near this waypoint\n", item);
		else
			strcpy(msg, "There is armor near this waypoint\n");

		SERVER_PRINT( msg);
	}
	
	if (flags & W_FL_AMMO)
	{
		if (item[0])
			sprintf(msg, "There is ammo (%s) near this waypoint\n", item);
		else
			strcpy(msg, "There is ammo near this waypoint\n");

		SERVER_PRINT( msg);
	}
	
	if (flags & W_FL_WEAPON)
	{
		if (item[0])
			sprintf(msg, "There is a weapon (%s) near this waypoint\n", item);
		else
			strcpy(msg, "There is a weapon near this waypoint\n");

		SERVER_PRINT( msg);
	}
	
	if (flags & W_FL_ITEM)
	{
		if (item[0])
			sprintf(msg, "There is an item (%s) near this waypoint\n", item);
		else
			strcpy(msg, "There is an item near this waypoint\n");

		SERVER_PRINT( msg);
	}

	if (flags & W_FL_JUMP)
		SERVER_PRINT( "Bot will jump here\n");
	
	if (flags & W_FL_DUCKJUMP)
		SERVER_PRINT( "Bot will duck jump here\n");

	if (flags & W_FL_SNIPER)
		SERVER_PRINT( "This is a sniper waypoint\n");

	if (flags & W_FL_FLAG)
	{
		if (mod_id == SI_DLL)
			SERVER_PRINT( "There are scientists near this waypoint\n");
		else
			SERVER_PRINT( "There is a flag near this waypoint\n");
	}
	
	if (flags & W_FL_FLAG_GOAL)
	{
		if (mod_id == SI_DLL)
			SERVER_PRINT( "There is an administrator near this waypoint\n");
		else
			SERVER_PRINT( "There is a flag goal near this waypoint\n");
	}

	if (flags & W_FL_DEFEND)
		SERVER_PRINT( "Bot will defend this area\n");

}


void WaypointThink(edict_t *pEntity)
{
	float distance, min_distance;
	Vector start, end;
	int i, index;
	
	if (g_auto_waypoint)  // is auto waypoint on?
	{
		// find the distance from the last used waypoint
		distance = (last_waypoint - pEntity->v.origin).Length();
		
		if (distance > 200)
		{
			min_distance = 9999.0;
			
			// check that no other reachable waypoints are nearby...
			for (i=0; i < num_waypoints; i++)
			{
				if (waypoints[i].flags & W_FL_DELETED)
					continue;
				
				if (waypoints[i].flags & W_FL_AIMING)
					continue;
				
				if (WaypointReachable(pEntity->v.origin, waypoints[i].origin, pEntity))
				{
					distance = (waypoints[i].origin - pEntity->v.origin).Length();
					
					if (distance < min_distance)
						min_distance = distance;
				}
			}
			
			// make sure nearest waypoint is far enough away...
			if (min_distance >= 200)
				WaypointAdd(pEntity);  // place a waypoint here
		}
	}
	
	min_distance = 9999.0;
	
	if (g_waypoint_on)  // display the waypoints if turned on...
	{
		for (i=0; i < num_waypoints; i++)
		{
			if ((waypoints[i].flags & W_FL_DELETED) == W_FL_DELETED)
				continue;
			
			distance = (waypoints[i].origin - pEntity->v.origin).Length();
			
			if (distance < 500)
			{
				if (distance < min_distance)
				{
					index = i; // store index of nearest waypoint
					min_distance = distance;
				}
				
				if ((wp_display_time[i] + 1.0) < gpGlobals->time)
				{
					if (waypoints[i].flags & W_FL_CROUCH)
					{
						start = waypoints[i].origin - Vector(0, 0, 17);
						end = start + Vector(0, 0, 34);
					}
					else if (waypoints[i].flags & W_FL_AIMING)
					{
						start = waypoints[i].origin + Vector(0, 0, 10);
						end = start + Vector(0, 0, 14);
					}
					else
					{
						start = waypoints[i].origin - Vector(0, 0, 34);
						end = start + Vector(0, 0, 68);
					}
					
					int color[3] = {255,255,255};
					if (waypoints[i].flags & W_FL_TEAM_SPECIFIC)
					{
						if ((waypoints[i].flags & W_FL_TEAM) == 0)
						{
							color[0] = color[1] = 0;
							color[2] = 255;	// blue
						}
						else if ((waypoints[i].flags & W_FL_TEAM) == 1)
						{
							color[0] = color[2] = 0;
							color[1] = 255;	// green
						}
					}
					// draw a team color waypoint
					WaypointDrawBeam(pEntity, start, end, 30, 0, color[0], color[1], color[2], 250, 5);
					
					wp_display_time[i] = gpGlobals->time;
				}
			}
		}
		
		// check if path waypointing is on...
		if (g_path_waypoint)
		{
			// check if player is close enough to a waypoint and time to draw path...
			if ((min_distance <= 50) && (f_path_time <= gpGlobals->time))
			{
				PATH *p;
				
				f_path_time = gpGlobals->time + 1.0;
				
				p = paths[index];
				
				while (p != nullptr)
				{
					i = 0;
					
					while (i < MAX_PATH_INDEX)
					{
						if (p->index[i] != -1)
						{
							Vector v_src = waypoints[index].origin;
							Vector v_dest = waypoints[p->index[i]].origin;
							
							int color[3] = {255,255,255};
							if (waypoints[p->index[i]].flags & W_FL_TEAM_SPECIFIC)
							{
								if ((waypoints[p->index[i]].flags & W_FL_TEAM) == 0)
								{
									color[0] = color[1] = 0;
									color[2] = 255;	// blue
								}
								else if ((waypoints[p->index[i]].flags & W_FL_TEAM) == 1)
								{
									color[0] = color[2] = 0;
									color[1] = 255;	// green
								}
							}
							// draw a team color line to this index's waypoint
							WaypointDrawBeam(pEntity, v_src, v_dest, 10, 2, color[0], color[1], color[2], 200, 10);
						}
						
						i++;
					}
					
					p = p->next;  // go to next node in linked list
				}
			}
		}
	}
}


// run Floyd's algorithm on the waypoint list to generate the least cost
// path matrix...
void WaypointFloyds(unsigned short *shortest_path, unsigned short *from_to)
{
	unsigned int x, y, z;
	int changed = 1;
	int distance;
	
	for (y=0; y < route_num_waypoints; y++)
	{
		for (z=0; z < route_num_waypoints; z++)
		{
			from_to[y * route_num_waypoints + z] = z;
		}
	}
	
	while (changed)
	{
		changed = 0;
		
		for (x=0; x < route_num_waypoints; x++)
		{
			for (y=0; y < route_num_waypoints; y++)
			{
				for (z=0; z < route_num_waypoints; z++)
				{
					if ((shortest_path[y * route_num_waypoints + x] == WAYPOINT_UNREACHABLE) ||
						(shortest_path[x * route_num_waypoints + z] == WAYPOINT_UNREACHABLE))
						continue;
					
					distance = shortest_path[y * route_num_waypoints + x] +
						shortest_path[x * route_num_waypoints + z];
					
					if (distance > WAYPOINT_MAX_DISTANCE)
						distance = WAYPOINT_MAX_DISTANCE;
					
					if ((distance < shortest_path[y * route_num_waypoints + z]) ||
						(shortest_path[y * route_num_waypoints + z] == WAYPOINT_UNREACHABLE))
					{
						shortest_path[y * route_num_waypoints + z] = distance;
						from_to[y * route_num_waypoints + z] = from_to[y * route_num_waypoints + x];
						changed = 1;
					}
				}
			}
		}
	}
}


// load the waypoint route files (.gb1, .gb2, etc.) or generate them if
// they don't exist...
void WaypointRouteInit()
{
	unsigned int index;
	bool build_matrix[4];
	int matrix;
	unsigned int array_size;
	unsigned int row;
	int i, offset;
	unsigned int a, b;
	float distance;
	unsigned short *pShortestPath, *pFromTo;
	char msg[80];
	unsigned int num_items;
	FILE *bfp;
	char filename[256];
	char filename2[256];
	char mapname[64];
	
	if (num_waypoints == 0)
		return;
	
	// save number of current waypoints in case waypoints get added later
	route_num_waypoints = num_waypoints;
	
	strcpy(mapname, STRING(gpGlobals->mapname));
	strcat(mapname, ".gbw");
	
	UTIL_BuildFileName(filename, "maps", mapname);
	
	build_matrix[0] = TRUE;  // always build matrix 0 (non-team and team 1)
	build_matrix[1] = FALSE;
	build_matrix[2] = FALSE;
	build_matrix[3] = FALSE;
	
	// find out how many route matrixes to create...
	for (index=0; index < route_num_waypoints; index++)
	{
		if (waypoints[index].flags & W_FL_TEAM_SPECIFIC)
		{
			if ((waypoints[index].flags & W_FL_TEAM) == 0x01)  // team 2?
				build_matrix[1] = TRUE;
			
			if ((waypoints[index].flags & W_FL_TEAM) == 0x02)  // team 3?
				build_matrix[2] = TRUE;
			
			if ((waypoints[index].flags & W_FL_TEAM) == 0x03)  // team 4?
				build_matrix[3] = TRUE;
		}
	}
	
	array_size = route_num_waypoints * route_num_waypoints;
	
	for (matrix=0; matrix < 4; matrix++)
	{
		if (build_matrix[matrix])
		{
			char ext_str[5];  // ".gbX\0"
			int file1, file2;
			struct stat stat1, stat2;
			
			sprintf(ext_str, ".gb%d", matrix+1);
			
			strcpy(mapname, STRING(gpGlobals->mapname));
			strcat(mapname, ext_str);
			
			UTIL_BuildFileName(filename2, "maps", mapname);
			
			if (access(filename2, 0) == 0)  // does the .gbX file exist?
			{
				file1 = open(filename, O_RDONLY);
				file2 = open(filename2, O_RDONLY);
				
				fstat(file1, &stat1);
				fstat(file2, &stat2);
				
				close(file1);
				close(file2);
				
				if (stat1.st_mtime < stat2.st_mtime)  // is .gbw older than .gbX file?
				{
					sprintf(msg, "loading Grave Bot waypoint paths for team %d\n", matrix+1);
					SERVER_PRINT( msg);
					
					shortest_path[matrix] = static_cast<unsigned short *>(malloc(sizeof(unsigned short) * array_size));
					
					if (shortest_path[matrix] == nullptr)
						ALERT(at_error, "Grave Bot - Error allocating memory for shortest path!");
					
					from_to[matrix] = static_cast<unsigned short *>(malloc(sizeof(unsigned short) * array_size));
					
					if (from_to[matrix] == nullptr)
						ALERT(at_error, "Grave Bot - Error allocating memory for from to matrix!");
					
					bfp = fopen(filename2, "rb");
					
					if (bfp != nullptr)
					{
						num_items = fread(shortest_path[matrix], sizeof(unsigned short), array_size, bfp);
						
						if (num_items != array_size)
						{
							// if couldn't read enough data, free memory to recalculate it
							
							SERVER_PRINT( "error reading enough path items, recalculating...\n");
							
							free(shortest_path[matrix]);
							shortest_path[matrix] = nullptr;
							
							free(from_to[matrix]);
							from_to[matrix] = nullptr;
						}
						else
						{
							num_items = fread(from_to[matrix], sizeof(unsigned short), array_size, bfp);
							
							if (num_items != array_size)
							{
								// if couldn't read enough data, free memory to recalculate it
								
								SERVER_PRINT( "error reading enough path items, recalculating...\n");
								
								free(shortest_path[matrix]);
								shortest_path[matrix] = nullptr;
								
								free(from_to[matrix]);
								from_to[matrix] = nullptr;
							}
						}
					}
					else
					{
						SERVER_PRINT( "Grave Bot - Error reading waypoint paths!\n");
						
						free(shortest_path[matrix]);
						shortest_path[matrix] = nullptr;
						
						free(from_to[matrix]);
						from_to[matrix] = nullptr;
					}
					
					fclose(bfp);
				}
			}
			
			if (shortest_path[matrix] == nullptr)
			{
				sprintf(msg, "calculating Grave Bot waypoint paths for team %d...\n", matrix+1);
				SERVER_PRINT( msg);
				
				shortest_path[matrix] = static_cast<unsigned short *>(malloc(sizeof(unsigned short) * array_size));
				
				if (shortest_path[matrix] == nullptr)
					ALERT(at_error, "Grave Bot - Error allocating memory for shortest path!");
				
				from_to[matrix] = static_cast<unsigned short *>(malloc(sizeof(unsigned short) * array_size));
				
				if (from_to[matrix] == nullptr)
					ALERT(at_error, "Grave Bot - Error allocating memory for from to matrix!");
				
				pShortestPath = shortest_path[matrix];
				pFromTo = from_to[matrix];
				
				for (index=0; index < array_size; index++)
					pShortestPath[index] = WAYPOINT_UNREACHABLE;
				
				for (index=0; index < route_num_waypoints; index++)
					pShortestPath[index * route_num_waypoints + index] = 0;  // zero diagonal
				
				for (row=0; row < route_num_waypoints; row++)
				{
					if (paths[row] != nullptr)
					{
						PATH *p = paths[row];
						
						while (p)
						{
							i = 0;
							
							while (i < MAX_PATH_INDEX)
							{
								if (p->index[i] != -1)
								{
									index = p->index[i];
									
									// check if this is NOT team specific OR matches this team
									if (!(waypoints[index].flags & W_FL_TEAM_SPECIFIC) ||
										((waypoints[index].flags & W_FL_TEAM) == matrix))
									{
										distance = (waypoints[row].origin - waypoints[index].origin).Length();
										
										if (distance > (float)WAYPOINT_MAX_DISTANCE)
											distance = (float)WAYPOINT_MAX_DISTANCE;
										
										if (distance > REACHABLE_RANGE)
										{
											sprintf(msg, "Waypoint path distance > %4.1f at from %d to %d\n",
												REACHABLE_RANGE, row, index);
											SERVER_PRINT( msg);
										}
										else
										{
											offset = row * route_num_waypoints + index;
											
											pShortestPath[offset] = static_cast<unsigned short>(distance);
										}
									}
								}
								
								i++;
							}
							
							p = p->next;  // go to next node in linked list
						}
					}
				}
				
				// run Floyd's Algorithm to generate the from_to matrix...
				WaypointFloyds(pShortestPath, pFromTo);
				
				for (a=0; a < route_num_waypoints; a++)
				{
					for (b=0; b < route_num_waypoints; b++)
						if (pShortestPath[a * route_num_waypoints + b] == WAYPOINT_UNREACHABLE)
							pFromTo[a * route_num_waypoints + b] = WAYPOINT_UNREACHABLE;
				}
				
				bfp = fopen(filename2, "wb");
				
				if (bfp != nullptr)
				{
					num_items = fwrite(shortest_path[matrix], sizeof(unsigned short), array_size, bfp);
					
					if (num_items != array_size)
					{
						// if couldn't write enough data, close file and delete it
						
						fclose(bfp);
						unlink(filename2);
					}
					else
					{
						num_items = fwrite(from_to[matrix], sizeof(unsigned short), array_size, bfp);
						
						fclose(bfp);
						
						if (num_items != array_size)
						{
							// if couldn't write enough data, delete file
							unlink(filename2);
						}
					}
				}
				else
				{
					SERVER_PRINT( "Grave Bot - Error writing waypoint paths!\n");
				}
				
				sprintf(msg, "Grave Bot waypoint path calculations for team %d complete!\n",matrix+1);
				SERVER_PRINT( msg);
			}
		}
	}
}


// return the next waypoint index for a path from the Floyd matrix when
// going from a source waypoint index (src) to a destination waypoint
// index (dest)...
unsigned short WaypointRouteFromTo(int src, int dest, int team)
{
   char msg[80];
	unsigned short *pFromTo;
	
	if (src == -1 || dest == -1 || src > num_waypoints || dest > num_waypoints)
	{
		sprintf (msg, "WaypointRouteFromTo: invalid src (%i) or dest (%i) index!\n",
			src, dest);
      SERVER_PRINT (msg);
		return -1;
	}

	if ((team < -1) || (team > 3))
		return -1;
	
	if (team == -1)  // -1 means non-team play
		team = 0;
	
	if (from_to[team] == nullptr)  // if no team specific waypoints use team 0
		team = 0;
	
	if (from_to[team] == nullptr)  // if no route information just return
		return -1;
	
	pFromTo = from_to[team];
	
	return pFromTo[src * route_num_waypoints + dest];
}


// return the total distance (based on the Floyd matrix) of a path from
// the source waypoint index (src) to the destination waypoint index
// (dest)...
int WaypointDistanceFromTo(int src, int dest, int team)
{
   char msg[80];
	unsigned short *pShortestPath;
	
	if (src == -1 || dest == -1)
	{
      sprintf (msg, "WaypointDistanceFromTo: invalid src (%i) or dest (%i) index!\n",
			src, dest);
      SERVER_PRINT (msg);
		return -1;
	}

	if ((team < -1) || (team > 3))
		return -1;
	
	if (team == -1)	// -1 means non-team play
		team = 0;
	
	if (from_to[team] == nullptr)  // if no team specific waypoints use team 0
		team = 0;
	
	if (from_to[team] == nullptr)  // if no route information just return
		return -1;
	
	pShortestPath = shortest_path[team];
	
	return (int)(pShortestPath[src * route_num_waypoints + dest]);
}

