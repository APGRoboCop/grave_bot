<<<<<<< HEAD
=======
<<<<<<< HEAD
>>>>>>> 594320fd6db9c6f9b3b7d743a0270568ddbba6bd
//
// HPB bot - botman's High Ping Bastard bot
//
// (http://planethalflife.com/botman/)
//
// bot_client.cpp
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
#include "bot_client.h"
#include "bot_weapons.h"
#include "waypoint.h"

#ifdef METAMOD_BUILD
   // instant damage (from cbase.h)
   #define DMG_CRUSH			(1 << 0)	// crushed by falling or moving object
   #define DMG_BURN			(1 << 3)	// heat burned
   #define DMG_FREEZE			(1 << 4)	// frozen
   #define DMG_FALL			(1 << 5)	// fell too far
   #define DMG_SHOCK			(1 << 8)	// electric shock
   #define DMG_DROWN			(1 << 14)	// Drowning
   #define DMG_NERVEGAS		(1 << 16)	// nerve toxins, very bad
   #define DMG_RADIATION		(1 << 18)	// radiation exposure
   #define DMG_DROWNRECOVER	(1 << 19)	// drowning recovery
   #define DMG_ACID			(1 << 20)	// toxic chemicals or acid burns
   #define DMG_SLOWBURN		(1 << 21)	// in an oven
   #define DMG_SLOWFREEZE		(1 << 22)	// in a subzero freezer
#endif

// types of damage to ignore...
#define IGNORE_DAMAGE (DMG_CRUSH | DMG_FREEZE | DMG_FALL | DMG_SHOCK | \
                       DMG_DROWN | DMG_NERVEGAS | DMG_RADIATION | \
                       DMG_DROWNRECOVER | DMG_ACID | DMG_SLOWBURN | \
                       DMG_SLOWFREEZE | 0xFF000000)

extern int mod_id;
extern bot_t bots[32];

bot_weapon_t weapon_defs[MAX_WEAPONS]; // array of weapon definitions

extern long g_lTeamScore[2];
extern int g_iSciCount[2];

extern bot_research_t g_Researched[2][NUM_RESEARCH_OPTIONS];
int g_iResearchGoal[2] = { 0, 0 };

extern void RoleCount( void );
extern void RoleDetermine( void );

extern WAYPOINT waypoints[MAX_WAYPOINTS];
extern int num_waypoints;  // number of waypoints currently in use
extern int num_def_waypoints;

// This message is sent when a client joins the game.  All of the weapons
// are sent with the weapon ID and information about what ammo is used.
void BotClient_Valve_WeaponList(void *p, edict_t *pEdict)
{	/*
	if (!pEdict)
		return;

	static bot_t *pBot = NULL;
	pBot = UTIL_GetBotPointer(pEdict);

	if (!pBot)
		return;
	*/
	static int state = 0;   // current state machine state
	static bot_weapon_t bot_weapon;
	
	if (state == 0)
	{
		state++;
		strcpy(bot_weapon.szClassname, (char *)p);
	}
	else if (state == 1)
	{
		state++;
		bot_weapon.iAmmo1 = *(int *)p;  // ammo index 1
	}
	else if (state == 2)
	{
		state++;
		bot_weapon.iAmmo1Max = *(int *)p;  // max ammo1
	}
	else if (state == 3)
	{
		state++;
		bot_weapon.iAmmo2 = *(int *)p;  // ammo index 2
	}
	else if (state == 4)
	{
		state++;
		bot_weapon.iAmmo2Max = *(int *)p;  // max ammo2
	}
	else if (state == 5)
	{
		state++;
		bot_weapon.iSlot = *(int *)p;  // slot for this weapon
	}
	else if (state == 6)
	{
		state++;
		bot_weapon.iPosition = *(int *)p;  // position in slot
	}
	else if (state == 7)
	{
		state++;
		bot_weapon.iId = *(int *)p;  // weapon ID
	}
	else if (state == 8)
	{
		state = 0;
		
		bot_weapon.iFlags = *(int *)p;  // flags for weapon (WTF???)
		
		// store away this weapon with it's ammo information...
		weapon_defs[bot_weapon.iId] = bot_weapon;
	}
}

// This message is sent when a weapon is selected (either by the bot chosing
// a weapon or by the server auto assigning the bot a weapon).
void BotClient_Valve_CurrentWeapon(void *p, edict_t *pEdict)
{
	if (!pEdict)
		return;

	static bot_t *pBot = NULL;
	pBot = UTIL_GetBotPointer(pEdict);

	if (!pBot)
		return;

	static int state = 0;   // current state machine state
	static int iState;
	static int iId;
	static int iClip;
	
	if (state == 0)
	{
		state++;
		iState = *(int *)p;  // state of the current weapon
	}
	else if (state == 1)
	{
		state++;
		iId = *(int *)p;  // weapon ID of current weapon
	}
	else if (state == 2)
	{
		state = 0;
		
		iClip = *(int *)p;  // ammo currently in the clip for this weapon
		
		if (iId <= 31 && iId >= 0)
		{
			if (iState == 1)
			{
				pBot->current_weapon.iId = iId;
				pBot->current_weapon.iClip = iClip;
				pBot->current_weapon.iClip2 = -1;

				// update the ammo counts for this weapon...
				if (weapon_defs[iId].iAmmo1 != -1)
				{
					pBot->current_weapon.iAmmo1 =
						pBot->m_rgAmmo[weapon_defs[iId].iAmmo1];
				}
				if (weapon_defs[iId].iAmmo2 != -1)
				{
					pBot->current_weapon.iAmmo2 =
						pBot->m_rgAmmo[weapon_defs[iId].iAmmo2];
				}
			}
		}
	}
}

// This message is sent whenever ammo ammounts are adjusted (up or down).
void BotClient_Valve_AmmoX(void *p, edict_t *pEdict)
{
	if (!pEdict)
		return;

	static bot_t *pBot = NULL;
	pBot = UTIL_GetBotPointer(pEdict);

	if (!pBot)
		return;

	static int state = 0;   // current state machine state
	static int index;
	static int ammount;
	int ammo_index;
	
	if (state == 0)
	{
		state++;
		index = *(int *)p;  // ammo index (for type of ammo)
	}
	else if (state == 1)
	{
		state = 0;
		
		ammount = *(int *)p;  // the ammount of ammo currently available
		
		pBot->m_rgAmmo[index] = ammount;  // store it away
		
		ammo_index = pBot->current_weapon.iId;
		
		// update the ammo counts for this weapon...
		pBot->current_weapon.iAmmo1 =
			pBot->m_rgAmmo[weapon_defs[ammo_index].iAmmo1];
		pBot->current_weapon.iAmmo2 =
			pBot->m_rgAmmo[weapon_defs[ammo_index].iAmmo2];
	}
}

