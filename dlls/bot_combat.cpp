//
// HPB bot - botman's High Ping Bastard bot
//
// (http://planethalflife.com/botman/)
//
// bot_combat.cpp
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

extern int mod_id;
extern bot_weapon_t weapon_defs[MAX_WEAPONS];
extern bool b_observer_mode;
extern int team_allies[4];
extern edict_t *pent_info_ctfdetect;
extern float is_team_play;
extern bool checked_teamplay;
extern edict_t *listenserver_edict;
extern bool b_chat_debug;
FILE *fp;

float aim_tracking_x_scale[5] = {0, 1.0, 2.0, 3.0, 4.0};
float aim_tracking_y_scale[5] = {0, 1.0, 2.0, 3.0, 4.0};
// who is vomiting?
float g_flVomiting[32];
// reaction time multiplier
extern float bot_reaction_time;
float react_time_min[5] = {0.01, 0.05, 0.08, 0.1, 0.15};
float react_time_max[5] = {0.01, 0.08, 0.1, 0.15, 0.2};
// how transparent does our target have to be before we
// can't see them?  Diffs per bot level
int renderamt_threshold[5] = {16, 32, 48, 64, 80};

bot_research_t g_Researched[2][NUM_RESEARCH_OPTIONS];

float g_flWeaponSwitch = 0;

void BotCheckTeamplay()
{
//	ALERT(at_console, "BotCheckTeamplay\n");

	is_team_play = CVAR_GET_FLOAT("mp_teamplay");  // teamplay enabled?
	
	checked_teamplay = TRUE;
}


edict_t *BotFindEnemy( bot_t *pBot )
{
//	ALERT(at_console, "BotFindEnemy\n");
	Vector vecEnd;
	static bool flag=TRUE;
	edict_t *pent = nullptr;
	edict_t *pNewEnemy;
	edict_t *pRemember = nullptr;
	float nearestdistance;
	int i;
	
	edict_t *pEdict = pBot->pEdict;
	
	if (pEdict->v.flags & FL_GODMODE)
		return nullptr;
	
	if (pBot->pBotEnemy != nullptr)  // does the bot already have an enemy?
	{
		vecEnd = UTIL_GetOrigin(pBot->pBotEnemy) + pBot->pBotEnemy->v.view_ofs;

		// if the enemy is dead?
		if (!IsAlive(pBot->pBotEnemy))  // is the enemy dead?, assume bot killed it
		{
			// the enemy is dead, jump for joy about 10% of the time
			//if (RANDOM_LONG(1, 100) <= 10)
			//	pEdict->v.button |= IN_JUMP;
			
			// don't have an enemy anymore so null out the pointer...
			pBot->pBotEnemy = nullptr;
		}
		else if (FInViewCone( &vecEnd, pEdict ) &&
			FVisible( vecEnd, pEdict ))
		{
			{
				// if enemy is still visible and in field of view, keep it
				// keep track of when we last saw an enemy
				pBot->f_bot_see_enemy_time = gpGlobals->time;
				// remember our current enemy and check for a new one
				pRemember = pBot->pBotEnemy;
			}
		}
		else if ((!FInViewCone( &vecEnd, pEdict ) ||
			!FVisible( vecEnd, pEdict )) && (!pBot->b_engaging_enemy))
		{	// remember our enemy for 2 seconds even if they're not visible
			if (pBot->f_bot_see_enemy_time > (gpGlobals->time - 2))
				pRemember = pBot->pBotEnemy;
			pBot->pBotEnemy = nullptr;
			pBot->f_ignore_wpt_time = 0.0;
		}
	}
	
	pent = nullptr;
	pNewEnemy = nullptr;
	nearestdistance = 1000;
	
	if (pNewEnemy == nullptr)
	{
		edict_t *pMonster = nullptr;
		Vector vecEnd;
			
		nearestdistance = 2500;
		
		if (pRemember)
			nearestdistance = (UTIL_GetOrigin(pRemember) - UTIL_GetOrigin(pEdict)).Length();

		// search the world for monsters...
		while (!FNullEnt(pMonster = UTIL_FindEntityInSphere(pMonster, pEdict->v.origin, nearestdistance)))
		{
			// ignore our remembered enemy
			if ((pRemember != nullptr) && (pMonster == pRemember))
				continue;

			// not a player are they?
			if (pMonster->v.flags & FL_CLIENT)
				continue;

			// don't attack hornets
			if (mod_id != SI_DLL && FStrEq(STRING(pMonster->v.classname), "hornet"))
				continue;

			if (!FStrEq(STRING(pMonster->v.classname), "func_tech_breakable"))
			{
				if (!(pMonster->v.flags & FL_MONSTER))
					continue; // discard anything that is not a monster
				
				if (!IsAlive(pMonster))
					continue; // discard dead or dying monsters
				
				if ((pMonster->v.rendermode == kRenderTransTexture &&
					pMonster->v.renderamt < renderamt_threshold[pBot->bot_skill]) ||
					pMonster->v.effects & EF_NODRAW)
					continue;
			}
			// skip scientists in S&I if we already are carrying an item and don't have the mindray
			if (mod_id == SI_DLL && FStrEq(STRING(pMonster->v.classname), "monster_scientist") &&
				(pBot->i_carry_type > CARRY_NONE && !(pBot->pEdict->v.weapons & (1<<SI_WEAPON_MINDRAY))))
				continue;

			// is team play enabled?
			if (is_team_play > 0.0)
			{
				int player_team = UTIL_GetTeam(pMonster);
				int bot_team = UTIL_GetTeam(pEdict);
					
				// don't target your teammates as long as they're not a scientist and we don't have the mindray
				if (mod_id == SI_DLL && bot_team == player_team &&
					(!FStrEq(STRING(pMonster->v.classname), "monster_scientist") ||
					!(pBot->pEdict->v.weapons & (1<<SI_WEAPON_MINDRAY))))
					continue;
			}

			vecEnd = UTIL_GetOrigin(pMonster) + pMonster->v.view_ofs;
			
			// see if bot can't see the monster...
			if (!FInViewCone( &vecEnd, pEdict ) ||
				!FVisible( vecEnd, pEdict ))
				continue;
			
			float distance = (vecEnd - pEdict->v.origin).Length();

			if (mod_id == SI_DLL)
			{	// only notice scis if they're close and we don't have the mindray
				if (FStrEq(STRING(pMonster->v.classname), "monster_scientist") &&
					((pBot->i_carry_type == CARRY_NONE && distance > 128) ||
					(pBot->i_carry_type >= CARRY_SCI && !(pBot->pEdict->v.weapons & (1<<SI_WEAPON_MINDRAY)))))
					continue; 
			}

			if (distance < nearestdistance)
			{
				nearestdistance = distance;
				pNewEnemy = pMonster;
				
				pBot->pBotUser = nullptr;  // don't follow user when enemy found
			}
		}

		// search the world for players...
		for (i = 1; i <= gpGlobals->maxClients; i++)
		{
			edict_t *pPlayer = INDEXENT(i);
			
			// skip invalid players and skip self (i.e. this bot)
			if ((pPlayer) && (!pPlayer->free) && (pPlayer != pEdict) && (pPlayer->v.flags & FL_CLIENT))
			{
				// ignore our remembered enemy
				if ((pRemember != nullptr) && (pPlayer == pRemember))
					continue;

				// skip this player if not valid
				if (pPlayer->free)
					continue;

				// skip this player if not alive (i.e. dead or dying)
				if (!IsAlive(pPlayer))
					continue;
				
				if ((b_observer_mode) && !(pPlayer->v.flags & FL_FAKECLIENT))
					continue;
				// can we see them?
				if ((pPlayer->v.rendermode == kRenderTransTexture &&
					pPlayer->v.renderamt < renderamt_threshold[pBot->bot_skill]) ||
					pPlayer->v.effects & EF_NODRAW)
					continue;
				
				// can we hurt them? (stops bots from attacking kicked player's left over edict)
				if (pPlayer->v.takedamage == DAMAGE_NO)
					continue;

				if (!checked_teamplay)  // check for team play...
					BotCheckTeamplay();
				
				// is team play enabled?
				if (is_team_play > 0.0)
				{
					int player_team = UTIL_GetTeam(pPlayer);
					int bot_team = UTIL_GetTeam(pEdict);
					
					// don't target your teammates...
					if (bot_team == player_team)
						continue;
				}
				
				vecEnd = pPlayer->v.origin + pPlayer->v.view_ofs;
				
				// see if bot can see the player...
				if (FInViewCone( &vecEnd, pEdict ) &&
					FVisible( vecEnd, pEdict ))
				{
					float distance = (pPlayer->v.origin - pEdict->v.origin).Length();
					if (distance < nearestdistance)
					{
						nearestdistance = distance;
						pNewEnemy = pPlayer;
						
						pBot->pBotUser = nullptr;  // don't follow user when enemy found
					}
				}
			}
		}
	}
	// couldn't find a new enemy so remember the old one we can't see
	if (pNewEnemy == nullptr && pRemember != nullptr)
		pNewEnemy = pRemember;
	// are we engaging an enemy?  Don't forget about them
	if (pNewEnemy == nullptr && pBot->b_engaging_enemy && pBot->pBotEnemy != nullptr)
		pNewEnemy = pBot->pBotEnemy;

	if (pNewEnemy)
	{
		// face the enemy
		Vector v_enemy = pNewEnemy->v.origin - pEdict->v.origin;
		Vector bot_angles = UTIL_VecToAngles( v_enemy );
		/*	Let shoot at enemy handle this!
		pEdict->v.ideal_yaw = bot_angles.y;
		
		BotFixIdealYaw(pEdict);
		*/
		// we have a reaction time AND our new enemy is not the current enemy AND we're not remembering
		// an old enemy
		if ((bot_reaction_time > 0) && (pNewEnemy != pBot->pBotEnemy) && (!pRemember))
		{
			float react_delay;
			float delay_min = react_time_min[pBot->bot_skill] * bot_reaction_time;
			float delay_max = react_time_max[pBot->bot_skill] * bot_reaction_time;

			float distance_delay = log10(v_enemy.Length()) * 0.8;
			// don't get an advantage if they're too close
			if (distance_delay < 1.0) distance_delay = 1.0;

			react_delay = RANDOM_FLOAT(delay_min, delay_max) * distance_delay;
			
			pBot->f_reaction_target_time = gpGlobals->time + react_delay;

			//SERVER_PRINT( "%s reacting in %f seconds!\n", STRING(pEdict->v.netname), react_delay);
		}
		// get our origin
		vecEnd = UTIL_GetOrigin(pNewEnemy) + pNewEnemy->v.view_ofs;

		// keep track of when we last saw an enemy
		if (FInViewCone( &vecEnd, pEdict ) &&
			FVisible( vecEnd, pEdict ))	
			pBot->f_bot_see_enemy_time = gpGlobals->time;
	}
	
	// has the bot NOT seen an ememy for at least 5 seconds (time to reload)?
	if ((pBot->f_bot_see_enemy_time > 0) &&
		((pBot->f_bot_see_enemy_time + 5.0) <= gpGlobals->time))
	{
		pBot->f_bot_see_enemy_time = -1;  // so we won't keep reloading
		
		pEdict->v.button |= IN_RELOAD;  // press reload button
	}
	
	return (pNewEnemy);
}

