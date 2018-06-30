<<<<<<< HEAD
=======
<<<<<<< HEAD
>>>>>>> 594320fd6db9c6f9b3b7d743a0270568ddbba6bd
//
// HPB bot - botman's High Ping Bastard bot
//
// (http://planethalflife.com/botman/)
//
// bot_start.cpp
//

#ifndef _WIN32
#include <string.h>
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
#include "bot_func.h"
#include "bot_weapons.h"

extern int mod_id;

extern int max_team_players[4];
extern int team_class_limits[4];
extern int max_teams;
extern bool checked_teamplay;

void BotStartGame( bot_t *pBot )
{
	char c_team[32];
	char c_model[32];
	edict_t *pEdict = pBot->pEdict;
	int team[2] = {0,0};

	if (!checked_teamplay)  // check for team play...
		BotCheckTeamplay();

	if (mod_id == SI_DLL)
	{
		if ((pBot->start_action == MSG_SI_IDLE))
		{
			// go into spectator mode if we're not already, to fix rejoin bug
			if (pEdict->v.view_ofs != g_vecZero)
				FakeClientCommand(pEdict, "spectate", NULL, NULL);
			
			// select our team after one second
			if (pBot->f_create_time + 1.0 <= gpGlobals->time)
				pBot->start_action = MSG_SI_TEAM_SELECT;  // force team selection

			return;
		}
		else if (pBot->start_action == MSG_SI_TEAM_SELECT)
		{
			pBot->start_action = MSG_SI_MODEL_SELECT;  // force model selection

			if (pBot->bot_team == -1)
			{
				// pick team with least amount of players
				for (int i = 0; i < 2; i++)
					team[i] = UTIL_PlayersOnTeam(i);

				//SERVER_PRINT( "%s found MCL(1) has %i player(s), AFD(2) has %i player(s)...\n",
				//	pBot->name, team[0], team[1]);

				// choose a random team
				if (team[0] == team[1])
					sprintf(c_team, "%i", 3);
				// join team with least players
				else if (team[0] < team[1])
					sprintf(c_team, "%i", 1);
				else if (team[0] > team[1])
					sprintf(c_team, "%i", 2);
			}
			else	// forced to join a certain team
			{
				if (pBot->bot_team < 1)
					pBot->bot_team = 1;
				else if (pBot->bot_team > 3)
					pBot->bot_team = 3;

				sprintf(c_team, "%i", pBot->bot_team);
			}

			//SERVER_PRINT( "%s will join team %s...\n", pBot->name, c_team);
			// save our team
			pBot->bot_team = atoi(c_team) - 1;

			FakeClientCommand(pEdict, "setteam", c_team, NULL);

			return;
		}
		else if (pBot->start_action == MSG_SI_MODEL_SELECT)
		{
			pBot->start_action = MSG_SI_IDLE;  // switch back to idle
			pBot->f_create_time = gpGlobals->time;  // reset
			if (pBot->bot_class == -1)
			{	// random model for now
				sprintf(c_model, "%i", 3);
			}
			else
				sprintf(c_model, "%i", pBot->bot_class);

			pBot->bot_class = atoi(c_model) - 1;

			FakeClientCommand(pEdict, "setmodel", c_model, NULL);

			pBot->not_started = 0;
			return;
		}
	}
	else
		pBot->not_started = 0;
<<<<<<< HEAD
=======
=======
//
// HPB bot - botman's High Ping Bastard bot
//
// (http://planethalflife.com/botman/)
//
// bot_start.cpp
//

#ifndef _WIN32
#include <string.h>
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
#include "bot_func.h"
#include "bot_weapons.h"

extern int mod_id;

extern int max_team_players[4];
extern int team_class_limits[4];
extern int max_teams;
extern bool checked_teamplay;

void BotStartGame( bot_t *pBot )
{
	char c_team[32];
	char c_model[32];
	edict_t *pEdict = pBot->pEdict;
	int team[2] = {0,0};

	if (!checked_teamplay)  // check for team play...
		BotCheckTeamplay();

	if (mod_id == SI_DLL)
	{
		if ((pBot->start_action == MSG_SI_IDLE))
		{
			// go into spectator mode if we're not already, to fix rejoin bug
			if (pEdict->v.view_ofs != g_vecZero)
				FakeClientCommand(pEdict, "spectate", NULL, NULL);
			
			// select our team after one second
			if (pBot->f_create_time + 1.0 <= gpGlobals->time)
				pBot->start_action = MSG_SI_TEAM_SELECT;  // force team selection

			return;
		}
		else if (pBot->start_action == MSG_SI_TEAM_SELECT)
		{
			pBot->start_action = MSG_SI_MODEL_SELECT;  // force model selection

			if (pBot->bot_team == -1)
			{
				// pick team with least amount of players
				for (int i = 0; i < 2; i++)
					team[i] = UTIL_PlayersOnTeam(i);

				//SERVER_PRINT( "%s found MCL(1) has %i player(s), AFD(2) has %i player(s)...\n",
				//	pBot->name, team[0], team[1]);

				// choose a random team
				if (team[0] == team[1])
					sprintf(c_team, "%i", 3);
				// join team with least players
				else if (team[0] < team[1])
					sprintf(c_team, "%i", 1);
				else if (team[0] > team[1])
					sprintf(c_team, "%i", 2);
			}
			else	// forced to join a certain team
			{
				if (pBot->bot_team < 1)
					pBot->bot_team = 1;
				else if (pBot->bot_team > 3)
					pBot->bot_team = 3;

				sprintf(c_team, "%i", pBot->bot_team);
			}

			//SERVER_PRINT( "%s will join team %s...\n", pBot->name, c_team);
			// save our team
			pBot->bot_team = atoi(c_team) - 1;

			FakeClientCommand(pEdict, "setteam", c_team, NULL);

			return;
		}
		else if (pBot->start_action == MSG_SI_MODEL_SELECT)
		{
			pBot->start_action = MSG_SI_IDLE;  // switch back to idle
			pBot->f_create_time = gpGlobals->time;  // reset
			if (pBot->bot_class == -1)
			{	// random model for now
				sprintf(c_model, "%i", 3);
			}
			else
				sprintf(c_model, "%i", pBot->bot_class);

			pBot->bot_class = atoi(c_model) - 1;

			FakeClientCommand(pEdict, "setmodel", c_model, NULL);

			pBot->not_started = 0;
			return;
		}
	}
	else
		pBot->not_started = 0;
>>>>>>> 3f195a42c7b84bf4071b789634ebbe14b6297ebc
>>>>>>> 594320fd6db9c6f9b3b7d743a0270568ddbba6bd
}