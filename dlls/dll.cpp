//
// HPB bot - botman's High Ping Bastard bot
//
// (http://planethalflife.com/botman/)
//
// dll.cpp
//

#ifndef _WIN32
#include <cstring>
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

#include <ctime>
#include "bot.h"
#include "bot_func.h"
#include "bot_weapons.h"
#include "waypoint.h"


#define MENU_NONE  0
#define MENU_1     1
#define MENU_2     2
#define MENU_3     3
#define MENU_4     4

#ifndef METAMOD_BUILD
   #ifndef __linux__
   extern HINSTANCE h_Library;
   #else
   extern void *h_Library;
   #endif

   DLL_FUNCTIONS other_gFunctionTable;
#endif

extern enginefuncs_t g_engfuncs;
extern int debug_engine;
extern globalvars_t  *gpGlobals;
extern char g_argv[1024];
extern bool g_waypoint_on;
extern bool g_auto_waypoint;
extern bool g_path_waypoint;
extern int num_waypoints;  // number of waypoints currently in use
extern WAYPOINT waypoints[MAX_WAYPOINTS];
extern float wp_display_time[MAX_WAYPOINTS];
extern bot_t bots[32];
extern bool b_observer_mode;
extern bool b_chat_debug;
extern bool b_botdontshoot;
char welcome_msg[] = "Grave Bot by Ghoul - Based on HPB Bot template 4 by botman and Pierre-Marie Baty\n";

extern float g_flVomiting[32];

extern long g_lTeamScore[2];
extern int g_iSciCount[2];
extern float g_flDefend[2];
extern int g_iDefendGoal[2];
extern float g_flAttack[2];
extern int g_iAttackGoal[2];
extern int g_iDefendCount[2];
extern int g_iAttackCount[2];
// for user defined
float g_fl_si_defend[2] = { -1, -1 };

extern bot_research_t g_Researched[2][NUM_RESEARCH_OPTIONS];

static FILE *fp;

DLL_FUNCTIONS gFunctionTable;
const Vector g_vecZero = Vector(0,0,0);

int mod_id = 0;
int m_spriteTexture = 0;
int default_bot_skill = 2;
bool b_random_color = TRUE;
int isFakeClientCommand = 0;
int fake_arg_count;
float bot_check_time = 30.0;
int min_bots = -1;
int max_bots = -1;
int num_bots = 0;
int prev_num_bots = 0;
bool g_GameRules = FALSE;
edict_t *clients[32];
edict_t *listenserver_edict = nullptr;
float welcome_time = 0.0;
bool welcome_sent = FALSE;
int g_menu_waypoint;
int g_menu_state = 0;

float is_team_play = 0.0;
char team_names[MAX_TEAMS][MAX_TEAMNAME_LENGTH];
int num_teams = 0;
bool checked_teamplay = FALSE;
edict_t *pent_info_tfdetect = nullptr;
edict_t *pent_info_ctfdetect = nullptr;
edict_t *pent_info_frontline = nullptr;
edict_t *pent_item_tfgoal = nullptr;
int max_team_players[4];  // for TFC
int team_class_limits[4];  // for TFC
int team_allies[4];  // TFC bit mapped allies BLUE, RED, YELLOW, and GREEN
int max_teams = 0;  // for TFC
FLAG_S flags[MAX_FLAGS];  // for TFC
int num_flags = 0;  // for TFC

Vector g_vecPoint1 = g_vecZero;
Vector g_vecPoint2 = g_vecZero;

FILE *bot_cfg_fp = nullptr;
bool need_to_open_cfg = TRUE;
float bot_cfg_pause_time = 0.0;
float respawn_time = 0.0;
bool spawn_time_reset = FALSE;
// reaction time multiplier
float bot_reaction_time = 1.0;
extern float speed_mod[5];
extern float react_time_min[5];
extern float react_time_max[5];
/*
// TheFatal's method for calculating the msecval
int msecnum;
float msecdel;
float msecval;
*/

cvar_t sv_bot = {"gravebot",""};

char *show_menu_1 =
   {"Waypoint Tags\n\n1. Team Specific\n2. Wait for Lift\n3. Door\n4. Sniper Spot\n5. More..."};
char *show_menu_2 =
   {"Waypoint Tags\n\n1. Team 1\n2. Team 2\n3. Team 3\n4. Team 4\n5. CANCEL"};
char *show_menu_3 =
   {"Waypoint Tags\n\n1. Jump\n2. Ladder\n3. Weapon\n4. Ammo\n5. More..."};
char *show_menu_4 =
   {"Waypoint Tags\n\n1. Health\n2. Armor\n3. Longjump\n4. Duck Jump\n5. CANCEL"};


void BotNameInit();
void UpdateClientData(const struct edict_s *ent, int sendweapons, struct clientdata_s *cd);
void ProcessBotCfgFile();
bool ProcessCommand( edict_t *pEntity, const char *pcmd, const char *arg1, const char *arg2,
	const char *arg3, const char *arg4, const char *arg5);
void RoleDetermine();
void RoleCount();
char *RoleToString(int role);
char *SubroleToString(int subrole);
void ClearEdict( edict_t *pEdict);


#ifdef METAMOD_BUILD
// START of Metamod plugin API

enginefuncs_t meta_engfuncs;
gamedll_funcs_t *gpGamedllFuncs;
mutil_funcs_t *gpMetaUtilFuncs;
meta_globals_t *gpMetaGlobals;

META_FUNCTIONS gMetaFunctionTable =
{
	nullptr, // pfnGetEntityAPI()
	nullptr, // pfnGetEntityAPI_Post()
   GetEntityAPI2, // pfnGetEntityAPI2()
	nullptr, // pfnGetEntityAPI2_Post()
	nullptr, // pfnGetNewDLLFunctions()
	nullptr, // pfnGetNewDLLFunctions_Post()
   GetEngineFunctions, // pfnGetEngineFunctions()
	nullptr, // pfnGetEngineFunctions_Post()
};

plugin_info_t Plugin_info = {
   META_INTERFACE_VERSION, // interface version
   "Grave Bot", // plugin name
   GRAVEBOT_VERSION, // plugin version
   __DATE__, // date of creation
   "Ghoul & RoboCop", // plugin author
   "http://gravebot.bots-united.com/", // plugin URL
   "GRAVEBOT", // plugin logtag
   PT_STARTUP, // when loadable
   PT_ANYTIME, // when unloadable
};


C_DLLEXPORT int Meta_Query (char *ifvers, plugin_info_t **pPlugInfo, mutil_funcs_t *pMetaUtilFuncs)
{
   // this function is the first function ever called by metamod in the plugin DLL. Its purpose
   // is for metamod to retrieve basic information about the plugin, such as its meta-interface
   // version, for ensuring compatibility with the current version of the running metamod.

   // keep track of the pointers to metamod function tables metamod gives us
   gpMetaUtilFuncs = pMetaUtilFuncs;
   *pPlugInfo = &Plugin_info;

   // check for interface version compatibility
   if (strcmp (ifvers, Plugin_info.ifvers) != 0)
   {
      int mmajor = 0, mminor = 0, pmajor = 0, pminor = 0;

      LOG_CONSOLE (PLID, "%s: meta-interface version mismatch (metamod: %s, %s: %s)", Plugin_info.name, ifvers, Plugin_info.name, Plugin_info.ifvers);
      LOG_MESSAGE (PLID, "%s: meta-interface version mismatch (metamod: %s, %s: %s)", Plugin_info.name, ifvers, Plugin_info.name, Plugin_info.ifvers);

      // if plugin has later interface version, it's incompatible (update metamod)
      sscanf (ifvers, "%d:%d", &mmajor, &mminor);
      sscanf (META_INTERFACE_VERSION, "%d:%d", &pmajor, &pminor);

      if ((pmajor > mmajor) || ((pmajor == mmajor) && (pminor > mminor)))
      {
         LOG_CONSOLE (PLID, "metamod version is too old for this plugin; update metamod");
         LOG_ERROR (PLID, "metamod version is too old for this plugin; update metamod");
         return (FALSE);
      }

      // if plugin has older major interface version, it's incompatible (update plugin)
      else if (pmajor < mmajor)
      {
         LOG_CONSOLE (PLID, "metamod version is incompatible with this plugin; please find a newer version of this plugin");
         LOG_ERROR (PLID, "metamod version is incompatible with this plugin; please find a newer version of this plugin");
         return (FALSE);
      }
   }

   return (TRUE); // tell metamod this plugin looks safe
}


C_DLLEXPORT int Meta_Attach (PLUG_LOADTIME now, META_FUNCTIONS *pFunctionTable, meta_globals_t *pMGlobals, gamedll_funcs_t *pGamedllFuncs)
{
   // this function is called when metamod attempts to load the plugin. Since it's the place
   // where we can tell if the plugin will be allowed to run or not, we wait until here to make
   // our initialization stuff, like registering CVARs and dedicated server commands.

   // are we allowed to load this plugin now ?
   if (now > Plugin_info.loadable)
   {
      LOG_CONSOLE (PLID, "%s: plugin NOT attaching (can't load plugin right now)", Plugin_info.name);
      LOG_ERROR (PLID, "%s: plugin NOT attaching (can't load plugin right now)", Plugin_info.name);
      return (FALSE); // returning FALSE prevents metamod from attaching this plugin
   }

   // keep track of the pointers to engine function tables metamod gives us
   gpMetaGlobals = pMGlobals;
   memcpy (pFunctionTable, &gMetaFunctionTable, sizeof (META_FUNCTIONS));
   gpGamedllFuncs = pGamedllFuncs;

   // print a message to notify about plugin attaching
   LOG_CONSOLE (PLID, "%s: plugin attaching", Plugin_info.name);
   LOG_MESSAGE (PLID, "%s: plugin attaching", Plugin_info.name);

   // ask the engine to register the CVARs this plugin uses
   CVAR_REGISTER (&sv_bot);

   return (TRUE); // returning TRUE enables metamod to attach this plugin
}


C_DLLEXPORT int Meta_Detach (PLUG_LOADTIME now, PL_UNLOAD_REASON reason)
{
   // this function is called when metamod unloads the plugin. A basic check is made in order
   // to prevent unloading the plugin if its processing should not be interrupted.

   // is metamod allowed to unload the plugin ?
   if ((now > Plugin_info.unloadable) && (reason != PNL_CMD_FORCED))
   {
      LOG_CONSOLE (PLID, "%s: plugin NOT detaching (can't unload plugin right now)", Plugin_info.name);
      LOG_ERROR (PLID, "%s: plugin NOT detaching (can't unload plugin right now)", Plugin_info.name);
      return (FALSE); // returning FALSE prevents metamod from unloading this plugin
   }

   return (TRUE); // returning TRUE enables metamod to unload this plugin
}

// END of Metamod plugin API
#endif


void GameDLLInit()
{
	int i;

   #ifndef METAMOD_BUILD
	   CVAR_REGISTER (&sv_bot);
   #endif
	
	for (i=0; i<32; i++)
		clients[i] = nullptr;
	
	// initialize the bots array of structures...
	memset(bots, 0, sizeof(bots));

	BotNameInit();

   #ifndef METAMOD_BUILD
	   (*other_gFunctionTable.pfnGameInit)();
   #else
      RETURN_META (MRES_IGNORED);
   #endif
}

int DispatchSpawn( edict_t *pent )
{
	int index;
	
	if (gpGlobals->deathmatch)
	{
		char *pClassname = const_cast<char *>(STRING(pent->v.classname));
		
		if (debug_engine)
		{
			fp=fopen("bot.txt","a");
			fprintf(fp, "DispatchSpawn: %x %s\n",unsigned(pent),pClassname);
			if (pent->v.model != 0)
				fprintf(fp, " model=%s\n",STRING(pent->v.model));
			fclose(fp);
		}
		
		if (strcmp(pClassname, "worldspawn") == 0)
		{
			// do level initialization stuff here...
			
			WaypointInit();
			WaypointLoad(nullptr, ".gbw");
			
			pent_info_tfdetect = nullptr;
			pent_info_ctfdetect = nullptr;
			pent_info_frontline = nullptr;
			pent_item_tfgoal = nullptr;
			
			for (index=0; index < 4; index++)
			{
				max_team_players[index] = 0;  // no player limit
				team_class_limits[index] = 0;  // no class limits
				team_allies[index] = 0;
			}
			
			max_teams = 0;
			num_flags = 0;
			
			PRECACHE_SOUND("weapons/xbow_hit1.wav");      // waypoint add
			PRECACHE_SOUND("weapons/mine_activate.wav");  // waypoint delete
			PRECACHE_SOUND("common/wpn_hudoff.wav");      // path add/delete start
			PRECACHE_SOUND("common/wpn_hudon.wav");       // path add/delete done
			PRECACHE_SOUND("common/wpn_moveselect.wav");  // path add/delete cancel
			PRECACHE_SOUND("common/wpn_denyselect.wav");  // path add/delete error
			PRECACHE_MODEL( "models/mechgibs.mdl" );      // botcam

			m_spriteTexture = PRECACHE_MODEL( "sprites/lgtning.spr");
			
			g_GameRules = TRUE;
			
			is_team_play = 0.0;
			memset(team_names, 0, sizeof(team_names));
			num_teams = 0;
			checked_teamplay = FALSE;
			
			bot_cfg_pause_time = 0.0;
			respawn_time = 0.0;
			spawn_time_reset = FALSE;
			
			prev_num_bots = num_bots;
			num_bots = 0;
			
			bot_check_time = gpGlobals->time + 30.0;

			// reset all research data
			if (mod_id == SI_DLL)
			{
				for (int t = 0; t < 2; t++)
				{
					for (int r = 0; r < NUM_RESEARCH_OPTIONS; r++)
					{
						g_Researched[t][r].researched = g_Researched[t][r].canidate =
							g_Researched[t][r].disabled = g_Researched[t][r].stolen = false;
					}
					// colt is always availible
					g_Researched[t][RESEARCH_COLT].researched = true;
				}

				for (int i = 0; i < 32; i++)
					g_flVomiting[i] = -1;
			}
		}
	}
	
   #ifndef METAMOD_BUILD
   	return (*other_gFunctionTable.pfnSpawn)(pent);
   #else
      RETURN_META_VALUE (MRES_IGNORED, 0);
   #endif
}