int BotGetEnemyWeapon( edict_t *pEnemy )
{
//	ALERT(at_console, "BotGetEnemyWeapon\n");

	bot_weapon_select_t *pSelect = nullptr;
	pSelect = WeaponGetSelectPointer();

	if ((pEnemy->v.flags & FL_CLIENT) && (pSelect != nullptr))
	{
		int select_index = 0;

		while (pSelect[select_index].iId)
		{	// does our enemy weapon model match this weapon's model?
			// compare!
			if (strstr(STRING(pEnemy->v.weaponmodel), pSelect[select_index].weapon_model) != nullptr)
				break;

			select_index++;
		}

		return pSelect[select_index].iId;
	}

	return VALVE_WEAPON_UNKNOWN;
}

bool BotShouldEngageEnemy( bot_t *pBot, edict_t *pEnemy )
{	// this function might need some tweaking?
//	ALERT(at_console, "BotShouldEngageEnemy\n");
	
	bot_weapon_select_t *pSelect = nullptr;
	pSelect = WeaponGetSelectPointer();

	// must have enemy and the enemy must be a client
	if ((pSelect == nullptr) || (pEnemy == nullptr) || 
		(strcmp(STRING(pEnemy->v.classname), "player") != 0))
		return FALSE;

	// never engage the enemy if we have a sci/rsrc
	if (mod_id == SI_DLL && pBot->i_carry_type)
		return FALSE;

	int our_weapon = WeaponGetSelectIndex(pBot->current_weapon.iId);
	int enemy_weapon = WeaponGetSelectIndex(BotGetEnemyWeapon(pEnemy));

	float primary_ammo = BotAssessPrimaryAmmo(pBot, pBot->current_weapon.iId);
	float secondary_ammo = BotAssessSecondaryAmmo(pBot, pBot->current_weapon.iId);
	// don't engage if our ammo is low!
	if (((primary_ammo == AMMO_CRITICAL) && (secondary_ammo == AMMO_CRITICAL)) ||
		((primary_ammo == AMMO_CRITICAL) && (secondary_ammo == AMMO_NONE)) ||
		((primary_ammo == AMMO_NONE) && (secondary_ammo == AMMO_CRITICAL)))
		return FALSE;

	for (int i = 0; i < 4; i++)
	{
		// is our weapon not good compared to their weapon? AND
		// our health is less than their health plus armor
		if ((pSelect[our_weapon].priority > (pSelect[enemy_weapon].priority + i)) &&
			((pBot->pEdict->v.health + pBot->pEdict->v.armorvalue) < (pEnemy->v.health +
			pEnemy->v.armorvalue + (25 * i))))
		{
			if (b_chat_debug)
			{
				sprintf(pBot->debugchat, "I won't engage %s! i = %i (%s - %i > %s - %i) (%.0f < %.0f)\n",
					STRING(pEnemy->v.netname), i, 
					pSelect[our_weapon].weapon_name, pSelect[our_weapon].priority,
					pSelect[enemy_weapon].weapon_name, pSelect[enemy_weapon].priority,
					pBot->pEdict->v.health + pBot->pEdict->v.armorvalue,
					pEnemy->v.health + pEnemy->v.armorvalue + (25 * i));
				UTIL_HostSay(pBot->pEdict, 0, pBot->debugchat);
			}
			return FALSE;
		}
	}

	return TRUE;
}

