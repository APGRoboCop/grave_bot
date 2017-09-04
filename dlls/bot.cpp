//
// HPB bot - botman's High Ping Bastard bot
//
// (http://planethalflife.com/botman/)
//
// bot.cpp
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
#include "waypoint.h"
#include "bot_weapons.h"

#include <ctype.h>
//#include <sys/stat.h>


extern int mod_id;
extern WAYPOINT waypoints[MAX_WAYPOINTS];
extern int num_waypoints;  // number of waypoints currently in use
extern int default_bot_skill;
extern edict_t *pent_info_ctfdetect;
extern bool checked_teamplay;
extern int max_team_players[4];
extern int team_class_limits[4];
extern int max_teams;
extern bot_research_t g_Researched[2][NUM_RESEARCH_OPTIONS];
extern float is_team_play;
extern edict_t *clients[32];
static FILE *fp;

double PI = 3.1415926535897932384626433832795;

#define PLAYER_SEARCH_RADIUS     40.0


bot_t bots[32];   // max of 32 bots in a game
bool b_observer_mode = FALSE;
bool b_chat_debug = FALSE;
bool b_botdontshoot = FALSE;
extern bool b_random_color;
extern bot_weapon_t weapon_defs[MAX_WEAPONS];
extern bot_weapon_select_t valve_weapon_select[];
extern edict_t *listenserver_edict;
extern void RoleCount( void );
extern void RoleDetermine( void );
extern char *RoleToString(int role);
extern char *SubroleToString(int subrole);

int number_names = 0;

#define MAX_BOT_NAMES 100

#define VALVE_MAX_SKINS    10
#define GEARBOX_MAX_SKINS  20

// indicate which models are currently used for random model allocation
bool valve_skin_used[VALVE_MAX_SKINS] = {
	FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE};
		
// store the names of the models...
char *valve_bot_skins[VALVE_MAX_SKINS] = {
	"barney", "gina", "gman", "gordon", "helmet",
	"hgrunt", "recon", "robo", "scientist", "zombie"};
				
// store the player names for each of the models...
char *valve_bot_names[VALVE_MAX_SKINS] = {
	"Barney", "Gina", "G-Man", "Gordon", "Helmet",
	"H-Grunt", "Recon", "Robo", "Scientist", "Zombie"};
						
char bot_names[MAX_BOT_NAMES][BOT_NAME_LEN+1];
						
// how often (out of 1000 times) the bot will pause, based on bot skill
float pause_frequency[5] = {4, 7, 10, 15, 20};
						
float pause_time[5][2] = {
{0.2, 0.5}, {0.5, 1.0}, {0.7, 1.3}, {1.0, 1.7}, {1.2, 2.0}};

int yaw_speed[5] = {786,640,384,256,192};
int pitch_speed[5] = {832,786,640,384,256};
float speed_mod[5] = {1.0, 1.0, 1.0, 1.0, 1.0};

float sound_sense[5] = {768, 512, 384, 256, 128};

// percentage to defend { team1, team2 }
float g_flDefend[2] = { 0.5, 0.5 };
int g_iDefendGoal[2] = { 0, 0 };
// percentage to attack { team1, team2 }
float g_flAttack[2] = { 0.5, 0.5 };
int g_iAttackGoal[2] = { 0, 0 };
// actual defending amount
int g_iDefendCount[2] = { 0, 0 };
// actual attack amount
int g_iAttackCount[2] = { 0, 0 };
// Science and Industry, current team scores
long g_lTeamScore[2] = {0, 0};
// Science and Industry, current sci count
int g_iSciCount[2] = {0, 0};

/*							
// TheFatal's method for calculating the msecval
extern int msecnum;
extern float msecdel;
extern float msecval;
*/
#define CREATE_FAKE_CLIENT (*g_engfuncs.pfnCreateFakeClient)



#ifndef METAMOD_BUILD
   #ifndef __linux__
   extern HINSTANCE h_Library;
   #else
   extern void *h_Library;
   #endif

   // this is the LINK_ENTITY_TO_CLASS function that creates a player (bot)
   void player( entvars_t *pev )
   {
	   static LINK_ENTITY_FUNC otherClassName = NULL;
	   if (otherClassName == NULL)
		   otherClassName = (LINK_ENTITY_FUNC)GetProcAddress(h_Library, "player");
	   if (otherClassName != NULL)
	   {
		   (*otherClassName)(pev);
	   }
   }
#endif


void BotSpawnInit( bot_t *pBot )
{
	int i = 0;
//	ALERT(at_console, "BotSpawnInit\n");
	pBot->f_max_speed = CVAR_GET_FLOAT("sv_maxspeed");/* * speed_mod[pBot->bot_skill];*/
	pBot->v_curr_direction = g_vecZero;
	pBot->v_prev_origin = Vector(9999.0, 9999.0, 9999.0);
	pBot->prev_time = gpGlobals->time;
	
	pBot->pBotPickupItem = NULL;

	strcpy(pBot->debugchat, "\0");

	pBot->waypoint_origin = Vector(0, 0, 0);
	pBot->f_ignore_wpt_time = 0.0;
	pBot->f_waypoint_time = 0.0;
	pBot->curr_waypoint_index = -1;
	pBot->item_waypoint = -1;
	for (i = 0; i < 5; i++)
		pBot->prev_waypoint_index[i] = -1;
	
	pBot->f_random_waypoint_time = gpGlobals->time;
	pBot->waypoint_goal = -1;
	pBot->f_waypoint_goal_time = gpGlobals->time + 1.0;
	pBot->waypoint_near_flag = FALSE;
	pBot->waypoint_flag_origin = Vector(0, 0, 0);
	pBot->prev_waypoint_distance = 0.0;
	pBot->v_goal = g_vecZero;
	pBot->f_goal_proximity = 0.0;
	// how often we randomly turn
	pBot->f_random_turn_time = 0.0;

	pBot->f_check_drop_time = 0.0;
	pBot->f_avoid_drop_time = 0.0;

	for (i = 0; i < 6; i++)
		pBot->exclude_points[i] = -1;

	pBot->wpt_goal_type = WPT_GOAL_NONE;
	pBot->f_evaluate_goal_time = 0.0;

	pBot->b_engaging_enemy = FALSE;
	pBot->f_engage_enemy_check = 0.0;

	pBot->blinded_time = 0.0;
	
	pBot->prev_speed = 0.0;  // fake "paused" since bot is NOT stuck
	
	pBot->f_find_item = 0.0;
	
	pBot->ladder_dir = LADDER_UNKNOWN;
	pBot->f_start_use_ladder_time = 0.0;
	pBot->f_end_use_ladder_time = 0.0;
	
	pBot->f_wall_check_time = 0.0;
	pBot->f_wall_on_right = 0.0;
	pBot->f_wall_on_left = 0.0;
	pBot->f_dont_avoid_wall_time = 0.0;
	pBot->f_look_for_waypoint_time = 0.0;
	pBot->f_jump_time = 0.0;
	pBot->f_delay_duck_time = 0.0;
	pBot->f_do_duck_time = 0.0;
	pBot->f_dont_check_stuck = 0.0;
	
	pBot->f_sound_sensitivity = sound_sense[pBot->bot_skill];

	// pick a wander direction (50% of the time to the left, 50% to the right)
	if (RANDOM_LONG(1, 100) <= 50)
		pBot->wander_dir = WANDER_LEFT;
	else
		pBot->wander_dir = WANDER_RIGHT;
	
	pBot->f_exit_water_time = 0.0;
	
	pBot->dmg_origin = g_vecZero;
	pBot->f_dmg_time = 0.0;

	pBot->pAvoid = NULL;
	pBot->f_avoid_time = 0.0;
	pBot->f_do_avoid_time = 0.0;
	pBot->avoid_dir = g_vecZero;

	pBot->b_last_engage = FALSE;
	pBot->pBotEnemy = NULL;
	pBot->f_bot_see_enemy_time = gpGlobals->time;
	pBot->f_bot_find_enemy_time = gpGlobals->time;
	pBot->f_aim_tracking_time = 0.0;
	pBot->f_aim_x_angle_delta = 0.0;
	pBot->f_aim_y_angle_delta = 0.0;
	pBot->f_assess_grenade_time = 0.0;
	pBot->f_reload_time = 0.0;
	pBot->f_combat_longjump = 0.0;
	pBot->f_longjump_time = 0.0;
	pBot->b_combat_longjump = FALSE;
	pBot->b_strafe_direction = RANDOM_LONG(0,1) ? true : false;
	pBot->f_strafe_chng_dir = 0.0;

	pBot->f_mindray_regen_time = 0.0;

	pBot->pBotUser = NULL;
	pBot->f_bot_use_time = 0.0;
	pBot->f_sniper_aim_time = 0.0;
	
	pBot->f_switch_weapon_time = gpGlobals->time;
	pBot->f_shoot_time = gpGlobals->time;
	pBot->f_primary_charging = -1.0;
	pBot->f_secondary_charging = -1.0;
	pBot->charging_weapon_id = 0;
	
	pBot->f_pause_time = 0.0;
	pBot->f_sound_update_time = 0.0;
	pBot->bot_has_flag = FALSE;
	
//	pBot->b_see_tripmine = FALSE;
//	pBot->b_shoot_tripmine = FALSE;
//	pBot->v_tripmine = Vector(0,0,0);
	
	pBot->b_use_health_station = FALSE;
	pBot->f_use_health_time = 0.0;
	pBot->b_use_HEV_station = FALSE;
	pBot->f_use_HEV_time = 0.0;
	
	pBot->b_use_button = FALSE;
	pBot->f_use_button_time = 0;
	pBot->b_lift_moving = FALSE;
	
	pBot->f_reaction_target_time = 0.0;

	pBot->b_longjump = FALSE;

	if (mod_id == SI_DLL)
	{	// get longjump
		if (g_Researched[UTIL_GetTeam(pBot->pEdict)][RESEARCH_LEGS_2].researched ||
		g_Researched[UTIL_GetTeam(pBot->pEdict)][RESEARCH_LEGS_2].stolen)
			pBot->b_longjump = TRUE;

		// get our max armor
		if ((g_Researched[pBot->bot_team][RESEARCH_ARMOR_100].researched ||
			g_Researched[pBot->bot_team][RESEARCH_ARMOR_100].stolen) &&
			!g_Researched[pBot->bot_team][RESEARCH_ARMOR_100].disabled &&
			pBot->max_armor < 100)
			pBot->max_armor = 100;
		else if ((g_Researched[pBot->bot_team][RESEARCH_ARMOR_75].researched ||
			g_Researched[pBot->bot_team][RESEARCH_ARMOR_75].stolen) &&
			!g_Researched[pBot->bot_team][RESEARCH_ARMOR_75].disabled &&
			pBot->max_armor < 75)
			pBot->max_armor = 75;
		else if ((g_Researched[pBot->bot_team][RESEARCH_ARMOR_50].researched ||
			g_Researched[pBot->bot_team][RESEARCH_ARMOR_50].stolen) &&
			!g_Researched[pBot->bot_team][RESEARCH_ARMOR_50].disabled &&
			pBot->max_armor < 50)
			pBot->max_armor = 50;
		else if ((g_Researched[pBot->bot_team][RESEARCH_ARMOR_25].researched ||
			g_Researched[pBot->bot_team][RESEARCH_ARMOR_25].stolen) &&
			!g_Researched[pBot->bot_team][RESEARCH_ARMOR_25].disabled &&
			pBot->max_armor < 25)
			pBot->max_armor = 25;

		// get our max health
		if ((g_Researched[pBot->bot_team][RESEARCH_STRENGTH2].researched ||
			g_Researched[pBot->bot_team][RESEARCH_STRENGTH2].stolen) &&
			!g_Researched[pBot->bot_team][RESEARCH_STRENGTH2].disabled &&
			pBot->max_health < 150)
			pBot->max_health = 150;
		else if ((g_Researched[pBot->bot_team][RESEARCH_STRENGTH].researched ||
			g_Researched[pBot->bot_team][RESEARCH_STRENGTH].stolen) &&
			!g_Researched[pBot->bot_team][RESEARCH_STRENGTH].disabled &&
			pBot->max_health < 125)
			pBot->max_health = 125;
	}

	memset(&(pBot->current_weapon), 0, sizeof(pBot->current_weapon));
	memset(&(pBot->m_rgAmmo), 0, sizeof(pBot->m_rgAmmo));

	pBot->pGoalEnt = NULL;
	pBot->v_defend = g_vecZero;
	pBot->b_defend_patrol = false;
	pBot->defend_wpt = -1;

	// haven't gathered our equipment
	pBot->f_equip_time = gpGlobals->time;
	pBot->b_equipped = false;

	// Science and Industry specific
	pBot->i_carry_type = CARRY_NONE;
	strcpy(pBot->c_carry_name, "");

	pBot->f_role_check = 0.0;

	/*
	if (!pBot->b_role_locked)
	{
		pBot->role = ROLE_NONE;
		pBot->subrole = ROLE_SUB_NONE;
	}
	*/

	if (!pBot->not_started)
		BotCheckRole(pBot);
}