// This message is sent when the bot picks up some ammo (AmmoX messages are
// also sent so this message is probably not really necessary except it
// allows the HUD to draw pictures of ammo that have been picked up.  The
// bots don't really need pictures since they don't have any eyes anyway.
void BotClient_Valve_AmmoPickup(void *p, edict_t *pEdict)
{
	if (!pEdict)
		return;

	static bot_t *pBot = NULL;
	pBot = UTIL_GetBotPointer(pEdict);

	if (!pBot)
		return;

	static int state = 0;   // current state machine state
	static int index;
	static int ammount;
	int ammo_index;
	
	if (state == 0)
	{
		state++;
		index = *(int *)p;
	}
	else if (state == 1)
	{
		state = 0;
		
		ammount = *(int *)p;
		
		pBot->m_rgAmmo[index] = ammount;
		
		ammo_index = pBot->current_weapon.iId;
		
		// update the ammo counts for this weapon...
		pBot->current_weapon.iAmmo1 =
			pBot->m_rgAmmo[weapon_defs[ammo_index].iAmmo1];
		pBot->current_weapon.iAmmo2 =
			pBot->m_rgAmmo[weapon_defs[ammo_index].iAmmo2];
	}
}

// This message gets sent when the bots are getting damaged.
void BotClient_Valve_Damage(void *p, edict_t *pEdict)
{
	if (!pEdict)
		return;

	static bot_t *pBot = NULL;
	pBot = UTIL_GetBotPointer(pEdict);

	if (!pBot)
		return;

	static int state = 0;   // current state machine state
	static int damage_armor;
	static int damage_taken;
	static int damage_bits;  // type of damage being done
	static Vector damage_origin;
	
	if (state == 0)
	{
		state++;
		damage_armor = *(int *)p;
	}
	else if (state == 1)
	{
		state++;
		damage_taken = *(int *)p;
	}
	else if (state == 2)
	{
		state++;
		damage_bits = *(int *)p;
	}
	else if (state == 3)
	{
		state++;
		damage_origin.x = *(float *)p;
	}
	else if (state == 4)
	{
		state++;
		damage_origin.y = *(float *)p;
	}
	else if (state == 5)
	{
		state = 0;
		
		damage_origin.z = *(float *)p;
		
		if ((damage_armor > 0) || (damage_taken > 0))
		{
			// ignore certain types of damage...
			if (damage_bits & IGNORE_DAMAGE)
				return;
			
			// if the bot doesn't have an enemy and someone is shooting at it then
			// turn in the attacker's direction...
			if (pBot->pBotEnemy == NULL)
			{
				pBot->dmg_origin = damage_origin;
				pBot->f_dmg_time = gpGlobals->time + 1.0;
				
				// stop using health or HEV stations...
				pBot->b_use_health_station = FALSE;
				pBot->b_use_HEV_station = FALSE;
			}
		}
	}
}

// This message gets sent when the bots get killed
void BotClient_Valve_DeathMsg(void *p, edict_t *pEdict)
{	
}

void BotClient_Valve_ScreenFade(void *p, edict_t *pEdict)
{
	if (!pEdict)
		return;

	static bot_t *pBot = NULL;
	pBot = UTIL_GetBotPointer(pEdict);

	if (!pBot)
		return;

	static int state = 0;   // current state machine state
	static int duration;
	static int hold_time;
	static int fade_flags;
	int length;
	
	if (state == 0)
	{
		state++;
		duration = *(int *)p;
	}
	else if (state == 1)
	{
		state++;
		hold_time = *(int *)p;
	}
	else if (state == 2)
	{
		state++;
		fade_flags = *(int *)p;
	}
	else if (state == 6)
	{
		state = 0;
		
		length = (duration + hold_time) / 4096;
		pBot->blinded_time = gpGlobals->time + length - 2.0;
	}
	else
	{
		state++;
	}
}

// used to check if the bot has gotten a long jump
void BotClient_Valve_ItemPickup(void *p, edict_t *pEdict)
{
	if (!pEdict)
		return;

	static bot_t *pBot = NULL;
	pBot = UTIL_GetBotPointer(pEdict);

	if (!pBot)
		return;

	char itemname[64];
	strcpy(itemname,(char *)p);

	if (strcmp(itemname, "item_longjump") == 0)
	{
//		SERVER_PRINT( "%s has a longjump module now!\n", STRING(pBot->pEdict->v.netname));
		pBot->b_longjump = true;
	}
}

void BotClient_Valve_WeapPickup(void *p, edict_t *pEdict)
{
	// nothing to here
}

void BotClient_Valve_Battery(void *p, edict_t *pEdict)
{
	// nothing to here
}

//////////////////////////
// Science and Industry //
//////////////////////////

// WeaponList message in science and industry does
// not include HUD slot or position
void BotClient_SI_WeaponList(void *p, edict_t *pEdict)
{	/*
	if (!pEdict)
		return;

	static bot_t *pBot = NULL;
	pBot = UTIL_GetBotPointer(pEdict);

	if (!pBot)
		return;
	*/
	static int state = 0;   // current state machine state
	static bot_weapon_t bot_weapon;
	
	if (state == 0)
	{
		state++;
		strcpy(bot_weapon.szClassname, (char *)p);
	}
	else if (state == 1)
	{
		state++;
		bot_weapon.iAmmo1 = *(int *)p;  // ammo index 1
	}
	else if (state == 2)
	{
		state++;
		bot_weapon.iAmmo1Max = *(int *)p;  // max ammo1
	}
	else if (state == 3)
	{
		state++;
		bot_weapon.iAmmo2 = *(int *)p;  // ammo index 2
	}
	else if (state == 4)
	{
		state++;
		bot_weapon.iAmmo2Max = *(int *)p;  // max ammo2
	}
	else if (state == 5)
	{
		state++;
		bot_weapon.iId = *(int *)p;  // weapon ID
	}
	else if (state == 6)
	{
		state = 0;
		
		bot_weapon.iFlags = *(int *)p;  // flags for weapon (WTF???)
		
		// store away this weapon with it's ammo information...
		weapon_defs[bot_weapon.iId] = bot_weapon;
	}
}

