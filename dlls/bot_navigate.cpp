//
// HPB bot - botman's High Ping Bastard bot
//
// (http://planethalflife.com/botman/)
//
// bot_navigate.cpp
//

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
#include "bot_func.h"
#include "bot_weapons.h"
#include "waypoint.h"

extern bot_t bots[32];
extern int mod_id;
extern WAYPOINT waypoints[MAX_WAYPOINTS];
extern int num_waypoints;  // number of waypoints currently in use
extern int num_def_waypoints;
extern int team_allies[4];
extern edict_t *pent_info_ctfdetect;
extern float is_team_play;
extern bool checked_teamplay;
extern FLAG_S flags[MAX_FLAGS];
extern int num_flags;
extern edict_t *listenserver_edict;
// actual defending amount
extern int g_iDefendCount[2];
// actual attack amount
extern int g_iAttackCount[2];
extern bot_research_t g_Researched[2][NUM_RESEARCH_OPTIONS];
extern char *RoleToString(int role);
extern char *SubroleToString(int subrole);
extern bool b_chat_debug;
static FILE *fp;

void BotFixIdealPitch(edict_t *pEdict)
{
//	ALERT(at_console, "BotFixIdealPitch\n");
	// check for wrap around of angle...
	if (pEdict->v.idealpitch > 180)
		pEdict->v.idealpitch -= 360;
	
	if (pEdict->v.idealpitch < -180)
		pEdict->v.idealpitch += 360;
}


// returns the number of degrees left to turn toward ideal pitch...
float BotChangePitch( bot_t *pBot, float speed )
{
//	ALERT(at_console, "BotChangePitch\n");
	edict_t *pEdict = pBot->pEdict;
	float ideal;
	float current;
	float current_180;  // current +/- 180 degrees
	float diff;
	
	// turn from the current v_angle pitch to the idealpitch by selecting
	// the quickest way to turn to face that direction
	
	current = pEdict->v.v_angle.x;
	
	ideal = pEdict->v.idealpitch;
	
	// find the difference in the current and ideal angle
	diff = std::fabs(current - ideal);
	
	speed = speed * pBot->f_frame_time;  // angles per second
	
	// check if difference is less than the max degrees per turn
	if (diff < speed)
		speed = diff;  // just need to turn a little bit (less than max)
	
	// here we have four cases, both angle positive, one positive and
	// the other negative, one negative and the other positive, or
	// both negative.  handle each case separately...
	
	if ((current >= 0) && (ideal >= 0))  // both positive
	{
		if (current > ideal)
			current -= speed;
		else
			current += speed;
	}
	else if ((current >= 0) && (ideal < 0))
	{
		current_180 = current - 180;
		
		if (current_180 > ideal)
			current += speed;
		else
			current -= speed;
	}
	else if ((current < 0) && (ideal >= 0))
	{
		current_180 = current + 180;
		if (current_180 > ideal)
			current += speed;
		else
			current -= speed;
	}
	else  // (current < 0) && (ideal < 0)  both negative
	{
		if (current > ideal)
			current -= speed;
		else
			current += speed;
	}
	
	// check for wrap around of angle...
	if (current > 180)
		current -= 360;
	if (current < -180)
		current += 360;
	
	pEdict->v.v_angle.x = current;
	
	return diff;  // return number of degrees left to turn
}


void BotFixIdealYaw(edict_t *pEdict)
{
//	ALERT(at_console, "BotFixIdealYaw\n");
	// check for wrap around of angle...
	if (pEdict->v.ideal_yaw > 180)
		pEdict->v.ideal_yaw -= 360;
	
	if (pEdict->v.ideal_yaw < -180)
		pEdict->v.ideal_yaw += 360;
}


// returns the number of degrees left to turn toward ideal yaw...
float BotChangeYaw( bot_t *pBot, float speed )
{
//	ALERT(at_console, "BotChangeYaw\n");
	edict_t *pEdict = pBot->pEdict;
	float ideal;
	float current;
	float current_180;  // current +/- 180 degrees
	float diff;
	
	// turn from the current v_angle yaw to the ideal_yaw by selecting
	// the quickest way to turn to face that direction
	
	current = pEdict->v.v_angle.y;
	
	ideal = pEdict->v.ideal_yaw;
	
	// find the difference in the current and ideal angle
	diff = std::fabs(current - ideal);

	// speed that we can turn during this frame...
	speed = speed * pBot->f_frame_time;
	
	// check if difference is less than the max degrees per turn
	if (diff < speed)
		speed = diff;  // just need to turn a little bit (less than max)
	
	// here we have four cases, both angle positive, one positive and
	// the other negative, one negative and the other positive, or
	// both negative.  handle each case separately...
	
	if ((current >= 0) && (ideal >= 0))  // both positive
	{
		if (current > ideal)
			current -= speed;
		else
			current += speed;
	}
	else if ((current >= 0) && (ideal < 0))
	{
		current_180 = current - 180;	
		if (current_180 > ideal)
			current += speed;
		else
			current -= speed;
	}
	else if ((current < 0) && (ideal >= 0))
	{
		current_180 = current + 180;
		if (current_180 > ideal)
			current += speed;
		else
			current -= speed;
	}
	else  // (current < 0) && (ideal < 0)  both negative
	{
		if (current > ideal)
			current -= speed;
		else
			current += speed;
	}
	
	// check for wrap around of angle...
	if (current > 180)
		current -= 360;
	if (current < -180)
		current += 360;
	
	pEdict->v.v_angle.y = current;
	
	return diff;  // return number of degrees left to turn
}

void BotFixBodyAngles(edict_t *pEdict)
{
//	ALERT(at_console, "BotFixBodyAngles\n");
   // check for wrap around of angle...
   if (pEdict->v.angles.x >= 180)
      pEdict->v.angles.x -= 360 * ((int) (pEdict->v.angles.x / 360) + 1);

   if (pEdict->v.angles.x < -180)
      pEdict->v.angles.x += 360 * ((int) (-pEdict->v.angles.x / 360) + 1);

   if (pEdict->v.angles.y >= 180)
      pEdict->v.angles.y -= 360 * ((int) (pEdict->v.angles.y / 360) + 1);

   if (pEdict->v.angles.y < -180)
      pEdict->v.angles.y += 360 * ((int) (-pEdict->v.angles.y / 360) + 1);

   if (pEdict->v.angles.z >= 180)
      pEdict->v.angles.z -= 360 * ((int) (pEdict->v.angles.z / 360) + 1);

   if (pEdict->v.angles.z < -180)
      pEdict->v.angles.z += 360 * ((int) (-pEdict->v.angles.z / 360) + 1);
}


void BotFixViewAngles(edict_t *pEdict)
{
//	ALERT(at_console, "BotFixViewAngles\n");
   // check for wrap around of angle...
   if (pEdict->v.v_angle.x >= 180)
      pEdict->v.v_angle.x -= 360 * ((int) (pEdict->v.v_angle.x / 360) + 1);

   if (pEdict->v.v_angle.x < -180)
      pEdict->v.v_angle.x += 360 * ((int) (-pEdict->v.v_angle.x / 360) + 1);

   if (pEdict->v.v_angle.y >= 180)
      pEdict->v.v_angle.y -= 360 * ((int) (pEdict->v.v_angle.y / 360) + 1);

   if (pEdict->v.v_angle.y < -180)
      pEdict->v.v_angle.y += 360 * ((int) (-pEdict->v.v_angle.y / 360) + 1);

   if (pEdict->v.v_angle.z >= 180)
      pEdict->v.v_angle.z -= 360 * ((int) (pEdict->v.v_angle.z / 360) + 1);

   if (pEdict->v.v_angle.z < -180)
      pEdict->v.v_angle.z += 360 * ((int) (-pEdict->v.v_angle.z / 360) + 1);
}


bool BotFindWaypoint( bot_t *pBot )
{
//	ALERT(at_console, "BotFindWaypoint\n");
	int index, select_index;
	int team;
	PATH *pPath = nullptr;
	int path_index;
	float distance, min_distance[3];
	int min_index[3];
	
	edict_t *pEdict = pBot->pEdict;
	
	team = UTIL_GetTeam(pEdict);
	
	for (index=0; index < 3; index++)
	{
		min_distance[index] = 9999.0;
		min_index[index] = -1;
	}
	
	index = WaypointFindPath(&pPath, &path_index, pBot->curr_waypoint_index, team);
	
	while (index != -1)
	{
		// if index is not a current or recent previous waypoint...
		if ((index != pBot->curr_waypoint_index) &&
			(index != pBot->prev_waypoint_index[0]) &&
			(index != pBot->prev_waypoint_index[1]) &&
			(index != pBot->prev_waypoint_index[2]) &&
			(index != pBot->prev_waypoint_index[3]) &&
			(index != pBot->prev_waypoint_index[4]))
		{
			// find the distance from the bot to this waypoint
			distance = (pEdict->v.origin - waypoints[index].origin).Length();
			
			if (distance < min_distance[0])
			{
				min_distance[2] = min_distance[1];
				min_index[2] = min_index[1];
				
				min_distance[1] = min_distance[0];
				min_index[1] = min_index[0];
				
				min_distance[0] = distance;
				min_index[0] = index;
			}
			else if (distance < min_distance [1])
			{
				min_distance[2] = min_distance[1];
				min_index[2] = min_index[1];
				
				min_distance[1] = distance;
				min_index[1] = index;
			}
			else if (distance < min_distance[2])
			{
				min_distance[2] = distance;
				min_index[2] = index;
			}
		}
		
		// find the next path to a waypoint
		index = WaypointFindPath(&pPath, &path_index, pBot->curr_waypoint_index, team);
	}
	
	select_index = -1;
	
	// about 20% of the time choose a waypoint at random
	// (don't do this any more often than every 10 seconds)
	
	if ((RANDOM_LONG(1, 100) <= 20) &&
		(pBot->f_random_waypoint_time <= gpGlobals->time))
	{
		pBot->f_random_waypoint_time = gpGlobals->time + 10.0;
		
		if (min_index[2] != -1)
			index = RANDOM_LONG(0, 2);
		else if (min_index[1] != -1)
			index = RANDOM_LONG(0, 1);
		else if (min_index[0] != -1)
			index = 0;
		else
			return FALSE;  // no waypoints found!
		
		select_index = min_index[index];
	}
	else
	{
		// use the closest waypoint that has been recently used
		select_index = min_index[0];
	}
	
	if (select_index != -1)  // was a waypoint found?
	{
		pBot->prev_waypoint_index[4] = pBot->prev_waypoint_index[3];
		pBot->prev_waypoint_index[3] = pBot->prev_waypoint_index[2];
		pBot->prev_waypoint_index[2] = pBot->prev_waypoint_index[1];
		pBot->prev_waypoint_index[1] = pBot->prev_waypoint_index[0];
		pBot->prev_waypoint_index[0] = pBot->curr_waypoint_index;
		
		pBot->curr_waypoint_index = select_index;
		pBot->waypoint_origin = waypoints[select_index].origin;
		
		pBot->f_waypoint_time = gpGlobals->time;
		
		return TRUE;
	}
	
	return FALSE;  // couldn't find a waypoint
}