void BotNameInit( void )
{
//	ALERT(at_console, "BotNameInit\n");
	FILE *bot_name_fp;
	char bot_name_filename[256];
	int str_index;
	char name_buffer[80];
	int length, index;
	
	UTIL_BuildFileName(bot_name_filename, "grave_bot_names.txt", NULL);
	
	bot_name_fp = fopen(bot_name_filename, "r");
	
	if (bot_name_fp != NULL)
	{
		while ((number_names < MAX_BOT_NAMES) &&
			(fgets(name_buffer, 80, bot_name_fp) != NULL))
		{
			length = strlen(name_buffer);
			
			if (name_buffer[length-1] == '\n')
			{
				name_buffer[length-1] = 0;  // remove '\n'
				length--;
			}
			
			str_index = 0;
			while (str_index < length)
			{
				if ((name_buffer[str_index] < ' ') || (name_buffer[str_index] > '~') ||
					(name_buffer[str_index] == '"'))
					for (index=str_index; index < length; index++)
						name_buffer[index] = name_buffer[index+1];
					
					str_index++;
			}
			
			if (name_buffer[0] != 0)
			{
				strncpy(bot_names[number_names], name_buffer, BOT_NAME_LEN);
				
				number_names++;
			}
		}
		
		fclose(bot_name_fp);
	}
	else
	{
		SERVER_PRINT( "Grave Bot - Couldn't find grave_bot_names.txt!\n");
	}
}


void BotPickName( char *name_buffer )
{
//	ALERT(at_console, "BotPickName\n");
	int name_index, index;
	bool used;
	edict_t *pPlayer;
	int attempts = 0;
	
	// see if a name exists from a kicked bot (if so, reuse it)
	for (index=0; index < 32; index++)
	{
		if ((bots[index].is_used == FALSE) && (bots[index].name[0]))
		{
			strcpy(name_buffer, bots[index].name);
			
			return;
		}   
	}
	
	name_index = RANDOM_LONG(1, number_names) - 1;  // zero based
	
	// check make sure this name isn't used
	used = TRUE;
	
	while (used)
	{
		used = FALSE;
		
		for (index = 1; index <= gpGlobals->maxClients; index++)
		{
			pPlayer = INDEXENT(index);
			
			if (pPlayer && !pPlayer->free && pPlayer->v.flags & FL_CLIENT)
			{
				if (strcmp(bot_names[name_index], STRING(pPlayer->v.netname)) == 0)
				{
					used = TRUE;
					break;
				}
			}
		}
		
		if (used)
		{
			name_index++;
			
			if (name_index == number_names)
				name_index = 0;
			
			attempts++;
			
			if (attempts == number_names)
				used = FALSE;  // break out of loop even if already used
		}
	}
	
	strcpy(name_buffer, bot_names[name_index]);
}


void BotCreate( edict_t *pPlayer, const char *arg1, const char *arg2,
	const char *arg3, const char *arg4, const char *arg5)
{
//	ALERT(at_console, "BotCreate\n");
	edict_t *BotEnt;
	bot_t *pBot;
	char c_skin[BOT_SKIN_LEN+1];
	char c_name[BOT_NAME_LEN+1];
	int top_color, bottom_color;
	char c_topcolor[4], c_bottomcolor[4];
	int skill;
	int index;
	int i, j, length;
	bool found = FALSE;
	
	top_color = -1;
	bottom_color = -1;
   
	int  max_skin_index;
	
	max_skin_index = VALVE_MAX_SKINS;
	
	strcpy(c_skin, "");
	strcpy(c_name, "");

	/*SERVER_PRINT( "Calling BotCreate: %s %s %s %s %s\n", arg1 ? arg1 : "NULL", arg2 ? arg2 : "NULL",
		arg3 ? arg3 : "NULL", arg4 ? arg4 : "NULL", arg5 ? arg5 : "NULL");*/

	if (mod_id == CRABBED_DLL || mod_id == VALVE_DLL)
	{	
		if ((arg1 == NULL) || (*arg1 == 0) || (strcmp(arg1, "NULL") == 0))
		{
			bool *pSkinUsed;
			
			// pick a random skin
			index = RANDOM_LONG(0, VALVE_MAX_SKINS-1);
			pSkinUsed = &valve_skin_used[0];
			
			// check if this skin has already been used...
			while (pSkinUsed[index] == TRUE)
			{
				index++;
				
				if (index == max_skin_index)
					index = 0;
			}
			
			pSkinUsed[index] = TRUE;
			
			// check if all skins are now used...
			for (i = 0; i < max_skin_index; i++)
			{
				if (pSkinUsed[i] == FALSE)
					break;
			}
			
			// if all skins are used, reset used to FALSE for next selection
			if (i == max_skin_index)
			{
				for (i = 0; i < max_skin_index; i++)
					pSkinUsed[i] = FALSE;
			}
			strcpy( c_skin, valve_bot_skins[index] );
		}
		else
		{
			strncpy( c_skin, arg1, BOT_SKIN_LEN-1 );
			c_skin[BOT_SKIN_LEN] = 0;  // make sure c_skin is null terminated
		}
		
		for (i = 0; c_skin[i] != 0; i++)
			c_skin[i] = tolower( c_skin[i] );  // convert to all lowercase
		
		index = 0;
		
		while ((!found) && (index < max_skin_index))
		{
			if (strcmp(c_skin, valve_bot_skins[index]) == 0)
				found = TRUE;
			else
				index++;
		}
		
		if (found == TRUE)
		{
			if ((arg2 != NULL) && (*arg2 != 0) && (strcmp(arg2, "NULL") != 0))
			{
				strncpy( c_name, arg2, BOT_SKIN_LEN-1 );
				c_name[BOT_SKIN_LEN] = 0;  // make sure c_name is null terminated
			}
			else
			{
				if (number_names > 0)
					BotPickName( c_name );
				else
					strcpy( c_name, valve_bot_names[index] );
			}
		}
		else
		{
			char dir_name[32];
			char filename[128];
			
			struct stat stat_str;
			
			GET_GAME_DIR(dir_name);
			
			sprintf(filename, "%s/models/player/%s", dir_name, c_skin);
			
			if (stat(filename, &stat_str) != 0)
			{
				sprintf(filename, "valve/models/player/%s", c_skin);
				
				if (stat(filename, &stat_str) != 0)
				{
					char err_msg[80];
					
					sprintf( err_msg, "model \"%s\" is unknown.\n", c_skin );
					SERVER_PRINT( err_msg );
					
					SERVER_PRINT("use barney, gina, gman, gordon, helmet, hgrunt,\n");
					SERVER_PRINT("    recon, robo, scientist, or zombie\n");
					return;
				}
			}
			
			if ((arg2 != NULL) && (*arg2 != 0) && (strcmp(arg2, "NULL") != 0))
			{
				strncpy( c_name, arg2, BOT_NAME_LEN-1 );
				c_name[BOT_NAME_LEN] = 0;  // make sure c_name is null terminated
			}
			else
			{
				if (number_names > 0)
					BotPickName( c_name );
				else
				{
					// copy the name of the model to the bot's name...
					strncpy( c_name, arg1, BOT_NAME_LEN-1 );
					c_name[BOT_NAME_LEN] = 0;  // make sure c_name is null terminated
				}
			}
		}
			
		skill = 0;
		
		if ((arg3 != NULL) && (*arg3 != 0))
			skill = atoi(arg3);
			
		if ((skill < 1) || (skill > 5))
			skill = default_bot_skill;

		if ((arg4 != NULL) && (*arg4 != 0))
			top_color = atoi(arg4);
			
		if ((top_color < 0) || (top_color > 255))
			top_color = -1;
		else
			sprintf(c_topcolor, "%d", top_color);
			
		if ((arg5 != NULL) && (*arg5 != 0))
			bottom_color = atoi(arg5);
			
		if ((bottom_color < 0) || (bottom_color > 255))
			bottom_color = -1;
		else
			sprintf(c_bottomcolor, "%d", bottom_color);
			
		if ((top_color == -1) && (bottom_color == -1) && (b_random_color))
		{
			top_color = RANDOM_LONG(0, 255);
			sprintf(c_topcolor, "%d", top_color);
				
			bottom_color = RANDOM_LONG(0, 255);
			sprintf(c_bottomcolor, "%d", bottom_color);
		}
	}
	else if (mod_id == SI_DLL)
	{
		if ((arg1 != NULL) && (*arg1 != 0) && (strcmp(arg1, "NULL") != 0))
		{
			strncpy( c_name, arg1, BOT_NAME_LEN-1 );
			c_name[BOT_NAME_LEN] = 0;  // make sure c_name is null terminated
		}
		else
		{
			if (number_names > 0)
				BotPickName( c_name );
			else
				strcpy(c_name, "Grave-Bot");
		}
		
		skill = 0;
		
		if ((arg2 != NULL) && (*arg2 != 0))
			skill = atoi(arg2);
		
		if ((skill < 1) || (skill > 5))
			skill = default_bot_skill;
	}
	
	length = strlen(c_name);
	
	// remove any illegal characters from name...
	for (i = 0; i < length; i++)
	{
		if ((c_name[i] <= ' ') || (c_name[i] > '~') ||
			(c_name[i] == '"'))
		{
			for (j = i; j < length; j++)  // shuffle chars left (and null)
				c_name[j] = c_name[j+1];
			length--;
		}			   
	}

	BotEnt = CREATE_FAKE_CLIENT( c_name );

	if (FNullEnt( BotEnt ))
	{
		SERVER_PRINT( "Max. Players reached.  Can't create bot!\n");
	}
	else
	{
		char ptr[128];  // allocate space for message from ClientConnect
		char *infobuffer;
		int clientIndex;
		int index;
		
		SERVER_PRINT( "Creating Grave Bot bot...\n");
		
		index = 0;
		while ((bots[index].is_used) && (index < 32))
			index++;
		
		if (index == 32)
		{
			SERVER_PRINT( "Can't create bot!\n");
			return;
		}

		pBot = &bots[index];
		pBot->pEdict = BotEnt;

		// create the player entity by calling MOD's player function
		// (from LINK_ENTITY_TO_CLASS for player object)
      #ifndef METAMOD_BUILD
		   player( VARS(BotEnt) );
      #else
         CALL_GAME_ENTITY (PLID, "player", VARS(BotEnt));
      #endif

		infobuffer = GET_INFOKEYBUFFER( BotEnt );
		clientIndex = ENTINDEX( BotEnt );
		
		if (mod_id == CRABBED_DLL || mod_id == VALVE_DLL)
		{
			SET_CLIENT_KEYVALUE( clientIndex, infobuffer, "model", c_skin );
		
			if (top_color != -1)
				SET_CLIENT_KEYVALUE( clientIndex, infobuffer, "topcolor", c_topcolor );
			
			if (bottom_color != -1)
				SET_CLIENT_KEYVALUE( clientIndex, infobuffer, "bottomcolor", c_bottomcolor );
		}

      #ifndef METAMOD_BUILD
		ClientConnect( BotEnt, c_name, "127.0.0.1", ptr );
      #else
		MDLL_ClientConnect( BotEnt, c_name, "127.0.0.1", ptr );
      #endif

		// Pieter van Dijk - use instead of DispatchSpawn() - Hip Hip Hurray!
      #ifndef METAMOD_BUILD
		ClientPutInServer( BotEnt );
      #else
		// HPB_bot metamod fix - START

		// we have to do the ClientPutInServer() hook's job ourselves since calling the MDLL_
		// function calls directly the gamedll one, and not ours. You are allowed to call this
		// an "awful hack".

		while ((i < 32) && (clients[i] != NULL))
			i++;

		if (i < 32)
			clients[i] = BotEnt;  // store this clients edict in the clients array

		// HPB_bot metamod fix - END

		MDLL_ClientPutInServer( BotEnt );
      #endif


		BotEnt->v.flags |= FL_FAKECLIENT;

		// initialize all the variables for this bot...
		pBot->index = index;
		pBot->is_used = TRUE;
		pBot->respawn_state = RESPAWN_IDLE;
		pBot->f_create_time = gpGlobals->time;
		pBot->name[0] = 0;  // name not set by server yet
			
		strcpy(pBot->skin, c_skin);
		
		pBot->top_color = top_color;
		pBot->bottom_color = bottom_color;

		if (!checked_teamplay)  // check for team play...
			BotCheckTeamplay();

		pBot->not_started = 1;  // hasn't joined game yet
		
		if (mod_id == SI_DLL)
			pBot->start_action = MSG_SI_IDLE;
		else
			pBot->start_action = 0;// not needed for non-team MODs
		
		pBot->need_to_initialize = FALSE;  // don't need to initialize yet
		
		BotEnt->v.idealpitch = BotEnt->v.v_angle.x;
		BotEnt->v.ideal_yaw = BotEnt->v.v_angle.y;
		BotEnt->v.pitch_speed = pitch_speed[skill - 1];  // depends on skill
		BotEnt->v.yaw_speed = yaw_speed[skill - 1]; // depend on skill

		pBot->idle_angle = 0.0;
		pBot->idle_angle_time = 0.0;
		pBot->round_end = 0;
		
		pBot->bot_skill = skill - 1;  // 0 based for array indexes
		
		pBot->bot_team = -1;
		pBot->bot_class = -1;

		// TODO: give more control over this!
		pBot->i_engage_aggressiveness = RANDOM_LONG(50,100);
		// HLDM has VALVE_MAX_NORMAL BATTERY as default, health is 100
		if (mod_id == CRABBED_DLL || mod_id == VALVE_DLL)
		{
			pBot->max_armor = VALVE_MAX_NORMAL_BATTERY;
			pBot->max_health = 100;
		}
		// S&I depends on research, but starts at 0, health is 100
		else if (mod_id == SI_DLL)
		{
			pBot->max_armor = 0;
			pBot->max_health = 100;
		}

		if (mod_id == SI_DLL)
		{	// let creator assign a team
			if ((arg3 != NULL) && (*arg3 != 0) && (strcmp(arg3, "NULL") != 0))
				pBot->bot_team = atoi(arg3);
			// let creator assign a class (model for S&I)
			if ((arg4 != NULL) && (*arg4 != 0) && (strcmp(arg4, "NULL") != 0))
				pBot->bot_class = atoi(arg4);
		}

		// initially we're not locked into our role
		// a client may command us to be though
		// only set this on bot create
		pBot->b_role_locked = false;

		BotSpawnInit(pBot);
	}
}