// Science and Industry also has a secondary clip
void BotClient_SI_CurrentWeapon(void *p, edict_t *pEdict)
{
	if (!pEdict)
		return;

	static bot_t *pBot = NULL;
	pBot = UTIL_GetBotPointer(pEdict);

	if (!pBot)
		return;

	static int state = 0;   // current state machine state
	static int iState;
	static int iId;
	static int iClip;
	static int iClip2;
	
	if (state == 0)
	{
		state++;
		iState = *(int *)p;  // state of the current weapon
	}
	else if (state == 1)
	{
		state++;
		iId = *(int *)p;  // weapon ID of current weapon
	}
	else if (state == 2)
	{
		state++;
		iClip = *(int *)p;  // ammo currently in the clip for this weapon
	}
	else if (state == 3)
	{
		state = 0;
		
		iClip2 = *(int *)p;  // ammo currently in the secondary clip for this weapon
		
		if (iId <= 31 && iId >= 0)
		{
			if (iState == 1)
			{
				pBot->current_weapon.iId = iId;
				pBot->current_weapon.iClip = iClip;
				pBot->current_weapon.iClip2 = iClip2;
				
				// update the ammo counts for this weapon...
				if (weapon_defs[iId].iAmmo1 != -1)
				{
					pBot->current_weapon.iAmmo1 =
						pBot->m_rgAmmo[weapon_defs[iId].iAmmo1];
				}
				if (weapon_defs[iId].iAmmo2 != -1)
				{
					pBot->current_weapon.iAmmo2 =
						pBot->m_rgAmmo[weapon_defs[iId].iAmmo2];
				}
			}
		}
	}
}

void BotClient_SI_AmmoX(void *p, edict_t *pEdict)
{
	BotClient_Valve_AmmoX(p, pEdict);
}

void BotClient_SI_AmmoPickup(void *p, edict_t *pEdict)
{
	BotClient_Valve_AmmoPickup(p, pEdict);
}

void BotClient_SI_Damage(void *p, edict_t *pEdict)
{
	BotClient_Valve_Damage(p, pEdict);
}

void BotClient_SI_DeathMsg(void *p, edict_t *pEdict)
{
	BotClient_Valve_DeathMsg(p, pEdict);
}

void BotClient_SI_ScreenFade(void *p, edict_t *pEdict)
{
	BotClient_Valve_ScreenFade(p, pEdict);
}

void BotClient_SI_ItemPickup(void *p, edict_t *pEdict)
{
	BotClient_Valve_ItemPickup(p, pEdict);
}

void BotClient_SI_WeapPickup(void *p, edict_t *pEdict)
{
	BotClient_Valve_WeapPickup(p, pEdict);
}

void BotClient_SI_TeamCash(void *p, edict_t *pEdict)
{	/*
	if (!pEdict)
		return;

	static bot_t *pBot = NULL;
	pBot = UTIL_GetBotPointer(pEdict);

	if (!pBot)
		return;
	*/
	static int state = 0;   // current state machine state
	static int team = -1;
	static long cash = -1;

	if (state == 0)
	{
		state++;
		cash = *(long *)p;
	}
	else if (state == 1)
	{
		state = 0;
		team = *(int *)p;
	}

	if (team > -1)
	{
		g_lTeamScore[team] = cash;
		cash = -1;
		team = -1;
	}
}

void BotClient_SI_CarryInfo(void *p, edict_t *pEdict)
{
//   char msg[256];

	if (!pEdict)
		return;

	static bot_t *pBot = NULL;
	pBot = UTIL_GetBotPointer(pEdict);

//	if (!pBot)
//		return;

	static int state = 0;   // current state machine state
	static int carrying = 0;

	if (state == 0)
	{
		state++;
		carrying = *(int *)p;
		// make sure this is a valid bot
		if (pBot)
		{
			// are we carrying a scientist (or resource)
			pBot->i_carry_type = carrying;
			// if we have a goal, forget about it (since we either dropped the sci to
			// the admin, dropped it when we died, or just picked one up, all three involve
			// obtaining a new goal)
			if (pBot->waypoint_goal != -1)
			{	// get new goal!
				pBot->f_ignore_wpt_time = 0.0;
				pBot->waypoint_goal = -1;
			}
			// forget our enemy
			pBot->pBotEnemy = NULL;
			// reeval goal and role
			pBot->f_evaluate_goal_time = pBot->f_role_check = 0;
		}

		float distance = 0;
		float mindistance = 9999;
		int final_index = -1;
		int team = UTIL_GetTeam(pEdict);
		int closest = WaypointFindNearest(pEdict, 512, team);
		int bot_clostest = -1;
		// find the closest bot on the enemy team and tell them to attack this person
		for (int bot = 0; bot < 32; bot++)
		{
			if (bots[bot].is_used && bots[bot].bot_team != team && bots[bot].bot_team != -1 &&
				IsAlive(bots[bot].pEdict))
			{
				bot_clostest = WaypointFindNearest(bots[bot].pEdict, 512, bots[bot].bot_team);
				distance = WaypointDistanceFromTo(bot_clostest, closest, bots[bot].bot_team);
				//distance += (waypoints[closest].origin - pEdict->v.origin).Length();
				if (distance < mindistance)
				{	// remember this bot and reset min distance
					mindistance = distance;
					final_index = bot;
				}
			}
		}
		// force this bot to go after the thief
		if (final_index > -1 && final_index < 32)
		{	// loop through nearby entities for the entity this player just grabbed
			edict_t *pEntity = NULL;
			while ((pEntity = UTIL_FindEntityInSphere(pEntity, pEdict->v.origin, 32)) != NULL)
			{	// can't be same as player
				if (pEntity == pEdict)
					continue;
				// must be following player
				if (pEntity->v.aiment != pEdict)
					continue;
				// must not be a carry ent
				if (!IsCarryEnt(pEntity))
					continue;
				// this must be the right ent
				bots[final_index].role = ROLE_DEFEND;
				if (carrying == CARRY_SCI)
					bots[final_index].subrole = ROLE_SUB_DEF_SCIS;
				else if (carrying >= CARRY_RSRC)
					bots[final_index].subrole = ROLE_SUB_DEF_RSRC;

				bots[final_index].v_goal = g_vecZero;
				bots[final_index].pGoalEnt = pEntity;
				bots[final_index].b_defend_patrol = false;

				//sprintf(msg, "Forcing %s to attack %s for thievery!\n", bots[final_index].name,
				//	STRING(pEdict->v.netname));
				//SERVER_PRINT (msg);

				break;
			}
		}
	}
	else if (state == 1)
	{	// who are we carrying?
		state = 0;
		if (pBot)
			strcpy(pBot->c_carry_name, (char *)p);
	}
}