void DispatchThink( edict_t *pent )
{
	if(strcmp(STRING(pent->v.classname), "botcam") == 0)
	{
		if (pent->v.euser1)
		{
			int offset = 16;
			bot_t *pBot = UTIL_GetBotPointer(pent->v.euser1);
			if (pBot->f_move_speed < 0) offset = 80;

			pent->v.angles = pent->v.euser1->v.v_angle;

			MAKE_VECTORS(pent->v.euser1->v.v_angle);
			TraceResult tr;
			UTIL_TraceLine(pent->v.euser1->v.origin + pent->v.euser1->v.view_ofs,
				pent->v.euser1->v.origin + pent->v.euser1->v.view_ofs + gpGlobals->v_forward * offset,
				ignore_monsters, pent->v.euser1, &tr );

			pent->v.origin = tr.vecEndPos;
		}
		pent->v.velocity=Vector(0,0,0);
		if (pent->v.angles.y >= 360)
			pent->v.angles.y -= 360;
		if (pent->v.angles.y < 0)
			pent->v.angles.y += 360;
		pent->v.nextthink = gpGlobals->time;
	}

   #ifndef METAMOD_BUILD
   	(*other_gFunctionTable.pfnThink)(pent);
   #else
      RETURN_META (MRES_IGNORED);
   #endif
}

#ifndef METAMOD_BUILD
   void DispatchUse( edict_t *pentUsed, edict_t *pentOther )
   {
	   (*other_gFunctionTable.pfnUse)(pentUsed, pentOther);
   }
   void DispatchTouch( edict_t *pentTouched, edict_t *pentOther )
   {
	   (*other_gFunctionTable.pfnTouch)(pentTouched, pentOther);
   }
   void DispatchBlocked( edict_t *pentBlocked, edict_t *pentOther )
   {
	   (*other_gFunctionTable.pfnBlocked)(pentBlocked, pentOther);
   }
   void DispatchKeyValue( edict_t *pentKeyvalue, KeyValueData *pkvd )
   {
	   static edict_t *temp_pent;
	   static int flag_index;
	   
	   //   fp=fopen("bot.txt","a"); fprintf(fp, "DispatchKeyValue: %x %s=%s\n",pentKeyvalue,pkvd->szKeyName,pkvd->szValue); fclose(fp);
	   
	   (*other_gFunctionTable.pfnKeyValue)(pentKeyvalue, pkvd);
   }
   void DispatchSave( edict_t *pent, SAVERESTOREDATA *pSaveData )
   {
	   (*other_gFunctionTable.pfnSave)(pent, pSaveData);
   }
   int DispatchRestore( edict_t *pent, SAVERESTOREDATA *pSaveData, int globalEntity )
   {
	   return (*other_gFunctionTable.pfnRestore)(pent, pSaveData, globalEntity);
   }
   void DispatchObjectCollisionBox( edict_t *pent )
   {
	   (*other_gFunctionTable.pfnSetAbsBox)(pent);
   }
   void SaveWriteFields( SAVERESTOREDATA *pSaveData, const char *pname, void *pBaseData, TYPEDESCRIPTION *pFields, int fieldCount )
   {
	   (*other_gFunctionTable.pfnSaveWriteFields)(pSaveData, pname, pBaseData, pFields, fieldCount);
   }
   void SaveReadFields( SAVERESTOREDATA *pSaveData, const char *pname, void *pBaseData, TYPEDESCRIPTION *pFields, int fieldCount )
   {
	   (*other_gFunctionTable.pfnSaveReadFields)(pSaveData, pname, pBaseData, pFields, fieldCount);
   }
   void SaveGlobalState( SAVERESTOREDATA *pSaveData )
   {
	   (*other_gFunctionTable.pfnSaveGlobalState)(pSaveData);
   }
   void RestoreGlobalState( SAVERESTOREDATA *pSaveData )
   {
	   (*other_gFunctionTable.pfnRestoreGlobalState)(pSaveData);
   }
   void ResetGlobalState( void )
   {
	   (*other_gFunctionTable.pfnResetGlobalState)();
   }
#endif

BOOL ClientConnect( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ 128 ]  )
{ 
	if (gpGlobals->deathmatch)
	{
		int i;
		int count = 0;
		
		if (debug_engine)
		{
			fp = fopen("bot.txt", "a");
			fprintf(fp, "ClientConnect: pent=%x name=%s\n", unsigned(pEntity), pszName);
			fclose(fp);
		}
		
		// check if this client is the listen server client
		if (strcmp(pszAddress, "loopback") == 0)
		{
			// save the edict of the listen server client...
			listenserver_edict = pEntity;
		}
		
		// check if this is NOT a bot joining the server...
		if (strcmp(pszAddress, "127.0.0.1") != 0)
		{
			// don't try to add bots for 60 seconds, give client time to get added
			bot_check_time = gpGlobals->time + 60.0;
			
			for (i=0; i < 32; i++)
			{
				if (bots[i].is_used)  // count the number of bots in use
					count++;
			}
			
			// if there are currently more than the minimum number of bots running
			// then kick one of the bots off the server...
			if ((count > min_bots) && (min_bots != -1))
			{
				for (i=0; i < 32; i++)
				{
					if (bots[i].is_used)  // is this slot used?
					{
						char cmd[80];
						
						sprintf(cmd, "kick \"%s\"\n", bots[i].name);
						
						SERVER_COMMAND(cmd);  // kick the bot using (kick "name")
						
						break;
					}
				}
			}
		}
	}
	
   #ifndef METAMOD_BUILD
	   return (*other_gFunctionTable.pfnClientConnect)(pEntity, pszName, pszAddress, szRejectReason);
   #else
      RETURN_META_VALUE (MRES_IGNORED, 0);
   #endif
}

void ClientDisconnect( edict_t *pEntity )
{
	int i;

	if (gpGlobals->deathmatch)
	{		
		if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp, "ClientDisconnect: %x\n",unsigned(pEntity)); fclose(fp); }
		
		i = 0;
		while ((i < 32) && (clients[i] != pEntity))
			i++;
		
		if (i < 32)
			clients[i] = nullptr;		
		
		for (i = 0; i < 32; i++)
		{
			if (bots[i].pEdict == pEntity)
			{	
				// someone kicked this bot off of the server...				
				bots[i].is_used = FALSE;  // this slot is now free to use
				bots[i].kick_time = gpGlobals->time;  // save the kicked time
				break;
			}
		}
	}

   #ifndef METAMOD_BUILD
   	(*other_gFunctionTable.pfnClientDisconnect)(pEntity);
   #else
      RETURN_META (MRES_IGNORED);
   #endif
}

#ifndef METAMOD_BUILD
   void ClientKill( edict_t *pEntity )
   {
	   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp, "ClientKill: %x\n",pEntity); fclose(fp); }
	   (*other_gFunctionTable.pfnClientKill)(pEntity);
   }
#endif

void ClientPutInServer( edict_t *pEntity )
{
	if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp, "ClientPutInServer: %x\n",unsigned(pEntity)); fclose(fp); }
	
	int i = 0;
	
	while ((i < 32) && (clients[i] != nullptr))
		i++;
	
	if (i < 32)
		clients[i] = pEntity;  // store this clients edict in the clients array

	// FIX FOR BOT CRASH ON REJOIN
	for (i = 0; i < 32; i++)
	{
		if (bots[i].pEdict == pEntity)
		{	// bot crashes if we don't clear out all the edict data (team must = 0 or 1)
			// this is to simulate a player joining (which will have 0 in all fields)
			// for some reason the engine doesn't clear a bot's edict when they
			// are disconnected from the sever
			ClearEdict(pEntity);
			break;
		}
	}

   #ifndef METAMOD_BUILD
   	(*other_gFunctionTable.pfnClientPutInServer)(pEntity);
   #else
      RETURN_META (MRES_IGNORED);
   #endif
}

void ClientCommand( edict_t *pEntity )
{
	// only allow custom commands if deathmatch mode and NOT dedicated server and
	// client sending command is the listen server client...
	if ((gpGlobals->deathmatch) && (!IS_DEDICATED_SERVER()) &&
		(pEntity == listenserver_edict))
	{
		const char *pcmd = CMD_ARGV(0);
		const char *arg1 = CMD_ARGV(1);
		const char *arg2 = CMD_ARGV(2);
		const char *arg3 = CMD_ARGV(3);
		const char *arg4 = CMD_ARGV(4);
		const char *arg5 = CMD_ARGV(5);

		if (ProcessCommand(pEntity, pcmd, arg1, arg2, arg3, arg4, arg5))
         #ifndef METAMOD_BUILD
			   return;
         #else
            RETURN_META (MRES_SUPERCEDE);
         #endif
	}

   #ifndef METAMOD_BUILD
   	(*other_gFunctionTable.pfnClientCommand)(pEntity);
   #else
      RETURN_META (MRES_IGNORED);
   #endif
}

#ifndef METAMOD_BUILD
void ClientUserInfoChanged( edict_t *pEntity, char *infobuffer )
{
	if (debug_engine) { fp=fopen("bot.txt", "a"); fprintf(fp, "ClientUserInfoChanged: pEntity=%x infobuffer=%s\n", pEntity, infobuffer); fclose(fp); }
		(*other_gFunctionTable.pfnClientUserInfoChanged)(pEntity, infobuffer);
}
void ServerActivate( edict_t *pEdictList, int edictCount, int clientMax )
{
	(*other_gFunctionTable.pfnServerActivate)(pEdictList, edictCount, clientMax);
}
void ServerDeactivate( void )
{
	(*other_gFunctionTable.pfnServerDeactivate)();
}
void PlayerPreThink( edict_t *pEntity )
{
	(*other_gFunctionTable.pfnPlayerPreThink)(pEntity);
}
void PlayerPostThink( edict_t *pEntity )
{
	(*other_gFunctionTable.pfnPlayerPostThink)(pEntity);
}
#endif