int BotInFieldOfView(bot_t *pBot, Vector dest)
{
	// find angles from source to destination...
	Vector entity_angles = UTIL_VecToAngles( dest );
	
	// make yaw angle 0 to 360 degrees if negative...
	if (entity_angles.y < 0)
		entity_angles.y += 360;
	
	// get bot's current view angle...
	float view_angle = pBot->pEdict->v.v_angle.y;
	
	// make view angle 0 to 360 degrees if negative...
	if (view_angle < 0)
		view_angle += 360;
	
	// return the absolute value of angle to destination entity
	// zero degrees means straight ahead,  45 degrees to the left or
	// 45 degrees to the right is the limit of the normal view angle
	
	// rsm - START angle bug fix
	int angle = abs((int)view_angle - (int)entity_angles.y);
	
	if (angle > 180)
		angle = 360 - angle;
	
	return angle;
	// rsm - END
}


bool BotEntityIsVisible( bot_t *pBot, Vector dest )
{
	TraceResult tr;
	
	// trace a line from bot's eyes to destination...
	UTIL_TraceLine( pBot->pEdict->v.origin + pBot->pEdict->v.view_ofs,
		dest, ignore_monsters,
		pBot->pEdict->v.pContainingEntity, &tr );
	
	// check if line of sight to object is not blocked (i.e. visible)
	if (tr.flFraction >= 1.0)
		return TRUE;
	else
		return FALSE;
}

bool BotEntityIsVisible( bot_t *pBot, edict_t *pEntity )
{
	if (!pEntity)
		return FALSE;

	TraceResult tr;
	
	// trace a line from bot's eyes to destination...
	UTIL_TraceLine( pBot->pEdict->v.origin + pBot->pEdict->v.view_ofs,
		UTIL_GetOrigin(pEntity), ignore_monsters,
		pBot->pEdict->v.pContainingEntity, &tr );
	
	// check if line of sight to object is not blocked (i.e. visible)
	if (tr.flFraction >= 1.0 || tr.pHit == pEntity)
		return TRUE;
	else
		return FALSE;
}

// uses tracehull with head_hull to check for clip brushes and such
bool BotEntityIsNotBlocked( bot_t *pBot, Vector dest )
{
	TraceResult tr;
	
	// trace a line from bot's eyes to destination...
	UTIL_TraceHull( pBot->pEdict->v.origin + pBot->pEdict->v.view_ofs,
		dest, ignore_monsters, head_hull,
		pBot->pEdict->v.pContainingEntity, &tr );
	
	// check if line of sight to object is not blocked (i.e. visible)
	if (tr.flFraction >= 1.0)
		return TRUE;
	else
		return FALSE;
}