void BotClient_SI_Goal(void *p, edict_t *pEdict)
{	/*
	if (!pEdict)
		return;

	static bot_t *pBot = NULL;
	pBot = UTIL_GetBotPointer(pEdict);

	if (!pBot)
		return;
	*/
	static int state = 0;   // current state machine state
	static int team = -1;
	static int goal = -1;

	if (state == 0)
	{	// what is our research goal?
		state++;
		goal = *(int *)p;
	}
	else if (state == 1)
	{	// which team?
		state = 0;
		team = *(int *)p;
	}

	// update this team's goal
	if (goal >= 0 && team >= 0)
	{
		g_iResearchGoal[team] = goal;
		team = goal = -1;
	}
}

void BotClient_SI_Notice(void *p, edict_t *pEdict)
{
	if (!pEdict)
		return;

	static bot_t *pBot = NULL;
	pBot = UTIL_GetBotPointer(pEdict);

	if (!pBot)
		return;

	static int state = 0;   // current state machine state
	static int notice = -1;

	if (state == 0)
	{
		state++;
		notice = *(int *)p;
	}
	else if (state == 1)
	{	// string param1
		state++;
	}
	else if (state == 2)
	{	// string param2
		state++;
	}
	else if (state == 3)
	{	// string param3
		state = 0;
	}

	if (notice >= 0 && pBot->is_used && (pBot->pEdict->v.team == 0 ||
		pBot->pEdict->v.team == 1))
	{	// did we finish some research?
		if (notice == Notice_Research_Completed)
		{
			int team = UTIL_GetTeam(pEdict);
			g_Researched[team][g_iResearchGoal[team]].researched = true;
		}
		else if (notice == Notice_Sci_Missing)
		{	// someone took a scientist, rethink what the teams should be doing
			RoleCount();
			RoleDetermine();
			// reeval goal and role
			pBot->f_evaluate_goal_time = pBot->f_role_check = 0;
		}
	}
}

void BotClient_SI_Battery(void *p, edict_t *pEdict)
{
	if (!pEdict)
		return;

	static bot_t *pBot = NULL;
	pBot = UTIL_GetBotPointer(pEdict);

	if (!pBot)
		return;

	static int state = 0;   // current state machine state
	static int max_battery = -1;

	if (state == 0)
	{	// current battery
		state++;
	}
	else if (state == 1)
	{	// max battery
		state = 0;
		max_battery = *(int *)p;
	}

	// update this bot's max battery (armor)
	if (max_battery >= 0)
	{
		if (pBot->is_used)
			pBot->max_armor = max_battery;

		max_battery = -1;
	}
}

void BotClient_SI_VoteInfo(void *p, edict_t *pEdict)
{
	if (!pEdict)
		return;
	// this is commented out so that bots are aware of the new updated research paths
	// and abilities even if there are no bots on the team.  The bots will read this
	// message even if it is sent to players
	/*
	static bot_t *pBot = NULL;
	pBot = UTIL_GetBotPointer(pEdict);

	if (!pBot)
		return;
	*/
	static int state = 0;   // current state machine state
	static int data[4][2];
	static bool yes = false;
	static int sec = 0;
	static int byte = 0;
	static int team = -1;
	team = UTIL_GetTeam(pEdict);
	if (team < 0 || team > 1)
		return;
	// read our BYTEs and LONGs as integers
	if (state >= 0 && state <= 7)
	{
		data[sec][byte] = *(int *)p;
		
		if (byte > 0)
		{
			sec++;
			byte = 0;
		}
		else
			byte++;

		state++;
	}
	if (state == 8)
	{
		sec = byte = 0;

		// This looks nasty, but is actually straight forward. The VoteInfo message sends 4 "chunks" of data,
		// each chunk is then broken down into a LONG message and a BYTE message (these are all stored in
		// the code above, the static int "data" array).  
		// Now, the point in this message is to tell the client what they can and cannot vote for, this is
		// accomplished by telling the client what has been researched, what the canidates for research are
		// what has been stolen, and what is disabled (can't research at all).  There are two messages per
		// chunk because there are more than 32 technologies, so more than 32 bits are needed to say "yes" or
		// "no".  This code loops through the data array and reads the bits into a boolean, then modifies
		// the g_Researched array to indicate which research techs are already researched, canidates for
		// research, stolen, or disabled.  The bots NEED this message to know what they're capable of.
		int i = 0, r = 0;
		for (i = 0; i < 4; i++)
		{
			// look at the bits for the LONG data
			for (r = 0; r < 32; r++)
			{	// get our true or false based on value of the bit
				yes = ( data[i][0] & (1 << r) ) ? true : false;

				if (i == 0) g_Researched[team][r].researched = yes;
				else if (i == 1) g_Researched[team][r].canidate = yes;
				else if (i == 2) g_Researched[team][r].stolen = yes;
				else if (i == 3) g_Researched[team][r].disabled = yes;
			}
			// look at the bits for the BYTE data	
			for (r = 32; r < NUM_RESEARCH_OPTIONS; r++)
			{	// get our true or false based on value of the bit
				yes = ( data[i][1] & (1 << (r-32)) ) ? true : false;

				if (i == 0) g_Researched[team][r].researched = yes;
				else if (i == 1) g_Researched[team][r].canidate = yes;
				else if (i == 2) g_Researched[team][r].stolen = yes;
				else if (i == 3) g_Researched[team][r].disabled = yes;
			}
		}

		state = 0;
	}
<<<<<<< HEAD
=======
=======
//
// HPB bot - botman's High Ping Bastard bot
//
// (http://planethalflife.com/botman/)
//
// bot_client.cpp
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
#include "bot_client.h"
#include "bot_weapons.h"
#include "waypoint.h"

#ifdef METAMOD_BUILD
   // instant damage (from cbase.h)
   #define DMG_CRUSH			(1 << 0)	// crushed by falling or moving object
   #define DMG_BURN			(1 << 3)	// heat burned
   #define DMG_FREEZE			(1 << 4)	// frozen
   #define DMG_FALL			(1 << 5)	// fell too far
   #define DMG_SHOCK			(1 << 8)	// electric shock
   #define DMG_DROWN			(1 << 14)	// Drowning
   #define DMG_NERVEGAS		(1 << 16)	// nerve toxins, very bad
   #define DMG_RADIATION		(1 << 18)	// radiation exposure
   #define DMG_DROWNRECOVER	(1 << 19)	// drowning recovery
   #define DMG_ACID			(1 << 20)	// toxic chemicals or acid burns
   #define DMG_SLOWBURN		(1 << 21)	// in an oven
   #define DMG_SLOWFREEZE		(1 << 22)	// in a subzero freezer
#endif

// types of damage to ignore...
#define IGNORE_DAMAGE (DMG_CRUSH | DMG_FREEZE | DMG_FALL | DMG_SHOCK | \
                       DMG_DROWN | DMG_NERVEGAS | DMG_RADIATION | \
                       DMG_DROWNRECOVER | DMG_ACID | DMG_SLOWBURN | \
                       DMG_SLOWFREEZE | 0xFF000000)