Vector BotBodyTarget( edict_t *pBotEnemy, bot_t *pBot )
{
//	ALERT(at_console, "BotBodyTarget\n");

	if (!pBotEnemy)
		return g_vecZero;

	// get our origin for world brush entities
	if (strncmp(STRING(pBotEnemy->v.classname), "func_", 5) == 0)
		return VecBModelOrigin(pBotEnemy);

	if (strcmp("monster_headcrab", STRING(pBotEnemy->v.classname)) == 0)
		return (pBotEnemy->v.origin + Vector(0,0,8));

	// bots of skill 3 and higher aim at the torso
	if ((pBot->bot_skill + 1) >= 3)
	{	// monsters need special case, origin is at feet, so add half the view offset
		if (strncmp("monster_", STRING(pBotEnemy->v.classname), 5) == 0)
			return (pBotEnemy->v.origin + (pBotEnemy->v.view_ofs / 2));

		return (pBotEnemy->v.origin);
	}

	// aim for the torso with the mindray against scis
	if (mod_id == SI_DLL && FStrEq(STRING(pBotEnemy->v.classname), "monster_scientist") &&
		pBot->current_weapon.iId == SI_WEAPON_MINDRAY)
		return (pBotEnemy->v.origin + (pBotEnemy->v.view_ofs / 2));

	return (pBotEnemy->v.origin + pBotEnemy->v.view_ofs);
}