void StartFrame()
{
	if (gpGlobals->deathmatch)
	{
		edict_t *pPlayer;
		static float role_check_time = 5.0;
		static float check_server_cmd = 0.0;
		static int i, index, player_index, bot_index;
		static float previous_time = -1.0;
		char msg[256];
		int count;
		static float si_researchall = 0;
	
		// if a new map has started then (MUST BE FIRST IN StartFrame)...
		if ((gpGlobals->time + 0.1) < previous_time)
		{
			char filename[256];
			char mapname[64];

			check_server_cmd = 0.0;  // reset at start of map
			
			// check if mapname_bot.cfg file exists...
			
			strcpy(mapname, STRING(gpGlobals->mapname));
			strcat(mapname, "_bot.cfg");
			
			UTIL_BuildFileName(filename, "maps", mapname);
			
			if ((bot_cfg_fp = fopen(filename, "r")) != nullptr)
			{
				sprintf(msg, "Executing %s\n", filename);
				SERVER_PRINT( msg );
				
				for (index = 0; index < 32; index++)
				{
					bots[index].is_used = FALSE;
					bots[index].respawn_state = 0;
					bots[index].kick_time = 0.0;
				}
				
				if (IS_DEDICATED_SERVER())
					bot_cfg_pause_time = gpGlobals->time + 5.0;
				else
					bot_cfg_pause_time = gpGlobals->time + 20.0;
			}
			else
			{
				count = 0;
				
				// mark the bots as needing to be respawned...
				for (index = 0; index < 32; index++)
				{
					if (count >= prev_num_bots)
					{
						bots[index].is_used = FALSE;
						bots[index].respawn_state = 0;
						bots[index].kick_time = 0.0;
					}
					
					if (bots[index].is_used)  // is this slot used?
					{
						bots[index].respawn_state = RESPAWN_NEED_TO_RESPAWN;
						count++;
					}
					
					// check for any bots that were very recently kicked...
					if ((bots[index].kick_time + 5.0) > previous_time)
					{
						if (mod_id != SI_DLL)
							bots[index].respawn_state = RESPAWN_NEED_TO_RESPAWN;

						count++;
					}
					else
						bots[index].kick_time = 0.0;  // reset to prevent false spawns later
				}
				
				// set the respawn time
				if (IS_DEDICATED_SERVER())
					respawn_time = gpGlobals->time + 5.0;
				else
					respawn_time = gpGlobals->time + 20.0;
			}
			
			bot_check_time = gpGlobals->time + 30.0;
		}

		if (!IS_DEDICATED_SERVER())
		{
			if ((listenserver_edict != nullptr) && (welcome_sent == FALSE) &&
				(welcome_time < 1.0))
			{
				// are they out of observer mode yet?
				if (IsAlive(listenserver_edict))
					welcome_time = gpGlobals->time + 5.0;  // welcome in 5 seconds
			}
			
			if ((welcome_time > 0.0) && (welcome_time < gpGlobals->time) &&
				(welcome_sent == FALSE))
			{
				// let's send a welcome message to this client...
				UTIL_SayText(welcome_msg, listenserver_edict);
				
				welcome_sent = TRUE;  // clear this so we only do it once
			}
		}
		else
		{
			if ((welcome_sent == FALSE) &&
				(welcome_time < 1.0))
				welcome_time = gpGlobals->time + 5.0;  // welcome in 5 seconds
			
			if ((welcome_time > 0.0) && (welcome_time < gpGlobals->time) &&
				(welcome_sent == FALSE))
			{
				// let's send a welcome message
				printf(welcome_msg);
				
				welcome_sent = TRUE;  // clear this so we only do it once
			}
		}

		count = 0;
		
		for (bot_index = 0; bot_index < gpGlobals->maxClients; bot_index++)
		{
			if ((bots[bot_index].is_used) &&  // is this slot used AND
				(bots[bot_index].respawn_state == RESPAWN_IDLE))  // not respawning
			{
				BotThink(&bots[bot_index]);
				
				count++;
			}
		}
		
		if (count > num_bots)
			num_bots = count;
		
		for (player_index = 1; player_index <= gpGlobals->maxClients; player_index++)
		{
			pPlayer = INDEXENT(player_index);
			
			if (pPlayer && !pPlayer->free)
			{
				if ((g_waypoint_on) && FBitSet(pPlayer->v.flags, FL_CLIENT))
				{
					WaypointThink(pPlayer);
				}
			}
		}
		
		// are we currently respawning bots and is it time to spawn one yet?
		if ((respawn_time > 1.0) && (respawn_time <= gpGlobals->time))
		{
			int index = 0;
			
			// find bot needing to be respawned...
			while ((index < 32) &&
                (bots[index].respawn_state != RESPAWN_NEED_TO_RESPAWN))
				index++;
			
			if (index < 32)
			{
				bots[index].respawn_state = RESPAWN_IS_RESPAWNING;
				bots[index].is_used = FALSE;      // free up this slot
				
				// respawn 1 bot then wait a while (otherwise engine crashes)
				char c_skill[2];
				char c_topcolor[4];
				char c_bottomcolor[4];

				sprintf(c_skill, "%d", bots[index].bot_skill);
				sprintf(c_topcolor, "%d", bots[index].top_color);
				sprintf(c_bottomcolor, "%d", bots[index].bottom_color);
				
				BotCreate(nullptr, bots[index].skin, bots[index].name, c_skill, c_topcolor, c_bottomcolor);
				
				respawn_time = gpGlobals->time + 2;  // set next respawn time
				
				bot_check_time = gpGlobals->time + 5.0;
			}
			else
			{
				respawn_time = 0.0;
			}
		}
		
		if (g_GameRules)
		{
			if (need_to_open_cfg)  // have we open bot.cfg file yet?
			{
				char filename[256];
				char mapname[64];
				
				need_to_open_cfg = FALSE;  // only do this once!!!
				
				// check if mapname_bot.cfg file exists...
				
				strcpy(mapname, STRING(gpGlobals->mapname));
				strcat(mapname, "_bot.cfg");
				
				UTIL_BuildFileName(filename, "maps", mapname);
				
				if ((bot_cfg_fp = fopen(filename, "r")) != nullptr)
				{
					sprintf(msg, "Executing %s\n", filename);
					SERVER_PRINT( msg );
				}
				else
				{
					UTIL_BuildFileName(filename, "grave_bot.cfg", nullptr);
					
					sprintf(msg, "Executing %s\n", filename);
					SERVER_PRINT( msg );
					
					bot_cfg_fp = fopen(filename, "r");
					
					if (bot_cfg_fp == nullptr)
						SERVER_PRINT( "grave_bot.cfg file not found\n" );
				}
				
				if (IS_DEDICATED_SERVER())
					bot_cfg_pause_time = gpGlobals->time + 5.0;
				else
					bot_cfg_pause_time = gpGlobals->time + 20.0;
			}
			
			if (!IS_DEDICATED_SERVER() && !spawn_time_reset)
			{
				if (listenserver_edict != nullptr)
				{
					if (IsAlive(listenserver_edict))
					{
						spawn_time_reset = TRUE;
						
						if (respawn_time >= 1.0)
							respawn_time = fmin(respawn_time, gpGlobals->time + (float)1.0);
						
						if (bot_cfg_pause_time >= 1.0)
							bot_cfg_pause_time = fmin(bot_cfg_pause_time, gpGlobals->time + (float)1.0);
					}
				}
			}
			
			if ((bot_cfg_fp) &&
				(bot_cfg_pause_time >= 1.0) && (bot_cfg_pause_time <= gpGlobals->time))
			{
				// process bot.cfg file options...
				ProcessBotCfgFile();
			}
			
		}      
		
		// if time to check for server commands then do so...
		if ((check_server_cmd <= gpGlobals->time) && IS_DEDICATED_SERVER())
		{
			check_server_cmd = gpGlobals->time + 1.0;
			
			char *cvar_bot = const_cast<char *>(CVAR_GET_STRING("gravebot"));
			
			if ( cvar_bot && cvar_bot[0] )
			{
				char cmd_line[80];
				const char *cmd, *arg1, *arg2, *arg3, *arg4, *arg5;
				strcpy(cmd_line, cvar_bot);
				
				index = 0;
				cmd = cmd_line;
				arg1 = arg2 = arg3 = arg4 = arg5 = nullptr;

				// skip to blank or end of string...
				while ((cmd_line[index] != ' ') && (cmd_line[index] != 0))
					index++;

				if (cmd_line[index] == ' ')
				{
					cmd_line[index++] = 0;
					arg1 = &cmd_line[index];

					// skip to blank or end of string...
					while ((cmd_line[index] != ' ') && (cmd_line[index] != 0))
						index++;

					if (cmd_line[index] == ' ')
					{
						cmd_line[index++] = 0;
						arg2 = &cmd_line[index];

						// skip to blank or end of string...
						while ((cmd_line[index] != ' ') && (cmd_line[index] != 0))
							index++;

						if (cmd_line[index] == ' ')
						{
							cmd_line[index++] = 0;
							arg3 = &cmd_line[index];

							// skip to blank or end of string...
							while ((cmd_line[index] != ' ') && (cmd_line[index] != 0))
								index++;

							if (cmd_line[index] == ' ')
							{
								cmd_line[index++] = 0;
								arg4 = &cmd_line[index];
							}
						}
					}
				}
				// run the command
				ProcessCommand(nullptr, cmd, arg1, arg2, arg3, arg4, arg5);
	
				CVAR_SET_STRING("gravebot", "");
			}
		}
		
		// check if time to see if a bot needs to be created...
		if (bot_check_time < gpGlobals->time)
		{
			int count = 0;
			
			bot_check_time = gpGlobals->time + 5.0;
			
			for (i = 0; i < 32; i++)
			{
				if (clients[i] != nullptr)
					count++;
			}
			
			// if there are currently less than the maximum number of "players"
			// then add another bot using the default skill level...
			if ((count < max_bots) && (max_bots != -1))
			{
				BotCreate(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
			}
		}

		if (role_check_time <= gpGlobals->time)
		{	// redo our role check and determines
			role_check_time = gpGlobals->time + 1.0;
			RoleCount();
			RoleDetermine();
		}

		previous_time = gpGlobals->time;
   }

   #ifndef METAMOD_BUILD
      (*other_gFunctionTable.pfnStartFrame)();
   #else
      RETURN_META (MRES_IGNORED);
   #endif
}

#ifndef METAMOD_BUILD
   void ParmsNewLevel( void )
   {
	   (*other_gFunctionTable.pfnParmsNewLevel)();
   }
   void ParmsChangeLevel( void )
   {
	   (*other_gFunctionTable.pfnParmsChangeLevel)();
   }
   const char *GetGameDescription( void )
   {
	   return (*other_gFunctionTable.pfnGetGameDescription)();
   }
   void PlayerCustomization( edict_t *pEntity, customization_t *pCust )
   {
	   if (debug_engine) { fp=fopen("bot.txt", "a"); fprintf(fp, "PlayerCustomization: %x\n",pEntity); fclose(fp); }
	   (*other_gFunctionTable.pfnPlayerCustomization)(pEntity, pCust);
   }
   void SpectatorConnect( edict_t *pEntity )
   {
	   (*other_gFunctionTable.pfnSpectatorConnect)(pEntity);
   }
   void SpectatorDisconnect( edict_t *pEntity )
   {
	   (*other_gFunctionTable.pfnSpectatorDisconnect)(pEntity);
   }
   void SpectatorThink( edict_t *pEntity )
   {
	   (*other_gFunctionTable.pfnSpectatorThink)(pEntity);
   }
   void Sys_Error( const char *error_string )
   {
	   (*other_gFunctionTable.pfnSys_Error)(error_string);
   }
   void PM_Move ( struct playermove_s *ppmove, int server )
   {
	   (*other_gFunctionTable.pfnPM_Move)(ppmove, server);
   }
   void PM_Init ( struct playermove_s *ppmove )
   {
	   (*other_gFunctionTable.pfnPM_Init)(ppmove);
   }
   char PM_FindTextureType( char *name )
   {
	   return (*other_gFunctionTable.pfnPM_FindTextureType)(name);
   }
   void SetupVisibility( edict_t *pViewEntity, edict_t *pClient, unsigned char **pvs, unsigned char **pas )
   {
	   (*other_gFunctionTable.pfnSetupVisibility)(pViewEntity, pClient, pvs, pas);
   }
   void UpdateClientData ( const struct edict_s *ent, int sendweapons, struct clientdata_s *cd )
   {
	   (*other_gFunctionTable.pfnUpdateClientData)(ent, sendweapons, cd);
   }
   int AddToFullPack( struct entity_state_s *state, int e, edict_t *ent, edict_t *host, int hostflags, int player, unsigned char *pSet )
   {
	   return (*other_gFunctionTable.pfnAddToFullPack)(state, e, ent, host, hostflags, player, pSet);
   }
   void CreateBaseline( int player, int eindex, struct entity_state_s *baseline, struct edict_s *entity, int playermodelindex, vec3_t player_mins, vec3_t player_maxs )
   {
	   (*other_gFunctionTable.pfnCreateBaseline)(player, eindex, baseline, entity, playermodelindex, player_mins, player_maxs);
   }
   void RegisterEncoders( void )
   {
	   (*other_gFunctionTable.pfnRegisterEncoders)();
   }
   int GetWeaponData( struct edict_s *player, struct weapon_data_s *info )
   {
	   return (*other_gFunctionTable.pfnGetWeaponData)(player, info);
   }
   void CmdStart( const edict_t *player, const struct usercmd_s *cmd, unsigned int random_seed )
   {
	   (*other_gFunctionTable.pfnCmdStart)(player, cmd, random_seed);
   }
   void CmdEnd ( const edict_t *player )
   {
	   (*other_gFunctionTable.pfnCmdEnd)(player);
   }
   int ConnectionlessPacket( const struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size )
   {
	   return (*other_gFunctionTable.pfnConnectionlessPacket)(net_from, args, response_buffer, response_buffer_size);
   }
   int GetHullBounds( int hullnumber, float *mins, float *maxs )
   {
	   return (*other_gFunctionTable.pfnGetHullBounds)(hullnumber, mins, maxs);
   }
   void CreateInstancedBaselines( void )
   {
	   (*other_gFunctionTable.pfnCreateInstancedBaselines)();
   }
   int InconsistentFile( const edict_t *player, const char *filename, char *disconnect_message )
   {
	   if (debug_engine) { fp=fopen("bot.txt", "a"); fprintf(fp, "InconsistentFile: %x filename=%s\n",player,filename); fclose(fp); }
	   return (*other_gFunctionTable.pfnInconsistentFile)(player, filename, disconnect_message);
   }
   int AllowLagCompensation( void )
   {
	   return (*other_gFunctionTable.pfnAllowLagCompensation)();
   }
#endif


#ifdef METAMOD_BUILD
   C_DLLEXPORT int GetEntityAPI2 (DLL_FUNCTIONS *pFunctionTable, int *interfaceVersion)
   {
      // This exported function declares to metamod which functions from the game DLL interface our
      // DLL will use, by linking each used slot in the gFunctionTable structure to the address
      // of our actual functions. This way, when metamod will bind any function in any slot of
      // gFunctionTable, it will land directly in our functions here, just by following the pointer.

      // These functions will be called by metamod BEFORE calling the actual game DLL function.

      // cleanup the function table
      memset (&gFunctionTable, 0, sizeof (gFunctionTable));

      // keep track of pointers to the functions we use in the game DLL API
      gFunctionTable.pfnGameInit = GameDLLInit;
      gFunctionTable.pfnSpawn = DispatchSpawn;
      gFunctionTable.pfnThink = DispatchThink;
      gFunctionTable.pfnClientConnect = ClientConnect;
      gFunctionTable.pfnClientPutInServer = ClientPutInServer;
      gFunctionTable.pfnClientDisconnect = ClientDisconnect;
      gFunctionTable.pfnClientCommand = ClientCommand;
      gFunctionTable.pfnStartFrame = StartFrame;

      // copy the whole table for metamod to know which functions we are using here
      memcpy (pFunctionTable, &gFunctionTable, sizeof (DLL_FUNCTIONS));
      return (1); // alright
   }
#endif


void FakeClientCommand(edict_t *pBot, char *arg1, char *arg2, char *arg3)
{
	int length;
	
	memset(g_argv, 0, sizeof(g_argv));
	
	isFakeClientCommand = 1;
	
	if ((arg1 == nullptr) || (*arg1 == 0))
		return;
	
	if ((arg2 == nullptr) || (*arg2 == 0))
	{
		length = sprintf(&g_argv[0], "%s", arg1);
		fake_arg_count = 1;
	}
	else if ((arg3 == nullptr) || (*arg3 == 0))
	{
		length = sprintf(&g_argv[0], "%s %s", arg1, arg2);
		fake_arg_count = 2;
	}
	else
	{
		length = sprintf(&g_argv[0], "%s %s %s", arg1, arg2, arg3);
		fake_arg_count = 3;
	}
	
	g_argv[length] = 0;  // null terminate just in case
	
	strcpy(&g_argv[64], arg1);
	
	if (arg2)
		strcpy(&g_argv[128], arg2);
	
	if (arg3)
		strcpy(&g_argv[192], arg3);
	
#ifndef METAMOD_BUILD
	// allow the MOD DLL to execute the ClientCommand...
	ClientCommand(pBot);
#else
	// allow the MOD DLL to execute the ClientCommand...
	MDLL_ClientCommand(pBot);
#endif

	isFakeClientCommand = 0;
}


void ProcessBotCfgFile()
{
	int ch;
	char cmd_line[256];
	int cmd_index;
	static char server_cmd[80];
	char *cmd, *arg1, *arg2, *arg3, *arg4, *arg5;
	char msg[80];
	
	if (bot_cfg_pause_time > gpGlobals->time)
		return;
	
	if (bot_cfg_fp == nullptr)
		return;
	
	cmd_index = 0;
	cmd_line[cmd_index] = 0;
	
	ch = fgetc(bot_cfg_fp);
	
	// skip any leading blanks
	while (ch == ' ')
		ch = fgetc(bot_cfg_fp);
	
	while ((ch != EOF) && (ch != '\r') && (ch != '\n'))
	{
		if (ch == '\t')  // convert tabs to spaces
			ch = ' ';
		
		cmd_line[cmd_index] = ch;
		
		ch = fgetc(bot_cfg_fp);
		
		// skip multiple spaces in input file
		while ((cmd_line[cmd_index] == ' ') &&
			(ch == ' '))      
			ch = fgetc(bot_cfg_fp);
		
		cmd_index++;
	}
	
	if (ch == '\r')  // is it a carriage return?
	{
		ch = fgetc(bot_cfg_fp);  // skip the linefeed
	}
	
	// if reached end of file, then close it
	if (ch == EOF)
	{
		fclose(bot_cfg_fp);
		
		bot_cfg_fp = nullptr;
		
		bot_cfg_pause_time = 0.0;
	}
	
	cmd_line[cmd_index] = 0;  // terminate the command line
	
	// copy the command line to a server command buffer...
	strcpy(server_cmd, cmd_line);
	strcat(server_cmd, "\n");
	
	cmd_index = 0;
	cmd = cmd_line;
	arg1 = arg2 = arg3 = arg4 = arg5 = nullptr;
	
	// skip to blank or end of string...
	while ((cmd_line[cmd_index] != ' ') && (cmd_line[cmd_index] != 0))
		cmd_index++;
	
	if (cmd_line[cmd_index] == ' ')
	{
		cmd_line[cmd_index++] = 0;
		arg1 = &cmd_line[cmd_index];
		
		// skip to blank or end of string...
		while ((cmd_line[cmd_index] != ' ') && (cmd_line[cmd_index] != 0))
			cmd_index++;
		
		if (cmd_line[cmd_index] == ' ')
		{
			cmd_line[cmd_index++] = 0;
			arg2 = &cmd_line[cmd_index];
			
			// skip to blank or end of string...
			while ((cmd_line[cmd_index] != ' ') && (cmd_line[cmd_index] != 0))
				cmd_index++;
			
			if (cmd_line[cmd_index] == ' ')
			{
				cmd_line[cmd_index++] = 0;
				arg3 = &cmd_line[cmd_index];
				
				// skip to blank or end of string...
				while ((cmd_line[cmd_index] != ' ') && (cmd_line[cmd_index] != 0))
					cmd_index++;
				
				if (cmd_line[cmd_index] == ' ')
				{
					cmd_line[cmd_index++] = 0;
					arg4 = &cmd_line[cmd_index];

					// skip to blank or end of string...
					while ((cmd_line[cmd_index] != ' ') && (cmd_line[cmd_index] != 0))
						cmd_index++;
					
					if (cmd_line[cmd_index] == ' ')
					{
						cmd_line[cmd_index++] = 0;
						arg5 = &cmd_line[cmd_index];
					}
				}
			}
		}
	}
	
	if ((cmd_line[0] == '#') || (cmd_line[0] == 0))
		return;  // return if comment or blank line

	if (ProcessCommand(nullptr, cmd, arg1, arg2, arg3, arg4, arg5))
		return;

	sprintf(msg, "executing server command: %s\n", server_cmd);
	SERVER_PRINT( msg );
	
	SERVER_PRINT(msg);
	
	SERVER_COMMAND(server_cmd);
}

bool ProcessCommand( edict_t *pEntity, const char *pcmd, const char *arg1, const char *arg2,
	const char *arg3, const char *arg4, const char *arg5)
{
	// only allow custom commands if deathmatch mode
	if (gpGlobals->deathmatch)
	{
		bot_weapon_select_t *pSelect = nullptr;
		pSelect = WeaponGetSelectPointer();
		char msg[80];
		
		if (debug_engine)
		{
			fp=fopen("bot.txt","a"); fprintf(fp,"ProcessCommand: %s",pcmd);
			if ((arg1 != nullptr) && (*arg1 != 0))
				fprintf(fp," %s", arg1);
			if ((arg2 != nullptr) && (*arg2 != 0))
				fprintf(fp," %s", arg2);
			if ((arg3 != nullptr) && (*arg3 != 0))
				fprintf(fp," %s", arg3);
			if ((arg4 != nullptr) && (*arg4 != 0))
				fprintf(fp," %s", arg4);
			if ((arg4 != nullptr) && (*arg5 != 0))
				fprintf(fp," %s", arg5);
			fprintf(fp, "\n");
			fclose(fp);
		}
		
		if (FStrEq(pcmd, "addbot"))
		{	
			// The arguments are out of line because I don't feel
			// like editing the BotCreate function. :P
			// HLDM skill name skin topcolor bottomcolor
			// SI skill team model name
			if (mod_id != SI_DLL) BotCreate( pEntity, arg3, arg2, arg1, arg4, arg5 );
			else BotCreate( pEntity, arg2, arg3, arg4, arg1, arg5 );

			bot_check_time = gpGlobals->time + 5.0;
			
			return TRUE;
		}
		else if (FStrEq(pcmd, "botcam"))
		{
			if ((arg1 != nullptr) && (*arg1 != 0) )
			{	// set our view to a bot
				for (int i = 1; i <= gpGlobals->maxClients; i++)
				{
					edict_t *pPlayer = INDEXENT(i);
					
					// skip invalid players and skip self
					if ((pPlayer) && (!pPlayer->free) && (pPlayer != pEntity) && (pPlayer->v.flags & FL_CLIENT))
					{
						if (!(pPlayer->v.flags & FL_FAKECLIENT))
							continue; // has to be a bot

						if (strcmp(STRING(pPlayer->v.netname), arg1) != 0)
							continue;

						edict_t *pCamera;

						pCamera = CREATE_NAMED_ENTITY(MAKE_STRING("info_target"));
						DispatchSpawn(pCamera);
						pCamera->v.origin = pPlayer->v.origin + pPlayer->v.view_ofs;
						pCamera->v.angles = pPlayer->v.v_angle;
						//pCamera->v.velocity = pPlayer->v.velocity;
						pCamera->v.takedamage = DAMAGE_NO;
						pCamera->v.solid = SOLID_NOT;
						pCamera->v.owner = pEntity;
						pCamera->v.euser1 = pPlayer;
						pCamera->v.movetype = MOVETYPE_FLY; //noclip
						pCamera->v.classname = MAKE_STRING("botcam");
						pCamera->v.nextthink = gpGlobals->time;
						pCamera->v.renderamt = 0;
						//pCamera->v.rendermode = kRenderTransColor;
						//pCamera->v.renderfx = kRenderFxNone;
   						SET_MODEL(pCamera, "models/mechgibs.mdl");
						//SET_MODEL(pCamera, "models/nail.mdl");
						SET_VIEW(pEntity, pCamera);
					}
				}
			}
			else
			{	// return our view
				edict_t *pCCamera = nullptr;
				while ((pCCamera = FIND_ENTITY_BY_CLASSNAME( pCCamera, "botcam" )) != nullptr &&
						  (!FNullEnt(pCCamera)))
				{
    				if(pCCamera->v.owner == pEntity)
					{
        				pCCamera->v.flags |= FL_KILLME;
					}
				}
				SET_VIEW(pEntity,pEntity);
			}
			
			return TRUE;
		}
		else if (FStrEq(pcmd, "botinfo"))
		{
			if ((arg1 != nullptr) && (*arg1 != 0))
			{
				sprintf(msg, "Couldn't find a bot by the name \"%s\"\n", arg1);

				for (int i = 1; i <= gpGlobals->maxClients; i++)
				{
					edict_t *pPlayer = INDEXENT(i);
					
					// skip invalid players and skip self
					if ((pPlayer) && (!pPlayer->free) && (pPlayer != pEntity) && (pPlayer->v.flags & FL_CLIENT))
					{
						if (!(pPlayer->v.flags & FL_FAKECLIENT))
							continue; // has to be a bot

						if (strcmp(STRING(pPlayer->v.netname), arg1) != 0)
							continue;

						bot_t *pBot = UTIL_GetBotPointer(pPlayer);
						if (pBot)
						{
							sprintf(msg, "\nBot Info for \"%s\"\n", pBot->name);
							SERVER_PRINT( msg);

							sprintf(msg, "Skin: %s\n", pBot->skin);
							SERVER_PRINT( msg);

							sprintf(msg, "Skill: %i\n\n", pBot->bot_skill+1);
							SERVER_PRINT( msg);

							sprintf(msg, "Health: %.0f\n", pPlayer->v.health);
							SERVER_PRINT( msg);

							sprintf(msg, "Armor: %.0f\n\n", pPlayer->v.armorvalue);
							SERVER_PRINT( msg);

							sprintf(msg, "Curr Wpt: %i\n", pBot->curr_waypoint_index);
							SERVER_PRINT( msg);

							sprintf(msg, "Goal Wpt: %i\n", pBot->waypoint_goal);
							SERVER_PRINT( msg);

							sprintf(msg, "Item Wpt: %i\n\n", pBot->item_waypoint);
							SERVER_PRINT( msg);

							sprintf(msg, "Item: %s (%.2f, %.2f, %.2f)\n\n", pBot->pBotPickupItem != nullptr ? STRING(pBot->pBotPickupItem->v.classname) : "None",
								pBot->pBotPickupItem != nullptr ? UTIL_GetOrigin(pBot->pBotPickupItem).x : 0,
								pBot->pBotPickupItem != nullptr ? UTIL_GetOrigin(pBot->pBotPickupItem).y : 0,
								pBot->pBotPickupItem != nullptr ? UTIL_GetOrigin(pBot->pBotPickupItem).z : 0);
							SERVER_PRINT( msg);

							sprintf(msg, "(Has it?) Weapon : Primary : Secondary\n");
							SERVER_PRINT( msg);

							int select_index = 0;
							while (pSelect[select_index].iId)
							{
								sprintf(msg, "(%s) %s : %.2f : %.2f\n",
									(pPlayer->v.weapons & (1<<pSelect[select_index].iId)) ? "Yes" : "No",
									pSelect[select_index].weapon_name, 
									BotAssessPrimaryAmmo(pBot, pSelect[select_index].iId),
									BotAssessSecondaryAmmo(pBot, pSelect[select_index].iId));
								SERVER_PRINT( msg);
								select_index++;
							}

							sprintf(msg, "\n");
							SERVER_PRINT( msg);
						}
					}
				}
				SERVER_PRINT( msg);
			}

			return TRUE;
		}
		else if (FStrEq(pcmd, "observer"))
		{
			if ((arg1 != nullptr) && (*arg1 != 0))
			{
				int temp = atoi(arg1);
				if (temp)
					b_observer_mode = TRUE;
				else
					b_observer_mode = FALSE;
			}
			
			if (b_observer_mode)
				SERVER_PRINT( "observer mode ENABLED\n");
			else
				SERVER_PRINT( "observer mode DISABLED\n");
			
			return TRUE;
		}
		else if (FStrEq(pcmd, "bot_chat_debug"))
		{
			if ((arg1 != nullptr) && (*arg1 != 0))
			{
				int temp = atoi(arg1);
				if (temp)
					b_chat_debug = TRUE;
				else
					b_chat_debug = FALSE;
			}
			
			if (b_chat_debug)
				SERVER_PRINT( "bot_chat_debug ENABLED\n");
			else
				SERVER_PRINT( "bot_chat_debug DISABLED\n");
			
			return TRUE;
		}
		else if (FStrEq(pcmd, "botskill"))
		{
			if ((arg1 != nullptr) && (*arg1 != 0))
			{
				int temp = atoi(arg1);
				
				if ((temp < 1) || (temp > 5))
					SERVER_PRINT( "invalid botskill value!\n");
				else
					default_bot_skill = temp;
			}
			
			sprintf(msg, "botskill is %d\n", default_bot_skill);
			SERVER_PRINT( msg);
			
			return TRUE;
		}
		else if (FStrEq(pcmd, "bot_speed"))
		{
			sprintf(msg, "bot_speed command failed\n");

			if ((arg1 != nullptr) && (*arg1 != 0) && (arg2 != nullptr) && (*arg2 != 0))
			{
				int skill = atoi(arg1);
				
				if ((skill < 1) || (skill > 5))
					SERVER_PRINT( "invalid skill specified value!\n");
				else
				{	// change their speed
					float speed = atof(arg2);
					speed_mod[skill-1] = speed;
					sprintf(msg, "bot_speed for skill %i is %f\n", skill, speed);
				}
			}
			
			SERVER_PRINT( msg);
			
			return TRUE;
		}
		else if (FStrEq(pcmd, "vector") && (pEntity != nullptr))
		{
			sprintf(msg, "Command failed!\n");

			if ((arg1 != nullptr) && (*arg1 != 0))
			{
				if (FStrEq(arg1, "set1"))
				{
					g_vecPoint1 = pEntity->v.origin;
					sprintf(msg, "Point1 set at %2.f, %2.f, %2.f!\n", g_vecPoint1.x, g_vecPoint1.y, g_vecPoint1.z);
				}
				else if (FStrEq(arg1, "set2"))
				{
					g_vecPoint2 = pEntity->v.origin;
					sprintf(msg, "Point2 set at %2.f, %2.f, %2.f!\n", g_vecPoint2.x, g_vecPoint2.y, g_vecPoint2.z);
				}
				else if (FStrEq(arg1, "clear1"))
				{
					g_vecPoint1 = g_vecZero;
					sprintf(msg, "Point1 cleared!\n");
				}
				else if (FStrEq(arg1, "clear2"))
				{
					g_vecPoint2 = g_vecZero;
					sprintf(msg, "Point2 cleared!\n");
				}
				else if (FStrEq(arg1, "distance"))
				{
					float distance = (g_vecPoint2 - g_vecPoint1).Length();
					sprintf(msg, "Distance is %.2f!\n", distance);
				}
				else if (FStrEq(arg1, "distance2D"))
				{
					float distance = (g_vecPoint2 - g_vecPoint1).Length2D();
					sprintf(msg, "2D Distance is %.2f!\n", distance);
				}
			}
			
			SERVER_PRINT( msg);
			
			return TRUE;
		}
		else if (FStrEq(pcmd, "pause"))
		{
			bot_cfg_pause_time = gpGlobals->time + atoi( arg1 );
			
			return TRUE;
		}
		else if (FStrEq(pcmd, "botweapon"))
		{	// this command allows editing of weapon information
			sprintf(msg, "Could not complete request!\n");

			if ((arg1 != nullptr) && (*arg1 != 0))
			{
				if ((arg2 != nullptr) && (*arg2 != 0))
				{
					bool found = FALSE;
					int select_index = 0;
					while (pSelect[select_index].iId)
					{
						if (strcmp(pSelect[select_index].weapon_name, arg1) == 0)
						{
							found = TRUE;
							break;
						}

						select_index++;
					}

					if (found)
					{
						if (strcmp(arg2, "priority") == 0)
						{	// INTEGER
							if ((arg3 != nullptr) && (*arg3 != 0))
							{
								int temp = atoi(arg3);
								pSelect[select_index].priority = temp;
								sprintf(msg, "%s for weapon %s is now %i\n", arg2, pSelect[select_index].weapon_name, temp);
							}
						}
						else if (strcmp(arg2, "skill") == 0)
						{	// INTEGER
							if ((arg3 != nullptr) && (*arg3 != 0))
							{
								int temp = atoi(arg3);
								pSelect[select_index].skill_level = temp;
								sprintf(msg, "%s for weapon %s is now %i\n", arg2, pSelect[select_index].weapon_name, temp);
							}
						}
						else if (strcmp(arg2, "primary_min_distance") == 0)
						{	// FLOAT
							if ((arg3 != nullptr) && (*arg3 != 0))
							{
								float temp = atof(arg3);
								pSelect[select_index].primary_min_distance = temp;
								sprintf(msg, "%s for weapon %s is now %f\n", arg2, pSelect[select_index].weapon_name, temp);
							}
						}
						else if (strcmp(arg2, "primary_max_distance") == 0)
						{	// FLOAT
							if ((arg3 != nullptr) && (*arg3 != 0))
							{
								float temp = atof(arg3);
								pSelect[select_index].primary_max_distance = temp;
								sprintf(msg, "%s for weapon %s is now %f\n", arg2, pSelect[select_index].weapon_name, temp);
							}
						}
						else if (strcmp(arg2, "secondary_min_distance") == 0)
						{	// FLOAT
							if ((arg3 != nullptr) && (*arg3 != 0))
							{
								float temp = atof(arg3);
								pSelect[select_index].secondary_min_distance = temp;
								sprintf(msg, "%s for weapon %s is now %f\n", arg2, pSelect[select_index].weapon_name, temp);
							}
						}
						else if (strcmp(arg2, "secondary_max_distance") == 0)
						{	// FLOAT
							if ((arg3 != nullptr) && (*arg3 != 0))
							{
								float temp = atof(arg3);
								pSelect[select_index].secondary_max_distance = temp;
								sprintf(msg, "%s for weapon %s is now %f\n", arg2, pSelect[select_index].weapon_name, temp);
							}
						}
						else if (strcmp(arg2, "can_use_underwater") == 0)
						{	// BOOLEAN
							if ((arg3 != nullptr) && (*arg3 != 0))
							{
								bool temp = atoi(arg3) ? TRUE : FALSE;
								pSelect[select_index].can_use_underwater = temp;
								sprintf(msg, "%s for weapon %s is now %s\n", arg2, pSelect[select_index].weapon_name, temp ? "true" : "false");
							}
						}
						else if (strcmp(arg2, "reload_delay") == 0)
						{	// FLOAT
							if ((arg3 != nullptr) && (*arg3 != 0))
							{
								float temp = atof(arg3);
								pSelect[select_index].reload_delay = temp;
								sprintf(msg, "%s for weapon %s is now %f\n", arg2, pSelect[select_index].weapon_name, temp);
							}
						}
						else if (strcmp(arg2, "primary_fire_percent") == 0)
						{	// INTEGER
							if ((arg3 != nullptr) && (*arg3 != 0))
							{
								int temp = atoi(arg3);
								pSelect[select_index].primary_fire_percent = temp;
								sprintf(msg, "%s for weapon %s is now %i\n", arg2, pSelect[select_index].weapon_name, temp);
							}
						}
						else if (strcmp(arg2, "min_primary_ammo") == 0)
						{	// INTEGER
							if ((arg3 != nullptr) && (*arg3 != 0))
							{
								int temp = atoi(arg3);
								pSelect[select_index].min_primary_ammo = temp;
								sprintf(msg, "%s for weapon %s is now %i\n", arg2, pSelect[select_index].weapon_name, temp);
							}
						}
						else if (strcmp(arg2, "min_secondary_ammo") == 0)
						{	// INTEGER
							if ((arg3 != nullptr) && (*arg3 != 0))
							{
								int temp = atoi(arg3);
								pSelect[select_index].min_secondary_ammo = temp;
								sprintf(msg, "%s for weapon %s is now %i\n", arg2, pSelect[select_index].weapon_name, temp);
							}
						}
						else if (strcmp(arg2, "primary_fire_hold") == 0)
						{	// BOOLEAN
							if ((arg3 != nullptr) && (*arg3 != 0))
							{
								bool temp = atoi(arg3) ? TRUE : FALSE;
								pSelect[select_index].primary_fire_hold = temp;
								sprintf(msg, "%s for weapon %s is now %s\n", arg2, pSelect[select_index].weapon_name, temp ? "true" : "false");
							}
						}
						else if (strcmp(arg2, "secondary_fire_hold") == 0)
						{	// BOOLEAN
							if ((arg3 != nullptr) && (*arg3 != 0))
							{
								bool temp = atoi(arg3) ? TRUE : FALSE;
								pSelect[select_index].secondary_fire_hold = temp;
								sprintf(msg, "%s for weapon %s is now %s\n", arg2, pSelect[select_index].weapon_name, temp ? "true" : "false");
							}
						}
						else if (strcmp(arg2, "primary_fire_charge") == 0)
						{	// BOOLEAN
							if ((arg3 != nullptr) && (*arg3 != 0))
							{
								bool temp = atoi(arg3) ? TRUE : FALSE;
								pSelect[select_index].primary_fire_charge = temp;
								sprintf(msg, "%s for weapon %s is now %s\n", arg2, pSelect[select_index].weapon_name, temp ? "true" : "false");
							}
						}
						else if (strcmp(arg2, "secondary_fire_charge") == 0)
						{	// BOOLEAN
							if ((arg3 != nullptr) && (*arg3 != 0))
							{
								bool temp = atoi(arg3) ? TRUE : FALSE;
								pSelect[select_index].secondary_fire_charge = temp;
								sprintf(msg, "%s for weapon %s is now %s\n", arg2, pSelect[select_index].weapon_name, temp ? "true" : "false");
							}
						}
						else if (strcmp(arg2, "primary_charge_delay") == 0)
						{	// FLOAT
							if ((arg3 != nullptr) && (*arg3 != 0))
							{
								float temp = atof(arg3);
								pSelect[select_index].primary_charge_delay = temp;
								sprintf(msg, "%s for weapon %s is now %f\n", arg2, pSelect[select_index].weapon_name, temp);
							}
						}
						else if (strcmp(arg2, "secondary_charge_delay") == 0)
						{	// FLOAT
							if ((arg3 != nullptr) && (*arg3 != 0))
							{
								float temp = atof(arg3);
								pSelect[select_index].secondary_charge_delay = temp;
								sprintf(msg, "%s for weapon %s is now %f\n", arg2, pSelect[select_index].weapon_name, temp);
							}
						}
					}
				}
			}
			
			SERVER_PRINT( msg);
			
			return TRUE;
		}
		else if (FStrEq(pcmd, "bot_reaction_time"))
		{
			if ((arg1 != nullptr) && (*arg1 != 0))
			{
				float temp = atof(arg1);
				
				if (temp < 0)
					SERVER_PRINT( "invalid bot_reaction_time value!\n");
				else
					bot_reaction_time = temp;
			}
			
			if (bot_reaction_time > 0)
				sprintf(msg, "bot_reaction_time is %f\n", bot_reaction_time);
			else
				sprintf(msg, "bot_reaction_time is DISABLED\n");
			
			SERVER_PRINT( msg);
			
			return TRUE;
		}
		else if (FStrEq(pcmd, "bot_reaction_time_min"))
		{
			sprintf(msg, "Command failed!\n");

			if ((arg1 != nullptr) && (*arg1 != 0) && (arg2 != nullptr) && (*arg2 != 0))
			{
				int skill = atoi(arg1);
				float time = atof(arg2);
				
				if (skill < 1 || skill > 5)
					SERVER_PRINT( "invalid bot_reaction_time_min value!\n");
				else
				{
					sprintf(msg, "bot_reaction_time_min for skill %i is %f\n", skill, time);
					react_time_min[skill-1] = time;
				}
			}
			
			SERVER_PRINT( msg);
			
			return TRUE;
		}
		else if (FStrEq(pcmd, "bot_reaction_time_max"))
		{
			sprintf(msg, "Command failed!\n");

			if ((arg1 != nullptr) && (*arg1 != 0) && (arg2 != nullptr) && (*arg2 != 0))
			{
				int skill = atoi(arg1);
				float time = atof(arg2);
				
				if (skill < 1 || skill > 5)
					SERVER_PRINT( "invalid bot_reaction_time_max value!\n");
				else
				{
					sprintf(msg, "bot_reaction_time_max for skill %i is %f\n", skill, time);
					react_time_max[skill-1] = time;
				}
			}
			
			SERVER_PRINT( msg);
			
			return TRUE;
		}
		else if (FStrEq(pcmd, "bot_random_color"))
		{
			if ((arg1 != nullptr) && (*arg1 != 0))
			{
				int temp = atoi(arg1);
				
				if (temp)
					b_random_color = TRUE;
				else
					b_random_color = FALSE;
			}
			
			if (b_random_color)
				SERVER_PRINT( "random_color ENABLED\n");
			else
				SERVER_PRINT( "random_color DISABLED\n");
			
			return TRUE;
		}
		else if (FStrEq(pcmd, "getteam"))
		{
			sprintf(msg, "Command failed!\n");

			if ((arg1 != nullptr) && (*arg1 != 0))
			{
				sprintf(msg, "Couldn't find a player by the name \"%s\"\n", arg1);

				for (int i = 1; i <= gpGlobals->maxClients; i++)
				{
					edict_t *pPlayer = INDEXENT(i);
					
					// skip invalid players
					if ((pPlayer) && (!pPlayer->free) && (pPlayer->v.flags & FL_CLIENT))
					{
						if (strcmp(STRING(pPlayer->v.netname), arg1) != 0)
							continue;

						int team = UTIL_GetTeam(pPlayer);

						sprintf(msg, "%s team is %i\n", STRING(pPlayer->v.netname), team);
					}
				}
				SERVER_PRINT( msg);
			}
			
			return TRUE;
		}
		else if (FStrEq(pcmd, "onteam"))
		{
			sprintf(msg, "Command failed!\n");

			if ((arg1 != nullptr) && (*arg1 != 0))
			{
				int team = atoi(arg1);
				int players = UTIL_PlayersOnTeam(team);

				sprintf(msg, "There are %i player(s) on team %i\n", players, team);
				SERVER_PRINT( msg);
			}
			
			return TRUE;
		}
		else if (FStrEq(pcmd, "botdontshoot"))
		{
			if ((arg1 != nullptr) && (*arg1 != 0))
			{
				int temp = atoi(arg1);
				if (temp)
					b_botdontshoot = TRUE;
				else
					b_botdontshoot = FALSE;
			}
			
			if (b_botdontshoot)
				SERVER_PRINT( "botdontshoot ENABLED\n");
			else
				SERVER_PRINT( "botdontshoot DISABLED\n");
			
			return TRUE;
		}
		else if (FStrEq(pcmd, "botrole"))
		{
			sprintf(msg, "Bot (Team) - (Role) : (Subrole)\n");
			SERVER_PRINT( msg);

			for (int i = 1; i <= gpGlobals->maxClients; i++)
			{
				edict_t *pPlayer = INDEXENT(i);
				
				// skip invalid players
				if ((pPlayer) && (!pPlayer->free) && (pPlayer->v.flags & FL_CLIENT))
				{
					if (!(pPlayer->v.flags & FL_FAKECLIENT))
						continue; // has to be a bot
					
					// was a name specified?
					if ((arg1 != nullptr) && (*arg1 != 0) && (strcmp(STRING(pPlayer->v.netname), arg1) != 0))
						continue;

					bot_t *pBot = UTIL_GetBotPointer(pPlayer);
					if (pBot)
					{
						sprintf(msg, "%s (%s) - (%s) : (%s) : (%s | %s)\n", pBot->name,
							pPlayer->v.team ? "AFD" : "MCL",
							RoleToString(pBot->role),
							SubroleToString(pBot->subrole),
							(pBot->pGoalEnt) ? ((pBot->pGoalEnt->v.netname) ? STRING(pBot->pGoalEnt->v.netname) : "?") : "None",
							(pBot->pGoalEnt) ? ((pBot->pGoalEnt->v.classname) ? STRING(pBot->pGoalEnt->v.classname) : "?") : "None");
						SERVER_PRINT( msg);
					}
				}
			}
			
			return TRUE;
		}
		else if (FStrEq(pcmd, "bot_si_defend"))
		{
			if ((arg1 != nullptr) && (*arg1 != 0) &&
				(arg2 != nullptr) && (*arg2 != 0))
			{	// manually set this teams defend percent
				if (atoi(arg1) >= 1 && atoi(arg1) <= 2)
					g_fl_si_defend[atoi(arg1)-1] = atof(arg2) / 100;
			}
			else
			{	// print the values
				for (int i = 0; i < 2; i++)
				{
					sprintf(msg, "Defend Percent for %s = %.2f\n", i ? "AFD" : "MCL", g_fl_si_defend[i]);
					SERVER_PRINT( msg);
				}
			}
		}
		else if (FStrEq(pcmd, "kickall"))
		{	// kick all bots!
			for (int i = 1; i <= gpGlobals->maxClients; i++)
			{
				edict_t *pPlayer = INDEXENT(i);
					
				// skip invalid players
				if ((pPlayer) && (!pPlayer->free) && (pPlayer->v.flags & FL_CLIENT) &&
					(pPlayer->v.flags & FL_FAKECLIENT))
				{
					sprintf(msg, "kick %s\n", STRING(pPlayer->v.netname));
					SERVER_COMMAND(msg);
				}
			}
			
			return TRUE;
		}
		else if (FStrEq(pcmd, "debug_engine"))
		{
			if ((arg1 != nullptr) && (*arg1 != 0))
				debug_engine = atoi(arg1);
			
			time_t rawtime;
			struct tm *timeinfo;
				
			time( &rawtime );
			timeinfo = localtime( &rawtime );

			if (debug_engine)
			{
				fp = fopen("bot.txt","a");
				fprintf(fp, "\n\nDebug Engine started on %s...\n\n", asctime(timeinfo));
				fclose(fp);
				SERVER_PRINT( "debug_engine ENABLED!\n");
			}
			else
			{
				fp = fopen("bot.txt","a");
				fprintf(fp, "\n\nDebug Engine stopped on %s...\n\n", asctime(timeinfo));
				fclose(fp);
				SERVER_PRINT( "debug_engine DISABLED!\n");
			}
			
			return TRUE;
		}
		else if (FStrEq(pcmd, "waypoint") && (pEntity != nullptr))
		{
			if (FStrEq(arg1, "on"))
			{
				g_waypoint_on = TRUE;
				
				SERVER_PRINT( "waypoints are ON\n");
			}
			else if (FStrEq(arg1, "off"))
			{
				g_waypoint_on = FALSE;
				
				SERVER_PRINT( "waypoints are OFF\n");
			}
			else if (FStrEq(arg1, "add"))
			{
				if (!g_waypoint_on)
					g_waypoint_on = TRUE;  // turn waypoints on if off
				
				WaypointAdd(pEntity);
			}
			else if (FStrEq(arg1, "delete"))
			{
				if (!g_waypoint_on)
					g_waypoint_on = TRUE;  // turn waypoints on if off
				
				WaypointDelete(pEntity);
			}
			else if (FStrEq(arg1, "save"))
			{
				WaypointSave();
				
				SERVER_PRINT( "waypoints saved\n");
			}
			else if (FStrEq(arg1, "load"))
			{
				if (WaypointLoad(pEntity, ".gbw"))
					SERVER_PRINT( "waypoints loaded\n");
			}
			else if (FStrEq(arg1, "distance"))
			{
				sprintf(msg, "Command failed!\n");

				if ((arg2 != nullptr) && (*arg2 != 0) && (arg3 != nullptr) && (*arg3 != 0) &&
					(arg4 != nullptr) && (*arg4 != 0))
				{
					int src = atoi(arg2);
					int dest = atoi(arg3);
					int team = atoi(arg4);

					int distance = WaypointDistanceFromTo(atoi(arg2), atoi(arg3), atoi(arg4));
					sprintf(msg, "Distance from %i to %i (team %i) via matrix is %i\n",
						src, dest, team, distance);
				}

				SERVER_PRINT( msg);
			}
			else if (FStrEq(arg1, "menu"))
			{
				int index;
				
				if (num_waypoints < 1)
					return TRUE;
				
				index = WaypointFindNearest(pEntity, 50.0, -1);
				
				if (index == -1)
					return TRUE;
				
				g_menu_waypoint = index;
				g_menu_state = MENU_1;
				
				UTIL_ShowMenu(pEntity, 0x1F, -1, FALSE, show_menu_1);
			}
			else if (FStrEq(arg1, "clearflag"))
			{	// remove this waypoint's attributes (keep origin)
				int index;
				
				if (num_waypoints < 1)
					return TRUE;
				
				index = WaypointFindNearest(pEntity, 50.0, -1);
				
				if (index == -1)
					return TRUE;
				
				if (FStrEq(arg2, "team"))
				{	// clear team
					waypoints[index].flags &= ~W_FL_TEAM;
					waypoints[index].flags &= ~W_FL_TEAM_SPECIFIC; // on
				}
				else if (FStrEq(arg2, "jump"))
					waypoints[index].flags &= ~W_FL_JUMP;
				else if (FStrEq(arg2, "crouch"))	// crouch
					waypoints[index].flags &= ~W_FL_CROUCH;
				else if (FStrEq(arg2, "duckjump"))	// duck jump
					waypoints[index].flags &= ~W_FL_DUCKJUMP;
				else if (FStrEq(arg2, "ladder"))	// ladder
					waypoints[index].flags &= ~W_FL_LADDER;
				else if (FStrEq(arg2, "lift"))	// lift
					waypoints[index].flags &= ~W_FL_LIFT;
				else if (FStrEq(arg2, "door"))	// door
					waypoints[index].flags &= ~W_FL_DOOR;
				else if (FStrEq(arg2, "health"))	// health
					waypoints[index].flags &= ~W_FL_HEALTH;
				else if (FStrEq(arg2, "armor"))	// armor
					waypoints[index].flags &= ~W_FL_ARMOR;
				else if (FStrEq(arg2, "sniper"))	// sniper spot
					waypoints[index].flags &= ~W_FL_SNIPER;
				else if (FStrEq(arg2, "weapon"))	// weapon
					waypoints[index].flags &= ~W_FL_WEAPON;
				else if (FStrEq(arg2, "ammo"))	// ammo
					waypoints[index].flags &= ~W_FL_AMMO;
				else if (FStrEq(arg2, "item"))	// item
					waypoints[index].flags &= ~W_FL_ITEM;
				else if (FStrEq(arg2, "defend"))	// defend
					waypoints[index].flags &= ~W_FL_DEFEND;
				else if (FStrEq(arg2, "aim"))
				{	// aiming index
					int aim_index = WaypointFindNearestAiming(waypoints[index].origin);
					if (aim_index > -1 && aim_index < num_waypoints)
					{
						waypoints[aim_index].flags = W_FL_DELETED;
						waypoints[aim_index].origin = g_vecZero;
						wp_display_time[aim_index] = 0.0;
					}
				}
				else if (FStrEq(arg2, "itemname"))
					strcpy(waypoints[index].item, "");
				else
				{	// clear everything
					waypoints[index].flags = 0;
					strcpy(waypoints[index].item, "\0");
				}
			}
			else if (FStrEq(arg1, "setflag"))
			{	// set this waypoint's attributes
				int index;
				
				if (num_waypoints < 1)
					return TRUE;
				
				index = WaypointFindNearest(pEntity, 50.0, -1);
				
				if (index == -1)
					return TRUE;
				
				if (FStrEq(arg2, "team"))
				{
					int team = atoi(arg3);
					
					team--;  // make 0 to 3
					
					// this is kind of a kludge (team bits MUST be LSB!!!)
					waypoints[index].flags |= team;
					waypoints[index].flags |= W_FL_TEAM_SPECIFIC; // on
				}
				else if (FStrEq(arg2, "itemname"))	// itemname
					strcpy(waypoints[index].item, arg3);
				else if (FStrEq(arg2, "jump"))	// jump
					waypoints[index].flags |= W_FL_JUMP;
				else if (FStrEq(arg2, "crouch"))	// crouch
					waypoints[index].flags |= W_FL_CROUCH;
				else if (FStrEq(arg2, "duckjump"))	// duck jump
					waypoints[index].flags |= W_FL_DUCKJUMP;
				else if (FStrEq(arg2, "ladder"))	// ladder
					waypoints[index].flags |= W_FL_LADDER;
				else if (FStrEq(arg2, "lift"))	// lift
					waypoints[index].flags |= W_FL_LIFT;
				else if (FStrEq(arg2, "door"))	// door
					waypoints[index].flags |= W_FL_DOOR;
				else if (FStrEq(arg2, "health"))	// health
					waypoints[index].flags |= W_FL_HEALTH;
				else if (FStrEq(arg2, "armor"))	// armor
					waypoints[index].flags |= W_FL_ARMOR;
				else if (FStrEq(arg2, "sniper"))	// sniper spot
					waypoints[index].flags |= W_FL_SNIPER;
				else if (FStrEq(arg2, "weapon"))	// weapon
					waypoints[index].flags |= W_FL_WEAPON;
				else if (FStrEq(arg2, "ammo"))	// ammo
					waypoints[index].flags |= W_FL_AMMO;
				else if (FStrEq(arg2, "item"))	// item
					waypoints[index].flags |= W_FL_ITEM;
				else if (FStrEq(arg2, "defend"))
					waypoints[index].flags |= W_FL_DEFEND; // on
				else if (FStrEq(arg2, "update"))
					WaypointSearchItems(pEntity, waypoints[index].origin, index);
			}
			else if (FStrEq(arg1, "info"))
			{
				WaypointPrintInfo(pEntity);
			}
			else if (FStrEq(arg1, "update"))
			{
				for (int index=0; index < num_waypoints; index++)
				{
					if (waypoints[index].flags & W_FL_DELETED)
						continue;  // skip any deleted waypoints
					
					if (waypoints[index].flags & W_FL_AIMING)
						continue;  // skip any aiming waypoints
					
					WaypointSearchItems(pEntity, waypoints[index].origin, index);
				}
			}
			else
			{
				if (g_waypoint_on)
					SERVER_PRINT( "waypoints are ON\n");
				else
					SERVER_PRINT( "waypoints are OFF\n");
			}
			
			return TRUE;
		}
		else if (FStrEq(pcmd, "autowaypoint"))
		{
			if (FStrEq(arg1, "on"))
			{
				g_auto_waypoint = TRUE;
				g_waypoint_on = TRUE;  // turn this on just in case
			}
			else if (FStrEq(arg1, "off"))
			{
				g_auto_waypoint = FALSE;
			}
			
			if (g_auto_waypoint)
				sprintf(msg, "autowaypoint is ON\n");
			else
				sprintf(msg, "autowaypoint is OFF\n");
			
			SERVER_PRINT( msg);
			
			return TRUE;
		}
		else if (FStrEq(pcmd, "pathwaypoint"))
		{
			if (FStrEq(arg1, "on"))
			{
				g_path_waypoint = TRUE;
				g_waypoint_on = TRUE;  // turn this on just in case
				
				SERVER_PRINT( "pathwaypoint is ON\n");
			}
			else if (FStrEq(arg1, "off"))
			{
				g_path_waypoint = FALSE;
				
				SERVER_PRINT( "pathwaypoint is OFF\n");
			}
			else if (FStrEq(arg1, "create1"))
			{
				WaypointCreatePath(pEntity, 1);
			}
			else if (FStrEq(arg1, "create2"))
			{
				WaypointCreatePath(pEntity, 2);
			}
			else if (FStrEq(arg1, "remove1"))
			{
				WaypointRemovePath(pEntity, 1);
			}
			else if (FStrEq(arg1, "remove2"))
			{
				WaypointRemovePath(pEntity, 2);
			}
			
			return TRUE;
		}
		else if (FStrEq(pcmd, "menuselect") && (g_menu_state != MENU_NONE))
		{
			if (g_menu_state == MENU_1)  // main menu...
			{
				if (FStrEq(arg1, "1"))  // team specific...
				{
					g_menu_state = MENU_2;  // display team specific menu...
					
					UTIL_ShowMenu(pEntity, 0x1F, -1, FALSE, show_menu_2);
					
					return TRUE;
				}
				else if (FStrEq(arg1, "2"))  // wait for lift...
				{
					if (waypoints[g_menu_waypoint].flags & W_FL_LIFT)
						waypoints[g_menu_waypoint].flags &= ~W_FL_LIFT;  // off
					else
						waypoints[g_menu_waypoint].flags |= W_FL_LIFT;  // on
				}
				else if (FStrEq(arg1, "3"))  // door waypoint
				{
					if (waypoints[g_menu_waypoint].flags & W_FL_DOOR)
						waypoints[g_menu_waypoint].flags &= ~W_FL_DOOR;  // off
					else
						waypoints[g_menu_waypoint].flags |= W_FL_DOOR;  // on
				}
				else if (FStrEq(arg1, "4"))  // sniper spot
				{
					if (waypoints[g_menu_waypoint].flags & W_FL_SNIPER)
					{
						waypoints[g_menu_waypoint].flags &= ~W_FL_SNIPER;  // off
						// delete the aiming waypoint
						int aim_index = WaypointFindNearestAiming(waypoints[g_menu_waypoint].origin);
						if (aim_index > -1 && aim_index < num_waypoints)
						{
						waypoints[aim_index].flags = W_FL_DELETED;  // not being used
						waypoints[aim_index].origin = Vector(0,0,0);
						
						wp_display_time[aim_index] = 0.0;
						}
					}
					else
					{
						waypoints[g_menu_waypoint].flags |= W_FL_SNIPER;  // on
						
						// set the aiming waypoint...
						
						WaypointAddAiming(pEntity);
					}
				}
				else if (FStrEq(arg1, "5"))  // more...
				{
					g_menu_state = MENU_3;
					
					UTIL_ShowMenu(pEntity, 0x13, -1, FALSE, show_menu_3);
					
					return TRUE;
				}
			}
			else if (g_menu_state == MENU_2)  // team specific menu
			{
				if (waypoints[g_menu_waypoint].flags & W_FL_TEAM_SPECIFIC)
				{
					waypoints[g_menu_waypoint].flags &= ~W_FL_TEAM;
					waypoints[g_menu_waypoint].flags &= ~W_FL_TEAM_SPECIFIC; // off
				}
				else
				{
					int team = atoi(arg1);
					
					team--;  // make 0 to 3
					
					// this is kind of a kludge (team bits MUST be LSB!!!)
					waypoints[g_menu_waypoint].flags |= team;
					waypoints[g_menu_waypoint].flags |= W_FL_TEAM_SPECIFIC; // on
				}
			}
			else if (g_menu_state == MENU_3)  // second menu...
			{
				if (FStrEq(arg1, "1"))  // jump
				{
					if (waypoints[g_menu_waypoint].flags & W_FL_JUMP)
						waypoints[g_menu_waypoint].flags &= ~W_FL_JUMP;  // off
					else
						waypoints[g_menu_waypoint].flags |= W_FL_JUMP;  // on
				}
				else if (FStrEq(arg1, "2"))  // ladder
				{
					if (waypoints[g_menu_waypoint].flags & W_FL_LADDER)
						waypoints[g_menu_waypoint].flags &= ~W_FL_LADDER;  // off
					else
						waypoints[g_menu_waypoint].flags |= W_FL_LADDER;  // on
				}
				else if (FStrEq(arg1, "3"))  // weapon
				{
					if (waypoints[g_menu_waypoint].flags & W_FL_WEAPON)
					{
						waypoints[g_menu_waypoint].flags &= ~W_FL_WEAPON;  // off
						strcpy(waypoints[g_menu_waypoint].item, "\0");
					}
					else
					{
						waypoints[g_menu_waypoint].flags |= W_FL_WEAPON;  // on
						WaypointSearchItems(nullptr, waypoints[g_menu_waypoint].origin, g_menu_waypoint);
					}
				}
				else if (FStrEq(arg1, "4"))  // ammo
				{
					if (waypoints[g_menu_waypoint].flags & W_FL_AMMO)
					{
						waypoints[g_menu_waypoint].flags &= ~W_FL_AMMO;  // off
						strcpy(waypoints[g_menu_waypoint].item, "\0");
					}
					else
					{
						waypoints[g_menu_waypoint].flags |= W_FL_AMMO;  // on
						WaypointSearchItems(nullptr, waypoints[g_menu_waypoint].origin, g_menu_waypoint);
					}
				}
				else if (FStrEq(arg1, "5"))  // more...
				{
					g_menu_state = MENU_4;
					
					UTIL_ShowMenu(pEntity, 0x13, -1, FALSE, show_menu_4);
						
					return TRUE;
				}
			}
			else if (g_menu_state == MENU_4)  // third menu...
			{
				if (FStrEq(arg1, "1"))  // health
				{
					if (waypoints[g_menu_waypoint].flags & W_FL_HEALTH)
					{
						waypoints[g_menu_waypoint].flags &= ~W_FL_HEALTH;  // off
						strcpy(waypoints[g_menu_waypoint].item, "\0");
					}
					else
					{
						waypoints[g_menu_waypoint].flags |= W_FL_HEALTH;  // on
						WaypointSearchItems(nullptr, waypoints[g_menu_waypoint].origin, g_menu_waypoint);
					}
				}
				else if (FStrEq(arg1, "2"))  // armor
				{
					if (waypoints[g_menu_waypoint].flags & W_FL_ARMOR)
					{
						waypoints[g_menu_waypoint].flags &= ~W_FL_ARMOR;  // off
						strcpy(waypoints[g_menu_waypoint].item, "\0");
					}
					else
					{
						waypoints[g_menu_waypoint].flags |= W_FL_ARMOR;  // on
						WaypointSearchItems(nullptr, waypoints[g_menu_waypoint].origin, g_menu_waypoint);
					}
				}
				else if (FStrEq(arg1, "3"))  // long jump
				{
					if (waypoints[g_menu_waypoint].flags & W_FL_ITEM)
					{
						waypoints[g_menu_waypoint].flags &= ~W_FL_ITEM;  // off
						strcpy(waypoints[g_menu_waypoint].item, "\0");
					}
					else
					{
						waypoints[g_menu_waypoint].flags |= W_FL_ITEM;  // on
						WaypointSearchItems(nullptr, waypoints[g_menu_waypoint].origin, g_menu_waypoint);
					}
				}
				else if (FStrEq(arg1, "4"))  // duck jump
				{
					if (waypoints[g_menu_waypoint].flags & W_FL_DUCKJUMP)
					{
						waypoints[g_menu_waypoint].flags &= ~W_FL_DUCKJUMP;  // off
						strcpy(waypoints[g_menu_waypoint].item, "\0");
					}
					else
						waypoints[g_menu_waypoint].flags |= W_FL_DUCKJUMP;  // on
				}
			}
			
			g_menu_state = MENU_NONE;
			
			return TRUE;
		}
		else if (FStrEq(pcmd, "dump_research"))
		{
			FILE *pRF = fopen("si/dump_research.txt", "a");
			fprintf(pRF, "Dumping research information...\n");
			for (int t = 0; t < 2; t++)
			{
				fprintf(pRF, "\nTeam %s\n", t ? "AFD" : "MCL");
				for (int r = 0; r < NUM_RESEARCH_OPTIONS; r++)
				{
					fprintf(pRF, "%i %s %s %s %s\n", r,
						g_Researched[t][r].researched ? "r" : "-",
						g_Researched[t][r].canidate ? "c" : "-",
						g_Researched[t][r].stolen ? "s" : "-",
						g_Researched[t][r].disabled ? "d" : "-");
				}
			}
			fprintf(pRF, "\nDump complete, file closed...\n\n");
			fclose(pRF);

			return TRUE;
		}
		else if (FStrEq(pcmd, "search"))
		{
			edict_t *pent = nullptr;
			float radius = 50;
			if ((arg1 != nullptr) && (*arg1 != 0))
				radius = atof(arg1);
			char str[256];
			
			SERVER_PRINT( "searching...\n");
			
			while ((pent = UTIL_FindEntityInSphere( pent, pEntity->v.origin, radius )) != nullptr)
			{
				sprintf(str, "Found %s at %5.2f %5.2f %5.2f owned by %s following %s, model %s\n",
					STRING(pent->v.classname),
					pent->v.origin.x, pent->v.origin.y,
					pent->v.origin.z,
					(pent->v.owner != nullptr) ? STRING(pent->v.owner->v.netname) : "nobody",
					(pent->v.aiment != nullptr) ? STRING(pent->v.aiment->v.netname) : "nobody",
					STRING(pent->v.model));
				SERVER_PRINT( str);
				
				FILE *fp=fopen("bot.txt", "a");
				fprintf(fp, "ClientCommmand: search %s", str);
				fclose(fp);
			}
			return TRUE;
		}
	}

	return FALSE;
}

void RoleDetermine()
{
	// important entities (scis, resource, breakables)
	edict_t *pEntity = nullptr;
	int team = 0;
	// for looping
	int i = 0;

	int myteam = 0;
	int enemyteam = 1;

	float fTeamPercentScore[2] = 
		{ 0.5, 0.5};
	float fTeamPercentSci[2] = 
		{ 0.5, 0.5};
	
	int scitotal = 0;

	// sci count
	for (myteam = 0; myteam < 2; myteam++)
	{
		g_iSciCount[myteam] = 0;
		// find alive team scientists
		while ((pEntity = UTIL_FindEntityByClassname(pEntity, "monster_scientist")) != nullptr)
		{
			team = UTIL_GetTeam(pEntity);
			// skip enemy scientists
			if (team != myteam || team < 0 || team > 1)
				continue;

			// skip dead scientists
			if (!IsAlive(pEntity))
				continue;

			scitotal++;
			g_iSciCount[myteam]++;
		}
	}
	// loop through both teams and determine attack and defend percentages
	for (myteam = 0; myteam < 2; myteam++)
	{
		int plrtotal = UTIL_PlayersOnTeam(myteam);

		edict_t *pEntity = nullptr;

		if (myteam == 0) enemyteam = 1;
		else if (myteam == 1) enemyteam = 0;
		
		g_iDefendGoal[myteam] = g_iAttackGoal[myteam] = 0;
	
		// Score percent
		if (g_lTeamScore[enemyteam] > 0)
			fTeamPercentScore[myteam] = (float)g_lTeamScore[myteam] / (float)g_lTeamScore[enemyteam];
		else // forces everyone to attack if enemy is way ahead (mathematically can't happen!)
			fTeamPercentScore[myteam] = (float)g_lTeamScore[myteam];
		// Sci percent
		if (g_iSciCount[enemyteam] > 0)
			fTeamPercentSci[myteam] = (float)g_iSciCount[myteam] / (float)g_iSciCount[enemyteam];
		else // forces everyone to attack if enemy has all scis
			fTeamPercentSci[myteam] = (float)g_iSciCount[myteam];
		
		if (g_fl_si_defend[myteam] < 0)
		{
			g_flDefend[myteam] = 0.5 * fTeamPercentScore[myteam] * 
				(fTeamPercentSci[myteam] * (fTeamPercentSci[myteam] == 1.0) ? 
				1.0 : ((float)g_iSciCount[myteam] / ((scitotal != 0) ? scitotal : 1)));
		}
		else
			g_flDefend[myteam] = g_fl_si_defend[myteam];

		// can't have less than 0% defending
		if (g_flDefend[myteam] < 0)
			g_flDefend[myteam] = 0;
		// can't have more than 100% defending
		else if (g_flDefend[myteam] > 1)
			g_flDefend[myteam] = 1;

		g_flAttack[myteam] = 1.0 - g_flDefend[myteam];

		// can't have less than 0% attacking
		if (g_flAttack[myteam] < 0)
			g_flAttack[myteam] = 0;
		// can't have more than 100% attacking
		else if (g_flAttack[myteam] > 1)
			g_flAttack[myteam] = 1;
/*
		// FOR TESTING
		g_flDefend[myteam] = 1.0;
		g_flAttack[myteam] = 0.0;
*/
		// only calculate if not everyone is defending and not everyone is attacking
		if (g_flDefend[myteam] < 1.0 && g_flDefend[myteam] > 0.0)
		{
			float plrworth = 1/(float)plrtotal;
			for (i = 1; i <= plrtotal; i++)
			{	// this player above the defend percent?
				if (i * plrworth > g_flDefend[myteam])
					break;

				g_iDefendGoal[myteam]++;
			}
		}
		else if (g_flDefend[myteam] >= 1.0)	// everyone defend!
			g_iDefendGoal[myteam] = plrtotal;
		else if (g_flDefend[myteam] <= 0.0)	// everyone attack!
			g_iDefendGoal[myteam] = 0;

		// attack amount based off defense amount
		g_iAttackGoal[myteam] = plrtotal - g_iDefendGoal[myteam];
		/*
		// display role percents
		SERVER_PRINT( "Role percents for %s (%d:%i/%i): %i(%.2f) attack and %i(%.2f) defend! Score: %.3f - Sci: %.3f\n",
			myteam ? "AFD" : "MCL", g_lTeamScore[myteam], g_iSciCount[myteam], scitotal, 
			g_iAttackGoal[myteam], g_flAttack[myteam], g_iDefendGoal[myteam], g_flDefend[myteam],
			fTeamPercentScore[myteam], fTeamPercentSci[myteam]);
		*/
	}
}

void RoleCount()
{
	// ally players
	edict_t *pPlayer = nullptr;
	// important entities (scis, resource, breakables)
	edict_t *pEntity = nullptr;
	// closest important entity
	edict_t *pClosest = nullptr;

	int t = 0;
	int team = -1;
	int temp_index = -1;
	// for looping and finding the closest entities
	int i = 0;
	float distance;
	float mindistance = 9999;

	for (t = 0; t < 2; t++)
	{
		int teamtotal = UTIL_PlayersOnTeam(t);
		g_iDefendCount[t] = g_iAttackCount[t] = 0;
		// loop through all players on the same team
		for (i = 1; i <= gpGlobals->maxClients; i++)
		{
			pClosest = nullptr;
			pEntity = nullptr;
			distance = 0;
			mindistance = 9999;
			
			pPlayer = INDEXENT(i);
				
			if (pPlayer && !pPlayer->free && UTIL_GetTeam(pPlayer) == t &&
				pPlayer->v.flags & FL_CLIENT)
			{	
				team = UTIL_GetTeam(pPlayer);

				if (!(pPlayer->v.flags & FL_FAKECLIENT))
				{	// HUMAN PLAYERS
					/*
					EXPLAINATION: This code loops through all important entities (scis, resources, breakables)
					in a 512 radius sphere and finds the closest one.  The human player's role is then
					determined based off the team of the close entity.
					
					Examples:	Player is by a carry_scientist on their team, they must be defending it!
								Player is by a item_resource on the enemy team, they must be attacking
								the enemy to get the resource!
					*/
					temp_index = WaypointFindNearest(pPlayer, 512, team);
					
					while ((pEntity = UTIL_FindEntityInSphere( pEntity, pPlayer->v.origin, 512 )) != nullptr)
					{	// only consider goal entities (scis, resources, breakables)
						if ((strcmp("monster_scientist", STRING(pEntity->v.classname)) != 0) &&
							(strcmp("carry_scientist", STRING(pEntity->v.classname)) != 0) &&
							(strcmp("carry_resource", STRING(pEntity->v.classname)) != 0) &&
							(strcmp("item_resource", STRING(pEntity->v.classname)) != 0) &&
							(strcmp("func_tech_breakable", STRING(pEntity->v.classname)) != 0))
							continue;
						
						int ent_wpt_index = WaypointFindNearest(pEntity, 512, team);
						// valid nearby waypoint?
						if (ent_wpt_index != -1)
							distance = WaypointDistanceFromTo(temp_index, ent_wpt_index, team);
						// add in distance from waypoint to entity
						distance += (waypoints[temp_index].origin - pEntity->v.origin).Length();
						if (distance < mindistance)
						{
							mindistance = distance;
							// remember this entity
							pClosest = pEntity;
						}
					}

					// we found something!
					if (pClosest != nullptr)
					{	// same team? assume they're defending
						if (team == UTIL_GetTeam(pClosest))
							g_iDefendCount[t]++;
						else	// otherwise attacking
							g_iAttackCount[t]++;
					}
					else // nothing by this player, assume attacking
						g_iAttackCount[t]++;
				}
				else if (pPlayer->v.flags & FL_FAKECLIENT)
				{	// FAKE CLIENTS (BOTS)
					bot_t *pAllyBot = UTIL_GetBotPointer(pPlayer);

					if (pAllyBot != nullptr)
					{
						if (pAllyBot->role == ROLE_ATTACK)
							g_iAttackCount[t]++;
						else if (pAllyBot->role == ROLE_DEFEND)
							g_iDefendCount[t]++;
					}
				}
			}
		}

		// can't have negative amount of attackers/defenders
		if (g_iAttackCount[t] < 0)
			g_iAttackCount[t] = 0;
		if (g_iDefendCount[t] < 0)
			g_iDefendCount[t] = 0;
		// can't have more attackers/defenders than players on the team!
		if (g_iAttackCount[t] > teamtotal)
			g_iAttackCount[t] = teamtotal;
		if (g_iDefendCount[t] > teamtotal)
			g_iDefendCount[t] = teamtotal;
		/*
		SERVER_PRINT( "Found %i defenders and %i attackers for team %s...\n",
			g_iDefendCount[t],  g_iAttackCount[t], team ? "AFD" : "MCL");
		*/
	}
}

char *RoleToString(int role)
{
	if (role == ROLE_ATTACK)
		return "Attack";
	else if (role == ROLE_DEFEND)
		return "Defend";

	return "?";
}

char *SubroleToString(int subrole)
{
	if (subrole == ROLE_SUB_ATT_GET_SCI)
		return "Get Scientists";
	else if (subrole == ROLE_SUB_ATT_RTRN_SCI)
		return "Return Scientist";
	else if (subrole == ROLE_SUB_ATT_KILL_SCI)
		return "Kill Scientists";
	else if (subrole == ROLE_SUB_ATT_GET_RSRC)
		return "Get Resources";
	else if (subrole == ROLE_SUB_ATT_RTRN_RSRC)
		return "Return Resource";
	else if (subrole == ROLE_SUB_ATT_BREAK)
		return "Destroy Tech Breakables";
	else if (subrole == ROLE_SUB_DEF_SCIS)
		return "Defend Scientists";
	else if (subrole == ROLE_SUB_DEF_BASE)
		return "Defend Base";
	else if (subrole == ROLE_SUB_DEF_RSRC)
		return "Defend Resources";
	else if (subrole == ROLE_SUB_DEF_BREAK)
		return "Defend Tech Breakables";
	else if (subrole == ROLE_SUB_DEF_ALLY)
		return "Defend Ally";
	
	return "?";
}

void ClearEdict(edict_t *pEdict)
{
	if (!pEdict)
		return;

	int i = 0;

	pEdict->v.classname = 0;
	pEdict->v.globalname = 0;

	pEdict->v.origin = Vector(0,0,0);
	pEdict->v.oldorigin = Vector(0,0,0);
	pEdict->v.velocity = Vector(0,0,0);
	pEdict->v.basevelocity = Vector(0,0,0);
	pEdict->v.clbasevelocity = Vector(0,0,0);  // Base velocity that was passed in to server physics so 
							     //  client can predict conveyors correctly.  Server zeroes it, so we need to store here, too.
	pEdict->v.movedir = Vector(0,0,0);

	pEdict->v.angles = Vector(0,0,0);			// Model angles
	pEdict->v.avelocity = Vector(0,0,0);		// angle velocity (degrees per second)
	pEdict->v.punchangle = Vector(0,0,0);		// auto-decaying view angle adjustment
	pEdict->v.v_angle = Vector(0,0,0);		// Viewing angle (player only)

	// For parametric entities
	pEdict->v.endpos = Vector(0,0,0);
	pEdict->v.startpos = Vector(0,0,0);
	pEdict->v.impacttime = 0.0;
	pEdict->v.starttime = 0.0;

	pEdict->v.fixangle = 0;		// 0:nothing, 1:force view angles, 2:add avelocity
	pEdict->v.idealpitch = 0.0;
	pEdict->v.pitch_speed = 0.0;
	pEdict->v.ideal_yaw = 0.0;
	pEdict->v.yaw_speed = 0.0;

	pEdict->v.modelindex = 0;
	pEdict->v.model = 0;

	pEdict->v.viewmodel = 0;		// player's viewmodel
	pEdict->v.weaponmodel = 0;	// what other players see
	
	pEdict->v.absmin = Vector(0,0,0);		// BB max translated to world coord
	pEdict->v.absmax = Vector(0,0,0);		// BB max translated to world coord
	pEdict->v.mins = Vector(0,0,0);		// local BB min
	pEdict->v.maxs = Vector(0,0,0);		// local BB max
	pEdict->v.size = Vector(0,0,0);		// maxs - mins

	pEdict->v.ltime = 0.0;
	pEdict->v.nextthink = 0.0;

	pEdict->v.movetype = 0;
	pEdict->v.solid = 0;

	pEdict->v.skin = 0;			
	pEdict->v.body = 0;			// sub-model selection for studiomodels
	pEdict->v.effects = 0;
	
	pEdict->v.gravity = 0.0;		// % of "normal" gravity
	pEdict->v.friction = 0.0;		// inverse elasticity of MOVETYPE_BOUNCE
	
	pEdict->v.light_level = 0;

	pEdict->v.sequence;		// animation sequence
	pEdict->v.gaitsequence;	// movement animation sequence for player (0 for none)
	pEdict->v.frame = 0.0;			// % playback position in animation sequences (0..255)
	pEdict->v.animtime = 0.0;		// world time when frame was set
	pEdict->v.framerate = 0.0;		// animation playback rate (-8x to 8x)
	for (i = 0; i < 4; i++)
		pEdict->v.controller[i] = 0;	// bone controller setting (0..255)
	for (i = 0; i < 2; i++)
		pEdict->v.blending[i] = 0;	// blending amount between sub-sequences (0..255)

	pEdict->v.scale = 0.0;			// sprite rendering scale (0..255)

	pEdict->v.rendermode = 0;
	pEdict->v.renderamt = 0.0;
	pEdict->v.rendercolor = Vector(0,0,0);
	pEdict->v.renderfx = 0;

	pEdict->v.health = 0.0;
	pEdict->v.frags = 0.0;
	pEdict->v.weapons = 0;  // bit mask for available weapons
	pEdict->v.takedamage = 0.0;

	pEdict->v.deadflag = 0;
	pEdict->v.view_ofs = Vector(0,0,0);	// eye position

	pEdict->v.button = 0;
	pEdict->v.impulse = 0;

	pEdict->v.chain = nullptr;			// Entity pointer when linked into a linked list
	pEdict->v.dmg_inflictor = nullptr;
	pEdict->v.enemy = nullptr;
	pEdict->v.aiment = nullptr;		// entity pointer when MOVETYPE_FOLLOW
	pEdict->v.owner = nullptr;
	pEdict->v.groundentity = nullptr;

	pEdict->v.spawnflags = 0;
	pEdict->v.flags = 0;
	
	pEdict->v.colormap = 0;		// lowbyte topcolor, highbyte bottomcolor
	pEdict->v.team = 0;

	pEdict->v.max_health = 0.0;
	pEdict->v.teleport_time = 0.0;
	pEdict->v.armortype = 0.0;
	pEdict->v.armorvalue = 0.0;
	pEdict->v.waterlevel = 0;
	pEdict->v.watertype = 0;
	
	pEdict->v.target = 0;
	pEdict->v.targetname = 0;
	// disconnect needs this?
	//pEdict->v.netname = 0;
	pEdict->v.message = 0;

	pEdict->v.dmg_take = 0.0;
	pEdict->v.dmg_save = 0.0;
	pEdict->v.dmg = 0.0;
	pEdict->v.dmgtime = 0.0;
	
	pEdict->v.noise = 0;
	pEdict->v.noise1 = 0;
	pEdict->v.noise2 = 0;
	pEdict->v.noise3 = 0;
	
	pEdict->v.speed = 0.0;
	pEdict->v.air_finished = 0.0;
	pEdict->v.pain_finished = 0.0;
	pEdict->v.radsuit_finished = 0.0;
	// disconnect needs this?
	//pEdict->v.pContainingEntity = NULL;

	pEdict->v.playerclass = 0;
	pEdict->v.maxspeed = 0.0;

	pEdict->v.fov = 0.0;
	pEdict->v.weaponanim = 0;

	pEdict->v.pushmsec = 0;

	pEdict->v.bInDuck = 0;
	pEdict->v.flTimeStepSound = 0;
	pEdict->v.flSwimTime = 0;
	pEdict->v.flDuckTime = 0;
	pEdict->v.iStepLeft = 0;
	pEdict->v.flFallVelocity = 0.0;

	pEdict->v.gamestate = 0;

	pEdict->v.oldbuttons = 0;

	pEdict->v.groupinfo = 0;

	// For mods
	pEdict->v.iuser1 = 0;
	pEdict->v.iuser2 = 0;
	pEdict->v.iuser3 = 0;
	pEdict->v.iuser4 = 0;
	pEdict->v.fuser1 = 0.0;
	pEdict->v.fuser2 = 0.0;
	pEdict->v.fuser3 = 0.0;
	pEdict->v.fuser4 = 0.0;
	pEdict->v.vuser1 = Vector(0,0,0);
	pEdict->v.vuser2 = Vector(0,0,0);
	pEdict->v.vuser3 = Vector(0,0,0);
	pEdict->v.vuser4 = Vector(0,0,0);
	pEdict->v.euser1 = nullptr;
	pEdict->v.euser2 = nullptr;
	pEdict->v.euser3 = nullptr;
	pEdict->v.euser4 = nullptr;
}