bool BotHeadTowardWaypoint( bot_t *pBot )
{
//	ALERT(at_console, "BotHeadTowardWaypoint\n");
	int i;
	Vector v_src, v_dest;
	TraceResult tr;
	int index;
	bool status;
	float waypoint_distance, min_distance;
	int team;
	float pause_time = 0.0;
	bool touching;

	edict_t *pEdict = pBot->pEdict;
	
	if (!checked_teamplay)  // check for team play...
		BotCheckTeamplay();
	
	// is team play enabled
	if (is_team_play)
		team = UTIL_GetTeam(pEdict);
	else
		team = -1;  // not team play (all waypoints are valid for everyone)
	
	// check if the bot has been trying to get to this waypoint for a while...
	if ((pBot->f_waypoint_time + 5.0) < gpGlobals->time)
	{
		pBot->curr_waypoint_index = -1;  // forget about this waypoint
		pBot->waypoint_goal = -1;  // also forget about a goal
	}

	// no goal, no goal time
	if ((pBot->waypoint_goal == -1) && (pBot->f_waypoint_goal_time > gpGlobals->time + 2) &&
		(pBot->f_waypoint_goal_time != 0.0))
		pBot->f_waypoint_goal_time = 0.0;

	// check if we need to find a waypoint...
	if (pBot->curr_waypoint_index == -1)
	{		
		// did we just come off of a ladder or are we underwater?
		if (((pBot->f_end_use_ladder_time + 2.0) > gpGlobals->time) ||
			(pBot->pEdict->v.waterlevel == 3))
		{
			// find the nearest visible waypoint
			i = WaypointFindNearest(pEdict, REACHABLE_RANGE, team);
		}
		else
		{
			// find the nearest reachable waypoint
			i = WaypointFindReachable(pEdict, REACHABLE_RANGE, team);
		}
		
		if (i == -1)
		{
			pBot->curr_waypoint_index = -1;
			return FALSE;
		}
		
		pBot->curr_waypoint_index = i;
		pBot->waypoint_origin = waypoints[i].origin;
		
		pBot->f_waypoint_time = gpGlobals->time;
	}
	else
	{	// get the bots angles as vectors
		MAKE_VECTORS( pEdict->v.v_angle );
		Vector vecToWpt = (waypoints[pBot->curr_waypoint_index].origin - pEdict->v.origin);
		// longjump toward the waypoint
		// we have to be out of water, on the ground, able to longjump, far enough away to longjump, and
		// facing pretty close to the waypoint
		if ((pEdict->v.waterlevel == 0) && (pEdict->v.flags & FL_ONGROUND) &&
			(pBot->b_longjump) && (vecToWpt.Length() >= (LONGJUMP_DISTANCE * 
			(800 / CVAR_GET_FLOAT("sv_gravity")) * 0.6)) && (pEdict->v.velocity.Length() > 220) && 
			(DotProduct(gpGlobals->v_forward, vecToWpt.Normalize()) > 0.99))
		{	// trace a hull toward the current waypoint the distance of a longjump (depending on gravity)
			UTIL_TraceHull(UTIL_GetOrigin(pEdict), UTIL_GetOrigin(pEdict) + vecToWpt.Normalize() *
				(LONGJUMP_DISTANCE * (800 / CVAR_GET_FLOAT("sv_gravity"))),
				dont_ignore_monsters, head_hull, pEdict, &tr);
			// make sure it's clear
			if (tr.flFraction >= 1.0)
			{	// trace another hull straight down so we can get ground level here
				UTIL_TraceHull(tr.vecEndPos, tr.vecEndPos - Vector(0,0,8192), dont_ignore_monsters,
					head_hull, pEdict, &tr);
				// make sure the point we found is about level with 
				if (waypoints[pBot->curr_waypoint_index].origin.z - tr.vecEndPos.z < 52)
				{
					// in S&I, they auto longjump if running
					if (mod_id != SI_DLL)	// for HLDM/Crabbed
						pEdict->v.button |= IN_DUCK;
					else if (mod_id == SI_DLL) // have to be going forward in S&I
						pEdict->v.button |= IN_FORWARD;
					// jump!
					pEdict->v.button |= IN_JUMP;
					// recognize we'll be in the air for a second most likely
					pBot->f_longjump_time = gpGlobals->time + 1.0;
					//SERVER_PRINT( "%s doing longjump!\n", STRING(pEdict->v.netname));
				}
			}
		}

		// skip this part if bot is trying to get out of water...
		if (pBot->f_exit_water_time < gpGlobals->time)
		{
			// check if we can still see our target waypoint...
			
			v_src = pEdict->v.origin + pEdict->v.view_ofs;
			v_dest = waypoints[pBot->curr_waypoint_index].origin;
			
			// trace a line from bot's eyes to destination...
			UTIL_TraceLine( v_src, v_dest, ignore_monsters,
				pEdict->v.pContainingEntity, &tr );
			
			// check if line of sight to object is blocked (i.e. not visible)
			if (tr.flFraction < 1.0)
			{
				// did we just come off of a ladder or are we under water?
				if (((pBot->f_end_use_ladder_time + 2.0) > gpGlobals->time) ||
					(pBot->pEdict->v.waterlevel == 3))
				{
					// find the nearest visible waypoint
					i = WaypointFindNearest(pEdict, REACHABLE_RANGE, team);
				}
				else
				{
					// find the nearest reachable waypoint
					i = WaypointFindReachable(pEdict, REACHABLE_RANGE, team);
				}
				
				if (i == -1)
				{
					pBot->curr_waypoint_index = -1;
					return FALSE;
				}
				pBot->curr_waypoint_index = i;
				pBot->waypoint_origin = waypoints[i].origin;
				
				pBot->f_waypoint_time = gpGlobals->time;
			}
		}
	}
	
	// doesn't have a goal waypoint OR
	// bot is engaging an enemy OR
	// is "attacking" and defending an ally OR
	// is defending a sci/rsrc currently being stolen AND
	// our waypoint time allows all of this
	if ((pBot->waypoint_goal == -1 || pBot->b_engaging_enemy || (pBot->role == ROLE_ATTACK &&
		pBot->subrole == ROLE_SUB_DEF_ALLY) || (pBot->role == ROLE_DEFEND &&
		(pBot->subrole == ROLE_SUB_DEF_SCIS || pBot->subrole == ROLE_SUB_DEF_RSRC) && pBot->pGoalEnt &&
		pBot->pGoalEnt->v.aiment && IsCarryEnt(pBot->pGoalEnt))) &&
		pBot->f_waypoint_goal_time < gpGlobals->time)
	{
		// tracking something, pick goal much more often
		if (pBot->b_engaging_enemy || pBot->pGoalEnt != nullptr || pBot->v_defend != g_vecZero ||
			pBot->defend_wpt != -1)
			pBot->f_waypoint_goal_time = gpGlobals->time + 0.5;
		else // don't pick a goal more often than every 120 seconds...
			pBot->f_waypoint_goal_time = gpGlobals->time + 120.0;
			
		index = BotFindWaypointGoal(pBot);

		if (index != -1)
			pBot->waypoint_goal = index;
	}

	// find the distance to the target waypoint
	waypoint_distance = (UTIL_GetOrigin(pEdict) - waypoints[pBot->curr_waypoint_index].origin).Length();
	
	// set the minimum distance from waypoint to be considered "touching" it
	min_distance = 20.0;
	
	// if this is a crouch waypoint, bot must be fairly close...
	if (waypoints[pBot->curr_waypoint_index].flags & W_FL_CROUCH)
		min_distance = 20.0;
	
	if (waypoints[pBot->curr_waypoint_index].flags & W_FL_JUMP)
		min_distance = 20.0;

	if (waypoints[pBot->curr_waypoint_index].flags & W_FL_DUCKJUMP)
		min_distance = 20.0;
	
	// if this is a ladder waypoint, bot must be fairly close to get on ladder
	if (waypoints[pBot->curr_waypoint_index].flags & W_FL_LADDER)
		min_distance = 20.0;
	
	// if trying to get out of water, need to get very close to waypoint...
	if (pBot->f_exit_water_time >= gpGlobals->time)
		min_distance = 20.0;
	
	touching = FALSE;
	
	// did the bot run past the waypoint? (prevent the loop-the-loop problem)
	if ((pBot->prev_waypoint_distance > 1.0) &&
		(waypoint_distance > pBot->prev_waypoint_distance) && (waypoint_distance < min_distance))
		touching = TRUE;
	
	// are we close enough to a target waypoint...
	if (waypoint_distance < min_distance)
		touching = TRUE;
	
	// save current distance as previous
	pBot->prev_waypoint_distance = waypoint_distance;
	
	if (touching)
	{
		bool waypoint_found = FALSE;

		pBot->prev_waypoint_distance = 0.0;
		// reeval our goal
		BotEvaluateGoal( pBot );
		// check if the waypoint is a door waypoint
		if (waypoints[pBot->curr_waypoint_index].flags & W_FL_DOOR)
		{
			pBot->f_dont_avoid_wall_time = gpGlobals->time + 5.0;
		}
		
		// check if the next waypoint is a jump waypoint...
		if (waypoints[pBot->curr_waypoint_index].flags & W_FL_JUMP)
		{
			pEdict->v.button |= IN_JUMP;  // jump here
		}
	
		// check if the next waypoint is a duck-jump waypoint...
		if (waypoints[pBot->curr_waypoint_index].flags & W_FL_DUCKJUMP)
		{
			pEdict->v.button |= IN_JUMP;  // jump here
			pBot->f_delay_duck_time = gpGlobals->time + 0.1;
			pBot->f_do_duck_time = pBot->f_delay_duck_time + 0.4;
		}

		// check if the waypoint is a sniper waypoint...
		if (waypoints[pBot->curr_waypoint_index].flags & W_FL_SNIPER)
		{
			int aim_index;
				
			aim_index = WaypointFindNearestAiming(waypoints[pBot->curr_waypoint_index].origin);
				
			if (aim_index != -1)
			{
				Vector v_aim = waypoints[aim_index].origin - waypoints[pBot->curr_waypoint_index].origin;
					
				Vector aim_angles = UTIL_VecToAngles( v_aim );
					
				pEdict->v.ideal_yaw = aim_angles.y;
					
				BotFixIdealYaw(pEdict);
			}
				
			pBot->f_pause_time = gpGlobals->time + RANDOM_FLOAT(20.0, 30.0);
				
			// fix f_waypoint_time so bot won't think it is stuck
			pBot->f_waypoint_time = pBot->f_pause_time;
				
			return TRUE;
		}
		
		// check if the bot has reached the goal waypoint...
		if (pBot->curr_waypoint_index == pBot->waypoint_goal)
		{	// if this waypoint has an item, make sure we get it
			if (waypoints[pBot->waypoint_goal].item[0])
			{
				pBot->pBotPickupItem = WaypointFindItem(pBot->waypoint_goal);
				pBot->f_find_item = gpGlobals->time + 0.2;
				pBot->f_last_item_found = gpGlobals->time;
			}

			pBot->exclude_points[4] = pBot->exclude_points[3];
			pBot->exclude_points[3] = pBot->exclude_points[2];
			pBot->exclude_points[2] = pBot->exclude_points[1];
			pBot->exclude_points[1] = pBot->exclude_points[0];
			pBot->exclude_points[0] = pBot->curr_waypoint_index;

			/*
			// check if the next waypoint is a defend waypoint...
			if ((waypoints[pBot->curr_waypoint_index].flags & W_FL_DEFEND) && (pBot->role == ROLE_DEFEND) &&
				(pBot->subrole = ROLE_SUB_DEF_BASE))
			{
				pBot->f_pause_time = gpGlobals->time + 5;
					
				// fix f_waypoint_time so bot won't think it is stuck
				pBot->f_waypoint_time = pBot->f_pause_time;
			}
			*/

			if ((pBot->b_engaging_enemy) || (pBot->pGoalEnt != nullptr) || (pBot->v_defend != g_vecZero) ||
				(pBot->defend_wpt != -1))
				pBot->f_waypoint_goal_time = gpGlobals->time;
			else	// a little delay time, since we'll touch the waypoint before we actually get what it has
				pBot->f_waypoint_goal_time = gpGlobals->time + 0.25;

			pBot->waypoint_goal = -1;  // forget this goal waypoint
			pBot->wpt_goal_type = WPT_GOAL_NONE;
		}
		
		// test special case of bot underwater and close to surface...
		if (pBot->pEdict->v.waterlevel == 3)
		{
			Vector v_src, v_dest;
			TraceResult tr;
			int contents;
			
			// trace a line straight up 100 units...
			v_src = pEdict->v.origin;
			v_dest = v_src;
			v_dest.z = v_dest.z + 100.0;
			
			// trace a line to destination...
			UTIL_TraceLine( v_src, v_dest, ignore_monsters,
				pEdict->v.pContainingEntity, &tr );
			
			if (tr.flFraction >= 1.0)
			{
				// find out what the contents is of the end of the trace...
				contents = POINT_CONTENTS( tr.vecEndPos );
				
				// check if the trace endpoint is in open space...
				if (contents == CONTENTS_EMPTY)
				{
					// find the nearest visible waypoint
					i = WaypointFindNearest(tr.vecEndPos, pEdict, 100, team);
					
					if (i != -1)
					{
						waypoint_found = TRUE;
						pBot->curr_waypoint_index = i;
						pBot->waypoint_origin = waypoints[i].origin;
						
						pBot->f_waypoint_time = gpGlobals->time;
						
						// keep trying to exit water for next 3 seconds
						pBot->f_exit_water_time = gpGlobals->time + 3.0;
					}
				}
			}
		}
		
		// check if the bot has a goal waypoint...
		if (pBot->waypoint_goal != -1)
		{	// are we trying to get an item?
			int nextwaypoint = -1;

			// get the next waypoint to reach item...
			if (pBot->pBotPickupItem != nullptr && pBot->item_waypoint > -1)
				nextwaypoint = pBot->item_waypoint;
			else // get the next waypoint to reach goal...
				nextwaypoint = pBot->waypoint_goal;
		
			if (!pBot->b_longjump)
			{	// get the next waypoint
				i = WaypointRouteFromTo(pBot->curr_waypoint_index,
					nextwaypoint, team);
			}
			// in S&I, we can't be carrying anything
			else if (pBot->b_longjump && (mod_id != SI_DLL ||
				(mod_id == SI_DLL && pBot->i_carry_type == CARRY_NONE)))
			{	// search ahead for a waypoint
				int lj_wpt, prev_lj_wpt, final_lj_wpt; ;
				prev_lj_wpt = final_lj_wpt = lj_wpt = pBot->curr_waypoint_index;
				int tries = 8;
				while (tries > 0)
				{
					tries--;
					prev_lj_wpt = final_lj_wpt;
					lj_wpt = WaypointRouteFromTo(lj_wpt, nextwaypoint, team);
					// make sure we can see AND get to the waypoint, this requires a hull trace
					UTIL_TraceHull(UTIL_GetOrigin(pEdict), waypoints[lj_wpt].origin,
						dont_ignore_monsters, head_hull, pEdict, &tr);
					// is this new waypoint visisble?  Is it further away than the previous waypoint?
					if (lj_wpt != -1 && (tr.flFraction < 1 ||
						(waypoints[lj_wpt].origin - UTIL_GetOrigin(pEdict)).Length() < (waypoints[prev_lj_wpt].origin - UTIL_GetOrigin(pEdict)).Length()))
					{	// if not, remember the last waypoint that was
						final_lj_wpt = prev_lj_wpt;
						break;
					}
					// remember this waypoint
					final_lj_wpt = lj_wpt;
				}
				// don't jump to waypoints 16 points higher
				// don't jump to jump, crouch, ladder, or lift waypoints
				// don't jump to waypoints that are too close
				if (waypoints[final_lj_wpt].origin.z - waypoints[pBot->curr_waypoint_index].origin.z > 16 ||
					waypoints[final_lj_wpt].flags & W_FL_JUMP || waypoints[final_lj_wpt].flags & W_FL_CROUCH ||
					waypoints[final_lj_wpt].flags & W_FL_LADDER || waypoints[final_lj_wpt].flags & W_FL_LIFT ||
					waypoints[pBot->curr_waypoint_index].flags & W_FL_JUMP ||
					waypoints[pBot->curr_waypoint_index].flags & W_FL_CROUCH ||
					waypoints[pBot->curr_waypoint_index].flags & W_FL_LADDER ||
					waypoints[pBot->curr_waypoint_index].flags & W_FL_LIFT ||
					((waypoints[final_lj_wpt].origin - UTIL_GetOrigin(pEdict)).Length() < (LONGJUMP_DISTANCE * 
					(800 / CVAR_GET_FLOAT("sv_gravity")) * 0.6)))
				{	// find the next waypoint instead
					i = WaypointRouteFromTo(pBot->curr_waypoint_index,
						nextwaypoint, team);
				}
				else
					i = final_lj_wpt;
			}

			if (i != WAYPOINT_UNREACHABLE && i < num_waypoints && i > 0)  // can we get to the goal from here?
			{
				waypoint_found = TRUE;
				pBot->curr_waypoint_index = i;
				pBot->waypoint_origin = waypoints[i].origin;
				
				pBot->f_waypoint_time = gpGlobals->time;
			}
		}
		
		if (waypoint_found == FALSE)
		{
			index = 4;
			
			// try to find the next waypoint
			while (((status = BotFindWaypoint( pBot )) == FALSE) &&
				(index > 0))
			{
				// if waypoint not found, clear oldest prevous index and try again
				
				pBot->prev_waypoint_index[index] = -1;
				index--;
			}
			
			if (status == FALSE)
			{
				if (b_chat_debug)
				{
					sprintf(pBot->debugchat, "BotHeadTowardWaypoint fail\n");
					UTIL_HostSay(pBot->pEdict, 0, pBot->debugchat);
				}
				pBot->curr_waypoint_index = -1;  // indicate no waypoint found
				
				// clear all previous waypoints...
				for (index=0; index < 5; index++)
					pBot->prev_waypoint_index[index] = -1;
				
				return FALSE;
			}
		}
	}

	// get our distance to the goal ent (if we have one)
	Vector vecOrigin;
	float flGoalDist = 9999;
	if (pBot->pGoalEnt)
	{
		vecOrigin = UTIL_GetOrigin(pBot->pGoalEnt);

		flGoalDist = (vecOrigin - pEdict->v.origin).Length();
	}

	if (!pBot->pBotEnemy &&!(pBot->pGoalEnt && flGoalDist <= pBot->f_goal_proximity &&
		((pBot->role == ROLE_ATTACK && pBot->subrole == ROLE_SUB_DEF_ALLY) || (pBot->role == ROLE_DEFEND &&
		(pBot->subrole == ROLE_SUB_DEF_SCIS || pBot->subrole == ROLE_SUB_DEF_RSRC) &&
		IsCarryEnt(pBot->pGoalEnt)))) && /*!pBot->pBotPickupItem && */pBot->f_dmg_time < gpGlobals->time)
	{
		if (!pBot->pBotPickupItem || (pBot->pBotPickupItem &&
			!(FStrEq(STRING(pBot->pBotPickupItem->v.classname), "func_healthcharger") ||
			FStrEq(STRING(pBot->pBotPickupItem->v.classname), "func_recharge"))))
		{
			// keep turning towards the waypoint...
			
			Vector v_direction = waypoints[pBot->curr_waypoint_index].origin - pEdict->v.origin;
			
			Vector v_angles = UTIL_VecToAngles(v_direction);
		
			// if the bot is NOT on a ladder, change the yaw...
			if (pEdict->v.movetype != MOVETYPE_FLY)
			{
				pEdict->v.idealpitch = -v_angles.x;
				BotFixIdealPitch(pEdict);
				
				pEdict->v.ideal_yaw = v_angles.y;
				BotFixIdealYaw(pEdict);
			}
		}
	}
	
	return TRUE;
}