// specifing a weapon_choice allows you to choose the weapon the bot will
// use (assuming enough ammo exists for that weapon)
// BotFireWeapon will return TRUE if weapon was fired, FALSE otherwise
bool BotFireWeapon(Vector v_enemy, bot_t *pBot, int weapon_choice, bool nofire)
{
//ALERT(at_console, "BotFireWeapon\n");
	bot_weapon_select_t *pSelect = nullptr;
	bot_fire_delay_t *pDelay = nullptr;
	int select_index;
	int iId;
	int primary_percent;
	
	edict_t *pEdict = pBot->pEdict;
	
	float distance = v_enemy.Length();  // how far away is the enemy?
	
	pSelect = WeaponGetSelectPointer();
	pDelay = WeaponGetDelayPointer();
	
	bool use_primary[MAX_WEAPONS];
	bool use_secondary[MAX_WEAPONS];

	if (pSelect)
	{
		// are we charging the primary fire?
		if (pBot->f_primary_charging > 0)
		{
			iId = pBot->charging_weapon_id;
			
			// is it time to fire the charged weapon?
			if (pBot->f_primary_charging <= gpGlobals->time)
			{
				// we DON'T set pEdict->v.button here to release the
				// fire button which will fire the charged weapon
				
				pBot->f_primary_charging = -1;  // -1 means not charging
				
				// find the correct fire delay for this weapon
				select_index = 0;
				
				while ((pSelect[select_index].iId) &&
					(pSelect[select_index].iId != iId))
					select_index++;
				
				// set next time to shoot
				int skill = pBot->bot_skill;
				float base_delay, min_delay, max_delay;
				
				base_delay = pDelay[select_index].primary_base_delay;
				min_delay = pDelay[select_index].primary_min_delay[skill];
				max_delay = pDelay[select_index].primary_max_delay[skill];
				
				pBot->f_shoot_time = gpGlobals->time + base_delay +
					RANDOM_FLOAT(min_delay, max_delay);
				
				return TRUE;
			}
			else
			{
				pEdict->v.button |= IN_ATTACK;   // charge the weapon
				pBot->f_shoot_time = gpGlobals->time;  // keep charging
				
				return TRUE;
			}
		}
		
		// are we charging the secondary fire?
		if (pBot->f_secondary_charging > 0)
		{
			iId = pBot->charging_weapon_id;
			
			// is it time to fire the charged weapon?
			if (pBot->f_secondary_charging <= gpGlobals->time)
			{
				// we DON'T set pEdict->v.button here to release the
				// fire button which will fire the charged weapon
				
				pBot->f_secondary_charging = -1;  // -1 means not charging
				
				// find the correct fire delay for this weapon
				select_index = 0;
				
				while ((pSelect[select_index].iId) &&
					(pSelect[select_index].iId != iId))
					select_index++;
				
				// set next time to shoot
				int skill = pBot->bot_skill;
				float base_delay, min_delay, max_delay;
				
				base_delay = pDelay[select_index].secondary_base_delay;
				min_delay = pDelay[select_index].secondary_min_delay[skill];
				max_delay = pDelay[select_index].secondary_max_delay[skill];
				
				pBot->f_shoot_time = gpGlobals->time + base_delay +
					RANDOM_FLOAT(min_delay, max_delay);
				
				return TRUE;
			}
			else
			{
				pEdict->v.button |= IN_ATTACK2;  // charge the weapon
				pBot->f_shoot_time = gpGlobals->time;  // keep charging
				
				return TRUE;
			}
		}
		
		select_index = 0;
		int best_priority = MAX_WEAPONS;
		int final_index = 0;
/*
		while (pSelect[select_index].iId)
		{
			// is the bot NOT carrying this weapon?
			if (!(pEdict->v.weapons & (1<<pSelect[select_index].iId)))
			{
				//ALERT( at_console, "Skipping %s, don't have it\n", pSelect[select_index].weapon_name);
				select_index++;  // skip to next weapon
				continue;
			}
			
			if (pSelect[select_index].iId == pBot->current_weapon.iId)
			{
				select_index++;  // skip to next weapon
				continue;
			}
			
			final_index = select_index;
			break;
			//select_index++;
		}*/
	
		// loop through all the weapons until terminator is found...
		while (pSelect[select_index].iId)
		{
			// was a weapon choice specified? (and if so do they NOT match?)
			if (weapon_choice != 0 &&
				weapon_choice != pSelect[select_index].iId)
			{
				//ALERT( at_console, "Skipping %s, not our choice\n", pSelect[select_index].weapon_name);
				select_index++;  // skip to next weapon
				continue;
			}
			
			// is the bot NOT carrying this weapon?
			if (!(pEdict->v.weapons & (1<<pSelect[select_index].iId)))
			{
				//ALERT( at_console, "Skipping %s, don't have it\n", pSelect[select_index].weapon_name);
				select_index++;  // skip to next weapon
				continue;
			}   
			
			// is the bot NOT skilled enough to use this weapon?
			if ((pBot->bot_skill+1) > pSelect[select_index].skill_level &&
				weapon_choice == 0)
			{
				//ALERT( at_console, "Skipping %s, not skilled enough\n", pSelect[select_index].weapon_name);
				select_index++;  // skip to next weapon
				continue;
			}
			
			// is the bot underwater and does this weapon NOT work under water?
			if ((pEdict->v.waterlevel == 3) &&
				!(pSelect[select_index].can_use_underwater))
			{
				//ALERT( at_console, "Skipping %s, doesn't work underwater\n", pSelect[select_index].weapon_name);
				select_index++;  // skip to next weapon
				continue;
			}
			
			if (mod_id == SI_DLL && pSelect[select_index].iId == SI_WEAPON_MINDRAY && (!pBot->pBotEnemy ||
				!FStrEq(STRING(pBot->pBotEnemy->v.classname), "monster_scientist") ||
				(pBot->pBotEnemy && UTIL_GetTeam(pBot->pBotEnemy) != pBot->bot_team &&
				pBot->i_carry_type == CARRY_NONE)))
			{
				//ALERT( at_console, "Skipping %s, shouldn't mindray\n", pSelect[select_index].weapon_name);
				select_index++;
				continue;
			}
			
			// forget about the GI Distabilizer if they're already vomiting
			if (mod_id == SI_DLL && pSelect[select_index].iId == SI_WEAPON_VOMIT && pBot->pBotEnemy &&
				(g_flVomiting[ENTINDEX(pBot->pBotEnemy)-1] + 0.5) > gpGlobals->time)
			{
				//ALERT( at_console, "Skipping %s, already vomiting\n", pSelect[select_index].weapon_name);
				select_index++;
				continue;
			}

			// forget about the EMP Cannon if they don't have any armor
			if (mod_id == SI_DLL && pSelect[select_index].iId == SI_WEAPON_EMPCANNON && pBot->pBotEnemy &&
				pBot->pBotEnemy->v.armorvalue <= 0)
			{
				//ALERT( at_console, "Skipping %s, target has no armor\n", pSelect[select_index].weapon_name);
				select_index++;
				continue;
			}
			
			// is this weapon worse than our previous choice?
			if (((pSelect[select_index].priority >= best_priority) ||
				(pSelect[select_index].priority < 0)) && (best_priority != VALVE_WEAPON_CROWBAR ||
				best_priority != SI_WEAPON_BRIEFCASE))
			{
				//ALERT( at_console, "Skipping %s, priority too low\n", pSelect[select_index].weapon_name);
				select_index++;  // skip to next weapon
				continue;
			}

			iId = pSelect[select_index].iId;
			use_primary[select_index] = FALSE;
			use_secondary[select_index] = FALSE;
			primary_percent = RANDOM_LONG(1, 100);
			
			float primary_assess = BotAssessPrimaryAmmo(pBot, pSelect[select_index].iId);
			float secondary_assess = BotAssessSecondaryAmmo(pBot, pSelect[select_index].iId);
			// see if there is enough secondary ammo AND
			// the bot is far enough away to use secondary fire AND
			// the bot is close enough to the enemy to use secondary fire
			if (pSelect[select_index].primary_fire_percent < 100 &&
				primary_percent > pSelect[select_index].primary_fire_percent &&
				(secondary_assess != AMMO_CRITICAL ||
				(pBot->current_weapon.iId == iId &&
				pBot->current_weapon.iClip2 >= pSelect[select_index].min_secondary_ammo) ||
				(pBot->current_weapon.iId == iId &&
				pBot->current_weapon.iClip2 < pSelect[select_index].min_secondary_ammo &&
				pBot->current_weapon.iClip2 != -1 && secondary_assess != AMMO_CRITICAL)) &&
				distance >= pSelect[select_index].secondary_min_distance &&
				distance <= pSelect[select_index].secondary_max_distance)
			{
				use_secondary[select_index] = TRUE;
			}
			
			// NOTE: The EMP Cannon for Science and Industry has a wacky way of ammo handling
			// and does NOT use this if block to determine if it can be fired.
			//
			// is primary percent less than weapon primary percent AND
			// no ammo required for this weapon OR
			// enough ammo available to fire OR
			// we're currently on this weapon and it has a clip OR
			// we're currently on this weapon, clip ran out, but we still have ammo reserve AND
			// the bot is far enough away to use primary fire AND
			// the bot is close enough to the enemy to use primary fire
			if (!use_secondary[select_index] && ((mod_id != SI_DLL) ||
				(mod_id == SI_DLL && iId != SI_WEAPON_EMPCANNON)) &&
				(primary_assess != AMMO_CRITICAL || 
				(pBot->current_weapon.iId == iId &&
				pBot->current_weapon.iClip >= pSelect[select_index].min_primary_ammo) ||
				(pBot->current_weapon.iId == iId &&
				pBot->current_weapon.iClip < pSelect[select_index].min_primary_ammo &&
				pBot->current_weapon.iClip != -1 && primary_assess != AMMO_CRITICAL)) &&
				distance >= pSelect[select_index].primary_min_distance &&
				distance <= pSelect[select_index].primary_max_distance)
			{
				use_primary[select_index] = TRUE;
			}
			
			// EMP Cannon check
			// Our clip has ammo in it OR
			// we have a cannister left AND
			// the bot is far enough away to use primary fire AND
			// the bot is close enough to the enemy to use primary fire
			if (!use_secondary[select_index] && mod_id == SI_DLL && iId == SI_WEAPON_EMPCANNON &&
				((pBot->current_weapon.iId == iId &&
				pBot->current_weapon.iClip >= pSelect[select_index].min_primary_ammo) ||
				(pBot->m_rgAmmo[weapon_defs[iId].iAmmo2] > 0)) &&
				distance >= pSelect[select_index].primary_min_distance &&
				distance <= pSelect[select_index].primary_max_distance)
			{
				use_primary[select_index] = TRUE;
			}

			// see if there wasn't enough ammo to fire the weapon...
			if (use_primary[select_index] == FALSE && use_secondary[select_index] == FALSE)
			{
				//SERVER_PRINT( "Skipping %s, can't use primary or secondary\n", pSelect[select_index].weapon_name);
				select_index++;  // skip to next weapon
				continue;
			}

			final_index = select_index;
			best_priority = pSelect[select_index].priority;

			select_index++;
		}
	
//		ALERT( at_console, "Selected %s\n", pSelect[final_index].weapon_name);

		iId = pSelect[final_index].iId;

		// select this weapon if it isn't already selected
		if (pBot->current_weapon.iId != iId/* && g_flWeaponSwitch <= gpGlobals->time*/)
		{
			//ALERT(at_console, "Switch weapon\n");
			//g_flWeaponSwitch = gpGlobals->time + 1.0;
			//ALERT( at_console, "Switching to %s\n", pSelect[final_index].weapon_name);
			UTIL_SelectItem(pEdict, pSelect[final_index].weapon_name);
			pBot->f_shoot_time = gpGlobals->time + 0.5;
			pBot->f_reload_time = 0;
			return FALSE;
		}
		
		if (nofire) // just select the weapon
			return FALSE;
			
		if (pDelay[final_index].iId != iId)
		{
			char msg[80];
			sprintf(msg, "fire_delay mismatch for weapon id=%d\n",iId);
			SERVER_PRINT(msg);
				
			return FALSE;
		}
			
		if (use_primary[final_index] && pBot->current_weapon.iId == iId && pBot->current_weapon.iClip != -1 &&
			pBot->current_weapon.iClip < pSelect[final_index].min_primary_ammo &&
			pBot->f_reload_time <= gpGlobals->time)
		{	// reload if our clip is running out
			pBot->f_reload_time = gpGlobals->time + pSelect[final_index].reload_delay;
			pEdict->v.button |= IN_RELOAD;
			return FALSE;
		}
		
		// zoom in if crossbow
		if ((((mod_id == CRABBED_DLL || mod_id == VALVE_DLL) && iId == VALVE_WEAPON_CROSSBOW) ||
			(mod_id == SI_DLL && iId == SI_WEAPON_CROSSBOW)) &&
			(pEdict->v.fov == 0))
		{
			pEdict->v.button |= IN_ATTACK2;
			return TRUE;
		}

		if (use_primary[final_index])
		{
			pEdict->v.button |= IN_ATTACK;  // use primary attack
			// for dual uzies, we want to fire both guns at the same time
			if (mod_id == SI_DLL && iId == SI_WEAPON_SNUZI)
				pEdict->v.button |= IN_ATTACK2;

			if (pSelect[final_index].primary_fire_charge)
			{
				pBot->charging_weapon_id = iId;
					
				// release primary fire after the appropriate delay...
				pBot->f_primary_charging = gpGlobals->time +
					pSelect[final_index].primary_charge_delay;
					
				pBot->f_shoot_time = gpGlobals->time;  // keep charging
			}
			else
			{
				// set next time to shoot
				if (pSelect[final_index].primary_fire_hold)
					pBot->f_shoot_time = gpGlobals->time;  // don't let button up
				else
				{
					int skill = pBot->bot_skill;
					float base_delay, min_delay, max_delay;
						
					base_delay = pDelay[final_index].primary_base_delay;
					min_delay = pDelay[final_index].primary_min_delay[skill];
					max_delay = pDelay[final_index].primary_max_delay[skill];
						
					pBot->f_shoot_time = gpGlobals->time + base_delay +
						RANDOM_FLOAT(min_delay, max_delay);
				}
			}
		}
		else if (use_secondary[final_index]) // MUST be use_secondary...
		{
			pEdict->v.button |= IN_ATTACK2;  // use secondary attack
			// for dual uzies, we want to fire both guns at the same time
			if (mod_id == SI_DLL && iId == SI_WEAPON_SNUZI)
				pEdict->v.button |= IN_ATTACK;
			
			if (pSelect[final_index].secondary_fire_charge)
			{
				pBot->charging_weapon_id = iId;
				
				// release secondary fire after the appropriate delay...
				pBot->f_secondary_charging = gpGlobals->time +
					pSelect[final_index].secondary_charge_delay;
					
				pBot->f_shoot_time = gpGlobals->time;  // keep charging
			}
			else
			{
				// set next time to shoot
				if (pSelect[final_index].secondary_fire_hold)
					pBot->f_shoot_time = gpGlobals->time;  // don't let button up
				else
				{
					int skill = pBot->bot_skill;
					float base_delay, min_delay, max_delay;
						
					base_delay = pDelay[final_index].secondary_base_delay;
					min_delay = pDelay[final_index].secondary_min_delay[skill];
					max_delay = pDelay[final_index].secondary_max_delay[skill];
						
					pBot->f_shoot_time = gpGlobals->time + base_delay +
						RANDOM_FLOAT(min_delay, max_delay);
				}
			}
			return TRUE;  // weapon was fired
		}
	
	}
   
	// didn't have any available weapons or ammo, return FALSE
	return FALSE;
}