void BotFindItem( bot_t *pBot )
{
//	ALERT(at_console, "BotFindItem\n");
	edict_t *pent = NULL;
	edict_t *pPickupEntity = NULL;
	Vector pickup_origin;
	Vector entity_origin;
	float radius = 500;
	bool can_pickup;
	float min_distance;
	char item_name[40];
	int team = -1;
	int entteam = -1;
	TraceResult tr;
	Vector vecStart;
	Vector vecEnd;
	int angle_to_entity;
	edict_t *pEdict = pBot->pEdict;
	
	// forget about our item if it's been three seconds
	// forget about item if it we picked it up
	if (pBot->f_last_item_found > 0 && pBot->f_last_item_found < (gpGlobals->time - 5.0))
	{
		if (b_chat_debug)
		{
			sprintf(pBot->debugchat, "I tried to get to %s for too long!\n",
				STRING(pBot->pBotPickupItem->v.classname));
			UTIL_HostSay(pBot->pEdict, 0, pBot->debugchat);
		}
		pBot->f_find_item = gpGlobals->time + 2.0;
		pBot->f_last_item_found = -1;
		pBot->pBotPickupItem = NULL;
		pBot->item_waypoint = -1;
	}

	if (pBot->pBotPickupItem && ((pBot->pBotPickupItem->v.effects & EF_NODRAW) ||
		!BotEntityIsVisible(pBot, pBot->pBotPickupItem)))
	{
		if (b_chat_debug)
		{
			sprintf(pBot->debugchat, "I can't see %s anymore.\n",
				STRING(pBot->pBotPickupItem->v.classname));
			UTIL_HostSay(pBot->pEdict, 0, pBot->debugchat);
		}
		pBot->f_last_item_found = -1;
		pBot->pBotPickupItem = NULL;
		pBot->item_waypoint = -1;
	}

	// halt the rest of the function
	if (pBot->f_find_item > gpGlobals->time || pBot->pBotPickupItem)
		return;

	bot_weapon_select_t *pSelect = NULL;
	pSelect = WeaponGetSelectPointer();

	if (pSelect == NULL)
		return;

	if (!checked_teamplay)  // check for team play...
		BotCheckTeamplay();
	
	// is team play enabled
	if (is_team_play)
		team = UTIL_GetTeam(pEdict);

	// use a smaller search radius when waypoints are available
	if ((num_waypoints > 0) && (pBot->curr_waypoint_index != -1))
		radius = 256.0;
	else
		radius = 512.0;
	
	min_distance = radius + 1.0;
	
	while ((pent = UTIL_FindEntityInSphere( pent, pEdict->v.origin, radius )) != NULL)
	{
		can_pickup = FALSE;  // assume can't use it until known otherwise
		entteam = UTIL_GetTeam(pent);
		strcpy(item_name, STRING(pent->v.classname));
		
		// see if this is a "func_" type of entity (func_button, etc.)...
		if (strncmp("func_", item_name, 5) == 0)
		{
			// BModels have 0,0,0 for origin so must use VecBModelOrigin...
			entity_origin = VecBModelOrigin(pent);
			
			vecStart = pEdict->v.origin + pEdict->v.view_ofs;
			vecEnd = entity_origin;
			
			angle_to_entity = BotInFieldOfView( pBot, vecEnd - vecStart );
			
			// check if entity is outside field of view (+/- 45 degrees)
			if (angle_to_entity > 45)
				continue;  // skip this item if bot can't "see" it
			
			// check if entity is a ladder (ladders are a special case)
			// DON'T search for ladders if there are waypoints in this level...
			if ((strcmp("func_ladder", item_name) == 0) && (num_waypoints == 0))
			{
				// force ladder origin to same z coordinate as bot since
				// the VecBModelOrigin is the center of the ladder.  For
				// LONG ladders, the center MAY be hundreds of units above
				// the bot.  Fake an origin at the same level as the bot...
				
				entity_origin.z = pEdict->v.origin.z;
				vecEnd = entity_origin;
				
				// trace a line from bot's eyes to func_ladder entity...
				UTIL_TraceLine( vecStart, vecEnd, dont_ignore_monsters,
					pEdict->v.pContainingEntity, &tr);
				
				// check if traced all the way up to the entity (didn't hit wall)
				if (tr.flFraction >= 1.0)
				{
					// find distance to item for later use...
					float distance = (vecEnd - vecStart).Length( );
					
					// use the ladder about 100% of the time, if haven't
					// used a ladder in at least 5 seconds...
					if ((RANDOM_LONG(1, 100) <= 100) &&
						((pBot->f_end_use_ladder_time + 5.0) < gpGlobals->time))
					{
						// if close to ladder...
						if (distance < 100)
						{
							// don't avoid walls for a while
							pBot->f_dont_avoid_wall_time = gpGlobals->time + 5.0;
						}
						
						can_pickup = TRUE;
					}
				}
			}
			else
			{
				// trace a line from bot's eyes to func_ entity...
				UTIL_TraceLine( vecStart, vecEnd, dont_ignore_monsters,
					pEdict->v.pContainingEntity, &tr);
				
				// check if traced all the way up to the entity (didn't hit wall)
				if (strcmp(item_name, STRING(tr.pHit->v.classname)) == 0)
				{
					// find distance to item for later use...
					float distance = (vecEnd - vecStart).Length( );
					
					// check if entity is wall mounted health charger...
					if (strcmp("func_healthcharger", item_name) == 0)
					{
						if (mod_id == SI_DLL && entteam != team && entteam != -1)
							continue;
						// check if the bot can use this item and
						// check if the recharger is ready to use (has power left)...
						if ((pEdict->v.health < pBot->max_health) && (pent->v.frame == 0))
						{
							// check if flag not set...
							if (!pBot->b_use_health_station)
							{
								// check if close enough and facing it directly...
								if ((distance < PLAYER_SEARCH_RADIUS) &&
									(angle_to_entity <= 10))
								{
									pBot->b_use_health_station = TRUE;
									pBot->f_use_health_time = gpGlobals->time;
								}
								
								// if close to health station...
								if (distance < 100)
								{
									// don't avoid walls for a while
									pBot->f_dont_avoid_wall_time = gpGlobals->time + 5.0;
								}
								
								can_pickup = TRUE;
							}
						}
						else
						{
							// don't need or can't use this item...
							pBot->b_use_health_station = FALSE;
						}
					}
					
					// check if entity is wall mounted HEV charger...
					else if (strcmp("func_recharge", item_name) == 0)
					{
						if (mod_id == SI_DLL && entteam != team && entteam != -1)
							continue;

						// check if the bot can use this item and
						// check if the recharger is ready to use (has power left)...
						if ((pEdict->v.armorvalue < pBot->max_armor) &&
							(pent->v.frame == 0))
						{
							// check if flag not set and facing it...
							if (!pBot->b_use_HEV_station)
							{
								// check if close enough and facing it directly...
								if ((distance < PLAYER_SEARCH_RADIUS) &&
									(angle_to_entity <= 10))
								{
									pBot->b_use_HEV_station = TRUE;
									pBot->f_use_HEV_time = gpGlobals->time;
								}
								
								// if close to HEV recharger...
								if (distance < 100)
								{
									// don't avoid walls for a while
									pBot->f_dont_avoid_wall_time = gpGlobals->time + 5.0;
								}
								
								can_pickup = TRUE;
							}
						}
						else
						{
							// don't need or can't use this item...
							pBot->b_use_HEV_station = FALSE;
						}
					}
					// check if entity is a button...
					else if (strcmp("func_button", item_name) == 0)
					{
						// use the button about 100% of the time, if haven't
						// used a button in at least 5 seconds...
						if (RANDOM_LONG(1, 100) <= 100 &&
							((pBot->f_use_button_time + 5) < gpGlobals->time) &&
							(mod_id == SI_DLL && (entteam == team || entteam == -1) ||
							mod_id == CRABBED_DLL || mod_id == VALVE_DLL))
						{
							// check if flag not set and facing it...
							if (!pBot->b_use_button)
							{
								// check if close enough and facing it directly...
								if ((distance < PLAYER_SEARCH_RADIUS) &&
									(angle_to_entity <= 10))
								{
									pBot->b_use_button = TRUE;
									pBot->b_lift_moving = FALSE;
									pBot->f_use_button_time = gpGlobals->time;
								}
								
								// if close to button...
								if (distance < 100)
								{
									// don't avoid walls for a while
									pBot->f_dont_avoid_wall_time = gpGlobals->time + 5.0;
								}
								
								can_pickup = TRUE;
							}
						}
						else
						{
							// don't need or can't use this item...
							pBot->b_use_button = FALSE;
						}
					}
				}
			}
		}
		else	// everything else...
		{
			entity_origin = pent->v.origin;
			
			vecStart = pEdict->v.origin + pEdict->v.view_ofs;
			vecEnd = entity_origin;
			
			// find angles from bot origin to entity...
			angle_to_entity = BotInFieldOfView( pBot, vecEnd - vecStart );
			
			// check if entity is outside field of view (+/- 45 degrees)
			if (angle_to_entity > 45)
				continue;  // skip this item if bot can't "see" it
			
			// check if line of sight to object is not blocked (i.e. visible)
			if (BotEntityIsVisible( pBot, vecEnd ))
			{
				// check if entity weapon or ammo...
				if ((strncmp("weapon_", item_name, 7) == 0) || (strncmp("ammo_", item_name, 5) == 0))
				{
					// check if the item is not visible (i.e. has not respawned)
					if (pent->v.effects & EF_NODRAW)
						continue;
					
					if (mod_id == SI_DLL && entteam != team && entteam != -1)
						continue;

					if (mod_id != SI_DLL)
					{
						int select_index = 0;
						int weapon_index = 0;
						int i = 0;
						float ammo = AMMO_MAX;
						while (pSelect[select_index].iId)
						{
							if (strcmp(pSelect[select_index].weapon_name, item_name) == 0)
							{	// check ammo for weapon if weapon stay is off
								if (CVAR_GET_FLOAT("mp_weaponstay") == 0)
									ammo = BotAssessPrimaryAmmo(pBot, pSelect[select_index].iId);
								weapon_index = select_index;
							}

							i = 0;
							// is this ammo for one of our weapon's primary ammo count
							while (pSelect[select_index].primary_ammo_names[i][0])
							{	// if so, get percent
								if (strcmp(pSelect[select_index].primary_ammo_names[i], item_name) == 0)
									ammo = BotAssessPrimaryAmmo(pBot, pSelect[select_index].iId);

								i++;
							}

							// for second loop
							i = 0;
							// is this ammo for one of our weapon's secondary ammo count
							while (pSelect[select_index].secondary_ammo_names[i][0])
							{	// if so, get percent
								if (strcmp(pSelect[select_index].secondary_ammo_names[i], item_name) == 0)
									ammo = BotAssessSecondaryAmmo(pBot, pSelect[select_index].iId);

								i++;
							}

							select_index++;
						}
						// only pick it up if we need ammo or don't have the weapon
						if ((ammo < AMMO_MAX) || !(pEdict->v.weapons & (1<<pSelect[weapon_index].iId)))
							can_pickup = TRUE;
					}
				}
				
				// check if entity is a battery...
				else if (strcmp("item_battery", item_name) == 0)
				{
					// check if the item is not visible (i.e. has not respawned)
					if (pent->v.effects & EF_NODRAW)
						continue;

					if (mod_id == SI_DLL && entteam != team && entteam != -1)
						continue;
					
					// check if the bot can use this item...
					if (pEdict->v.armorvalue < pBot->max_armor)
					{
						can_pickup = TRUE;
					}
				}
				
				// check if entity is a healthkit...
				else if (strcmp("item_healthkit", item_name) == 0)
				{
					// check if the item is not visible (i.e. has not respawned)
					if (pent->v.effects & EF_NODRAW)
						continue;
				
					if (mod_id == SI_DLL && entteam != team && entteam != -1)
						continue;
					
					// check if the bot can use this item...
					if (pEdict->v.health < pBot->max_health)
					{
						can_pickup = TRUE;
					}
				}
				
				// check if entity is a long jump...
				else if (strcmp("item_longjump", item_name) == 0)
				{
					// check if the item is not visible (i.e. has not respawned)
					if (pent->v.effects & EF_NODRAW)
						continue;
					
					if (mod_id == SI_DLL && entteam != team && entteam != -1)
						continue;

					// check if the bot can use this item...
					if (!pBot->b_longjump)
					{
						can_pickup = TRUE;
					}
				}
				
				// check if entity is a packed up weapons box...
				else if (strcmp("weaponbox", item_name) == 0)
				{
					can_pickup = TRUE;
				}
				
				// check if entity is the spot from RPG laser
				else if (strcmp("laser_spot", item_name) == 0)
				{
				}
				
			}	// end if object is visible
		}  // end else not "func_" entity
	  
		if (can_pickup) // if the bot found something it can pickup...
		{
			float distance = (entity_origin - pEdict->v.origin).Length( );
			
			// see if it's the closest item so far...
			if (distance < min_distance)
			{
				min_distance = distance;		  // update the minimum distance
				pPickupEntity = pent; 	   // remember this entity
				pickup_origin = entity_origin;  // remember location of entity
			}
		}
	}  // end while loop
	
	if (pPickupEntity != NULL && pPickupEntity != pBot->pBotPickupItem)
	{
		if (!pBot->pBotEnemy && pBot->f_dmg_time < gpGlobals->time && pBot->curr_waypoint_index == -1)
		{
			// let's head off toward that item...
			Vector v_item = pickup_origin - pEdict->v.origin;
			
			Vector bot_angles = UTIL_VecToAngles( v_item );
			
			pEdict->v.ideal_yaw = bot_angles.y;
			
			BotFixIdealYaw(pEdict);
		}
		int item_wpt = WaypointFindNearest(pPickupEntity, 48, team);
		if (item_wpt != pBot->curr_waypoint_index)
			pBot->item_waypoint = WaypointFindNearest(pPickupEntity, 48, team);
		if (b_chat_debug)
		{
			sprintf(pBot->debugchat, "I found a(n) %s at %i\n", STRING(pPickupEntity->v.classname),
				pBot->item_waypoint);
			UTIL_HostSay(pBot->pEdict, 0, pBot->debugchat);
		}
		pBot->pBotPickupItem = pPickupEntity;  // save the item bot is trying to get
		pBot->f_last_item_found = gpGlobals->time;
	}
}