extern int mod_id;
extern bot_t bots[32];

bot_weapon_t weapon_defs[MAX_WEAPONS]; // array of weapon definitions

extern long g_lTeamScore[2];
extern int g_iSciCount[2];

extern bot_research_t g_Researched[2][NUM_RESEARCH_OPTIONS];
int g_iResearchGoal[2] = { 0, 0 };

extern void RoleCount( void );
extern void RoleDetermine( void );

extern WAYPOINT waypoints[MAX_WAYPOINTS];
extern int num_waypoints;  // number of waypoints currently in use
extern int num_def_waypoints;

// This message is sent when a client joins the game.  All of the weapons
// are sent with the weapon ID and information about what ammo is used.
void BotClient_Valve_WeaponList(void *p, edict_t *pEdict)
{	/*
	if (!pEdict)
		return;

	static bot_t *pBot = NULL;
	pBot = UTIL_GetBotPointer(pEdict);

	if (!pBot)
		return;
	*/
	static int state = 0;   // current state machine state
	static bot_weapon_t bot_weapon;
	
	if (state == 0)
	{
		state++;
		strcpy(bot_weapon.szClassname, (char *)p);
	}
	else if (state == 1)
	{
		state++;
		bot_weapon.iAmmo1 = *(int *)p;  // ammo index 1
	}
	else if (state == 2)
	{
		state++;
		bot_weapon.iAmmo1Max = *(int *)p;  // max ammo1
	}
	else if (state == 3)
	{
		state++;
		bot_weapon.iAmmo2 = *(int *)p;  // ammo index 2
	}
	else if (state == 4)
	{
		state++;
		bot_weapon.iAmmo2Max = *(int *)p;  // max ammo2
	}
	else if (state == 5)
	{
		state++;
		bot_weapon.iSlot = *(int *)p;  // slot for this weapon
	}
	else if (state == 6)
	{
		state++;
		bot_weapon.iPosition = *(int *)p;  // position in slot
	}
	else if (state == 7)
	{
		state++;
		bot_weapon.iId = *(int *)p;  // weapon ID
	}
	else if (state == 8)
	{
		state = 0;
		
		bot_weapon.iFlags = *(int *)p;  // flags for weapon (WTF???)
		
		// store away this weapon with it's ammo information...
		weapon_defs[bot_weapon.iId] = bot_weapon;
	}
}

// This message is sent when a weapon is selected (either by the bot chosing
// a weapon or by the server auto assigning the bot a weapon).
void BotClient_Valve_CurrentWeapon(void *p, edict_t *pEdict)
{
	if (!pEdict)
		return;

	static bot_t *pBot = NULL;
	pBot = UTIL_GetBotPointer(pEdict);

	if (!pBot)
		return;

	static int state = 0;   // current state machine state
	static int iState;
	static int iId;
	static int iClip;
	
	if (state == 0)
	{
		state++;
		iState = *(int *)p;  // state of the current weapon
	}
	else if (state == 1)
	{
		state++;
		iId = *(int *)p;  // weapon ID of current weapon
	}
	else if (state == 2)
	{
		state = 0;
		
		iClip = *(int *)p;  // ammo currently in the clip for this weapon
		
		if (iId <= 31 && iId >= 0)
		{
			if (iState == 1)
			{
				pBot->current_weapon.iId = iId;
				pBot->current_weapon.iClip = iClip;
				pBot->current_weapon.iClip2 = -1;

				// update the ammo counts for this weapon...
				if (weapon_defs[iId].iAmmo1 != -1)
				{
					pBot->current_weapon.iAmmo1 =
						pBot->m_rgAmmo[weapon_defs[iId].iAmmo1];
				}
				if (weapon_defs[iId].iAmmo2 != -1)
				{
					pBot->current_weapon.iAmmo2 =
						pBot->m_rgAmmo[weapon_defs[iId].iAmmo2];
				}
			}
		}
	}
}

// This message is sent whenever ammo ammounts are adjusted (up or down).
void BotClient_Valve_AmmoX(void *p, edict_t *pEdict)
{
	if (!pEdict)
		return;

	static bot_t *pBot = NULL;
	pBot = UTIL_GetBotPointer(pEdict);

	if (!pBot)
		return;

	static int state = 0;   // current state machine state
	static int index;
	static int ammount;
	int ammo_index;
	
	if (state == 0)
	{
		state++;
		index = *(int *)p;  // ammo index (for type of ammo)
	}
	else if (state == 1)
	{
		state = 0;
		
		ammount = *(int *)p;  // the ammount of ammo currently available
		
		pBot->m_rgAmmo[index] = ammount;  // store it away
		
		ammo_index = pBot->current_weapon.iId;
		
		// update the ammo counts for this weapon...
		pBot->current_weapon.iAmmo1 =
			pBot->m_rgAmmo[weapon_defs[ammo_index].iAmmo1];
		pBot->current_weapon.iAmmo2 =
			pBot->m_rgAmmo[weapon_defs[ammo_index].iAmmo2];
	}
}