void BotEvaluateGoal( bot_t *pBot )
{	// S&I has different evaluation
//	ALERT(at_console, "BotEvaluateGoal\n");
	if (mod_id == SI_DLL)
	{
		BotEvaluateGoalSI(pBot);
		return;
	}

	edict_t *pEdict = pBot->pEdict;

	// we're dying!  Forget about our goal
	if (pBot->waypoint_goal != -1 && pEdict->v.health <= 25 && pBot->wpt_goal_type != WPT_GOAL_HEALTH)
	{
		pBot->f_waypoint_goal_time = 0;
		pBot->waypoint_goal = -1;
		pBot->b_engaging_enemy = FALSE;
	}
}

// specifc goal evaluator for Science and Industry, this function decides
// a bot's subrole based on their main role, and picks what specific entity
// they are to defend/attack, etc.
void BotEvaluateGoalSI( bot_t *pBot )
{
//	ALERT(at_console, "BotEvaluateGoalSI\n");
	// to say stuff on subrole change
	int newsub = pBot->subrole;
	// misc wpt stuff
	int temp_index = -1;
	int team = -1;
	// search entities
	edict_t *pGoal = nullptr;
	edict_t *pEntity = nullptr;
	// make code smaller :P
	edict_t *pEdict = pBot->pEdict;
	// used in for/while loops
	int i = 0;
	// needed for brush ents
	Vector vecOrigin;

	if (!checked_teamplay)  // check for team play...
		BotCheckTeamplay();
	
	// is team play enabled
	if (is_team_play)
		team = UTIL_GetTeam(pEdict);

	// forget about what we're defending since we
	// figure out what to defend in this function
	pBot->pGoalEnt = nullptr;
	pBot->v_defend = g_vecZero;
	pBot->defend_wpt = -1;
	pBot->b_defend_patrol = false;

	///////////////
	// DEFENDING //
	///////////////
	if (pBot->role == ROLE_DEFEND)
	{
		int iDefSciCount = 0; // defending scientists
		int iDefLocCount = 0; // defending a location
		int iDefTechCount = 0; // defending tech breakables
		int iDefRsrcCount = 0; // defending resources
		// for checks
		bool bDefSci = false;
		bool bDefTech = false;
		bool bDefRsrc = false;
		// used later for defender counts (so we defend goal with least defenders)
		int iDefenders = 0;
		int iMinDefenders = 32;

		// figure out who is doing what
		for (i = 1; i <= gpGlobals->maxClients; i++)
		{
			bDefSci = false;
			bDefTech = false;
			bDefRsrc = false;
			temp_index = -1;

			edict_t *pPlayer = INDEXENT(i);
				
			// skip invalid players
			if ((pPlayer) && (!pPlayer->free) && (UTIL_GetTeam(pPlayer) == team) && (pPlayer != pEdict) &&
				(pPlayer->v.flags & FL_CLIENT))
			{	// figure out what our ally humans are doing
				if (!(pPlayer->v.flags & FL_FAKECLIENT))
				{	// find closest waypoint
					temp_index = WaypointFindNearest(pPlayer, 256, team);
					// look for nearby scientists, tech breakables, and resources
					while ((pEntity = UTIL_FindEntityInSphere(pEntity, pPlayer->v.origin, 512)) != nullptr)
					{
						if (UTIL_GetTeam(pEntity) != team)
							continue;

						if (!FStrEq(STRING(pEntity->v.classname), "monster_scientist") &&
							!FStrEq(STRING(pEntity->v.classname), "carry_scientist") &&
							!FStrEq(STRING(pEntity->v.classname), "func_tech_breakable") &&
							!FStrEq(STRING(pEntity->v.classname), "item_resource") &&
							!FStrEq(STRING(pEntity->v.classname), "carry_resource"))
							continue;

						if (FStrEq(STRING(pEntity->v.classname), "monster_scientist") ||
							FStrEq(STRING(pEntity->v.classname), "carry_scientist"))
							bDefSci = true;
						else if (FStrEq(STRING(pEntity->v.classname), "func_tech_breakable"))
							bDefTech = true;
						else if (FStrEq(STRING(pEntity->v.classname), "item_resource") ||
							FStrEq(STRING(pEntity->v.classname), "carry_resource"))
							bDefRsrc = true;

					}
					// determine if their closest waypoint is a defend waypoint
					if (temp_index != -1)
					{
						if (waypoints[temp_index].flags & W_FL_DEFEND)
							iDefLocCount++;
					}
					// are they defending scientists?
					if (bDefSci)
						iDefSciCount++;
					// are they defending a tech breakable?
					if (bDefTech)
						iDefTechCount++;
					// are they defending resource?
					if (bDefRsrc)
						iDefRsrcCount++;
				}
				else
				{	// loop through bots and look at their values
					bot_t *pAllyBot = UTIL_GetBotPointer(pPlayer);
					if (pAllyBot != nullptr)
					{	// must be defending
						if (pAllyBot->role == ROLE_DEFEND)
						{	// check out our subrole
							if (pAllyBot->subrole == ROLE_SUB_DEF_SCIS)
								iDefSciCount++;
							else if (pAllyBot->subrole == ROLE_SUB_DEF_BASE)
								iDefLocCount++;
							else if (pAllyBot->subrole == ROLE_SUB_DEF_RSRC)
								iDefRsrcCount++;
							else if (pAllyBot->subrole == ROLE_SUB_DEF_BREAK)
								iDefTechCount++;
						}
					}
				}
			}
		}

		float fDefSciPercent = 0;
		float fDefLocPercent = 0;
		float fDefTechPercent = 0;
		float fDefRsrcPercent = 0;

		char *szSearchEnts[] = { "carry_scientist", "monster_scientist", "carry_resource", "item_resource",
			"func_tech_breakable" };

		if (g_iDefendCount[team] > 0)
		{	// percentages of defense total that are defending scis, tech breakables, and locations
			fDefSciPercent = (float)iDefSciCount / (float)g_iDefendCount[team];
			fDefLocPercent = (float)iDefLocCount / (float)g_iDefendCount[team];
			fDefTechPercent = (float)iDefTechCount / (float)g_iDefendCount[team];
			fDefRsrcPercent = (float)iDefRsrcCount / (float)g_iDefendCount[team];
		}
/*
		SERVER_PRINT( "Sci %.2f Loc %.2f Tech %.2f Rsrc %.2f\n", fDefSciPercent, fDefLocPercent,
			fDefTechPercent, fDefRsrcPercent);
*/
		// start at 0
		i = 0;
		while (i < 5 && iMinDefenders > 0)
		{
			// this is long
			// our search ent is sci and percent is over 25 OR
			// our search ent is rsrc and percent is over 10 OR
			// our search ent is breakable and percent is over 10
			if (((FStrEq(szSearchEnts[i], "carry_scientist") ||
				FStrEq(szSearchEnts[i], "monster_scientist")) &&
				(fDefSciPercent >= 0.25)) ||
				((FStrEq(szSearchEnts[i], "carry_resource") ||
				FStrEq(szSearchEnts[i], "item_resource")) &&
				(fDefRsrcPercent >= 0.1)) ||
				((FStrEq(szSearchEnts[i], "func_tech_breakable")) &&
				(fDefTechPercent >= 0.1)))
			{
				i++;
				continue;
			}

			//iDefenders = 0;
			///iMinDefenders = 32;
			// loop through and find our goals!
			while ((pGoal = UTIL_FindEntityByClassname(pGoal, szSearchEnts[i])) != nullptr)
			{	// must be on our team
				if (UTIL_GetTeam(pGoal) != team && team != -1)
					continue;
				// can't be being carried
				//if (pGoal->v.aiment != NULL)
				//	continue;

				iDefenders = 0;
	
				vecOrigin = UTIL_GetOrigin(pGoal);

				// look around the entity we found (for non-bots)
				while ((pEntity = UTIL_FindEntityInSphere(pEntity, vecOrigin, 512)) != nullptr)
				{
					// non players and non team members are skipped
					if (!(pEntity->v.flags & FL_CLIENT) || (pEntity->v.flags & FL_FAKECLIENT) ||
						(strcmp(STRING(pEntity->v.classname), "player") != 0) ||
						!IsAlive(pEntity) ||
						UTIL_GetTeam(pEntity) != team ||
						pEntity == pEdict)
						continue;

					iDefenders++;
				}
				// check bots
				for (int bot = 0; bot < 32; bot++)
				{
					if (bots[bot].is_used && bot != pBot->index && 
						UTIL_GetTeam(bots[bot].pEdict) == team && bots[bot].pGoalEnt == pGoal)
						iDefenders++;
				}

				// defend the entity with the least defenders
				if (iDefenders < iMinDefenders)
				{
					iMinDefenders = iDefenders;
					// remember this entity
					pBot->pGoalEnt = pGoal;
					if (!IsCarryEnt(pGoal))
						pBot->b_defend_patrol = true;

					if (FStrEq(szSearchEnts[i], "carry_scientist") ||
						FStrEq(szSearchEnts[i], "monster_scientist"))
						newsub = ROLE_SUB_DEF_SCIS;
					else if (FStrEq(szSearchEnts[i], "carry_resource") ||
						FStrEq(szSearchEnts[i], "item_resource"))
						newsub = ROLE_SUB_DEF_RSRC;
					else if (FStrEq(szSearchEnts[i], "func_tech_breakable"))
						newsub = ROLE_SUB_DEF_BREAK;
				}
			}

			i++;
		}

		// couldn't find a scientist, breakable, or resource to defend
		if (pBot->pGoalEnt == nullptr)
		{	// reset these
			temp_index = -1;
			iDefenders = 0;
			iMinDefenders = 32;
			int exclude_points[6];
			for (i = 0; i < 6; i++)
				exclude_points[i] = -1;
			// for finding location to defend
			for (i = 0; i < num_def_waypoints; i++)
			{
				temp_index = WaypointFindNearestGoal(pEdict, pBot->curr_waypoint_index, team,
					W_FL_DEFEND, exclude_points);

				if (temp_index != -1)
				{
					exclude_points[4] = exclude_points[3];
					exclude_points[3] = exclude_points[2];
					exclude_points[2] = exclude_points[1];
					exclude_points[1] = exclude_points[0];
					exclude_points[0] = temp_index;

					iDefenders = 0;
					// look around the waypoint we found for non-bots
					while ((pEntity = UTIL_FindEntityInSphere(pEntity, waypoints[temp_index].origin, 128)) != nullptr)
					{
						// non players and non team members are skipped
						if (!(pEntity->v.flags & FL_CLIENT) ||
							(pEntity->v.flags & FL_FAKECLIENT) ||
							(strcmp(STRING(pEntity->v.classname), "player") != 0) ||
							!IsAlive(pEntity) ||
							UTIL_GetTeam(pEntity) != team ||
							pEntity == pEdict)
							continue;

						iDefenders++;
					}
					// examine bots to see if they're defending this point
					for (int bot = 0; bot < 32; bot++)
					{
						if (bots[bot].is_used && bot != bots[bot].index && 
							UTIL_GetTeam(bots[bot].pEdict) == team && bots[bot].defend_wpt == temp_index)
							iDefenders++;
					}

					// defend the closest location with the least defenders
					if (iDefenders < iMinDefenders)
					{	// add this waypoint into the exclusion
						iMinDefenders = iDefenders;
						// remember this location
						pBot->v_defend = waypoints[temp_index].origin;
						pBot->b_defend_patrol = true;
						pBot->defend_wpt = temp_index;

						newsub = ROLE_SUB_DEF_BASE;
					}
				}
			}
		}
	}
	///////////////
	// ATTACKING //
	///////////////
	if (pBot->role == ROLE_ATTACK)
	{
		int iAttSciCount = 0; // attacking scientists
		int iAttTechCount = 0; // attacking tech breakables
		int iAttRsrcCount = 0; // attacking resources
		int iAttAllyCount = 0; // attacking ally, er defending ally
		// for checks
		bool bAttSci = false;
		bool bAttTech = false;
		bool bAttRsrc = false;
		bool bAttAlly = false;
		// used later for a counts (so we attack goal with least attackers)
		int iAttackers = 0;
		int iMinAttackers = 32;

		// figure out who is doing what
		for (int i = 1; i <= gpGlobals->maxClients; i++)
		{
			bAttSci = false;
			bAttTech = false;
			bAttRsrc = false;
			bAttAlly = false;
			temp_index = -1;

			edict_t *pPlayer = INDEXENT(i);
				
			// skip invalid players
			if ((pPlayer) && (!pPlayer->free) && (UTIL_GetTeam(pPlayer) == team) && (pPlayer != pEdict) &&
				(pPlayer->v.flags & FL_CLIENT))
			{	// figure out what our ally humans are doing
				if (!(pPlayer->v.flags & FL_FAKECLIENT))
				{
					// look for nearby scientists, tech breakables, resources, and players
					while ((pEntity = UTIL_FindEntityInSphere(pEntity, pPlayer->v.origin, 512)) != nullptr)
					{
						if (pEntity == pPlayer)
							continue;

						if (!FStrEq(STRING(pEntity->v.classname), "monster_scientist") &&
							!FStrEq(STRING(pEntity->v.classname), "carry_scientist") &&
							!FStrEq(STRING(pEntity->v.classname), "func_tech_breakable") &&
							!FStrEq(STRING(pEntity->v.classname), "item_resource") &&
							!FStrEq(STRING(pEntity->v.classname), "carry_resource") &&
							!FStrEq(STRING(pEntity->v.classname), "player"))
							continue;

						if ((UTIL_GetTeam(pEntity) == team &&
							!FStrEq(STRING(pEntity->v.classname), "player")) ||
							(UTIL_GetTeam(pEntity) != team &&
							FStrEq(STRING(pEntity->v.classname), "player")))
							continue;

						if (FStrEq(STRING(pEntity->v.classname), "monster_scientist") ||
							FStrEq(STRING(pEntity->v.classname), "carry_scientist"))
							bAttSci = true;
						else if (FStrEq(STRING(pEntity->v.classname), "func_tech_breakable"))
							bAttTech = true;
						else if (FStrEq(STRING(pEntity->v.classname), "item_resource") ||
							FStrEq(STRING(pEntity->v.classname), "carry_resource"))
							bAttRsrc = true;
						else if (FStrEq(STRING(pEntity->v.classname), "player"))
							bAttAlly = true;
					}

					// are they attacking scientists?
					if (bAttSci)
						iAttSciCount++;
					// are they attacking a tech breakable?
					if (bAttTech)
						iAttTechCount++;
					// are they attacking resources?
					if (bAttRsrc)
						iAttRsrcCount++;
					// are they defending an ally?
					if (bAttAlly)
						iAttAllyCount++;
					// assume sci if we can't determine
					if (!bAttSci && !bAttTech && !bAttRsrc && !bAttAlly)
						iAttSciCount++;
				}
				else
				{	// loop through bots and look at their values
					bot_t *pAllyBot = UTIL_GetBotPointer(pPlayer);
					if (pAllyBot != nullptr)
					{	// must be attacking
						if (pAllyBot->role == ROLE_ATTACK)
						{	// check out our subrole
							if (pAllyBot->subrole == ROLE_SUB_ATT_GET_SCI ||
								pAllyBot->subrole == ROLE_SUB_ATT_RTRN_SCI)
								iAttSciCount++;
							else if (pAllyBot->subrole == ROLE_SUB_ATT_GET_RSRC ||
								pAllyBot->subrole == ROLE_SUB_ATT_RTRN_RSRC)
								iAttRsrcCount++;
							else if (pAllyBot->subrole == ROLE_SUB_ATT_BREAK)
								iAttTechCount++;
							else if (pAllyBot->subrole == ROLE_SUB_DEF_ALLY)
								iAttAllyCount++;
						}
					}
				}
			}
		}

		float fAttSciPercent = 0;
		float fAttAllyPercent = 0;
		float fAttTechPercent = 0;
		float fAttRsrcPercent = 0;

		char *szSearchEnts[] = { "carry_scientist", "monster_scientist", "player", "carry_resource", "item_resource",
			"func_tech_breakable"};

		if (g_iAttackCount[team] > 0)
		{	// percentages of attack total that are attack scis, rsrcs, tech breakables, and defending players
			fAttSciPercent = (float)iAttSciCount / (float)g_iAttackCount[team];
			fAttAllyPercent = (float)iAttAllyCount / (float)g_iAttackCount[team];
			fAttTechPercent = (float)iAttTechCount / (float)g_iAttackCount[team];
			fAttRsrcPercent = (float)iAttRsrcCount / (float)g_iAttackCount[team];
		}
/*
		SERVER_PRINT( "%s got percents: Sci %.2f Ally %.2f Tech %.2f Rsrc %.2f\n", pBot->name, 
			fAttSciPercent, fAttAllyPercent, fAttTechPercent, fAttRsrcPercent);
*/
		float distance = 0;
		float mindistance = 9999;

		if (pBot->i_carry_type != CARRY_NONE)
		{
			// find our goal
			while ((pGoal = UTIL_FindEntityByClassname(pGoal, "info_administrator")) != nullptr)
			{
				// skip enemy admin
				if (UTIL_GetTeam(pGoal) != team)
					continue;

				temp_index = WaypointFindNearest(pGoal, 512, team);
				distance = WaypointDistanceFromTo(pBot->curr_waypoint_index, temp_index, team);
				if (distance < mindistance)
				{
					pBot->pGoalEnt = pGoal;
					pBot->f_goal_proximity = 0.0;
					if (pBot->i_carry_type == CARRY_SCI)
						newsub = ROLE_SUB_ATT_RTRN_SCI;
					else if (pBot->i_carry_type >= CARRY_RSRC)
						newsub = ROLE_SUB_ATT_RTRN_RSRC;
				}
			}
		}
		else
		{
			// start at 0
			i = 0;
			while (i < 6 && pBot->pGoalEnt == nullptr)
			{
				// this is long
				// our search ent is sci and percent is over 40 OR
				// our search ent is player and percent is over 40 OR
				// our search ent is rsrc and percent is over 10 OR
				// our search ent is breakable and percent is over 10
				if (((FStrEq(szSearchEnts[i], "carry_scientist") ||
					FStrEq(szSearchEnts[i], "monster_scientist")) &&
					(fAttSciPercent >= 0.50)) ||
					/*((FStrEq(szSearchEnts[i], "player")) &&
					(fAttAllyPercent >= 0.25)) ||*/
					((FStrEq(szSearchEnts[i], "carry_resource") ||
					FStrEq(szSearchEnts[i], "item_resource")) &&
					(fAttRsrcPercent >= 0.15)) ||
					((FStrEq(szSearchEnts[i], "func_tech_breakable")) &&
					(fAttTechPercent >= 0.1)))
				{
					i++;
					continue;
				}

				iAttackers = 0;
				iMinAttackers = 32;
				distance = 0;
				mindistance = 9999;
				// loop through and find our goals!
				while ((pGoal = UTIL_FindEntityByClassname(pGoal, szSearchEnts[i])) != nullptr)
				{	// so we don't defend ourselves
					if (pGoal == pEdict)
						continue;
					// must not be on our team (unless ally player or carry scientist)
					if (((UTIL_GetTeam(pGoal) == team &&
						!FStrEq(szSearchEnts[i], "player")) ||
						(UTIL_GetTeam(pGoal) != team &&
						FStrEq(szSearchEnts[i], "player"))) &&
						team != -1)
						continue;
					// don't defend other bots that are defend other allies
					// prevents two ally defenders from defending one another
					// or defending a roled defender
					bot_t *pAllyBot = UTIL_GetBotPointer(pGoal);
					if (pAllyBot && (pAllyBot->role == ROLE_DEFEND || pAllyBot->subrole == ROLE_SUB_DEF_ALLY))
						continue;
					// can't be being carried by own team
					if (pGoal->v.aiment && UTIL_GetTeam(pGoal->v.aiment) == team)
						continue;

					iAttackers = 0;

					vecOrigin = UTIL_GetOrigin(pGoal);

					// look around the entity we found (for non-bots)
					while ((pEntity = UTIL_FindEntityInSphere(pEntity, vecOrigin, 512)) != nullptr)
					{
						// non players and non team members are skipped
						if (!(pEntity->v.flags & FL_CLIENT) || (pEntity->v.flags & FL_FAKECLIENT) ||
							(strcmp(STRING(pEntity->v.classname), "player") != 0) ||
							!IsAlive(pEntity) ||
							UTIL_GetTeam(pEntity) != team ||
							pEntity == pEdict)
							continue;

						iAttackers++;
					}
					// check bots
					for (int bot = 0; bot < 32; bot++)
					{
						if (bots[bot].is_used && bot != pBot->index && 
							UTIL_GetTeam(bots[bot].pEdict) == team && bots[bot].pGoalEnt == pGoal)
							iAttackers++;
					}
					temp_index = WaypointFindNearest(pGoal, 512, team);
					distance = WaypointDistanceFromTo(pBot->curr_waypoint_index, temp_index, team);
					distance += (vecOrigin - waypoints[temp_index].origin).Length();
					// attack the entity with the least attackers
					if (((iAttackers <= iMinAttackers) && (distance < mindistance)) ||
						((iAttackers < iMinAttackers) && (distance > mindistance)))
					{
						iMinAttackers = iAttackers;
						mindistance = distance;
						// remember this entity
						pBot->pGoalEnt = pGoal;

						if (FStrEq(szSearchEnts[i], "carry_scientist") ||
							FStrEq(szSearchEnts[i], "monster_scientist"))
							newsub = ROLE_SUB_ATT_GET_SCI;
						else if (FStrEq(szSearchEnts[i], "carry_resource") ||
							FStrEq(szSearchEnts[i], "item_resource"))
							newsub = ROLE_SUB_ATT_GET_RSRC;
						else if (FStrEq(szSearchEnts[i], "func_tech_breakable"))
							newsub = ROLE_SUB_ATT_BREAK;
//						else if (FStrEq(szSearchEnts[i], "player"))
//							newsub = ROLE_SUB_DEF_ALLY;
					}
				}

				i++;
			}
		}
	}

	if (pBot->subrole != newsub)
	{
		char szSub[64];
		pBot->subrole = newsub;
		// so we get a new goal based on the new sub
		pBot->waypoint_goal = -1;
		pBot->f_waypoint_goal_time = -1;

		if (b_chat_debug)
		{
			sprintf(szSub, "Changing role to %s...", SubroleToString(pBot->subrole));
			// all teams for now (WILL CHANGE FOR PUBLIC RELEASE)
			UTIL_HostSay(pEdict, 0/*team+1*/, szSub);
		}
	}
}