// NOTE
// This function only decides if a bot should be attacking or defending
// It does NOT handle subroles, BotEvaluateGoalSI does that
void BotCheckRole( bot_t *pBot)
{	// must be Science and Industry to do role checks
//	ALERT(at_console, "BotCheckRole\n");
	if (mod_id != SI_DLL)
		return;

	int newrole = pBot->role;
	int newsub = pBot->subrole;

	edict_t *pEdict = pBot->pEdict;
	int team = UTIL_GetTeam(pEdict);

	if (team < 0 || team > 1)
		return;

	int teamtotal = UTIL_PlayersOnTeam(team);

	// am I the only one on the team?
	if (teamtotal == 1)
	{
		if (((g_flDefend[team] <= g_flAttack[team]) && (newrole != ROLE_ATTACK)) ||
			((pBot->i_carry_type) && (newrole != ROLE_ATTACK)))
		{	// attack
			newrole = ROLE_ATTACK;
		}
		else if ((g_flDefend[team] > g_flAttack[team]) && (newrole != ROLE_DEFEND) &&
			(!pBot->i_carry_type))
		{	// defend 
			newrole = ROLE_DEFEND;
		}
	}
	else if (teamtotal > 1)
	{
		if (((g_iAttackCount[team] < g_iAttackGoal[team]) && (newrole != ROLE_ATTACK)) ||
			((pBot->i_carry_type) && (newrole != ROLE_ATTACK)))
		{	// tell this bot to attack!
			newrole = ROLE_ATTACK;
		}
		else if ((g_iDefendCount[team] < g_iDefendGoal[team]) && (newrole != ROLE_DEFEND) &&
			(!pBot->i_carry_type))
		{	// tell this bot to defend!
			newrole = ROLE_DEFEND;
		}
		/*
		SERVER_PRINT( "%s(%i) has %i/%i attacker(s) and %i/%i defender(s)...\n",
			team ? "AFD" : "MCL", teamtotal, g_iAttackCount[team], g_iAttackGoal[team],
			g_iDefendCount[team], g_iDefendGoal[team]);
		*/
	}

	// change role
	if (newrole != pBot->role)
	{
		if (b_chat_debug)
		{
			sprintf(pBot->debugchat, "I changed role to %s...\n", RoleToString(newrole));
			UTIL_HostSay(pBot->pEdict, 0, pBot->debugchat);
		}
		pBot->role = newrole;
		pBot->subrole = ROLE_SUB_NONE;
		// forget about what we're defending
		pBot->pGoalEnt = NULL;
		pBot->v_defend = g_vecZero;
		pBot->defend_wpt = -1;
		// recalc count and determiner
		RoleCount();
		RoleDetermine();
		// reeval goal
		BotEvaluateGoal(pBot);
	}
}