// This message is sent when the bot picks up some ammo (AmmoX messages are
// also sent so this message is probably not really necessary except it
// allows the HUD to draw pictures of ammo that have been picked up.  The
// bots don't really need pictures since they don't have any eyes anyway.
void BotClient_Valve_AmmoPickup(void *p, edict_t *pEdict)
{
	if (!pEdict)
		return;

	static bot_t *pBot = NULL;
	pBot = UTIL_GetBotPointer(pEdict);

	if (!pBot)
		return;

	static int state = 0;   // current state machine state
	static int index;
	static int ammount;
	int ammo_index;
	
	if (state == 0)
	{
		state++;
		index = *(int *)p;
	}
	else if (state == 1)
	{
		state = 0;
		
		ammount = *(int *)p;
		
		pBot->m_rgAmmo[index] = ammount;
		
		ammo_index = pBot->current_weapon.iId;
		
		// update the ammo counts for this weapon...
		pBot->current_weapon.iAmmo1 =
			pBot->m_rgAmmo[weapon_defs[ammo_index].iAmmo1];
		pBot->current_weapon.iAmmo2 =
			pBot->m_rgAmmo[weapon_defs[ammo_index].iAmmo2];
	}
}

// This message gets sent when the bots are getting damaged.
void BotClient_Valve_Damage(void *p, edict_t *pEdict)
{
	if (!pEdict)
		return;

	static bot_t *pBot = NULL;
	pBot = UTIL_GetBotPointer(pEdict);

	if (!pBot)
		return;

	static int state = 0;   // current state machine state
	static int damage_armor;
	static int damage_taken;
	static int damage_bits;  // type of damage being done
	static Vector damage_origin;
	
	if (state == 0)
	{
		state++;
		damage_armor = *(int *)p;
	}
	else if (state == 1)
	{
		state++;
		damage_taken = *(int *)p;
	}
	else if (state == 2)
	{
		state++;
		damage_bits = *(int *)p;
	}
	else if (state == 3)
	{
		state++;
		damage_origin.x = *(float *)p;
	}
	else if (state == 4)
	{
		state++;
		damage_origin.y = *(float *)p;
	}
	else if (state == 5)
	{
		state = 0;
		
		damage_origin.z = *(float *)p;
		
		if ((damage_armor > 0) || (damage_taken > 0))
		{
			// ignore certain types of damage...
			if (damage_bits & IGNORE_DAMAGE)
				return;
			
			// if the bot doesn't have an enemy and someone is shooting at it then
			// turn in the attacker's direction...
			if (pBot->pBotEnemy == NULL)
			{
				pBot->dmg_origin = damage_origin;
				pBot->f_dmg_time = gpGlobals->time + 1.0;
				
				// stop using health or HEV stations...
				pBot->b_use_health_station = FALSE;
				pBot->b_use_HEV_station = FALSE;
			}
		}
	}
}

// This message gets sent when the bots get killed
void BotClient_Valve_DeathMsg(void *p, edict_t *pEdict)
{	
}

void BotClient_Valve_ScreenFade(void *p, edict_t *pEdict)
{
	if (!pEdict)
		return;

	static bot_t *pBot = NULL;
	pBot = UTIL_GetBotPointer(pEdict);

	if (!pBot)
		return;

	static int state = 0;   // current state machine state
	static int duration;
	static int hold_time;
	static int fade_flags;
	int length;
	
	if (state == 0)
	{
		state++;
		duration = *(int *)p;
	}
	else if (state == 1)
	{
		state++;
		hold_time = *(int *)p;
	}
	else if (state == 2)
	{
		state++;
		fade_flags = *(int *)p;
	}
	else if (state == 6)
	{
		state = 0;
		
		length = (duration + hold_time) / 4096;
		pBot->blinded_time = gpGlobals->time + length - 2.0;
	}
	else
	{
		state++;
	}
}

// used to check if the bot has gotten a long jump
void BotClient_Valve_ItemPickup(void *p, edict_t *pEdict)
{
	if (!pEdict)
		return;

	static bot_t *pBot = NULL;
	pBot = UTIL_GetBotPointer(pEdict);

	if (!pBot)
		return;

	char itemname[64];
	strcpy(itemname,(char *)p);

	if (strcmp(itemname, "item_longjump") == 0)
	{
//		SERVER_PRINT( "%s has a longjump module now!\n", STRING(pBot->pEdict->v.netname));
		pBot->b_longjump = true;
	}
}

void BotClient_Valve_WeapPickup(void *p, edict_t *pEdict)
{
	// nothing to here
}

void BotClient_Valve_Battery(void *p, edict_t *pEdict)
{
	// nothing to here
}

//////////////////////////
// Science and Industry //
//////////////////////////

// WeaponList message in science and industry does
// not include HUD slot or position
void BotClient_SI_WeaponList(void *p, edict_t *pEdict)
{	/*
	if (!pEdict)
		return;

	static bot_t *pBot = NULL;
	pBot = UTIL_GetBotPointer(pEdict);

	if (!pBot)
		return;
	*/
	static int state = 0;   // current state machine state
	static bot_weapon_t bot_weapon;
	
	if (state == 0)
	{
		state++;
		strcpy(bot_weapon.szClassname, (char *)p);
	}
	else if (state == 1)
	{
		state++;
		bot_weapon.iAmmo1 = *(int *)p;  // ammo index 1
	}
	else if (state == 2)
	{
		state++;
		bot_weapon.iAmmo1Max = *(int *)p;  // max ammo1
	}
	else if (state == 3)
	{
		state++;
		bot_weapon.iAmmo2 = *(int *)p;  // ammo index 2
	}
	else if (state == 4)
	{
		state++;
		bot_weapon.iAmmo2Max = *(int *)p;  // max ammo2
	}
	else if (state == 5)
	{
		state++;
		bot_weapon.iId = *(int *)p;  // weapon ID
	}
	else if (state == 6)
	{
		state = 0;
		
		bot_weapon.iFlags = *(int *)p;  // flags for weapon (WTF???)
		
		// store away this weapon with it's ammo information...
		weapon_defs[bot_weapon.iId] = bot_weapon;
	}
}