int BotFindWaypointGoal( bot_t *pBot )
{	// call S&I specific function if this is S&I
//	ALERT(at_console, "BotFindWaypointGoal\n");
	if (mod_id == SI_DLL)
		return BotFindWaypointGoalSI( pBot );

	// misc wpt stuff
	int index = -1, temp_index = -1;
	int team = -1;
	int count = 0;
	// make code smaller :P
	edict_t *pEdict = pBot->pEdict;
	// for ammo info
	int select_index = 0;
	bot_weapon_select_t *pSelect = nullptr;
	pSelect = WeaponGetSelectPointer();
	// for looping and finding the closest waypoint
	float distance;
	float mindistance = 9999;
	// used in for/while loops
	int i = 0;
	// current weapon
	int iId = pBot->current_weapon.iId;

	if (pSelect == nullptr)
		return -1;

	if (!checked_teamplay)  // check for team play...
		BotCheckTeamplay();
	
	// is team play enabled
	if (is_team_play)
		team = UTIL_GetTeam(pEdict);

	int random = RANDOM_LONG(1,100);
	int health_chance = floor((pBot->max_health - pEdict->v.health));
	// this forces to get more health if it's less than 25
	if (health_chance != 0) health_chance += 25;
	if (random < health_chance)
	{	// look for health if we're pretty dead
		index = WaypointFindNearestGoal(pEdict, pBot->curr_waypoint_index,
			team, W_FL_HEALTH);

		while ((index == -1) && (count < 3))
		{
			index = WaypointFindRandomGoal(pEdict, team, W_FL_HEALTH);

			count++;
		}
		// clear our count
		count = 0;

		if (index != -1)
			pBot->wpt_goal_type = WPT_GOAL_HEALTH;

		return index;
	}

	if (!pBot->b_engaging_enemy)
	{	// only if not engaging
		if (pEdict->v.health < pBot->max_health)
		{	// find nearest health
			temp_index = WaypointFindNearestGoal(pEdict, pBot->curr_waypoint_index,
				team, W_FL_HEALTH);

			while ((temp_index == -1) && (count < 3))
			{
				temp_index = WaypointFindRandomGoal(pEdict, team, W_FL_HEALTH);

				count++;
			}
			// clear our count
			count = 0;
			// get distance
			if (temp_index > -1 && temp_index < num_waypoints)
			{
				distance = WaypointDistanceFromTo(pBot->curr_waypoint_index, temp_index, team);

				if (distance < mindistance)
				{
					mindistance = distance;
					index = temp_index;
					pBot->wpt_goal_type = WPT_GOAL_HEALTH;
				}
			}
		}

		if (pEdict->v.armorvalue < 100)
		{	// find nearest armor
			temp_index = WaypointFindNearestGoal(pEdict, pBot->curr_waypoint_index,
				team, W_FL_ARMOR);

			while ((temp_index == -1) && (count < 3))
			{
				temp_index = WaypointFindRandomGoal(pEdict, team, W_FL_ARMOR);

				count++;
			}
			// clear our count
			count = 0;
			// get distance
			if (temp_index > -1 && temp_index < num_waypoints)
			{
				distance = WaypointDistanceFromTo(pBot->curr_waypoint_index, temp_index, team);

				if (distance < mindistance)
				{
					mindistance = distance;
					index = temp_index;
					pBot->wpt_goal_type = WPT_GOAL_ARMOR;
				}
			}
		}

		if (!pBot->b_longjump)
		{	// find a longjump module
			temp_index = WaypointFindNearestGoal(pEdict, pBot->curr_waypoint_index,
				team, W_FL_ITEM, "item_longjump");

			while ((temp_index == -1) && (count < 3))
			{
				index = WaypointFindRandomGoal(pEdict, team, W_FL_ITEM, "item_longjump" );
				count++;
			}
			count = 0;
			// get distance
			if (temp_index > -1 && temp_index < num_waypoints)
			{
				distance = WaypointDistanceFromTo(pBot->curr_waypoint_index, temp_index, team);

				if (distance < mindistance)
				{
					mindistance = distance;
					index = temp_index;
					pBot->wpt_goal_type = WPT_GOAL_ITEM;
				}
			}
		}

		select_index = WeaponGetSelectIndex(iId);
		// our first priority is to check if our current weapon has low ammo
		float primary_assess = BotAssessPrimaryAmmo(pBot, iId);
		float secondary_assess = BotAssessSecondaryAmmo(pBot, iId);	
		// final weapon to use
		int final_index = select_index;
		// to find the closest ammo waypoint
		int ammo_wpt[3] = {-1,-1,-1};
		float ammo_dist[3] = {0,0,0};
		float pri = 1.0;
		float sec = 1.0;
		float minpri = 1.0;
		float minsec = 1.0;

		//if (primary_assess > AMMO_LOW)
		{	// only go for a new weapon if this weapon has enough ammo
			select_index = 0;
			// loop through all weapons and find the closest one we
			// don't currently have
			while (pSelect[select_index].iId)
			{
				// bot shouldn't be carrying this weapon
				if (!(pEdict->v.weapons & (1<<pSelect[select_index].iId)))
				{
					// find our weapon
					temp_index = WaypointFindNearestGoal(pEdict, pBot->curr_waypoint_index, team, W_FL_WEAPON, pSelect[select_index].weapon_name);
							
					if (temp_index != -1)
					{
						distance = WaypointDistanceFromTo(pBot->curr_waypoint_index, temp_index, team); 
						if (distance < mindistance)
						{
							index = temp_index;
							mindistance = distance;
							pBot->wpt_goal_type = WPT_GOAL_WEAPON;
						}
					}
				}
				select_index++;
			}
		}

		if (((primary_assess == AMMO_NONE) || (primary_assess > AMMO_LOW)) && 
			((secondary_assess == AMMO_NONE) || (secondary_assess > AMMO_LOW)))
		{
			select_index = WeaponGetSelectIndex(WeaponGetBest());

			while (pSelect[select_index].iId)
			{
				// bot should be carrying this weapon
				if ((pEdict->v.weapons & (1<<pSelect[select_index].iId)))
				{
					pri = BotAssessPrimaryAmmo(pBot, pSelect[select_index].iId);
					sec = BotAssessSecondaryAmmo(pBot, pSelect[select_index].iId);
					// (have primary ammo type AND it's less than minimum) ||
					// (have secondary ammo type AND it's less than minimum)
					if (((pri != AMMO_NONE) && (pri < minpri)) || ((sec != AMMO_NONE) && (sec < minsec)))
					{	// primary is less?
						if ((pri != AMMO_NONE) && (pri < minpri))
							minpri = pri;
						// secondary is less?
						if ((sec != AMMO_NONE) && (sec < minsec))
							minsec = sec;
						// either way
						final_index = select_index;
					}
				}  						
				select_index++;
			}
		}

		primary_assess = BotAssessPrimaryAmmo(pBot, pSelect[final_index].iId);
		secondary_assess = BotAssessSecondaryAmmo(pBot, pSelect[final_index].iId);	

		// get primary ammo if it is lower (or equal)
		if ((primary_assess <= secondary_assess) && (primary_assess < AMMO_MAX) &&
			(primary_assess != AMMO_NONE))
		{
			// find possible ammo points
			for (i = 0; i < 3; i++)
			{
				if ((i != 2) && (pSelect[final_index].primary_ammo_names[i][0]))
				{
					ammo_wpt[i] = WaypointFindNearestGoal(pEdict, pBot->curr_waypoint_index,
						team, W_FL_AMMO, pSelect[final_index].primary_ammo_names[i]);
				}
				else if (i == 2 && CVAR_GET_FLOAT("mp_weaponstay") == 0)
				{	// look for weapon itself (if weapon stay is off)
					ammo_wpt[i] = WaypointFindNearestGoal(pEdict, pBot->curr_waypoint_index,
						team, W_FL_WEAPON, pSelect[final_index].weapon_name);
				}
				// if we found a valid ammo waypoint, get it's distance
				if (ammo_wpt[i] != -1)
					ammo_dist[i] = WaypointDistanceFromTo(pBot->curr_waypoint_index, ammo_wpt[i], team);
			}
			// find the closest ammo point of points that were found
			for (i = 0; i < 3; i++)
			{
				if ((ammo_wpt[i] != -1) && (ammo_dist[i] < mindistance))
				{
					mindistance = ammo_dist[i];
					index = ammo_wpt[i];
					pBot->wpt_goal_type = WPT_GOAL_AMMO;
				}
			}
		}
		else if ((secondary_assess < AMMO_MAX) && (secondary_assess != AMMO_NONE))
		{
			// find possible ammo points
			for (i = 0; i < 2; i++)
			{
				if ((pSelect[final_index].secondary_ammo_names[i][0]))
				{
					ammo_wpt[i] = WaypointFindNearestGoal(pEdict, pBot->curr_waypoint_index,
						team, W_FL_AMMO, pSelect[final_index].secondary_ammo_names[i]);
				}
				// if we found a valid ammo waypoint, get it's dsitance
				if (ammo_wpt[i] != -1)
					ammo_dist[i] = WaypointDistanceFromTo(pBot->curr_waypoint_index, ammo_wpt[i], team);
			}
			// find the closest ammo point of points that were found
			for (i = 0; i < 2; i++)
			{
				if ((ammo_wpt[i] != -1) && (ammo_dist[i] < mindistance))
				{
					mindistance = ammo_dist[i];
					index = ammo_wpt[i];
					pBot->wpt_goal_type = WPT_GOAL_AMMO;
				}
			}
		}
	}
	else if (pBot->b_engaging_enemy && pBot->pBotEnemy)
	{	// find a waypoint near our enemy
		index = WaypointFindNearest(pBot->pBotEnemy, 512, team);

		if (index != -1)
		{
			if (b_chat_debug)
			{
				sprintf(pBot->debugchat, "I found a waypoint goal for enemy at %i!\n", int(pBot->name), index);
				UTIL_HostSay(pBot->pEdict, 0, pBot->debugchat);
			}
			pBot->wpt_goal_type = WPT_GOAL_ENEMY;
		}
	}

	// we couldn't find ANYTHING, go somewhere random
	if (index == -1)
	{
		index = WaypointFindRandomGoal(pEdict, team, 0);

		if (index != -1)
			pBot->wpt_goal_type = WPT_GOAL_NONE;
	}


	if (index != -1 && b_chat_debug)
	{
		switch (pBot->wpt_goal_type)
		{
			case WPT_GOAL_HEALTH:
				sprintf(pBot->debugchat, "I am going for some health!\n");
				break;
			case WPT_GOAL_ARMOR:
				sprintf(pBot->debugchat, "I am going for some armor!\n");
				break;
			case WPT_GOAL_WEAPON:
				sprintf(pBot->debugchat, "I am going for a weapon!\n");
				break;
			case WPT_GOAL_AMMO:
				sprintf(pBot->debugchat, "I am going for some ammo!\n");
				break;
			case WPT_GOAL_ITEM:
				sprintf(pBot->debugchat, "I am going for an item!\n");
				break;
			case WPT_GOAL_ENEMY:
				sprintf(pBot->debugchat, "I am tracking/engaging an enemy!\n");
				break;
			default:
				sprintf(pBot->debugchat, "I have an unknown goal!\n");
				break;

			UTIL_HostSay(pBot->pEdict, 0, pBot->debugchat);
		}
	}

	return index;
}