Vector BotGetLead( bot_t *pBot, edict_t *pEntity, float flProjSpeed )
{
//	ALERT(at_console, "BotGetLead\n");

	if (!pEntity)
		return BotBodyTarget( pBot->pBotEnemy, pBot );
	// get our origin and distance to the entity
	Vector vecOrigin = BotBodyTarget( pBot->pBotEnemy, pBot );
	float flDistance = (vecOrigin - UTIL_GetOrigin(pBot->pEdict)).Length();
	// factor in the entity's velocity multiplied by the percent of distance out of our
	// weapon's projectile speed
	Vector vecNewOrigin = vecOrigin;
	// so we don't divide by 0
	if (flProjSpeed > 0)
	{
		vecNewOrigin.x += pEntity->v.velocity.x * (flDistance/flProjSpeed);
		vecNewOrigin.y += pEntity->v.velocity.y * (flDistance/flProjSpeed);
		vecNewOrigin.z += pEntity->v.velocity.z * (flDistance/flProjSpeed);
	}
	// factor in a small amount of the bot's current velocity
	vecNewOrigin.x += pBot->pEdict->v.velocity.x * -0.005;
	vecNewOrigin.y += pBot->pEdict->v.velocity.y * -0.005;
	vecNewOrigin.z += pBot->pEdict->v.velocity.z * -0.005;

	return vecNewOrigin;
}