// Science and Industry also has a secondary clip
void BotClient_SI_CurrentWeapon(void *p, edict_t *pEdict)
{
	if (!pEdict)
		return;

	static bot_t *pBot = NULL;
	pBot = UTIL_GetBotPointer(pEdict);

	if (!pBot)
		return;

	static int state = 0;   // current state machine state
	static int iState;
	static int iId;
	static int iClip;
	static int iClip2;
	
	if (state == 0)
	{
		state++;
		iState = *(int *)p;  // state of the current weapon
	}
	else if (state == 1)
	{
		state++;
		iId = *(int *)p;  // weapon ID of current weapon
	}
	else if (state == 2)
	{
		state++;
		iClip = *(int *)p;  // ammo currently in the clip for this weapon
	}
	else if (state == 3)
	{
		state = 0;
		
		iClip2 = *(int *)p;  // ammo currently in the secondary clip for this weapon
		
		if (iId <= 31 && iId >= 0)
		{
			if (iState == 1)
			{
				pBot->current_weapon.iId = iId;
				pBot->current_weapon.iClip = iClip;
				pBot->current_weapon.iClip2 = iClip2;
				
				// update the ammo counts for this weapon...
				if (weapon_defs[iId].iAmmo1 != -1)
				{
					pBot->current_weapon.iAmmo1 =
						pBot->m_rgAmmo[weapon_defs[iId].iAmmo1];
				}
				if (weapon_defs[iId].iAmmo2 != -1)
				{
					pBot->current_weapon.iAmmo2 =
						pBot->m_rgAmmo[weapon_defs[iId].iAmmo2];
				}
			}
		}
	}
}

void BotClient_SI_AmmoX(void *p, edict_t *pEdict)
{
	BotClient_Valve_AmmoX(p, pEdict);
}

void BotClient_SI_AmmoPickup(void *p, edict_t *pEdict)
{
	BotClient_Valve_AmmoPickup(p, pEdict);
}

void BotClient_SI_Damage(void *p, edict_t *pEdict)
{
	BotClient_Valve_Damage(p, pEdict);
}

void BotClient_SI_DeathMsg(void *p, edict_t *pEdict)
{
	BotClient_Valve_DeathMsg(p, pEdict);
}

void BotClient_SI_ScreenFade(void *p, edict_t *pEdict)
{
	BotClient_Valve_ScreenFade(p, pEdict);
}

void BotClient_SI_ItemPickup(void *p, edict_t *pEdict)
{
	BotClient_Valve_ItemPickup(p, pEdict);
}

void BotClient_SI_WeapPickup(void *p, edict_t *pEdict)
{
	BotClient_Valve_WeapPickup(p, pEdict);
}

void BotClient_SI_TeamCash(void *p, edict_t *pEdict)
{	/*
	if (!pEdict)
		return;

	static bot_t *pBot = NULL;
	pBot = UTIL_GetBotPointer(pEdict);

	if (!pBot)
		return;
	*/
	static int state = 0;   // current state machine state
	static int team = -1;
	static long cash = -1;

	if (state == 0)
	{
		state++;
		cash = *(long *)p;
	}
	else if (state == 1)
	{
		state = 0;
		team = *(int *)p;
	}

	if (team > -1)
	{
		g_lTeamScore[team] = cash;
		cash = -1;
		team = -1;
	}
}

void BotClient_SI_CarryInfo(void *p, edict_t *pEdict)
{
//   char msg[256];

	if (!pEdict)
		return;

	static bot_t *pBot = NULL;
	pBot = UTIL_GetBotPointer(pEdict);

//	if (!pBot)
//		return;

	static int state = 0;   // current state machine state
	static int carrying = 0;

	if (state == 0)
	{
		state++;
		carrying = *(int *)p;
		// make sure this is a valid bot
		if (pBot)
		{
			// are we carrying a scientist (or resource)
			pBot->i_carry_type = carrying;
			// if we have a goal, forget about it (since we either dropped the sci to
			// the admin, dropped it when we died, or just picked one up, all three involve
			// obtaining a new goal)
			if (pBot->waypoint_goal != -1)
			{	// get new goal!
				pBot->f_ignore_wpt_time = 0.0;
				pBot->waypoint_goal = -1;
			}
			// forget our enemy
			pBot->pBotEnemy = NULL;
			// reeval goal and role
			pBot->f_evaluate_goal_time = pBot->f_role_check = 0;
		}

		float distance = 0;
		float mindistance = 9999;
		int final_index = -1;
		int team = UTIL_GetTeam(pEdict);
		int closest = WaypointFindNearest(pEdict, 512, team);
		int bot_clostest = -1;
		// find the closest bot on the enemy team and tell them to attack this person
		for (int bot = 0; bot < 32; bot++)
		{
			if (bots[bot].is_used && bots[bot].bot_team != team && bots[bot].bot_team != -1 &&
				IsAlive(bots[bot].pEdict))
			{
				bot_clostest = WaypointFindNearest(bots[bot].pEdict, 512, bots[bot].bot_team);
				distance = WaypointDistanceFromTo(bot_clostest, closest, bots[bot].bot_team);
				//distance += (waypoints[closest].origin - pEdict->v.origin).Length();
				if (distance < mindistance)
				{	// remember this bot and reset min distance
					mindistance = distance;
					final_index = bot;
				}
			}
		}
		// force this bot to go after the thief
		if (final_index > -1 && final_index < 32)
		{	// loop through nearby entities for the entity this player just grabbed
			edict_t *pEntity = NULL;
			while ((pEntity = UTIL_FindEntityInSphere(pEntity, pEdict->v.origin, 32)) != NULL)
			{	// can't be same as player
				if (pEntity == pEdict)
					continue;
				// must be following player
				if (pEntity->v.aiment != pEdict)
					continue;
				// must not be a carry ent
				if (!IsCarryEnt(pEntity))
					continue;
				// this must be the right ent
				bots[final_index].role = ROLE_DEFEND;
				if (carrying == CARRY_SCI)
					bots[final_index].subrole = ROLE_SUB_DEF_SCIS;
				else if (carrying >= CARRY_RSRC)
					bots[final_index].subrole = ROLE_SUB_DEF_RSRC;

				bots[final_index].v_goal = g_vecZero;
				bots[final_index].pGoalEnt = pEntity;
				bots[final_index].b_defend_patrol = false;

				//sprintf(msg, "Forcing %s to attack %s for thievery!\n", bots[final_index].name,
				//	STRING(pEdict->v.netname));
				//SERVER_PRINT (msg);

				break;
			}
		}
	}
	else if (state == 1)
	{	// who are we carrying?
		state = 0;
		if (pBot)
			strcpy(pBot->c_carry_name, (char *)p);
	}
}