// Science and Industry goal finder is
// Unlike HLDM/Crabbed, this function just finds
// a goal waypoint based on the bot's role
// it doesn't actually tell them what
// to do!
int BotFindWaypointGoalSI( bot_t *pBot )
{
//	ALERT(at_console, "BotFindWaypointGoalSI\n");
	// misc wpt stuff
	int index = -1;
	int temp_index = -1;
	int team = -1;
	int count = 0;
	// search ents
	edict_t *pEntity = nullptr;
	// make code smaller :P
	edict_t *pEdict = pBot->pEdict;
	// distance checks
	float distance = 0;
	float mindistance = 9999;
	// for weapon goals
	bot_weapon_select_t *pSelect = nullptr;
	pSelect = WeaponGetSelectPointer();

	if (!checked_teamplay)  // check for team play...
		BotCheckTeamplay();
	
	// is team play enabled
	if (is_team_play)
		team = UTIL_GetTeam(pEdict);
	// stop trying to get stuff if it's been 10 seconds
	if (!pBot->b_equipped && pBot->f_equip_time < (gpGlobals->time - 10) && pBot->f_equip_time != -1)
	{	// for debugging
		if (b_chat_debug)
			UTIL_HostSay(pEdict, 0, "I've got my goods");
		pBot->b_equipped = true;
		pBot->f_equip_time = 0;
	}
	///////////////
	// DEFENDING //
	///////////////
	if (pBot->b_equipped)
	{
		if (pBot->role == ROLE_DEFEND)
		{	// check ammo count if we're defending
			pBot->b_equipped = false;
			pBot->f_equip_time = gpGlobals->time + 20.0;
			int select_index = 0;
			while (pSelect[select_index].iId)
			{
				if (pSelect[select_index].iId == SI_WEAPON_BRIEFCASE)
				{
					select_index++;
					continue;
				}
				// is the bot NOT carrying this weapon?
				if (!(pEdict->v.weapons & (1<<pSelect[select_index].iId)))
				{
					select_index++;  // skip to next weapon
					continue;
				}
				// higher than critical, we can attack
				if (BotAssessPrimaryAmmo(pBot, pSelect[select_index].iId) != AMMO_CRITICAL)
				{
					pBot->b_equipped = true;
					pBot->f_equip_time = 0;
					break;
				}

				select_index++;
			}
			
			// these subroles all have specific entities for the bot to defend
			if (((pBot->subrole == ROLE_SUB_DEF_SCIS) || (pBot->subrole == ROLE_SUB_DEF_RSRC) ||
				(pBot->subrole == ROLE_SUB_DEF_BREAK)) && (pBot->pGoalEnt != nullptr))
			{	// find the closest waypoint to our defense entity
				index = WaypointFindNearest(pBot->pGoalEnt, 512, team, pBot->exclude_points);
				//index = WaypointFindNearestGoal(pEdict, temp_index, team, 0, pBot->exclude_points);

				if (index == -1)
				{
					for (int i = 0; i < 6; i++)
						pBot->exclude_points[i] = -1;
				}
			}
			// these subroles all have specific locations for the bot to defend
			else if (((pBot->subrole = ROLE_SUB_DEF_BASE)) && (pBot->v_defend != g_vecZero))
			{
				index = WaypointFindNearest(pBot->v_defend, pEdict, 512, team, pBot->exclude_points);
				//index = WaypointFindNearestGoal(pBot->v_defend, pEdict, 512, team, 0, pBot->exclude_points);

				if (index == -1)
				{
					for (int i = 0; i < 6; i++)
						pBot->exclude_points[i] = -1;
				}
			}
		}
		///////////////
		// ATTACKING //
		///////////////
		else if (pBot->role == ROLE_ATTACK)
		{
			if (pBot->pGoalEnt != nullptr)
			{	// find the closest waypoint to our attack entity
				index = WaypointFindNearest(pBot->pGoalEnt, 512, team, pBot->exclude_points);
				//index = WaypointFindNearestGoal(pEdict, temp_index, team, 0, pBot->exclude_points);

				if (index == -1)
				{
					for (int i = 0; i < 6; i++)
						pBot->exclude_points[i] = -1;
				}
			}
		}
	}
	// we're not equipped, that is our first priority
	// only try to equip ourselves for 20 seconds (or if equip time is -1, which forces full equip)
	else if ((!pBot->b_equipped) && ((pBot->f_equip_time >= (gpGlobals->time - 20)) ||
		(pBot->f_equip_time == -1)))
	{	// find the closest weapon we don't have
		int select_index = 0;
		while (pSelect[select_index].iId)
		{
			// skip if not researched/stolen or disabled
			if ((!g_Researched[team][WeaponIdToResearch(pSelect[select_index].iId)].researched &&
				!g_Researched[team][WeaponIdToResearch(pSelect[select_index].iId)].stolen) ||
				(g_Researched[team][WeaponIdToResearch(pSelect[select_index].iId)].disabled))
			{
				select_index++;
				continue;
			}


			// is the bot carrying this weapon and weapon has full ammo?
			if (pEdict->v.weapons & (1<<pSelect[select_index].iId) &&
				BotAssessPrimaryAmmo(pBot, pSelect[select_index].iId) >= 1.0)
			{
				select_index++;  // skip to next weapon
				continue;
			}

			distance = 0;
			mindistance = 9999;
			// find our weapon (weapon_experimental)
			temp_index = WaypointFindNearestWeapon(pEdict, pBot->curr_waypoint_index, team,
				W_FL_WEAPON, pSelect[select_index].iId);
			// failed?
			if (temp_index == -1)
			{	// find our weapon (weapon_generic)
				temp_index = WaypointFindNearestGoal(pEdict, pBot->curr_waypoint_index, team, W_FL_WEAPON,
					"weapon_generic");
			}
						
			if (temp_index != -1)
			{
				distance = WaypointDistanceFromTo(pBot->curr_waypoint_index, temp_index, team); 
				if (distance < mindistance)
				{
					if (b_chat_debug)
					{
						sprintf(pBot->debugchat, "My goal is %s - %f\n", pSelect[select_index].weapon_name,
							BotAssessPrimaryAmmo(pBot, pSelect[select_index].iId));
						UTIL_HostSay(pBot->pEdict, 0, pBot->debugchat);
					}
					index = temp_index;
					mindistance = distance;

					pBot->wpt_goal_type = WPT_GOAL_WEAPON;
				}
			}

			select_index++;
		}

		if (index == -1 && pEdict->v.armorvalue < pBot->max_armor)
		{	// get armor if it's not full
			index = WaypointFindNearestGoal(pEdict, pBot->curr_waypoint_index,
				team, W_FL_ARMOR);
			// if we can't find a nearest, find a random
			while ((index == -1) && (count < 3))
			{
				index = WaypointFindRandomGoal(pEdict, team, W_FL_ARMOR);

				count++;
			}
			// clear our count
			count = 0;

			if (index != -1)
				pBot->wpt_goal_type = WPT_GOAL_ARMOR;
		}

		// couldn't find anything, or didn't need to, consider equipped
		if (index == -1)
		{	// for debugging
			if (b_chat_debug)
				UTIL_HostSay(pEdict, 0, "I've got my goods");
			pBot->b_equipped = true;
			pBot->f_equip_time = 0;
			pBot->f_evaluate_goal_time = pBot->f_role_check = 0;
		}
	}

	return index;
}