void BotShootAtEnemy( bot_t *pBot )
{
//	ALERT(at_console, "BotShootAtEnemy\n");

	if (!pBot->pBotEnemy)
		return;

	int team = UTIL_GetTeam(pBot->pEdict);
	float f_distance;
	float f_velocity;
	TraceResult tr;
	edict_t *pEdict = pBot->pEdict;
	
	Vector v_enemy_origin = BotBodyTarget( pBot->pBotEnemy, pBot );
	Vector v_lead_origin = BotGetLead(pBot, pBot->pBotEnemy, WeaponProjectileSpeed(pBot->current_weapon.iId));
	// aim for the head and/or body
	Vector v_enemy = v_lead_origin - GetGunPosition(pEdict);

	Vector enemy_angle = UTIL_VecToAngles( v_enemy );
	
	if (enemy_angle.x > 180)
		enemy_angle.x -= 360;
	
	if (enemy_angle.y > 180)
		enemy_angle.y -= 360;
	
	// adjust the view angle pitch to aim correctly
	enemy_angle.x = -enemy_angle.x;
	
	float d_x, d_y;
	
	d_x = (enemy_angle.x - pEdict->v.v_angle.x);
	d_y = (enemy_angle.y - pEdict->v.v_angle.y);

	if (pBot->f_aim_tracking_time < gpGlobals->time && pBot->bot_skill > 0)
	{
		pBot->f_aim_tracking_time = gpGlobals->time + RANDOM_FLOAT(0.5, 3.0);

		pBot->f_aim_x_angle_delta = 
			RANDOM_FLOAT(-aim_tracking_x_scale[pBot->bot_skill],aim_tracking_x_scale[pBot->bot_skill]);
		pBot->f_aim_y_angle_delta = 
			RANDOM_FLOAT(-aim_tracking_y_scale[pBot->bot_skill],aim_tracking_y_scale[pBot->bot_skill]);

//		SERVER_PRINT( "%s x delta is %.2f, y delta is %.2f\n", pBot->name,
//			pBot->f_aim_x_angle_delta, pBot->f_aim_y_angle_delta);
	}
	// bot skill 1 (0 for indexes) has perfect aim
	if (pBot->bot_skill > 0)
	{	// speed of enemy matters (don't let it be less than 1)
		f_velocity = fmax(pBot->pBotEnemy->v.velocity.Length() * 0.01, 1);
		// multiple our deltas by the velocity
		d_x += pBot->f_aim_x_angle_delta * f_velocity;
		d_y += pBot->f_aim_y_angle_delta * f_velocity;
	}

	if (d_x > 180.0f)
		d_x -= 360.0f;
	if (d_x < -180.0f)
		d_x += 360.0f;

	if (d_y > 180.0f)
		d_y -= 360.0f;
	if (d_y < -180.0f)
		d_y += 360.0f;

	if (!pBot->b_combat_longjump)
	{
		pEdict->v.idealpitch = pEdict->v.v_angle.x + d_x;
		BotFixIdealPitch(pEdict);
		
		pEdict->v.ideal_yaw = pEdict->v.v_angle.y + d_y;
		BotFixIdealYaw(pEdict);
	}

	Vector bot_angle, vecSrc, vecEnd;
	//v_enemy.z = 0;  // ignore z component (up & down)
	
	f_distance = v_enemy.Length();  // how far away is the enemy scum?

	// allow 15 seconds for the mindray to regen it's ammo if it's low
	if (mod_id == SI_DLL && pBot->current_weapon.iId == SI_WEAPON_MINDRAY &&
		BotAssessPrimaryAmmo(pBot, SI_WEAPON_MINDRAY) == AMMO_CRITICAL)
		pBot->f_mindray_regen_time = gpGlobals->time + 15.0;

	// if this is Science and Industry and our enemy is a scientist, run at them
	// and attack with the briefcase!
	if (pBot->pBotEnemy && mod_id == SI_DLL &&
		FStrEq(STRING(pBot->pBotEnemy->v.classname), "monster_scientist"))
	{
		int enemy_team = UTIL_GetTeam(pBot->pBotEnemy);
		// engage if enemy scientist and we don't have anything
		if (enemy_team != team && pBot->i_carry_type == CARRY_NONE)
		{
			pBot->f_move_speed = pBot->f_max_speed;
			pBot->f_ignore_wpt_time = gpGlobals->time + 0.2;
		}

		if (FInViewCone(&v_enemy_origin, pEdict) && FVisible(v_enemy_origin, pEdict))
		{	// switch to the briefcase
			if (pBot->f_shoot_time <= gpGlobals->time && f_distance <= 128.0 && enemy_team != team &&
				pBot->current_weapon.iId != SI_WEAPON_BRIEFCASE && pBot->i_carry_type == CARRY_NONE)
				BotFireWeapon(v_enemy, pBot, SI_WEAPON_BRIEFCASE, true);
			// actually hit them
			if (pBot->f_shoot_time <= gpGlobals->time && f_distance <= 50.0 && enemy_team != team &&
				pBot->i_carry_type == CARRY_NONE)
				BotFireWeapon(v_enemy, pBot, SI_WEAPON_BRIEFCASE);
/*			// use mindray if we have it, it's an enemy sci and we're carrying something, or it's an ally sci
			else if (pBot->f_shoot_time <= gpGlobals->time && pBot->f_mindray_regen_time < gpGlobals->time &&
				((pBot->i_carry_type > CARRY_NONE && enemy_team != team) || (enemy_team == team)) &&
				pBot->pEdict->v.weapons & (1<<SI_WEAPON_MINDRAY))
				BotFireWeapon(v_enemy, pBot, SI_WEAPON_MINDRAY);*/
		}

		return;
	}

	if (pBot->f_engage_enemy_check <= gpGlobals->time)
		pBot->b_last_engage = BotShouldEngageEnemy(pBot, pBot->pBotEnemy);

	bool bShouldEngage = pBot->b_last_engage;

	if ((RANDOM_LONG(1,100) < pBot->i_engage_aggressiveness) && (!pBot->b_engaging_enemy) && 
		(bShouldEngage))
	{
		// remember our current goal so we can go back to it once the enemy is dead (or we flee)
		if (pBot->waypoint_goal != -1)
			pBot->old_waypoint_goal = pBot->waypoint_goal;

		pBot->b_engaging_enemy = TRUE;
		if (b_chat_debug)
		{
			sprintf(pBot->debugchat, "I am going to engage %s...\n", STRING(pBot->pBotEnemy->v.netname));
			UTIL_HostSay(pBot->pEdict, 0, pBot->debugchat);
		}
	}

	// see if we should engage the enemy every half second
	if (pBot->f_engage_enemy_check <= gpGlobals->time)
		pBot->f_engage_enemy_check = gpGlobals->time + 0.5;

	if (pBot->b_engaging_enemy && !bShouldEngage)
	{
		if (b_chat_debug)
		{
			sprintf(pBot->debugchat, "I gave up engaging %s!\n", STRING(pBot->pBotEnemy->v.netname));
			UTIL_HostSay(pBot->pEdict, 0, pBot->debugchat);
		}
		pBot->f_ignore_wpt_time = 0.0;
		pBot->b_engaging_enemy = FALSE;
		if (pBot->old_waypoint_goal != -1)
		{
			pBot->waypoint_goal = pBot->old_waypoint_goal;
			pBot->old_waypoint_goal = -1;
		}
	}
	else if (pBot->b_engaging_enemy && bShouldEngage)
	{
		// we're close, forget about waypoints and try to use combat tactics
		if (f_distance < 512 && FInViewCone( &v_enemy_origin, pEdict ) &&
			FHullClear( v_enemy_origin, pEdict ))
		{
			pBot->f_ignore_wpt_time = gpGlobals->time + 0.1;
		}

	}

	// find the difference in the current and ideal angle
	float diff = std::fabs(pEdict->v.v_angle.y - pEdict->v.ideal_yaw);

	if (pBot->curr_waypoint_index == -1 || pBot->f_ignore_wpt_time > gpGlobals->time)
	{
		pBot->f_move_speed = pBot->f_max_speed;
		if (((f_distance < 64 && !pBot->b_longjump) || (f_distance < 128 && pBot->b_longjump)) &&
			pBot->current_weapon.iId != SI_WEAPON_BRIEFCASE && pBot->current_weapon.iId != VALVE_WEAPON_CROWBAR)
			pBot->f_move_speed *= -1;
		// bot skill 5 doesn't even try to strafe
		if ((pBot->bot_skill + 1) < 5)
		{	/*
			// get the direction we're traveling
			bot_angle = pBot->pEdict->v.v_angle;
			bot_angle.x = 0;
			float dgrad = asin(pBot->f_strafe_speed / pBot->f_max_speed) * 180 / PI;
			bot_angle.y += dgrad;

			if (bot_angle.y > 180)
				bot_angle.y -= 360;
			if (bot_angle.y < -180)
				bot_angle.y += 360;

			MAKE_VECTORS(bot_angle);
			*/
			vecSrc = pEdict->v.origin;
			vecEnd = pBot->v_curr_direction.Normalize() * pBot->f_max_speed * pBot->f_frame_time;
			// trace a line to see if anything is in our way
			UTIL_TraceLine(vecSrc, vecSrc + vecEnd, dont_ignore_monsters, pEdict, &tr);

			// switch our strafe direction every so often or if something is in the way
			if (pBot->f_strafe_chng_dir <= gpGlobals->time || tr.flFraction < 1.0)
			{
				pBot->f_strafe_chng_dir = gpGlobals->time + RANDOM_FLOAT(1.0, 3.0);
				pBot->b_strafe_direction = !pBot->b_strafe_direction;//RANDOM_LONG(0,1) ? true : false;
			}
			// go strafe crazy!
			pBot->f_strafe_speed = pBot->b_strafe_direction ? pBot->f_max_speed : -pBot->f_max_speed;

			// try to do a tricky longjump
			if ((pBot->b_longjump) && (pBot->f_combat_longjump < gpGlobals->time) && 
				(!pBot->b_combat_longjump) && (pEdict->v.flags & FL_ONGROUND) &&
				(f_distance > 128) && (f_distance < (LONGJUMP_DISTANCE * 
				(800 / CVAR_GET_FLOAT("sv_gravity")))) && (diff <= 1))
			{
				pBot->b_longjump_dir = RANDOM_LONG(0,1) ? true : false;
				bot_angle = pBot->pEdict->v.v_angle;
				bot_angle.x = -bot_angle.x;
				vecSrc = pEdict->v.origin;
				// get a random angle (-30 or 30)
				int mod = pBot->b_longjump_dir ? -1 : 1;
				for (int i = 0; i < 2; i++)
				{
					Vector target_angle = bot_angle;
					target_angle.y += 30 * mod;

					if (target_angle.y > 180)
						target_angle.y -= 360;
					if (target_angle.y < -180)
						target_angle.y += 360;
							
					MAKE_VECTORS( target_angle );
					vecEnd = gpGlobals->v_forward * (LONGJUMP_DISTANCE * 
						(800 / CVAR_GET_FLOAT("sv_gravity")));

					// are we all clear?
					UTIL_TraceLine(vecSrc, vecSrc + vecEnd, dont_ignore_monsters, pEdict, &tr);

//					if (listenserver_edict)
//						WaypointDrawBeam(listenserver_edict, vecSrc, vecSrc + vecEnd, 20, 0, 255, 32, 32, 200, 10);

					if (tr.flFraction >= 1.0)
					{
						//SERVER_PRINT( "Clear longjump path found\n");
						pBot->b_combat_longjump = TRUE;
						pEdict->v.ideal_yaw += 30 * mod;
						diff = 1;
						BotFixIdealYaw(pEdict);
						break;
					}
					// try other direction;
					mod *= -1;
				}
				pBot->f_combat_longjump = gpGlobals->time + 0.2;
			}
		}

		// stop trying to longjump after half a second
		if ((pBot->f_combat_longjump < gpGlobals->time - 0.5) && (pBot->b_combat_longjump))
			pBot->b_combat_longjump = FALSE;

		// longjump
		if ((pEdict->v.waterlevel == 0) && (pEdict->v.flags & FL_ONGROUND) &&
			(pBot->b_longjump) && (pEdict->v.velocity.Length() > 220) && (pBot->b_combat_longjump) &&
			(diff <= 0.01))
		{
			// don't try to move for 1.0 seconds, otherwise the longjump
			// is fucked up	
			pBot->f_longjump_time = gpGlobals->time + 1.0;
			// we're don trying to do a longjump
			pBot->b_combat_longjump = FALSE;
			// don't do another one for a certain amount of time
			if (RANDOM_LONG(1,100) > 10)
				pBot->f_combat_longjump = gpGlobals->time + 1.0;//RANDOM_FLOAT(0.5, 1.0);
			else
				pBot->f_combat_longjump = gpGlobals->time + RANDOM_LONG(3.0,10.0);
			// actually do the longjump
			if (mod_id != SI_DLL)	// S&I auto longjumps, HLDM/Crabbed don't
				pEdict->v.button |= IN_DUCK;
			else if (mod_id == SI_DLL) // have to be going forward in S&I
				pEdict->v.button |= IN_FORWARD;
			pEdict->v.button |= IN_JUMP;
			//SERVER_PRINT( "%s doing longjump!\n", STRING(pEdict->v.netname));
		}

		if (pBot->f_longjump_time > gpGlobals->time)
			pBot->f_move_speed = pBot->f_strafe_speed = 0;
	}
	// is it time to shoot yet?
	if ((pBot->f_shoot_time <= gpGlobals->time) && !(pBot->pEdict->v.flags & FL_GODMODE) &&
		!(pBot->pBotEnemy->v.flags & FL_GODMODE) && 
		(FInViewCone(&v_enemy_origin, pEdict)) && (FVisible(v_enemy_origin, pEdict)) && 
		(pBot->f_reaction_target_time < gpGlobals->time) && (pBot->f_reload_time < gpGlobals->time)/* && (diff <= 1)*/)
	{
		// select the best weapon to use at this distance and fire...
		BotFireWeapon(v_enemy, pBot, 0);
	}
}