void BotThink( bot_t *pBot )
{
//	ALERT(at_console, "BotThink\n");
	int index = 0;
	Vector v_diff;			   // vector from previous to current location
	float pitch_degrees;
	float yaw_degrees;
	float moved_distance;	   // length of v_diff vector (distance bot moved)
	TraceResult tr;
	bool found_waypoint;
	bool is_idle;
	bool bCrouchJump;

	edict_t *pEdict = pBot->pEdict;
	
	pEdict->v.flags |= FL_FAKECLIENT;
	
	if (pBot->name[0] == 0)  // name filled in yet?
		strcpy(pBot->name, STRING(pBot->pEdict->v.netname));
	// make sure our team is correct
	pBot->bot_team = UTIL_GetTeam(pEdict);
	// our max speed depends on our skill
//	pBot->f_max_speed = CVAR_GET_FLOAT("sv_maxspeed") * speed_mod[pBot->bot_skill];
	
	// TheFatal - START from Advanced Bot Framework (Thanks Rich!)
	
	// adjust the millisecond delay based on the frame rate interval...
	if (pBot->msecdel <= gpGlobals->time)
	{
		pBot->msecdel = gpGlobals->time + 0.5;
		if (pBot->msecnum > 0)
			pBot->msecval = 450.0/pBot->msecnum;
		pBot->msecnum = 0;
	}
	else
		pBot->msecnum++;
	
	if (pBot->msecval < 1)    // don't allow msec to be less than 1...
		pBot->msecval = 1;
	
	if (pBot->msecval > 100)  // ...or greater than 100
		pBot->msecval = 100;
	
	// TheFatal - END
	
	pBot->f_frame_time = pBot->msecval / 1000;  // calculate frame time

	pEdict->v.button = 0;
	pBot->f_move_speed = 0.0;
	pBot->f_strafe_speed = 0.0;

	// if the bot hasn't selected stuff to start the game yet, go do that...
	if (pBot->not_started)
	{
		BotStartGame( pBot );
		
		BotFixIdealPitch (pEdict);
		BotFixIdealYaw (pEdict);
		BotFixBodyAngles (pEdict);
		BotFixViewAngles (pEdict);

		g_engfuncs.pfnRunPlayerMove( pEdict, pEdict->v.v_angle,
			pBot->f_move_speed * speed_mod[pBot->bot_skill], pBot->f_strafe_speed * speed_mod[pBot->bot_skill], 0,
			pEdict->v.button, 0, pBot->msecval);
		
		return;
	}

	// if the bot is dead, randomly press fire to respawn...
	if ((pEdict->v.health < 1) || (pEdict->v.deadflag != DEAD_NO))
	{
		if (pBot->need_to_initialize)
		{
			BotSpawnInit(pBot);

			pBot->need_to_initialize = FALSE;
		}
		
		if ((RANDOM_LONG(1, 100) > 50) && (mod_id != SI_DLL))
			pEdict->v.button = IN_ATTACK;
		
		BotFixIdealPitch (pEdict);
		BotFixIdealYaw (pEdict);
		BotFixBodyAngles (pEdict);
		BotFixViewAngles (pEdict);
		
		g_engfuncs.pfnRunPlayerMove( pEdict, pEdict->v.v_angle,
			pBot->f_move_speed * speed_mod[pBot->bot_skill],
			pBot->f_strafe_speed * speed_mod[pBot->bot_skill], 0, pEdict->v.button, 0, pBot->msecval);
		
		return;
	}
	
	// set this for the next time the bot dies so it will initialize stuff
	if (pBot->need_to_initialize == FALSE)
	{
		pBot->need_to_initialize = TRUE;
		pBot->f_bot_spawn_time = gpGlobals->time;
	}

	is_idle = FALSE;
	
	if (pBot->blinded_time > gpGlobals->time)
	{
		is_idle = TRUE;  // don't do anything while blinded
	}
	
	if (is_idle)
	{
		if (pBot->idle_angle_time <= gpGlobals->time)
		{
			pBot->idle_angle_time = gpGlobals->time + RANDOM_FLOAT(0.5, 2.0);
			
			pEdict->v.ideal_yaw = pBot->idle_angle + RANDOM_FLOAT(0.0, 40.0) - 20.0;
			
			BotFixIdealYaw(pEdict);
		}
		
		// turn towards ideal_yaw by yaw_speed degrees (slower than normal)
		BotChangeYaw( pBot, pEdict->v.yaw_speed / 2 );
		
		BotFixIdealPitch (pEdict);
		BotFixIdealYaw (pEdict);
		BotFixBodyAngles (pEdict);
		BotFixViewAngles (pEdict);
		
		g_engfuncs.pfnRunPlayerMove( pEdict, pEdict->v.v_angle,
			pBot->f_move_speed * speed_mod[pBot->bot_skill],
			pBot->f_strafe_speed * speed_mod[pBot->bot_skill], 0, pEdict->v.button, 0, pBot->msecval);
		
		return;
	}
	else
	{
		pBot->idle_angle = pEdict->v.v_angle.y;
	}

	if (pBot->pBotEnemy == NULL && pBot->f_sound_update_time <= gpGlobals->time)
	{
		pBot->f_sound_update_time = gpGlobals->time + 0.5;
		BotListenForFakeSound(pBot);
	}

	pBot->f_move_speed = pBot->f_max_speed;  // set to max speed
	
	if (pBot->prev_time <= gpGlobals->time)
	{
		// see how far bot has moved since the previous position...
		v_diff = pBot->v_prev_origin - pEdict->v.origin;
		moved_distance = v_diff.Length();
		
		// save current position as previous
		pBot->v_prev_origin = pEdict->v.origin;
		pBot->prev_time = gpGlobals->time + 0.2;
	}
	else
	{
		moved_distance = 2.0;
	}

	// if the bot is not underwater AND not in the air (or on ladder),
	// check if the bot is about to fall off high ledge or into water...
	if (pEdict->v.waterlevel != 3 && pEdict->v.flags & FL_ONGROUND &&
		pEdict->v.movetype != MOVETYPE_FLY && pBot->f_check_drop_time < gpGlobals->time &&
		(pBot->curr_waypoint_index == -1 || (pBot->curr_waypoint_index > -1 &&
		!(waypoints[pBot->curr_waypoint_index].flags & W_FL_LADDER))))
	{
		pBot->f_check_drop_time = gpGlobals->time + 0.05;

		if (BotLookForDrop( pBot ))
			pBot->f_avoid_drop_time = gpGlobals->time + 1.0;
	}

	// turn towards ideal_pitch by pitch_speed degrees
	pitch_degrees = BotChangePitch( pBot, pEdict->v.pitch_speed );
	
	// turn towards ideal_yaw by yaw_speed degrees
	yaw_degrees = BotChangeYaw( pBot, pEdict->v.yaw_speed );

				
	found_waypoint = FALSE;
				
	// it is time to look for a waypoint AND
	// there are waypoints in this level...
				
	if ((pBot->f_look_for_waypoint_time <= gpGlobals->time) &&
		(num_waypoints != 0))
	{
		found_waypoint = BotHeadTowardWaypoint(pBot);
	}
/*	
	if ((pitch_degrees >= 45) || (yaw_degrees >= 45))
	{
		pBot->f_move_speed = 0.0;  // don't move while turning a lot
	}
	else if ((pitch_degrees >= 30) || (yaw_degrees >= 30))
	{
		pBot->f_move_speed = pBot->f_move_speed / 4;  // slow down while turning
	}
	else if ((pitch_degrees >= 20) || (yaw_degrees >= 20))
	{
		pBot->f_move_speed = pBot->f_move_speed / 3;  // slow down while turning
	}
	
	
	// if the bot is under water, adjust pitch by pitch_speed degrees
	if ((pEdict->v.waterlevel == 2) ||
		(pEdict->v.waterlevel == 3))
	{
		// turn towards ideal_pitch by pitch_speed degrees
		pitch_degrees = BotChangePitch( pBot, pEdict->v.pitch_speed );
	}
	else
		pitch_degrees = 0.0;
	
	// turn towards ideal_yaw by yaw_speed degrees
	yaw_degrees = BotChangeYaw( pBot, pEdict->v.yaw_speed );
	
	if ((pitch_degrees >= pEdict->v.pitch_speed) ||
		(yaw_degrees >= pEdict->v.yaw_speed))
	{
		pBot->f_move_speed = 0.0;  // don't move while turning a lot
	}
	else if ((pitch_degrees >= 10) || 
		(yaw_degrees >= 10))  // turning more than 10 degrees?
	{
		pBot->f_move_speed = pBot->f_move_speed / 4;  // slow down while turning
	}
	else  // else handle movement related actions...
*/	{
		if (b_botdontshoot == 0)
		{
			pBot->pBotEnemy = BotFindEnemy( pBot );
		}
		else
			pBot->pBotEnemy = NULL;  // clear enemy pointer (no ememy for you!)
		
		// does an enemy exist and we're not on a ladder
		if (pBot->pBotEnemy != NULL && pEdict->v.movetype != MOVETYPE_FLY &&
			pBot->f_bot_spawn_time + 1.0 < gpGlobals->time)
		{
			BotShootAtEnemy( pBot );  // shoot at the enemy
			
			pBot->f_pause_time = 0;  // dont't pause if enemy exists
		}

		else if (pBot->f_pause_time > gpGlobals->time)	// is bot "paused"?
		{
			// you could make the bot look left then right, or look up
			// and down, to make it appear that the bot is hunting for
			// something (don't do anything right now)
		}
		
		// is bot being "used" and can still follow "user"?
		else if ((pBot->pBotUser != NULL) && BotFollowUser( pBot ))
		{
			// do nothing here!
		}
		
		else
		{
			// no enemy, let's just wander around...
			if (pBot->b_engaging_enemy)
			{
//				SERVER_PRINT( "%s gave up engaging...\n", STRING(pEdict->v.netname));
				pBot->b_engaging_enemy = FALSE;
				// remember our old goal if we had one
				if (pBot->old_waypoint_goal != -1)
				{
					pBot->waypoint_goal = pBot->old_waypoint_goal;
					pBot->old_waypoint_goal = -1;
				}
			}
			
			if ((pEdict->v.waterlevel != 2) &&	// is bot NOT under water?
				(pEdict->v.waterlevel != 3) && pEdict->v.movetype != MOVETYPE_FLY)
			{
				// reset pitch to 0 (level horizontally)
				pEdict->v.idealpitch = 0;
				pEdict->v.v_angle.x = 0;
			}
			
			pEdict->v.v_angle.z = 0;  // reset roll to 0 (straight up and down)
			
			pEdict->v.angles.x = 0;
			pEdict->v.angles.y = pEdict->v.v_angle.y;
			pEdict->v.angles.z = 0;	

			if (pBot->pBotPickupItem)
			{
				if (FStrEq(STRING(pBot->pBotPickupItem->v.classname), "func_healthcharger"))
				{	// check if we can use the charger
					int angle_to_entity = BotInFieldOfView(pBot, UTIL_GetOrigin(pBot->pBotPickupItem));
					float distance = (UTIL_GetOrigin(pBot->pBotPickupItem) - UTIL_GetOrigin(pEdict)).Length();
					if (pEdict->v.health < pBot->max_health && distance <= 64 && angle_to_entity <= 20 &&
						pBot->pBotPickupItem->v.frame == 0)
					{
						pBot->f_pause_time = gpGlobals->time + 0.1;
						pEdict->v.button |= IN_USE;
					}

					Vector v_direction = UTIL_GetOrigin(pBot->pBotPickupItem) - UTIL_GetOrigin(pEdict);
					Vector v_angles = UTIL_VecToAngles(v_direction);

					// if the bot is NOT on a ladder, change the yaw...
					if (pEdict->v.movetype != MOVETYPE_FLY)
					{
						pEdict->v.ideal_yaw = v_angles.y;
						BotFixIdealYaw(pEdict);
					}
					// is this charger still active? Do we have full health?
					if (pBot->pBotPickupItem->v.frame != 0 || pEdict->v.health >= pBot->max_health)
						pBot->pBotPickupItem = NULL;
				}
				else if (FStrEq(STRING(pBot->pBotPickupItem->v.classname), "func_recharge"))
				{	// check if we can use the charger
					int angle_to_entity = BotInFieldOfView(pBot, UTIL_GetOrigin(pBot->pBotPickupItem));
					float distance = (UTIL_GetOrigin(pBot->pBotPickupItem) - UTIL_GetOrigin(pEdict)).Length();
					if (pEdict->v.armorvalue < pBot->max_armor && distance <= 64 && angle_to_entity <= 20 &&
						pBot->pBotPickupItem->v.frame == 0)
					{
						pBot->f_pause_time = gpGlobals->time + 0.1;
						pEdict->v.button |= IN_USE;
					}

					Vector v_direction = UTIL_GetOrigin(pBot->pBotPickupItem) - UTIL_GetOrigin(pEdict);
					Vector v_angles = UTIL_VecToAngles(v_direction);

					// if the bot is NOT on a ladder, change the yaw...
					if (pEdict->v.movetype != MOVETYPE_FLY)
					{
						pEdict->v.ideal_yaw = v_angles.y;
						BotFixIdealYaw(pEdict);
					}
					// is this charger still active? Do we have full armor?
					if (pBot->pBotPickupItem->v.frame != 0 || pEdict->v.armorvalue >= pBot->max_armor)
						pBot->pBotPickupItem = NULL;
				}
			}
			/*
			// check if should use wall mounted health station...
			if (pBot->b_use_health_station)
			{
				if ((pBot->f_use_health_time + 10.0) > gpGlobals->time)
				{
					pBot->f_move_speed = 0;  // don't move while using health station
					
					pEdict->v.button = IN_USE;
				}
				else
				{
					// bot is stuck trying to "use" a health station...
					
					pBot->b_use_health_station = FALSE;
					
					// don't look for items for a while since the bot
					// could be stuck trying to get to an item
					pBot->f_find_item = gpGlobals->time + 0.5;
				}
			}
			
			// check if should use wall mounted HEV station...
			else if (pBot->b_use_HEV_station)
			{
				if ((pBot->f_use_HEV_time + 10.0) > gpGlobals->time)
				{
					pBot->f_move_speed = 0;  // don't move while using HEV station
					
					pEdict->v.button = IN_USE;
				}
				else
				{
					// bot is stuck trying to "use" a HEV station...
					
					pBot->b_use_HEV_station = FALSE;
					
					// don't look for items for a while since the bot
					// could be stuck trying to get to an item
					pBot->f_find_item = gpGlobals->time + 0.5;
				}
			}
			*/
			
			else if (pBot->b_use_button)
			{
				pBot->f_move_speed = 0;  // don't move while using elevator
				
				BotUseLift( pBot, moved_distance );
			}
			
			else
			{
				if (pEdict->v.waterlevel == 3)	// check if the bot is underwater...
				{
					BotUnderWater( pBot );
				}
				
				// TODO:
				// Will probably redo this ladder handling code, it's not that great
				// check if the bot is on a ladder...
				if (pEdict->v.movetype == MOVETYPE_FLY)
				{
					// check if bot JUST got on the ladder...
					if ((pBot->f_end_use_ladder_time + 1.0) < gpGlobals->time)
						pBot->f_start_use_ladder_time = gpGlobals->time;
					
					// go handle the ladder movement
					BotOnLadder( pBot, moved_distance );
					
					pBot->f_dont_avoid_wall_time = gpGlobals->time + 2.0;
					pBot->f_end_use_ladder_time = gpGlobals->time;
				}
				else
				{
					// check if the bot JUST got off the ladder...
					if ((pBot->f_end_use_ladder_time + 1.0) > gpGlobals->time)
					{
						pBot->ladder_dir = LADDER_UNKNOWN;
					}
				}
				
				// if the bot isn't headed toward a waypoint...
				if (found_waypoint == FALSE)
				{
					TraceResult tr;
					
					// check if we should be avoiding walls
					if (pBot->f_dont_avoid_wall_time <= gpGlobals->time)
					{
						// let's just randomly wander around
						if (BotStuckInCorner( pBot ))
						{
							pEdict->v.ideal_yaw += 180;  // turn 180 degrees
							
							BotFixIdealYaw(pEdict);
							
							pBot->f_move_speed = 0;  // don't move while turning
							pBot->f_dont_avoid_wall_time = gpGlobals->time + 1.0;
							
							moved_distance = 2.0;  // dont use bot stuck code
						}
						else
						{
							// check if there is a wall on the left...
							if (!BotCheckWallOnLeft( pBot ))
							{
								// if there was a wall on the left over 1/2 a second ago then
								// 20% of the time randomly turn between 45 and 60 degrees
								
								if ((pBot->f_wall_on_left != 0) &&
									(pBot->f_wall_on_left <= gpGlobals->time - 0.5) &&
									(RANDOM_LONG(1, 100) <= 20))
								{
									pEdict->v.ideal_yaw += RANDOM_LONG(45, 60);
									
									BotFixIdealYaw(pEdict);
									
									pBot->f_move_speed = 0;  // don't move while turning
									pBot->f_dont_avoid_wall_time = gpGlobals->time + 1.0;
								}
								
								pBot->f_wall_on_left = 0;  // reset wall detect time
							}
							else if (!BotCheckWallOnRight( pBot ))
							{
								// if there was a wall on the right over 1/2 a second ago then
								// 20% of the time randomly turn between 45 and 60 degrees
								
								if ((pBot->f_wall_on_right != 0) &&
									(pBot->f_wall_on_right <= gpGlobals->time - 0.5) &&
									(RANDOM_LONG(1, 100) <= 20))
								{
									pEdict->v.ideal_yaw -= RANDOM_LONG(45, 60);
									
									BotFixIdealYaw(pEdict);
									
									pBot->f_move_speed = 0;  // don't move while turning
									pBot->f_dont_avoid_wall_time = gpGlobals->time + 1.0;
								}
								
								pBot->f_wall_on_right = 0;	// reset wall detect time
							}
						}
					}
					
					// check if bot is about to hit a wall.  TraceResult gets returned
					if ((pBot->f_dont_avoid_wall_time <= gpGlobals->time) &&
						BotCantMoveForward( pBot, &tr ))
					{
						// ADD LATER
						// need to check if bot can jump up or duck under here...
						// ADD LATER
						
						BotTurnAtWall( pBot, &tr );
					}
				}
				
				// check if bot is on a ladder and has been on a ladder for
				// more than 5 seconds...
				if ((pEdict->v.movetype == MOVETYPE_FLY) &&
					(pBot->f_start_use_ladder_time > 0.0) &&
					((pBot->f_start_use_ladder_time + 5.0) <= gpGlobals->time))
				{
					// bot is stuck on a ladder...
					
					BotRandomTurn(pBot);
					
					// don't look for items for 2 seconds
					pBot->f_find_item = gpGlobals->time + 2.0;
					
					pBot->f_start_use_ladder_time = 0.0;  // reset start ladder time
				}
				
				// check if the bot hasn't moved much since the last location
				// (and NOT on a ladder since ladder stuck handled elsewhere)
				// (don't check for stuck if f_dont_check_stuck in the future)
				if ((moved_distance <= 1.0) && (pBot->prev_speed >= 1.0) &&
					(pEdict->v.movetype != MOVETYPE_FLY) &&
					(pBot->f_dont_check_stuck < gpGlobals->time))
				{
					// the bot must be stuck!
					
					pBot->f_dont_avoid_wall_time = gpGlobals->time + 1.0;
					pBot->f_look_for_waypoint_time = gpGlobals->time + 1.0;
					
					if (BotCanJumpUp( pBot, &bCrouchJump ))  // can the bot jump onto something?
					{
						if ((pBot->f_jump_time + 2.0) <= gpGlobals->time)
						{
							pBot->f_jump_time = gpGlobals->time;
							pEdict->v.button |= IN_JUMP;  // jump up and move forward

							if (bCrouchJump)
							{
								pBot->f_delay_duck_time = gpGlobals->time + 0.1;
								pBot->f_do_duck_time = pBot->f_delay_duck_time + 0.4;
							}
						}
						else
						{
							// bot already tried jumping less than two seconds ago, just turn
							BotRandomTurn(pBot);
						}
					}
					else if (BotCanDuckUnder( pBot ))  // can the bot duck under something?
					{
						pEdict->v.button |= IN_DUCK;  // duck down and move forward
					}
					else
					{
						BotRandomTurn(pBot);
						
						// is the bot trying to get to an item?...
						if (pBot->pBotPickupItem != NULL)
						{
							// don't look for items for a while since the bot
							// could be stuck trying to get to an item
							pBot->f_find_item = gpGlobals->time + 0.5;
						}
					}
				}
				/* don't pause at all!
				// should the bot pause for a while here?
				// (don't pause on ladders or while being "used"...
				if ((RANDOM_LONG(1, 1000) <= pause_frequency[pBot->bot_skill]) &&
					(pEdict->v.movetype != MOVETYPE_FLY) &&
					(pBot->pBotUser == NULL))
				{
					// set the time that the bot will stop "pausing"
					pBot->f_pause_time = gpGlobals->time +
						RANDOM_FLOAT(pause_time[pBot->bot_skill][0],
						pause_time[pBot->bot_skill][1]);
				}
				*/
			}
		}
	}

	// duck for duration of duck time
	if (pBot->f_do_duck_time > gpGlobals->time && pBot->f_delay_duck_time < gpGlobals->time)
		pEdict->v.button |= IN_DUCK;

	if (pBot->pBotEnemy == NULL && pBot->f_switch_weapon_time < gpGlobals->time)
	{	// try to select our best weapon
		pBot->f_switch_weapon_time = gpGlobals->time + 1.0;
		MAKE_VECTORS( pEdict->v.v_angle );
		BotFireWeapon((pEdict->v.origin + gpGlobals->v_forward * 256) - pEdict->v.origin, pBot, 0, true);
		//SERVER_PRINT( "Weapon switched!");
	}

	// check if bot should look for items now or not...
	if (pBot->f_find_item < gpGlobals->time)
	{
		BotFindItem( pBot );  // see if there are any visible items
	}

	if ((pBot->f_role_check < gpGlobals->time) && (!pBot->b_role_locked))
	{	// redo our role every 5 seconds
		pBot->f_role_check = gpGlobals->time + 5.0;
		BotCheckRole(pBot);
	}

	// look for dangers
	if (pBot->f_assess_grenade_time < gpGlobals->time)
	{
		pBot->f_assess_grenade_time = gpGlobals->time + 0.2;
		BotAssessGrenades(pBot);
	}

	// should we be going after this goal?
	if (pBot->f_evaluate_goal_time <= gpGlobals->time)
	{
		pBot->f_evaluate_goal_time = gpGlobals->time + 2.0;
		BotEvaluateGoal(pBot);
	}

	if (pBot->curr_waypoint_index != -1)  // does the bot have a waypoint?
	{
		// check if the next waypoint is a door waypoint...
		if (waypoints[pBot->curr_waypoint_index].flags & W_FL_DOOR)
		{
			pBot->f_move_speed = pBot->f_max_speed / 3;	// slow down for doors
		}
		
		// check if the next waypoint is a ladder waypoint...
		if (waypoints[pBot->curr_waypoint_index].flags & W_FL_LADDER)
		{
			if (waypoints[pBot->curr_waypoint_index].origin.z > pEdict->v.origin.z)
			{
				pBot->ladder_dir = LADDER_UP;
				pBot->f_dont_check_stuck = gpGlobals->time + 1.0;
			}
			else if (waypoints[pBot->curr_waypoint_index].origin.z < pEdict->v.origin.z)
			{
				pBot->ladder_dir = LADDER_DOWN;
				pBot->f_dont_check_stuck = gpGlobals->time + 1.0;
			}
			else
			{
				pBot->ladder_dir = LADDER_UNKNOWN;
				pBot->f_dont_check_stuck = gpGlobals->time + 1.0;
			}
		}
		
		// check if the next waypoint is a crouch waypoint...
		if (waypoints[pBot->curr_waypoint_index].flags & W_FL_CROUCH &&
			!(waypoints[pBot->curr_waypoint_index].flags & W_FL_DUCKJUMP))
			pEdict->v.button |= IN_DUCK;  // duck down while moving forward
		
		// check if the waypoint is a sniper waypoint AND
		// bot isn't currently aiming at an ememy...
		if ((waypoints[pBot->waypoint_goal].flags & W_FL_SNIPER) &&
			(waypoints[pBot->curr_waypoint_index].flags & W_FL_SNIPER) &&
			(pBot->pBotEnemy == NULL))
		{
			{
				// check if it's time to adjust aim yet...
				if (pBot->f_sniper_aim_time <= gpGlobals->time)
				{
					int aim_index;
					
					aim_index = WaypointFindNearestAiming(waypoints[pBot->curr_waypoint_index].origin);
					
					if (aim_index != -1)
					{
						Vector v_aim = waypoints[aim_index].origin - waypoints[pBot->curr_waypoint_index].origin;
						
						Vector aim_angles = UTIL_VecToAngles( v_aim );
						
						aim_angles.y += RANDOM_LONG(0, 30) - 15;
						
						pEdict->v.ideal_yaw = aim_angles.y;
						
						BotFixIdealYaw(pEdict);
					}
					
					// don't adjust aim again until after a few seconds...
					pBot->f_sniper_aim_time = gpGlobals->time + RANDOM_FLOAT(3.0, 5.0);
				}
			}
		}
	}
	// someone shot at us, take a look over there (sound also uses this code)
	if ((pBot->pBotEnemy == NULL) && (pBot->f_dmg_time >= gpGlobals->time))
	{
		Vector v_direction = pBot->dmg_origin - pEdict->v.origin;
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

	// always forget goal
	pBot->v_goal = g_vecZero;
	// is our goal ent still around?
	if (pBot->pGoalEnt != NULL)
	{
		if (FStrEq(STRING(pBot->pGoalEnt->v.classname), "monster_scientist") && !IsAlive(pBot->pGoalEnt))
			pBot->pGoalEnt = NULL;
		if (pBot->pGoalEnt && pBot->pGoalEnt->v.effects & EF_NODRAW)
			pBot->pGoalEnt = NULL;

		if (pBot->pGoalEnt == NULL)
		{	// reset our goal point and current waypoint index
			pBot->f_waypoint_goal_time = gpGlobals->time;
			pBot->waypoint_goal = pBot->curr_waypoint_index = -1;
		}
	}
	// our goal vector is who/where we're defending
	if ((pBot->role == ROLE_DEFEND && !pBot->b_defend_patrol) ||
		(pBot->role == ROLE_ATTACK))
	{	// defend entity?
		if (pBot->pGoalEnt != NULL)
		{
			pBot->v_goal = UTIL_GetOrigin(pBot->pGoalEnt);
			// if we're defending an ally or defending a dropped sci/rsrc, our goal proximity
			// should be 128
			if ((pBot->role == ROLE_ATTACK && pBot->subrole == ROLE_SUB_DEF_ALLY) ||
				(pBot->role == ROLE_DEFEND && (pBot->subrole == ROLE_SUB_DEF_SCIS ||
				pBot->subrole == ROLE_SUB_DEF_RSRC) && IsCarryEnt(pBot->pGoalEnt)))
				pBot->f_goal_proximity = 128.0;
			else
				pBot->f_goal_proximity = 0.0;

			if (!pBot->pBotEnemy)
			{
				// get our distance to the goal ent (if we have one)
				Vector vecOrigin;
				float flGoalDist = 9999;
				
				vecOrigin = UTIL_GetOrigin(pBot->pGoalEnt);
				
				flGoalDist = (vecOrigin - pEdict->v.origin).Length();			
				// do a random turn if we're defending an ally or carry sci/rsrc and are close enough and
				// not climbing a ladder
				if (pBot->f_random_turn_time <= gpGlobals->time && flGoalDist <= pBot->f_goal_proximity &&
					pEdict->v.movetype != MOVETYPE_FLY && ((pBot->role == ROLE_ATTACK &&
					pBot->subrole == ROLE_SUB_DEF_ALLY) || (pBot->role == ROLE_DEFEND &&
					(pBot->subrole == ROLE_SUB_DEF_RSRC || pBot->subrole == ROLE_SUB_DEF_SCIS) &&
					!pBot->pGoalEnt->v.aiment && IsCarryEnt(pBot->pGoalEnt))))
				{
					pBot->f_random_turn_time = gpGlobals->time + RANDOM_FLOAT(1.0, 3.0);
					pBot->pEdict->v.ideal_yaw = RANDOM_LONG(-180, 180);
					BotFixIdealYaw(pEdict);
				}
				// look at our goal ent if we're supposed to be defending a carry sci/rsrc that's being carried
				// so the bot sees the enemy carrying it
				else if (pBot->role == ROLE_DEFEND && pBot->pGoalEnt->v.aiment &&
					(pBot->subrole == ROLE_SUB_DEF_RSRC || pBot->subrole == ROLE_SUB_DEF_SCIS) &&
					IsCarryEnt(pBot->pGoalEnt))
				{
					Vector v_direction = vecOrigin - pEdict->v.origin;
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

				// are we defending another player?
				if (pBot->role == ROLE_ATTACK && pBot->subrole == ROLE_SUB_DEF_ALLY)
				{	// if they're attacking, look toward that position
					if (pBot->pGoalEnt->v.button & IN_ATTACK || pBot->pGoalEnt->v.button & IN_ATTACK2)
					{
						TraceResult tr;
						MAKE_VECTORS( pBot->pGoalEnt->v.v_angle );

						vecOrigin = pBot->pGoalEnt->v.origin + pBot->pGoalEnt->v.view_ofs;
						UTIL_TraceLine( vecOrigin, vecOrigin + gpGlobals->v_forward * 8192,
							dont_ignore_monsters, pBot->pGoalEnt, &tr);

						pBot->dmg_origin = tr.vecEndPos;
						pBot->f_dmg_time = gpGlobals->time + 1.0;
					}
				}
			}
		}
		
		// defend vector?
		if (pBot->v_defend != g_vecZero)
		{
			pBot->v_goal = pBot->v_defend;
			/*if (pBot->role == ROLE_ATTACK && pBot->subrole == ROLE_SUB_DEF_ALLY)
				pBot->f_goal_proximity = 32.0;
			else
				pBot->f_goal_proximity = 0.0;*/
		}
		
	}

	if (pBot->f_avoid_time <= gpGlobals->time && !pBot->pAvoid)
	{
		pBot->f_avoid_time = gpGlobals->time + 0.1;
		// avoid contact
//		if (mod_id != SI_DLL)
			BotAvoidContact(pBot);
	}
	// dodge crabs
	// we do this check every frame because
	// crabs are so fast, and dodging them is a top priority
	if (mod_id == CRABBED_DLL)
		BotDodgeCrabs(pBot);

	// only try to avoid for so long
	if (pBot->pAvoid && pBot->f_do_avoid_time <= gpGlobals->time)
	{
		pBot->avoid_dir = g_vecZero;
		pBot->pAvoid = NULL;
	}
/*
	// head toward a waypoint if they're in the level, we have one, and aren't trying to get an item
	if(pBot->f_look_for_waypoint_time <= gpGlobals->time && !found_waypoint)
	{
		found_waypoint = BotHeadTowardWaypoint(pBot);
	}
*/
	if (((pBot->curr_waypoint_index != -1) || (pBot->avoid_dir != g_vecZero) || (pBot->v_goal != g_vecZero))
		&& (pBot->f_ignore_wpt_time < gpGlobals->time) && (pEdict->v.movetype != MOVETYPE_FLY))
	{
		bool bGoGoal = false;
		if ((pBot->v_goal != g_vecZero) && ((pBot->v_goal - pEdict->v.origin).Length() < 256) &&
			FVisible(pBot->v_goal, pEdict))
			bGoGoal = true;

		Vector direction;

		if (num_waypoints != 0 && pBot->curr_waypoint_index != -1 && (pBot->pBotPickupItem == NULL || 
			(pBot->pBotPickupItem && pBot->item_waypoint > -1)) &&
			pBot->avoid_dir == g_vecZero && !bGoGoal)
		{	
			direction = waypoints[pBot->curr_waypoint_index].origin - pEdict->v.origin;
		}
		else if (pBot->pBotPickupItem != NULL && pBot->avoid_dir == g_vecZero && !bGoGoal &&
			pBot->item_waypoint == -1)
		{	// head toward item instead
			direction = UTIL_GetOrigin(pBot->pBotPickupItem) - pEdict->v.origin;
		}
		else if (pBot->v_goal != g_vecZero && bGoGoal && pBot->avoid_dir == g_vecZero)
		{	// go toward our goal
			direction = pBot->v_goal - pEdict->v.origin;
		}
		else if (pBot->avoid_dir != g_vecZero)
		{	// go toward our avoidance direction
			direction = (pEdict->v.origin + pBot->avoid_dir * 64) - pEdict->v.origin;
		}
		// update the direction we're currently traveling
		pBot->v_curr_direction = direction;
		// little bit of triangle math here
		double dgrad;
		Vector v;
			
		v = UTIL_VecToAngles(direction);
		v.y = v.y + 180;
		if (v.y > 180)
			v.y -= 360;
		if (v.y < -180)
			v.y += 360;
		// get the difference between our goal and current angles
		dgrad = (double)(v.y);
		dgrad = dgrad - (double)(pEdict->v.v_angle.y);
		dgrad = dgrad + 180;
		// wrap fix
		if (dgrad > 180)
			dgrad -= 360;
		if (dgrad < -180)
			dgrad += 360;
		// radians
		dgrad = dgrad * PI;
		dgrad = dgrad / 180;
		// send our movement toward our goal
		pBot->f_move_speed = (pBot->f_max_speed) * (cos(dgrad));
		pBot->f_strafe_speed = -(pBot->f_max_speed) * (sin(dgrad));
	}

	// stop moving if we're close to our goal
	if (pBot->v_goal != g_vecZero && (pBot->v_goal - pEdict->v.origin).Length() < pBot->f_goal_proximity)
		pBot->f_move_speed = pBot->f_strafe_speed = 0;

	// are we trying to avoid a drop?
	if (pBot->f_avoid_drop_time > gpGlobals->time)
	{	// reverse direction
		pBot->f_move_speed *= -1;
		pBot->f_strafe_speed *= -1;
	}
	// is the bot "paused"? or longjumping?
	if (pBot->f_pause_time > gpGlobals->time || pBot->f_longjump_time > gpGlobals->time)
		pBot->f_move_speed = pBot->f_strafe_speed = 0;	// don't move while pausing
	
	// set the body angles the same way the bot is looking/aiming
	pEdict->v.angles.x = -pEdict->v.v_angle.x / 3;
	pEdict->v.angles.y = pEdict->v.v_angle.y;
	pEdict->v.angles.z = pEdict->v.v_angle.z;
	
	// save the previous speed (for checking if stuck)
	pBot->prev_speed = pBot->f_move_speed;

#ifdef _DEBUG	// must be in debug mode
	// only show lines if we're a listen server
	static float drawtime[32];
	if ((drawtime[pBot->index] < gpGlobals->time) && !(IS_DEDICATED_SERVER()))
	{
		drawtime[pBot->index-1] = gpGlobals->time + 0.2;
		if (pBot->curr_waypoint_index != -1)
			UTIL_DrawBeam(listenserver_edict, pEdict->v.origin, waypoints[pBot->curr_waypoint_index].origin, 
				1, 20, 0, 0, 255, 0, 200, 10);
		if (pBot->waypoint_goal != -1)
			UTIL_DrawBeam(listenserver_edict, pEdict->v.origin, waypoints[pBot->waypoint_goal].origin, 
				1, 20, 0, 255, 0, 0, 200, 10);
		if (pBot->pBotPickupItem)
			UTIL_DrawBeam(listenserver_edict, pEdict->v.origin, UTIL_GetOrigin(pBot->pBotPickupItem), 
				1, 20, 0, 255, 0, 255, 200, 10);
	}
#endif

	g_engfuncs.pfnRunPlayerMove( pEdict, pEdict->v.v_angle, pBot->f_move_speed * speed_mod[pBot->bot_skill],
		pBot->f_strafe_speed * speed_mod[pBot->bot_skill], 0, pEdict->v.button, 0, pBot->msecval);
	
	return;
}

// checks to see if any players are attacking or walking
void BotListenForFakeSound( bot_t *pBot )
{
//	ALERT(at_console, "BotListenForFakeSound\n");
	float fVolume = 1.0;
	edict_t *pEdict = pBot->pEdict;
	float distance = 10000;
	float nearest = 9999;
	bool bHearingSound = FALSE;

	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		edict_t *pPlayer = INDEXENT(i);
					
		// skip invalid players and skip self
		if ((pPlayer) && (!pPlayer->free) && (pPlayer != pEdict) && (pPlayer->v.flags & FL_CLIENT))
		{	// skip humans if observer mode is on
			if (b_observer_mode && !(pPlayer->v.flags & FL_FAKECLIENT))
				continue;

			// is the player attacking?
			distance = (pPlayer->v.origin - pEdict->v.origin).Length();

			if ((pPlayer->v.button & IN_ATTACK) && (distance < nearest))
			{
				bHearingSound = TRUE;
				nearest = distance;
			}
			// is the player moving and in water or on ground?
			if (((pPlayer->v.velocity).Length2D() > 220.0) &&
				(pPlayer->v.waterlevel > 0 || pPlayer->v.flags & FL_ONGROUND) &&
				(distance < nearest))
			{
				bHearingSound = TRUE;
				nearest = distance;	
			}

			// we're looking at something else right now
			if (pBot->f_dmg_time >= gpGlobals->time)
			{
				float dmg_distance = (pBot->dmg_origin - pEdict->v.origin).Length();
				if (dmg_distance < distance) // forget this sound if we're interested in something closer
					distance = 9999;
			}
			// are we close enough to hear this sound?
			if (distance < (pBot->f_sound_sensitivity * fVolume) && bHearingSound)
			{
				bHearingSound = FALSE;
				if (pBot->pBotEnemy == NULL && pPlayer != pEdict && pPlayer->v.owner != pEdict &&
					UTIL_GetTeam(pEdict) != UTIL_GetTeam(pPlayer) && (!pBot->pBotPickupItem ||
					(pBot->pBotPickupItem &&
					!(FStrEq(STRING(pBot->pBotPickupItem->v.classname), "func_healthcharger") ||
					FStrEq(STRING(pBot->pBotPickupItem->v.classname), "func_recharge")))))
				{
					if (b_chat_debug)
					{
						sprintf(pBot->debugchat, "I can hear %s!\n", STRING(pPlayer->v.netname));
						UTIL_HostSay(pBot->pEdict, 0, pBot->debugchat);
					}
					// just use dmg time, does the same thing
					pBot->dmg_origin = pPlayer->v.origin;
					pBot->f_dmg_time = gpGlobals->time + 1.0;
				}
			}
		}
	}	
}

// Whenever a sound is made, loop through all bots and see who can hear it
void BotListenForSound(edict_t *pEntity, const char *pszSample, float fVolume)
{
//	ALERT(at_console, "BotListenForSound\n");
	edict_t *pEdict;
	bot_t *pBot;
	int bot_index = 0;

	if (!pEntity)
		return;

	float distance;

	for (bot_index = 0; bot_index < 32; bot_index++)
	{	// validate bot
		pBot = &bots[bot_index];
		if (pBot && pBot->is_used)
		{
			pEdict = pBot->pEdict;
			// get our distance
			distance = (pEntity->v.origin - pEdict->v.origin).Length();
			// we're looking at something else right now
			if (pBot->f_dmg_time >= gpGlobals->time)
			{
				float dmg_distance = (pBot->dmg_origin - pEdict->v.origin).Length();
				if (dmg_distance < distance) // forget this sound if we're interested in something closer
					distance = 9999;
			}
			// are we close enough to hear this sound?
			if (distance < (pBot->f_sound_sensitivity * fVolume) && (!pBot->pBotPickupItem || (pBot->pBotPickupItem &&
			!(FStrEq(STRING(pBot->pBotPickupItem->v.classname), "func_healthcharger") ||
			FStrEq(STRING(pBot->pBotPickupItem->v.classname), "func_recharge")))))
			{
				if (pBot->pBotEnemy == NULL && pEntity != pEdict && pEntity->v.owner != pEdict &&
					UTIL_GetTeam(pEdict) != UTIL_GetTeam(pEntity))
				{
					// just use dmg time, does the same thing
					pBot->dmg_origin = pEntity->v.origin;
					pBot->f_dmg_time = gpGlobals->time + 1.0;

					if (b_chat_debug)
					{
						sprintf(pBot->debugchat, "I heard %s\n", pBot->name, pszSample);
						UTIL_HostSay(pBot->pEdict, 0, pBot->debugchat);
					}
				}
			}
		}
	}
}