void BotOnLadder( bot_t *pBot, float moved_distance )
{
//	ALERT(at_console, "BotOnLadder\n");
   Vector v_src, v_dest, view_angles;
   TraceResult tr;
   float angle = 0.0;
   bool done = FALSE;

   edict_t *pEdict = pBot->pEdict;

   if (pBot->curr_waypoint_index == -1)
   {
	   // check if the bot has JUST touched this ladder...
	   if (pBot->ladder_dir == LADDER_UNKNOWN)
	   {
		   // try to square up the bot on the ladder...
		   while ((!done) && (angle < 180.0))
		   {
			   // try looking in one direction (forward + angle)
			   view_angles = pEdict->v.v_angle;
			   view_angles.y = pEdict->v.v_angle.y + angle;
			   
			   if (view_angles.y < 0.0)
				   view_angles.y += 360.0;
			   if (view_angles.y > 360.0)
				   view_angles.y -= 360.0;
			   
			   MAKE_VECTORS( view_angles );
			   
			   v_src = pEdict->v.origin + pEdict->v.view_ofs;
			   v_dest = v_src + gpGlobals->v_forward * 30;
			   
			   UTIL_TraceLine( v_src, v_dest, dont_ignore_monsters,
				   pEdict->v.pContainingEntity, &tr);
			   
			   if (tr.flFraction < 1.0)  // hit something?
			   {
				   if (strcmp("func_wall", STRING(tr.pHit->v.classname)) == 0)
				   {
					   // square up to the wall...
					   view_angles = UTIL_VecToAngles(tr.vecPlaneNormal);
					   
					   // Normal comes OUT from wall, so flip it around...
					   view_angles.y += 180;
					   
					   if (view_angles.y > 180)
						   view_angles.y -= 360;
					   
					   pEdict->v.ideal_yaw = view_angles.y;
					   
					   BotFixIdealYaw(pEdict);
					   
					   done = TRUE;
				   }
			   }
			   else
			   {
				   // try looking in the other direction (forward - angle)
				   view_angles = pEdict->v.v_angle;
				   view_angles.y = pEdict->v.v_angle.y - angle;
				   
				   if (view_angles.y < 0.0)
					   view_angles.y += 360.0;
				   if (view_angles.y > 360.0)
					   view_angles.y -= 360.0;
				   
				   MAKE_VECTORS( view_angles );
				   
				   v_src = pEdict->v.origin + pEdict->v.view_ofs;
				   v_dest = v_src + gpGlobals->v_forward * 30;
				   
				   UTIL_TraceLine( v_src, v_dest, dont_ignore_monsters,
					   pEdict->v.pContainingEntity, &tr);
				   
				   if (tr.flFraction < 1.0)  // hit something?
				   {
					   if (strcmp("func_wall", STRING(tr.pHit->v.classname)) == 0)
					   {
						   // square up to the wall...
						   view_angles = UTIL_VecToAngles(tr.vecPlaneNormal);
						   
						   // Normal comes OUT from wall, so flip it around...
						   view_angles.y += 180;
						   
						   if (view_angles.y > 180)
							   view_angles.y -= 360;
						   
						   pEdict->v.ideal_yaw = view_angles.y;
						   
						   BotFixIdealYaw(pEdict);
						   
						   done = TRUE;
					   }
				   }
			   }
			   
			   angle += 10;
		   }
		   
		   if (!done)  // if didn't find a wall, just reset ideal_yaw...
		   {
			   // set ideal_yaw to current yaw (so bot won't keep turning)
			   pEdict->v.ideal_yaw = pEdict->v.v_angle.y;
			   
			   BotFixIdealYaw(pEdict);
		   }
	   }
	   
	   // moves the bot up or down a ladder.  if the bot can't move
	   // (i.e. get's stuck with someone else on ladder), the bot will
	   // change directions and go the other way on the ladder.
	   
	   if (pBot->ladder_dir == LADDER_UP)  // is the bot currently going up?
	   {
		   pEdict->v.v_angle.x = -60;  // look upwards
		   
		   // check if the bot hasn't moved much since the last location...
		   if ((moved_distance <= 1) && (pBot->prev_speed >= 1.0))
		   {
			   // the bot must be stuck, change directions...
			   
			   pEdict->v.v_angle.x = 60;  // look downwards
			   pBot->ladder_dir = LADDER_DOWN;
		   }
	   }
	   else if (pBot->ladder_dir == LADDER_DOWN)  // is the bot currently going down?
	   {
		   pEdict->v.v_angle.x = 60;  // look downwards
		   
		   // check if the bot hasn't moved much since the last location...
		   if ((moved_distance <= 1) && (pBot->prev_speed >= 1.0))
		   {
			   // the bot must be stuck, change directions...
			   
			   pEdict->v.v_angle.x = -60;  // look upwards
			   pBot->ladder_dir = LADDER_UP;
		   }
	   }
	   else  // the bot hasn't picked a direction yet, try going up...
	   {
		   pEdict->v.v_angle.x = -60;  // look upwards
		   pBot->ladder_dir = LADDER_UP;
	   }
	}
	else
	{	// keep turning toward the waypoint
		Vector v_direction = waypoints[pBot->curr_waypoint_index].origin - pEdict->v.origin;
		Vector v_angles = UTIL_VecToAngles(v_direction);
	
		pEdict->v.idealpitch = -v_angles.x;
		BotFixIdealPitch(pEdict);
			
		pEdict->v.ideal_yaw = v_angles.y;
		BotFixIdealYaw(pEdict);

		// check if the bot hasn't moved much since the last location...
		if ((moved_distance <= 1) && (pBot->prev_speed >= 1.0))
		{	// the bot must be stuck, try to jump
			pEdict->v.button |= IN_JUMP;
		}
	}

	// move forward (i.e. in the direction the bot is looking, up or down)
	pEdict->v.button |= IN_FORWARD;
}


void BotUnderWater( bot_t *pBot )
{
//	ALERT(at_console, "BotUnderWater\n");
   bool found_waypoint = FALSE;

   edict_t *pEdict = pBot->pEdict;

   // are there waypoints in this level (and not trying to exit water)?
   if ((num_waypoints > 0) &&
       (pBot->f_exit_water_time < gpGlobals->time))
   {
      // head towards a waypoint
      found_waypoint = BotHeadTowardWaypoint(pBot);
   }

   if (found_waypoint == FALSE)
   {
      // handle movements under water.  right now, just try to keep from
      // drowning by swimming up towards the surface and look to see if
      // there is a surface the bot can jump up onto to get out of the
      // water.  bots DON'T like water!

      Vector v_src, v_forward;
      TraceResult tr;
      int contents;
   
      // swim up towards the surface
      pEdict->v.v_angle.x = -60;  // look upwards
   
      // move forward (i.e. in the direction the bot is looking, up or down)
      pEdict->v.button |= IN_FORWARD;
   
      // set gpGlobals angles based on current view angle (for TraceLine)
      MAKE_VECTORS( pEdict->v.v_angle );
   
      // look from eye position straight forward (remember: the bot is looking
      // upwards at a 60 degree angle so TraceLine will go out and up...
   
      v_src = pEdict->v.origin + pEdict->v.view_ofs;  // EyePosition()
      v_forward = v_src + gpGlobals->v_forward * 90;
   
      // trace from the bot's eyes straight forward...
      UTIL_TraceLine( v_src, v_forward, dont_ignore_monsters,
                      pEdict->v.pContainingEntity, &tr);
   
      // check if the trace didn't hit anything (i.e. nothing in the way)...
      if (tr.flFraction >= 1.0)
      {
         // find out what the contents is of the end of the trace...
         contents = POINT_CONTENTS( tr.vecEndPos );
   
         // check if the trace endpoint is in open space...
         if (contents == CONTENTS_EMPTY)
         {
            // ok so far, we are at the surface of the water, continue...
   
            v_src = tr.vecEndPos;
            v_forward = v_src;
            v_forward.z -= 90;
   
            // trace from the previous end point straight down...
            UTIL_TraceLine( v_src, v_forward, dont_ignore_monsters,
                            pEdict->v.pContainingEntity, &tr);
   
            // check if the trace hit something...
            if (tr.flFraction < 1.0)
            {
               contents = POINT_CONTENTS( tr.vecEndPos );
   
               // if contents isn't water then assume it's land, jump!
               if (contents != CONTENTS_WATER)
               {
                  pEdict->v.button |= IN_JUMP;
               }
            }
         }
      }
   }
}


void BotUseLift( bot_t *pBot, float moved_distance )
{
//	ALERT(at_console, "BotUseLift\n");
   edict_t *pEdict = pBot->pEdict;

   // just need to press the button once, when the flag gets set...
   if (pBot->f_use_button_time == gpGlobals->time)
   {
      pEdict->v.button = IN_USE;

      // face opposite from the button
      pEdict->v.ideal_yaw += 180;  // rotate 180 degrees

      BotFixIdealYaw(pEdict);
   }

   // check if the bot has waited too long for the lift to move...
   if (((pBot->f_use_button_time + 2.0) < gpGlobals->time) &&
       (!pBot->b_lift_moving))
   {
      // clear use button flag
      pBot->b_use_button = FALSE;

      // bot doesn't have to set f_find_item since the bot
      // should already be facing away from the button

      pBot->f_move_speed = pBot->f_max_speed;
   }

   // check if lift has started moving...
   if ((moved_distance > 1) && (!pBot->b_lift_moving))
   {
      pBot->b_lift_moving = TRUE;
   }

   // check if lift has stopped moving...
   if ((moved_distance <= 1) && (pBot->b_lift_moving))
   {
      TraceResult tr1, tr2;
      Vector v_src, v_forward, v_right, v_left;
      Vector v_down, v_forward_down, v_right_down, v_left_down;

      pBot->b_use_button = FALSE;

      // TraceLines in 4 directions to find which way to go...

      MAKE_VECTORS( pEdict->v.v_angle );

      v_src = pEdict->v.origin + pEdict->v.view_ofs;
      v_forward = v_src + gpGlobals->v_forward * 90;
      v_right = v_src + gpGlobals->v_right * 90;
      v_left = v_src + gpGlobals->v_right * -90;

      v_down = pEdict->v.v_angle;
      v_down.x = v_down.x + 45;  // look down at 45 degree angle

      MAKE_VECTORS( v_down );

      v_forward_down = v_src + gpGlobals->v_forward * 100;
      v_right_down = v_src + gpGlobals->v_right * 100;
      v_left_down = v_src + gpGlobals->v_right * -100;

      // try tracing forward first...
      UTIL_TraceLine( v_src, v_forward, dont_ignore_monsters,
                      pEdict->v.pContainingEntity, &tr1);
      UTIL_TraceLine( v_src, v_forward_down, dont_ignore_monsters,
                      pEdict->v.pContainingEntity, &tr2);

      // check if we hit a wall or didn't find a floor...
      if ((tr1.flFraction < 1.0) || (tr2.flFraction >= 1.0))
      {
         // try tracing to the RIGHT side next...
         UTIL_TraceLine( v_src, v_right, dont_ignore_monsters,
                         pEdict->v.pContainingEntity, &tr1);
         UTIL_TraceLine( v_src, v_right_down, dont_ignore_monsters,
                         pEdict->v.pContainingEntity, &tr2);

         // check if we hit a wall or didn't find a floor...
         if ((tr1.flFraction < 1.0) || (tr2.flFraction >= 1.0))
         {
            // try tracing to the LEFT side next...
            UTIL_TraceLine( v_src, v_left, dont_ignore_monsters,
                            pEdict->v.pContainingEntity, &tr1);
            UTIL_TraceLine( v_src, v_left_down, dont_ignore_monsters,
                            pEdict->v.pContainingEntity, &tr2);

            // check if we hit a wall or didn't find a floor...
            if ((tr1.flFraction < 1.0) || (tr2.flFraction >= 1.0))
            {
               // only thing to do is turn around...
               pEdict->v.ideal_yaw += 180;  // turn all the way around
            }
            else
            {
               pEdict->v.ideal_yaw += 90;  // turn to the LEFT
            }
         }
         else
         {
            pEdict->v.ideal_yaw -= 90;  // turn to the RIGHT
         }

         BotFixIdealYaw(pEdict);
      }

      BotChangeYaw( pBot, pEdict->v.yaw_speed );

      pBot->f_move_speed = pBot->f_max_speed;
   }
}


bool BotStuckInCorner( bot_t *pBot )
{
//	ALERT(at_console, "BotStuckInCorner\n");
   TraceResult tr;
   Vector v_src, v_dest;
   edict_t *pEdict = pBot->pEdict;
   
   MAKE_VECTORS( pEdict->v.v_angle );

   // trace 45 degrees to the right...
   v_src = pEdict->v.origin;
   v_dest = v_src + gpGlobals->v_forward*20 + gpGlobals->v_right*20;

   UTIL_TraceLine( v_src, v_dest, dont_ignore_monsters,
                   pEdict->v.pContainingEntity, &tr);

   if (tr.flFraction >= 1.0)
      return FALSE;  // no wall, so not in a corner

   // trace 45 degrees to the left...
   v_src = pEdict->v.origin;
   v_dest = v_src + gpGlobals->v_forward*20 - gpGlobals->v_right*20;

   UTIL_TraceLine( v_src, v_dest, dont_ignore_monsters,
                   pEdict->v.pContainingEntity, &tr);

   if (tr.flFraction >= 1.0)
      return FALSE;  // no wall, so not in a corner

   return TRUE;  // bot is in a corner
}