// look around for tripmines, timed grenades, contact grenades, 
// satchels, RPG rockets, and snarks.  Shoot them. 
void BotAssessGrenades( bot_t *pBot )
{
//	ALERT(at_console, "BotAssessGrenades\n");
	edict_t *pEdict = pBot->pEdict;
	edict_t *pGrenade = nullptr;
	edict_t *pNewGrenade = nullptr;
	Vector vecEnd;
	float nearestdistance = 16384;
	float mindistance = 256;
	// search the world for grenades...
	while (!FNullEnt(pGrenade = UTIL_FindEntityInSphere (pGrenade, pEdict->v.origin, 1000)))
	{
		vecEnd = pGrenade->v.origin;
		
		if (mod_id == CRABBED_DLL)
		{	// crabbed lets players shoot anything
			if ((strcmp("monster_satchel", STRING(pGrenade->v.classname)) != 0) && 
				(strcmp("monster_tripmine", STRING(pGrenade->v.classname)) != 0) && 
				(strcmp("grenade", STRING(pGrenade->v.classname)) != 0) && 
				(strcmp("rpg_rocket", STRING(pGrenade->v.classname)) != 0) && 
				(strcmp("monster_snark", STRING(pGrenade->v.classname)) != 0))
				continue;
		}
		else
		{
			if ((strcmp("monster_tripmine", STRING(pGrenade->v.classname)) != 0) && 
				(strcmp("monster_snark", STRING(pGrenade->v.classname)) != 0))
				continue;

		}
		// don't shoot our own grenades!
		if (pGrenade->v.owner == pEdict)
            continue;

		// don't shoot our teams grenades on S&I
		if (UTIL_GetTeam(pGrenade) == UTIL_GetTeam(pEdict))
            continue;

		// see if bot can't see the grenade...
		if (!FInViewCone( &vecEnd, pEdict ) ||
			!FVisible( vecEnd, pEdict ))
            continue;
		
		mindistance = 256;
		// we don't care how close snarks are, but the others explode
		if (strcmp("monster_snark", STRING(pGrenade->v.classname)) == 0)
			mindistance = 0;
		
		float distance = (pGrenade->v.origin - pEdict->v.origin).Length();
		// our current enemy is closer, forget the grenade
		if (pBot->pBotEnemy != nullptr &&
			(pGrenade->v.origin - UTIL_GetOrigin(pBot->pBotEnemy)).Length() < distance)
			continue;
		// is the grenade the right distance away?
		if (distance < nearestdistance && distance > mindistance)
		{
            nearestdistance = distance;
            pNewGrenade = pGrenade;
			
            pBot->pBotUser = nullptr;  // don't follow user when we've found a grenade
		}
	}
	
	if (pNewGrenade)
	{
		// the grenade is our enemy, blast it!
		//SERVER_PRINT( "%s - found %s!\n", STRING(pEdict->v.netname), STRING(pNewGrenade->v.classname));
		pBot->pBotEnemy = pNewGrenade;
	}
}