void BotClient_SI_Goal(void *p, edict_t *pEdict)
{	/*
	if (!pEdict)
		return;

	static bot_t *pBot = NULL;
	pBot = UTIL_GetBotPointer(pEdict);

	if (!pBot)
		return;
	*/
	static int state = 0;   // current state machine state
	static int team = -1;
	static int goal = -1;

	if (state == 0)
	{	// what is our research goal?
		state++;
		goal = *(int *)p;
	}
	else if (state == 1)
	{	// which team?
		state = 0;
		team = *(int *)p;
	}

	// update this team's goal
	if (goal >= 0 && team >= 0)
	{
		g_iResearchGoal[team] = goal;
		team = goal = -1;
	}
}

void BotClient_SI_Notice(void *p, edict_t *pEdict)
{
	if (!pEdict)
		return;

	static bot_t *pBot = NULL;
	pBot = UTIL_GetBotPointer(pEdict);

	if (!pBot)
		return;

	static int state = 0;   // current state machine state
	static int notice = -1;

	if (state == 0)
	{
		state++;
		notice = *(int *)p;
	}
	else if (state == 1)
	{	// string param1
		state++;
	}
	else if (state == 2)
	{	// string param2
		state++;
	}
	else if (state == 3)
	{	// string param3
		state = 0;
	}

	if (notice >= 0 && pBot->is_used && (pBot->pEdict->v.team == 0 ||
		pBot->pEdict->v.team == 1))
	{	// did we finish some research?
		if (notice == Notice_Research_Completed)
		{
			int team = UTIL_GetTeam(pEdict);
			g_Researched[team][g_iResearchGoal[team]].researched = true;
		}
		else if (notice == Notice_Sci_Missing)
		{	// someone took a scientist, rethink what the teams should be doing
			RoleCount();
			RoleDetermine();
			// reeval goal and role
			pBot->f_evaluate_goal_time = pBot->f_role_check = 0;
		}
	}
}

void BotClient_SI_Battery(void *p, edict_t *pEdict)
{
	if (!pEdict)
		return;

	static bot_t *pBot = NULL;
	pBot = UTIL_GetBotPointer(pEdict);

	if (!pBot)
		return;

	static int state = 0;   // current state machine state
	static int max_battery = -1;

	if (state == 0)
	{	// current battery
		state++;
	}
	else if (state == 1)
	{	// max battery
		state = 0;
		max_battery = *(int *)p;
	}

	// update this bot's max battery (armor)
	if (max_battery >= 0)
	{
		if (pBot->is_used)
			pBot->max_armor = max_battery;

		max_battery = -1;
	}
}

void BotClient_SI_VoteInfo(void *p, edict_t *pEdict)
{
	if (!pEdict)
		return;
	// this is commented out so that bots are aware of the new updated research paths
	// and abilities even if there are no bots on the team.  The bots will read this
	// message even if it is sent to players
	/*
	static bot_t *pBot = NULL;
	pBot = UTIL_GetBotPointer(pEdict);

	if (!pBot)
		return;
	*/
	static int state = 0;   // current state machine state
	static int data[4][2];
	static bool yes = false;
	static int sec = 0;
	static int byte = 0;
	static int team = -1;
	team = UTIL_GetTeam(pEdict);
	if (team < 0 || team > 1)
		return;
	// read our BYTEs and LONGs as integers
	if (state >= 0 && state <= 7)
	{
		data[sec][byte] = *(int *)p;
		
		if (byte > 0)
		{
			sec++;
			byte = 0;
		}
		else
			byte++;

		state++;
	}
	if (state == 8)
	{
		sec = byte = 0;

		// This looks nasty, but is actually straight forward. The VoteInfo message sends 4 "chunks" of data,
		// each chunk is then broken down into a LONG message and a BYTE message (these are all stored in
		// the code above, the static int "data" array).  
		// Now, the point in this message is to tell the client what they can and cannot vote for, this is
		// accomplished by telling the client what has been researched, what the canidates for research are
		// what has been stolen, and what is disabled (can't research at all).  There are two messages per
		// chunk because there are more than 32 technologies, so more than 32 bits are needed to say "yes" or
		// "no".  This code loops through the data array and reads the bits into a boolean, then modifies
		// the g_Researched array to indicate which research techs are already researched, canidates for
		// research, stolen, or disabled.  The bots NEED this message to know what they're capable of.
		int i = 0, r = 0;
		for (i = 0; i < 4; i++)
		{
			// look at the bits for the LONG data
			for (r = 0; r < 32; r++)
			{	// get our true or false based on value of the bit
				yes = ( data[i][0] & (1 << r) ) ? true : false;

				if (i == 0) g_Researched[team][r].researched = yes;
				else if (i == 1) g_Researched[team][r].canidate = yes;
				else if (i == 2) g_Researched[team][r].stolen = yes;
				else if (i == 3) g_Researched[team][r].disabled = yes;
			}
			// look at the bits for the BYTE data	
			for (r = 32; r < NUM_RESEARCH_OPTIONS; r++)
			{	// get our true or false based on value of the bit
				yes = ( data[i][1] & (1 << (r-32)) ) ? true : false;

				if (i == 0) g_Researched[team][r].researched = yes;
				else if (i == 1) g_Researched[team][r].canidate = yes;
				else if (i == 2) g_Researched[team][r].stolen = yes;
				else if (i == 3) g_Researched[team][r].disabled = yes;
			}
		}

		state = 0;
	}
>>>>>>> 3f195a42c7b84bf4071b789634ebbe14b6297ebc
>>>>>>> 594320fd6db9c6f9b3b7d743a0270568ddbba6bd
}