void BotTurnAtWall( bot_t *pBot, TraceResult *tr )
{
//	ALERT(at_console, "BotTurnAtWall\n");
   edict_t *pEdict = pBot->pEdict;
   Vector Normal;
   float Y, Y1, Y2, D1, D2, Z;

   // Find the normal vector from the trace result.  The normal vector will
   // be a vector that is perpendicular to the surface from the TraceResult.

   Normal = UTIL_VecToAngles(tr->vecPlaneNormal);

   // Since the bot keeps it's view angle in -180 < x < 180 degrees format,
   // and since TraceResults are 0 < x < 360, we convert the bot's view
   // angle (yaw) to the same format at TraceResult.

   Y = pEdict->v.v_angle.y;
   Y = Y + 180;
   if (Y > 359) Y -= 360;

   // Turn the normal vector around 180 degrees (i.e. make it point towards
   // the wall not away from it.  That makes finding the angles that the
   // bot needs to turn a little easier.

   Normal.y = Normal.y - 180;
   if (Normal.y < 0)
   Normal.y += 360;

   // Here we compare the bots view angle (Y) to the Normal - 90 degrees (Y1)
   // and the Normal + 90 degrees (Y2).  These two angles (Y1 & Y2) represent
   // angles that are parallel to the wall surface, but heading in opposite
   // directions.  We want the bot to choose the one that will require the
   // least amount of turning (saves time) and have the bot head off in that
   // direction.

   Y1 = Normal.y - 90;
   if (RANDOM_LONG(1, 100) <= 50)
   {
      Y1 = Y1 - RANDOM_FLOAT(5.0, 20.0);
   }
   if (Y1 < 0) Y1 += 360;

   Y2 = Normal.y + 90;
   if (RANDOM_LONG(1, 100) <= 50)
   {
      Y2 = Y2 + RANDOM_FLOAT(5.0, 20.0);
   }
   if (Y2 > 359) Y2 -= 360;

   // D1 and D2 are the difference (in degrees) between the bot's current
   // angle and Y1 or Y2 (respectively).

   D1 = std::fabs(Y - Y1);
   if (D1 > 179) D1 = std::fabs(D1 - 360);
   D2 = std::fabs(Y - Y2);
   if (D2 > 179) D2 = std::fabs(D2 - 360);

   // If difference 1 (D1) is more than difference 2 (D2) then the bot will
   // have to turn LESS if it heads in direction Y1 otherwise, head in
   // direction Y2.  I know this seems backwards, but try some sample angles
   // out on some graph paper and go through these equations using a
   // calculator, you'll see what I mean.

   if (D1 > D2)
      Z = Y1;
   else
      Z = Y2;

   // convert from TraceResult 0 to 360 degree format back to bot's
   // -180 to 180 degree format.

   if (Z > 180)
      Z -= 360;

   // set the direction to head off into...
   pEdict->v.ideal_yaw = Z;

   BotFixIdealYaw(pEdict);
}


bool BotCantMoveForward( bot_t *pBot, TraceResult *tr )
{
//	ALERT(at_console, "BotCantMoveForward\n");
   edict_t *pEdict = pBot->pEdict;

   // use some TraceLines to determine if anything is blocking the current
   // path of the bot.

   Vector v_src, v_forward;

   MAKE_VECTORS( pEdict->v.v_angle );

   // first do a trace from the bot's eyes forward...

   v_src = pEdict->v.origin + pEdict->v.view_ofs;  // EyePosition()
   v_forward = v_src + gpGlobals->v_forward * 40;

   // trace from the bot's eyes straight forward...
   UTIL_TraceLine( v_src, v_forward, dont_ignore_monsters,
                   pEdict->v.pContainingEntity, tr);

   // check if the trace hit something...
   if (tr->flFraction < 1.0)
   {
      return TRUE;  // bot's head will hit something
   }

   // bot's head is clear, check at waist level...

   v_src = pEdict->v.origin;
   v_forward = v_src + gpGlobals->v_forward * 40;

   // trace from the bot's waist straight forward...
   UTIL_TraceLine( v_src, v_forward, dont_ignore_monsters,
                   pEdict->v.pContainingEntity, tr);

   // check if the trace hit something...
   if (tr->flFraction < 1.0)
   {
      return TRUE;  // bot's body will hit something
   }

   return FALSE;  // bot can move forward, return false
}


bool BotCanJumpUp( bot_t *pBot, bool *bDuckJump)
{
//	ALERT(at_console, "BotCanJumpUp\n");
   // What I do here is trace 3 lines straight out, one unit higher than
   // the highest normal jumping distance.  I trace once at the center of
   // the body, once at the right side, and once at the left side.  If all
   // three of these TraceLines don't hit an obstruction then I know the
   // area to jump to is clear.  I then need to trace from head level,
   // above where the bot will jump to, downward to see if there is anything
   // blocking the jump.  There could be a narrow opening that the body
   // will not fit into.  These horizontal and vertical TraceLines seem
   // to catch most of the problems with falsely trying to jump on something
   // that the bot can not get onto.

   TraceResult tr;
   bool check_duck = FALSE;
   Vector v_jump, v_source, v_dest;
   edict_t *pEdict = pBot->pEdict;

   *bDuckJump = FALSE;

   // convert current view angle to vectors for TraceLine math...

   v_jump = pEdict->v.v_angle;
   v_jump.x = 0;  // reset pitch to 0 (level horizontally)
   v_jump.z = 0;  // reset roll to 0 (straight up and down)

   MAKE_VECTORS( v_jump );

   // use center of the body first...

   // maximum normal jump height is 45, so check one unit above that (46)
   v_source = pEdict->v.origin + Vector(0, 0, -36 + 46);
   v_dest = v_source + gpGlobals->v_forward * 24;

   // trace a line forward at maximum jump height...
   UTIL_TraceLine( v_source, v_dest, dont_ignore_monsters,
                   pEdict->v.pContainingEntity, &tr);

   // if trace hit something, check duck jump...
   if (tr.flFraction < 1.0)
      check_duck = TRUE;

   if (!check_duck)
   {
      // now check same height to one side of the bot...
      v_source = pEdict->v.origin + gpGlobals->v_right * 16 + Vector(0, 0, -36 + 46);
      v_dest = v_source + gpGlobals->v_forward * 24;

      // trace a line forward at maximum jump height...
      UTIL_TraceLine( v_source, v_dest, dont_ignore_monsters,
                      pEdict->v.pContainingEntity, &tr);

      // if trace hit something, check duck jump...
      if (tr.flFraction < 1.0)
         check_duck = TRUE;
   }

   if (!check_duck)
   {
      // now check same height on the other side of the bot...
      v_source = pEdict->v.origin + gpGlobals->v_right * -16 + Vector(0, 0, -36 + 46);
      v_dest = v_source + gpGlobals->v_forward * 24;

      // trace a line forward at maximum jump height...
      UTIL_TraceLine( v_source, v_dest, dont_ignore_monsters,
                      pEdict->v.pContainingEntity, &tr);

      // if trace hit something, check duck jump...
      if (tr.flFraction < 1.0)
         check_duck = TRUE;
   }

   if (check_duck)
   {
      // maximum crouch jump height is 63, so check one unit above that (64)
      v_source = pEdict->v.origin + Vector(0, 0, -36 + 64);
      v_dest = v_source + gpGlobals->v_forward * 24;

      // trace a line forward at maximum jump height...
      UTIL_TraceLine( v_source, v_dest, dont_ignore_monsters,
                      pEdict->v.pContainingEntity, &tr);

      // if trace hit something, return FALSE
      if (tr.flFraction < 1.0)
         return FALSE;

      // now check same height on the other side of the bot...
      v_source = pEdict->v.origin + gpGlobals->v_right * -16 + Vector(0, 0, -36 + 64);
      v_dest = v_source + gpGlobals->v_forward * 24;

      // trace a line forward at maximum jump height...
      UTIL_TraceLine( v_source, v_dest, dont_ignore_monsters,
                      pEdict->v.pContainingEntity, &tr);

      // if trace hit something, return FALSE
      if (tr.flFraction < 1.0)
         return FALSE;

      // now check same height on the other side of the bot...
      v_source = pEdict->v.origin + gpGlobals->v_right * -16 + Vector(0, 0, -36 + 64);
      v_dest = v_source + gpGlobals->v_forward * 24;

      // trace a line forward at maximum jump height...
      UTIL_TraceLine( v_source, v_dest, dont_ignore_monsters,
                      pEdict->v.pContainingEntity, &tr);

      // if trace hit something, return FALSE
      if (tr.flFraction < 1.0)
         return FALSE;
   }

   // now trace from head level downward to check for obstructions...

   // start of trace is 24 units in front of bot...
   v_source = pEdict->v.origin + gpGlobals->v_forward * 24;

   if (check_duck)
      // offset 36 units if crouch-jump (36 + 36)
      v_source.z = v_source.z + 72;
   else
      // offset 72 units from top of head (72 + 36)
      v_source.z = v_source.z + 108;


   if (check_duck)
      // end point of trace is 27 units straight down from start...
      // (27 is 72 minus the jump limit height which is 63 - 18 = 45)
      v_dest = v_source + Vector(0, 0, -27);
   else
      // end point of trace is 99 units straight down from start...
      // (99 is 108 minus the jump limit height which is 45 - 36 = 9)
      v_dest = v_source + Vector(0, 0, -99);


   // trace a line straight down toward the ground...
   UTIL_TraceLine( v_source, v_dest, dont_ignore_monsters,
                   pEdict->v.pContainingEntity, &tr);

   // if trace hit something, return FALSE
   if (tr.flFraction < 1.0)
      return FALSE;

   // now check same height to one side of the bot...
   v_source = pEdict->v.origin + gpGlobals->v_right * 16 + gpGlobals->v_forward * 24;

   if (check_duck)
      v_source.z = v_source.z + 72;
   else
      v_source.z = v_source.z + 108;

   if (check_duck)
      v_dest = v_source + Vector(0, 0, -27);
   else
      v_dest = v_source + Vector(0, 0, -99);

   // trace a line straight down toward the ground...
   UTIL_TraceLine( v_source, v_dest, dont_ignore_monsters,
                   pEdict->v.pContainingEntity, &tr);

   // if trace hit something, return FALSE
   if (tr.flFraction < 1.0)
      return FALSE;

   // now check same height on the other side of the bot...
   v_source = pEdict->v.origin + gpGlobals->v_right * -16 + gpGlobals->v_forward * 24;

   if (check_duck)
      v_source.z = v_source.z + 72;
   else
      v_source.z = v_source.z + 108;

   if (check_duck)
      v_dest = v_source + Vector(0, 0, -27);
   else
      v_dest = v_source + Vector(0, 0, -99);

   // trace a line straight down toward the ground...
   UTIL_TraceLine( v_source, v_dest, dont_ignore_monsters,
                   pEdict->v.pContainingEntity, &tr);

   // if trace hit something, return FALSE
   if (tr.flFraction < 1.0)
      return FALSE;

   return TRUE;
}

bool BotCanDuckUnder( bot_t *pBot )
{
//	ALERT(at_console, "BotCanDuckUnder\n");
   // What I do here is trace 3 lines straight out, one unit higher than
   // the ducking height.  I trace once at the center of the body, once
   // at the right side, and once at the left side.  If all three of these
   // TraceLines don't hit an obstruction then I know the area to duck to
   // is clear.  I then need to trace from the ground up, 72 units, to make
   // sure that there is something blocking the TraceLine.  Then we know
   // we can duck under it.

   TraceResult tr;
   Vector v_duck, v_source, v_dest;
   edict_t *pEdict = pBot->pEdict;

   // convert current view angle to vectors for TraceLine math...

   v_duck = pEdict->v.v_angle;
   v_duck.x = 0;  // reset pitch to 0 (level horizontally)
   v_duck.z = 0;  // reset roll to 0 (straight up and down)

   MAKE_VECTORS( v_duck );

   // use center of the body first...

   // duck height is 36, so check one unit above that (37)
   v_source = pEdict->v.origin + Vector(0, 0, -36 + 37);
   v_dest = v_source + gpGlobals->v_forward * 24;

   // trace a line forward at duck height...
   UTIL_TraceLine( v_source, v_dest, dont_ignore_monsters,
                   pEdict->v.pContainingEntity, &tr);

   // if trace hit something, return FALSE
   if (tr.flFraction < 1.0)
      return FALSE;

   // now check same height to one side of the bot...
   v_source = pEdict->v.origin + gpGlobals->v_right * 16 + Vector(0, 0, -36 + 37);
   v_dest = v_source + gpGlobals->v_forward * 24;

   // trace a line forward at duck height...
   UTIL_TraceLine( v_source, v_dest, dont_ignore_monsters,
                   pEdict->v.pContainingEntity, &tr);

   // if trace hit something, return FALSE
   if (tr.flFraction < 1.0)
      return FALSE;

   // now check same height on the other side of the bot...
   v_source = pEdict->v.origin + gpGlobals->v_right * -16 + Vector(0, 0, -36 + 37);
   v_dest = v_source + gpGlobals->v_forward * 24;

   // trace a line forward at duck height...
   UTIL_TraceLine( v_source, v_dest, dont_ignore_monsters,
                   pEdict->v.pContainingEntity, &tr);

   // if trace hit something, return FALSE
   if (tr.flFraction < 1.0)
      return FALSE;

   // now trace from the ground up to check for object to duck under...

   // start of trace is 24 units in front of bot near ground...
   v_source = pEdict->v.origin + gpGlobals->v_forward * 24;
   v_source.z = v_source.z - 35;  // offset to feet + 1 unit up

   // end point of trace is 72 units straight up from start...
   v_dest = v_source + Vector(0, 0, 72);

   // trace a line straight up in the air...
   UTIL_TraceLine( v_source, v_dest, dont_ignore_monsters,
                   pEdict->v.pContainingEntity, &tr);

   // if trace didn't hit something, return FALSE
   if (tr.flFraction >= 1.0)
      return FALSE;

   // now check same height to one side of the bot...
   v_source = pEdict->v.origin + gpGlobals->v_right * 16 + gpGlobals->v_forward * 24;
   v_source.z = v_source.z - 35;  // offset to feet + 1 unit up
   v_dest = v_source + Vector(0, 0, 72);

   // trace a line straight up in the air...
   UTIL_TraceLine( v_source, v_dest, dont_ignore_monsters,
                   pEdict->v.pContainingEntity, &tr);

   // if trace didn't hit something, return FALSE
   if (tr.flFraction >= 1.0)
      return FALSE;

   // now check same height on the other side of the bot...
   v_source = pEdict->v.origin + gpGlobals->v_right * -16 + gpGlobals->v_forward * 24;
   v_source.z = v_source.z - 35;  // offset to feet + 1 unit up
   v_dest = v_source + Vector(0, 0, 72);

   // trace a line straight up in the air...
   UTIL_TraceLine( v_source, v_dest, dont_ignore_monsters,
                   pEdict->v.pContainingEntity, &tr);

   // if trace didn't hit something, return FALSE
   if (tr.flFraction >= 1.0)
      return FALSE;

   return TRUE;
}