bool BotWeaponPrimaryDistance( bot_t *pBot, float distance, int weapon_id )
{
//	ALERT(at_console, "BotWeaponPrimaryDistance\n");
	bot_weapon_select_t *pSelect = nullptr;
	pSelect = WeaponGetSelectPointer();
	// select pointer not valid?
	if (pSelect == nullptr)
		return FALSE;

	int select_index = 0;
	// loop through all the weapons until terminator is found...
	while (pSelect[select_index].iId)
	{
		if (pSelect[select_index].iId != weapon_id)
		{
			select_index++;
			continue;
		}
		else
			break;
	}

	if ((distance < pSelect[select_index].primary_min_distance) &&
		(distance > pSelect[select_index].primary_max_distance))
		return FALSE;

	return TRUE;
}

bool BotWeaponSecondaryDistance( bot_t *pBot, float distance, int weapon_id )
{
//	ALERT(at_console, "BotWeaponSecondaryDistance\n");
	bot_weapon_select_t *pSelect = nullptr;
	pSelect = WeaponGetSelectPointer();
	// select pointer not valid?
	if (pSelect == nullptr)
		return FALSE;

	int select_index = 0;
	// loop through all the weapons until terminator is found...
	while (pSelect[select_index].iId)
	{
		if (pSelect[select_index].iId != weapon_id)
		{
			select_index++;
			continue;
		}
		else
			break;
	}

	if ((distance < pSelect[select_index].secondary_min_distance) &&
		(distance > pSelect[select_index].secondary_max_distance))
		return FALSE;

	return TRUE;
}

float BotAssessPrimaryAmmo( bot_t *pBot, int weapon_id )
{
//	ALERT(at_console, "BotAssessPrimaryAmmo\n");
	bot_weapon_select_t *pSelect = nullptr;
	pSelect = WeaponGetSelectPointer();
	int team = UTIL_GetTeam(pBot->pEdict);
	// select pointer not valid?
	if (pSelect == nullptr)
		return AMMO_NONE;
	// does this weapon even use ammo?	
	if (weapon_defs[weapon_id].iAmmo1 == -1 || weapon_defs[weapon_id].iAmmo1Max <= 0)
		return AMMO_NONE;

	int select_index = WeaponGetSelectIndex(weapon_id);

	int max = weapon_defs[weapon_id].iAmmo1Max;
	if (mod_id == SI_DLL)
	{
		if (team < 0 || team > 1)
			return AMMO_NONE;

		// we don't have either upgrade
		if (!g_Researched[team][RESEARCH_AMMO_REPLICATE].researched &&
			!g_Researched[team][RESEARCH_AMMO_REPLICATE].stolen &&
			!g_Researched[team][RESEARCH_AMMO_REPLICATE2].researched &&
			!g_Researched[team][RESEARCH_AMMO_REPLICATE2].stolen)
			max -= WeaponGetAmmoResearchDiff(weapon_id) * 2;
		// we have replicate 1, but not 2
		else if ((g_Researched[team][RESEARCH_AMMO_REPLICATE].researched ||
			g_Researched[team][RESEARCH_AMMO_REPLICATE].stolen) &&
			(!g_Researched[team][RESEARCH_AMMO_REPLICATE2].researched &&
			!g_Researched[team][RESEARCH_AMMO_REPLICATE2].stolen))
			max -= WeaponGetAmmoResearchDiff(weapon_id);
	}

	float ammo_percent = (float)pBot->m_rgAmmo[weapon_defs[weapon_id].iAmmo1] / (float)max;
	// is our ammo critical (can't attack with this weapon)
	if ((weapon_id != pBot->current_weapon.iId &&
		pBot->m_rgAmmo[weapon_defs[weapon_id].iAmmo1] < pSelect[select_index].min_primary_ammo) ||
		(weapon_id == pBot->current_weapon.iId &&
		pBot->m_rgAmmo[weapon_defs[weapon_id].iAmmo1] < pSelect[select_index].min_primary_ammo &&
		((pBot->current_weapon.iClip != -1 && pBot->current_weapon.iClip < pSelect[select_index].min_primary_ammo) ||
		(pBot->current_weapon.iClip == -1))))
		return AMMO_CRITICAL;

	return ammo_percent;
}

float BotAssessSecondaryAmmo( bot_t *pBot, int weapon_id )
{
//	ALERT(at_console, "BotAssessSecondaryAmmo\n");
	bot_weapon_select_t *pSelect = nullptr;
	pSelect = WeaponGetSelectPointer();
	int team = UTIL_GetTeam(pBot->pEdict);
	// select pointer not valid?
	if (pSelect == nullptr)
		return AMMO_NONE;
	// does this weapon even use ammo?	
	if (weapon_defs[weapon_id].iAmmo2 == -1 || weapon_defs[weapon_id].iAmmo2Max <= 0)
		return AMMO_NONE;

	int select_index = WeaponGetSelectIndex(weapon_id);

	int max = weapon_defs[weapon_id].iAmmo2Max;
	if (mod_id == SI_DLL)
	{
		if (team < 0 || team > 1)
			return AMMO_NONE;

		// we don't have either upgrade
		if (!g_Researched[team][RESEARCH_AMMO_REPLICATE].researched &&
			!g_Researched[team][RESEARCH_AMMO_REPLICATE].stolen &&
			!g_Researched[team][RESEARCH_AMMO_REPLICATE2].researched &&
			!g_Researched[team][RESEARCH_AMMO_REPLICATE2].stolen)
			max -= WeaponGetAmmoResearchDiff(weapon_id) * 2;
		// we have replicate 1, but not 2
		else if ((g_Researched[team][RESEARCH_AMMO_REPLICATE].researched ||
			g_Researched[team][RESEARCH_AMMO_REPLICATE].stolen) &&
			(!g_Researched[team][RESEARCH_AMMO_REPLICATE2].researched &&
			!g_Researched[team][RESEARCH_AMMO_REPLICATE2].stolen))
			max -= WeaponGetAmmoResearchDiff(weapon_id);
	}

	float ammo_percent = (float)pBot->m_rgAmmo[weapon_defs[weapon_id].iAmmo2] / (float)max;
	// is our ammo critical (can't attack with this weapon)
	if (pBot->m_rgAmmo[weapon_defs[weapon_id].iAmmo2] < pSelect[select_index].min_secondary_ammo && 
		(weapon_id == pBot->current_weapon.iId && pBot->current_weapon.iClip2 < pSelect[select_index].min_secondary_ammo))
		return AMMO_CRITICAL;
		
	return ammo_percent;
}