void BotRandomTurn( bot_t *pBot )
{
//	ALERT(at_console, "BotRandomTurn\n");
   pBot->f_move_speed = 0;  // don't move while turning
            
   if (RANDOM_LONG(1, 100) <= 10)
   {
      // 10 percent of the time turn completely around...
      pBot->pEdict->v.ideal_yaw += 180;
   }
   else
   {
      // turn randomly between 30 and 60 degress
      if (pBot->wander_dir == WANDER_LEFT)
         pBot->pEdict->v.ideal_yaw += RANDOM_LONG(30, 60);
      else
         pBot->pEdict->v.ideal_yaw -= RANDOM_LONG(30, 60);
   }
            
   BotFixIdealYaw(pBot->pEdict);
}


bool BotFollowUser( bot_t *pBot )
{
//	ALERT(at_console, "BotFollowUser\n");
   bool user_visible;
   float f_distance;
   edict_t *pEdict = pBot->pEdict;

   Vector vecEnd = pBot->pBotUser->v.origin + pBot->pBotUser->v.view_ofs;

   if (pBot->pEdict->v.waterlevel != 3)  // is bot NOT under water?
      pEdict->v.v_angle.x = 0;  // reset pitch to 0 (level horizontally)

   pEdict->v.v_angle.z = 0;  // reset roll to 0 (straight up and down)

   pEdict->v.angles.x = 0;
   pEdict->v.angles.y = pEdict->v.v_angle.y;
   pEdict->v.angles.z = 0;

   if (!IsAlive( pBot->pBotUser ))
   {
      // the bot's user is dead!
      pBot->pBotUser = nullptr;
      return FALSE;
   }

   user_visible = FInViewCone( &vecEnd, pEdict ) &&
                  FVisible( vecEnd, pEdict );

   // check if the "user" is still visible or if the user has been visible
   // in the last 5 seconds (or the player just starting "using" the bot)

   if (user_visible || (pBot->f_bot_use_time + 5 > gpGlobals->time))
   {
      if (user_visible)
         pBot->f_bot_use_time = gpGlobals->time;  // reset "last visible time"

      // face the user
      Vector v_user = pBot->pBotUser->v.origin - pEdict->v.origin;
      Vector bot_angles = UTIL_VecToAngles( v_user );

      pEdict->v.ideal_yaw = bot_angles.y;

      BotFixIdealYaw(pEdict);

      f_distance = v_user.Length( );  // how far away is the "user"?

      if (f_distance > 200)      // run if distance to enemy is far
         pBot->f_move_speed = pBot->f_max_speed;
      else if (f_distance > 50)  // walk if distance is closer
         pBot->f_move_speed = pBot->f_max_speed / 2;
      else                     // don't move if close enough
         pBot->f_move_speed = 0.0;

      return TRUE;
   }
   else
   {
      // person to follow has gone out of sight...
      pBot->pBotUser = nullptr;

      return FALSE;
   }
}


bool BotCheckWallOnLeft( bot_t *pBot )
{
//	ALERT(at_console, "BotCheckWallOnLeft\n");
   edict_t *pEdict = pBot->pEdict;
   Vector v_src, v_left;
   TraceResult tr;

   MAKE_VECTORS( pEdict->v.v_angle );

   // do a trace to the left...

   v_src = pEdict->v.origin;
   v_left = v_src + gpGlobals->v_right * -40;  // 40 units to the left

   UTIL_TraceLine( v_src, v_left, dont_ignore_monsters,
                   pEdict->v.pContainingEntity, &tr);

   // check if the trace hit something...
   if (tr.flFraction < 1.0)
   {
      if (pBot->f_wall_on_left < 1.0)
         pBot->f_wall_on_left = gpGlobals->time;

      return TRUE;
   }

   return FALSE;
}


bool BotCheckWallOnRight( bot_t *pBot )
{
//	ALERT(at_console, "BotCheckWallOnRight\n");
   edict_t *pEdict = pBot->pEdict;
   Vector v_src, v_right;
   TraceResult tr;

   MAKE_VECTORS( pEdict->v.v_angle );

   // do a trace to the right...

   v_src = pEdict->v.origin;
   v_right = v_src + gpGlobals->v_right * 40;  // 40 units to the right

   UTIL_TraceLine( v_src, v_right, dont_ignore_monsters,
                   pEdict->v.pContainingEntity, &tr);

   // check if the trace hit something...
   if (tr.flFraction < 1.0)
   {
      if (pBot->f_wall_on_right < 1.0)
         pBot->f_wall_on_right = gpGlobals->time;

      return TRUE;
   }

   return FALSE;
}

extern int m_spriteTexture;

// Search for close players and monsters and try to strafe/move around them
void BotAvoidContact( bot_t *pBot )
{
//	ALERT(at_console, "BotAvoidContact\n");
	edict_t *pEnt = nullptr;
	float flMaxDist = 128;
	Vector vecDir, vecBotDir;
	TraceResult tr;

/*	Vector bot_angle = pBot->pEdict->v.v_angle;
	// get the sin and clamp it's range from -1 to 1
	// OTHERWISE TRACELINE BELOW WILL CRASH
	float sin = pBot->f_strafe_speed / pBot->f_max_speed;
	if (sin < -1) sin = -1;
	if (sin > 1) sin = 1;

	float dgrad = asin(sin) * 180 / PI;
	bot_angle.y += dgrad;

	if (bot_angle.y > 180)
		bot_angle.y -= 360;
	if (bot_angle.y < -180)
		bot_angle.y += 360;

	MAKE_VECTORS(bot_angle);
*/
	// the bot saves the current direction it is trying to go each time it is calculated
	// this way we don't have to deal with asin
	vecBotDir = pBot->v_curr_direction.Normalize() * flMaxDist;//gpGlobals->v_forward * flMaxDist;

	Vector start = pBot->pEdict->v.origin;
	Vector end = start + vecBotDir;

	// trace a line to see if this entity is in the way of the direction we're traveling
	UTIL_TraceLine(pBot->pEdict->v.origin, pBot->pEdict->v.origin + 
		vecBotDir, dont_ignore_monsters, pBot->pEdict, &tr);

	// copy in whatever we hit
	if (tr.pHit)
	{
		// not us AND monster OR player AND is alive
		if (tr.pHit != pBot->pEdict && ((strncmp("monster_", STRING(tr.pHit->v.classname), 8) == 0) ||
			(strcmp("player", STRING(tr.pHit->v.classname)) == 0)) &&
			IsAlive(tr.pHit))
			pBot->pAvoid = tr.pHit;
	}	
/*
	while ((pEnt = UTIL_FindEntityInSphere( pEnt, pBot->pEdict->v.origin, 64 )) != NULL)
	{	// ignore yourself, duh
		if (pEnt == pBot->pEdict)
			continue;

		// only think about monsters & players
		if ((strncmp("monster_", STRING(pEnt->v.classname), 8) != 0) &&
			(strcmp("player", STRING(pEnt->v.classname)) != 0))
			continue;

		// dead, don't bother
		if (!IsAlive(pEnt))
			continue;

		vecDir = (pBot->pEdict->v.origin + pBot->pEdict->v.view_ofs) - 
			(pEnt->v.origin + pEnt->v.view_ofs);

		// trace a line to see if this entity is in the way of the direction we're traveling
		UTIL_TraceLine(pBot->pEdict->v.origin, pBot->pEdict->v.origin + 
			vecBotDir, dont_ignore_monsters, pBot->pEdict, &tr);
			
		if (tr.flFraction >= 1.0)
		{
			// trace a line to see if this entity is in the way of the opposite direction we're traveling
			UTIL_TraceLine(pBot->pEdict->v.origin, pBot->pEdict->v.origin - vecBotDir, dont_ignore_monsters, pBot->pEdict, &tr);
		}
		
		// find closest entity to avoid
		if ((vecDir.Length() < flMaxDist) && (tr.flFraction < 1.0))
		{
			flMaxDist = vecDir.Length();
			pBot->pAvoid = pEnt;
		}
	}
	*/
	// did we find a monster/player/thing
	if (pBot->pAvoid != nullptr)
	{
		vecDir = (pBot->pAvoid->v.origin + pBot->pAvoid->v.view_ofs) - 
			(pBot->pEdict->v.origin + pBot->pEdict->v.view_ofs);

		// this code is used to make the bot strafe out of the way regardless of its angles
		// dodge perpendicular to the entity's direction
		Vector vecMove = CrossProduct(vecDir.Normalize(), Vector(0,0,1));
		// randomly pick the other direction
		if (RANDOM_LONG(0,1)) vecMove = -vecMove;
		// trace a line to see if something is blocking where we should go
		UTIL_TraceLine(pBot->pEdict->v.origin, pBot->pEdict->v.origin + 
			vecMove * 64, dont_ignore_monsters, pBot->pEdict, &tr);
		
		if (tr.flFraction < 1.0)
		{
			// trace a line to see if something is blocking where we should go
			UTIL_TraceLine(pBot->pEdict->v.origin, pBot->pEdict->v.origin + 
				-vecMove * 64, dont_ignore_monsters, pBot->pEdict, &tr);

			if (tr.flFraction >= 1.0)
			{	// go the opposite way
				vecMove = -vecMove;
			}
		}

		//if (tr.flFraction >= 1.0)
		{
			pBot->f_do_avoid_time = gpGlobals->time + 0.5;
			pBot->avoid_dir = vecMove;
		}
	}
}

// This code is similar to the avoid contact code, expect it only checks for headcrabs
// and also only checks if the crabs are hurling toward the bot.
void BotDodgeCrabs( bot_t *pBot )
{
//	ALERT(at_console, "BotDodgeCrabs\n");
	edict_t *pEnt = nullptr;
	float flMaxDist = 512;
	Vector vecDir;
	TraceResult tr;

	while ((pEnt = UTIL_FindEntityInSphere( pEnt, pBot->pEdict->v.origin, 512 )) != nullptr)
	{
		// only think about headcrabs
		if (strcmp("monster_headcrab", STRING(pEnt->v.classname)) != 0)
			continue;
			
		// forget crabs that are on ground
		if (pEnt->v.flags & FL_ONGROUND)
			continue;
			
		// dead, don't bother
		if (!IsAlive(pEnt))
			continue;
			
		vecDir = (pBot->pEdict->v.origin + pBot->pEdict->v.view_ofs) - 
			(pEnt->v.origin + pEnt->v.view_ofs);
			
		// find closest crab
		if (vecDir.Length() < flMaxDist)
		{
			flMaxDist = vecDir.Length();
			pBot->pAvoid = pEnt;
		}
	}
	
	// did we find a crab?
	if (pBot->pAvoid != nullptr)
	{
		vecDir = (pBot->pEdict->v.origin + pBot->pEdict->v.view_ofs) - 
			(pBot->pAvoid->v.origin + pBot->pAvoid->v.view_ofs);
		
		if (DotProduct((pBot->pAvoid->v.velocity).Normalize(), vecDir.Normalize()) > 0.9)
		{	// this code is used to make the bot strafe out of the way regardless of its angles
			// dodge perpendicular to the crab's direction
			Vector vecMove = CrossProduct(vecDir.Normalize(), Vector(0,0,1));
			
			// trace a line to see if something is blocking where we should go
			UTIL_TraceLine(pBot->pEdict->v.origin, pBot->pEdict->v.origin + 
				vecMove * 64, dont_ignore_monsters, pBot->pEdict, &tr);
			
			if (tr.flFraction < 1.0)
			{
				// trace a line to see if something is blocking where we should go
				UTIL_TraceLine(pBot->pEdict->v.origin, pBot->pEdict->v.origin + 
					-vecMove * 64, dont_ignore_monsters, pBot->pEdict, &tr);

				if (tr.flFraction >= 1.0)
				{	// go the opposite way
					vecMove = -vecMove;
				}
			}

			pBot->f_do_avoid_time = gpGlobals->time + 0.5;
			pBot->avoid_dir = vecMove;
		}
	}
}

bool BotLookForDrop( bot_t *pBot )
{
//	ALERT(at_console, "BotLookForDrop\n");
	edict_t *pEdict = pBot->pEdict;
	TraceResult tr;
	Vector vecStart, vecEnd;
	// trace a little bit in the direction we're traveling
	vecStart = pEdict->v.origin;
	vecEnd = vecStart + (pEdict->v.velocity / 8);
	UTIL_TraceLine(vecStart, vecEnd, ignore_monsters,
		pEdict->v.pContainingEntity, &tr);
	// make sure nothing is in the way
	if (tr.flFraction >= 1.0)
	{	// trace down 400 units
		vecStart = tr.vecEndPos;
		vecEnd = vecStart - Vector(0,0,400);
		UTIL_TraceLine(vecStart, vecEnd, ignore_monsters, pEdict->v.pContainingEntity, &tr);
	
		int iContents = POINT_CONTENTS( tr.vecEndPos );
		// full trace and it's not water down there?
		if (tr.flFraction >= 1.0 && iContents != CONTENTS_WATER)
			return true;
	}
	// no drop found
	return false;
}