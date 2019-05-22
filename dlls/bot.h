//
// HPB_bot - botman's High Ping Bastard bot
//
// (http://planethalflife.com/botman/)
//
// bot.h
//

#ifndef BOT_H
#define BOT_H

// stuff for Win32 vs. Linux builds

#ifdef __linux__
#define Sleep sleep
typedef int BOOL;
#endif

#ifndef METAMOD_BUILD
   #define GRAVEBOT_VERSION "0.4b3-APG" // non-metamod
#else
   #define GRAVEBOT_VERSION "0.4b3_mm-APG" // metamod
#endif


#ifndef METAMOD_BUILD
   // stuff for Win32 vs. Linux builds

   #include <studio.h>

   #ifndef __linux__

   typedef int (FAR *GETENTITYAPI)(DLL_FUNCTIONS *, int);
   typedef int (FAR *GETNEWDLLFUNCTIONS)(NEW_DLL_FUNCTIONS *, int *);
   typedef void (DLLEXPORT *GIVEFNPTRSTODLL)(enginefuncs_t *, globalvars_t *);
   typedef int (DLLEXPORT *SERVER_GETBLENDINGINTERFACE) (int, struct sv_blending_interface_s **, struct engine_studio_api_s *, float (*)[3][4], float (*)[MAXSTUDIOBONES][3][4]);
   typedef void (FAR *LINK_ENTITY_FUNC)(entvars_t *);

   #else

   #include <dlfcn.h>
   #define GetProcAddress dlsym

   typedef int BOOL;

   typedef int (*GETENTITYAPI)(DLL_FUNCTIONS *, int);
   typedef int (*GETNEWDLLFUNCTIONS)(NEW_DLL_FUNCTIONS *, int *);
   typedef void (*GIVEFNPTRSTODLL)(enginefuncs_t *, globalvars_t *);
   typedef int (*SERVER_GETBLENDINGINTERFACE) (int, struct sv_blending_interface_s **, struct engine_studio_api_s *, float (*)[3][4], float (*)[MAXSTUDIOBONES][3][4]);
   typedef void (*LINK_ENTITY_FUNC)(entvars_t *);

   #endif

   // defined in metamod but not in the HL SDK
   #define GET_INFOKEYBUFFER (*g_engfuncs.pfnGetInfoKeyBuffer)
   #define SET_CLIENT_KEYVALUE (*g_engfuncs.pfnSetClientKeyValue)
   #define SERVER_PRINT (*g_engfuncs.pfnServerPrint)

#endif


// define constants used to identify the MOD we are playing...

#define VALVE_DLL		 1
#define CRABBED_DLL      2
#define SI_DLL			 3

// define some function prototypes...
#ifndef METAMOD_BUILD

   BOOL ClientConnect( edict_t *pEntity, const char *pszName,
                       const char *pszAddress, char szRejectReason[ 128 ] );
   void ClientPutInServer( edict_t *pEntity );
   void ClientCommand( edict_t *pEntity );

#endif

void FakeClientCommand(edict_t *pBot, char *arg1, char *arg2, char *arg3);


const char *Cmd_Args( void );
const char *Cmd_Argv( int argc );
int Cmd_Argc( void );


#define LADDER_UNKNOWN  0
#define LADDER_UP       1
#define LADDER_DOWN     2

#define WANDER_LEFT  1
#define WANDER_RIGHT 2

#define RESPAWN_IDLE             1
#define RESPAWN_NEED_TO_RESPAWN  2
#define RESPAWN_IS_RESPAWNING    3

#define BOT_SKIN_LEN 32
#define BOT_NAME_LEN 32

#define MAX_BOT_WHINE 100

// in normal gravity, how far the longjump hurls us
#define LONGJUMP_DISTANCE	540

// waypoint goal types
#define WPT_GOAL_NONE		0
#define WPT_GOAL_HEALTH		1
#define WPT_GOAL_ARMOR		2
#define WPT_GOAL_WEAPON		3
#define WPT_GOAL_AMMO		4
#define WPT_GOAL_ITEM		5
#define WPT_GOAL_ENEMY		6
#define WPT_GOAL_DEFEND		7
#define WPT_GOAL_LOCATION	8

// start messages
// Science and Industry
#define MSG_SI_IDLE			1
#define MSG_SI_TEAM_SELECT	2
#define MSG_SI_MODEL_SELECT	3

enum
{
	ROLE_NONE = 0,
	ROLE_DEFEND,
	ROLE_ATTACK
};

enum
{	// no subrole
	ROLE_SUB_NONE = 0,
	// attack subroles
	ROLE_SUB_ATT_GET_SCI,
	ROLE_SUB_ATT_RTRN_SCI,
	ROLE_SUB_ATT_KILL_SCI,
	ROLE_SUB_ATT_GET_RSRC,
	ROLE_SUB_ATT_RTRN_RSRC,
	ROLE_SUB_ATT_BREAK,
	// def ally is actually considered an attack sub role
	// since it's support for another attacker
	ROLE_SUB_DEF_ALLY,
	// defend subroles
	ROLE_SUB_DEF_SCIS,
	ROLE_SUB_DEF_BASE,
	ROLE_SUB_DEF_RSRC,
	ROLE_SUB_DEF_BREAK,
};

enum
{
	CARRY_NONE = 0,
	CARRY_SCI,
	CARRY_RSRC
};

extern double pi;

typedef struct
{
	int  iId;     // weapon ID
	int  iClip;   // amount of ammo in the clip
	int  iClip2;  // amount of ammo in secondary clip
	int  iAmmo1;  // amount of ammo in primary reserve
	int  iAmmo2;  // amount of ammo in secondary reserve
} bot_current_weapon_t;

typedef struct
{
	bool can_modify;
	char text[129];
} bot_chat_t;

#define MAX_BOT_CHAT			100
#define MAX_BOT_CHAT_SECTIONS	20
#define	MAX_BOT_CHAT_RECENT		5

typedef struct
{
	int	index;
	char section[12];
	bot_chat_t chat[MAX_BOT_CHAT];
	int	count;
} bot_chat_section_t;

typedef struct
{
	int section_index[MAX_BOT_CHAT_RECENT][MAX_BOT_CHAT_SECTIONS];
	int chat_index[MAX_BOT_CHAT_RECENT][MAX_BOT_CHAT];
} bot_chat_recent_t;

typedef struct
{
	int		index;
	bool	is_used;
	int		respawn_state;
	edict_t	*pEdict;
	bool	need_to_initialize;
	char	name[BOT_NAME_LEN+1];
	char	skin[BOT_SKIN_LEN+1];
	int		bot_skill;
	int		not_started;
	int		start_action;
	float	kick_time;
	float	f_create_time;
	float	f_frame_time;
	// for chat debug
	char	debugchat[128];
	// TheFatal - START
	int		msecnum;
	float	msecdel;
	float	msecval;
	// TheFatal - END

	// things from pev in CBasePlayer...
	int		bot_team;
	int		bot_class;
	int		top_color;
	int		bottom_color;
	float	idle_angle;
	float	idle_angle_time;  // for Front Line Force
	int		round_end;        // round has ended (in round based games)
	float	blinded_time;

	edict_t	*killer_edict;

	float	f_max_speed;
	float	prev_speed;
	float	prev_time;
	Vector	v_prev_origin;
	Vector	v_curr_direction;

	int		max_armor;
	int		max_health;

	float	f_find_item;
	float	f_last_item_found;
	edict_t	*pBotPickupItem;
	int		item_waypoint;

	int		ladder_dir;
	float	f_start_use_ladder_time;
	float	f_end_use_ladder_time;
	bool	waypoint_top_of_ladder;
	
	float	f_wall_check_time;
	float	f_wall_on_right;
	float	f_wall_on_left;
	float	f_dont_avoid_wall_time;
	float	f_look_for_waypoint_time;
	float	f_jump_time;
	float	f_delay_duck_time;
	float	f_do_duck_time;
	float	f_dont_check_stuck;
	
	float	f_avoid_drop_time;
	float	f_check_drop_time;

	float	f_random_turn_time;

	int		wander_dir;
	float	f_exit_water_time;
	
	float	f_ignore_wpt_time;
	Vector	waypoint_origin;
	float	f_waypoint_time;
	int		curr_waypoint_index;
	int		prev_waypoint_index[5];
	float	f_random_waypoint_time;
	int		old_waypoint_goal;
	int		waypoint_goal;
	float	f_waypoint_goal_time;
	bool	waypoint_near_flag;
	Vector	waypoint_flag_origin;
	float	prev_waypoint_distance;
	int		exclude_points[6];
	int		wpt_goal_type;
	float	f_evaluate_goal_time;
	Vector	v_goal;
	float	f_goal_proximity;

	float	f_dmg_time;
	Vector	dmg_origin;

	float	f_avoid_time;
	float	f_do_avoid_time;
	edict_t	*pAvoid;
	Vector	avoid_dir;

	edict_t	*pBotEnemy;
	float	f_bot_see_enemy_time;
	float	f_bot_find_enemy_time;
	bool	b_engaging_enemy;
	float	f_engage_enemy_check;
	bool	b_last_engage;
	int		i_engage_aggressiveness;
	float	f_reaction_target_time;  // time when enemy targeting starts
	float	f_assess_grenade_time;
	float	f_reload_time;
	float	f_combat_longjump;
	bool	b_combat_longjump;
	float	f_longjump_time;
	bool	b_longjump_dir; // FALSE = left, TRUE = right
	bool	b_strafe_direction; // FALSE = left, TRUE = right
	float	f_strafe_chng_dir;

	float	f_aim_tracking_time;
	float	f_aim_x_angle_delta;
	float	f_aim_y_angle_delta;
	
	float	f_mindray_regen_time;

	edict_t	*pBotUser;
	float	f_bot_use_time;
	float	f_bot_spawn_time;
	float	f_sniper_aim_time;
	
	float	f_switch_weapon_time;
	float	f_shoot_time;
	float	f_primary_charging;
	float	f_secondary_charging;
	int		charging_weapon_id;
	float	f_move_speed;
	float	f_strafe_speed;
	float	f_pause_time;
	float	f_sound_update_time;
	// distance we can hear default volume sounds
	float	f_sound_sensitivity;
	bool	bot_has_flag;
	
	bool	b_use_health_station;
	float	f_use_health_time;
	bool	b_use_HEV_station;
	float	f_use_HEV_time;
	
	bool	b_use_button;
	float	f_use_button_time;
	bool	b_lift_moving;
	
	bool	b_longjump;

	bot_current_weapon_t current_weapon;  // one current weapon for each bot
	int m_rgAmmo[MAX_AMMO_SLOTS];  // total ammo amounts (1 array for each bot)

	int		role;
	int		subrole;
	bool	b_role_locked;
	float	f_role_check;

	Vector	v_defend;
	edict_t	*pGoalEnt;
	bool	b_defend_patrol;
	int		defend_wpt;

	float	f_equip_time;
	bool	b_equipped;
	// Science and Industry specific
	int		i_carry_type;
	char	c_carry_name[32];

} bot_t;


#define MAX_TEAMS 32
#define MAX_TEAMNAME_LENGTH 16


#define MAX_FLAGS  5

typedef struct {
	bool mdl_match;
	int  team_no;
	edict_t *edict;
} FLAG_S;

// new UTIL.CPP functions...
edict_t *UTIL_FindEntityInSphere( edict_t *pentStart, const Vector &vecCenter, float flRadius );
edict_t *UTIL_FindEntityByString( edict_t *pentStart, const char *szKeyword, const char *szValue );
edict_t *UTIL_FindEntityByClassname( edict_t *pentStart, const char *szName );
edict_t *UTIL_FindEntityByTargetname( edict_t *pentStart, const char *szName );
void UTIL_SayText( const char *pText, edict_t *pEdict );
void UTIL_HostSay( edict_t *pEntity, int teamonly, char *message );
int UTIL_GetTeam(edict_t *pEntity);
int UTIL_GetClass(edict_t *pEntity);
int UTIL_PlayersOnTeam( int team );
int UTIL_GetBotIndex(edict_t *pEdict);
bool IsCarryEnt( edict_t *pEntity );
bot_t *UTIL_GetBotPointer(edict_t *pEdict);
bool IsAlive(edict_t *pEdict);
bool FInViewCone(Vector *pOrigin, edict_t *pEdict);
bool FVisible( const Vector &vecOrigin, edict_t *pEdict );
bool FHullClear( const Vector &vecOrigin, edict_t *pEdict );
Vector Center(edict_t *pEdict);
Vector GetGunPosition(edict_t *pEdict);
void UTIL_SelectItem(edict_t *pEdict, char *item_name);
Vector UTIL_GetOrigin(edict_t *pEdict);
Vector VecBModelOrigin(edict_t *pEdict);
bool UpdateSounds(edict_t *pEdict, edict_t *pPlayer);
void UTIL_ShowMenu( edict_t *pEdict, int slots, int displaytime, bool needmore, char *pText );
void UTIL_BuildFileName(char *filename, char *arg1, char *arg2);
void UTIL_StringToStringArray( char *pVector[], int count, const char *pString );
void UTIL_DrawBeam(edict_t *pEntity, Vector start, Vector end, int life, int width,
	int noise, int red, int green, int blue, int brightness, int speed);

char *BotChatTypeToString( int iType );
int BotChatTypeToIndex( const char *pszType );
void LoadBotChat(void);
void BotTrimBlanks(char *in_string, char *out_string);
int BotChatTrimTag(char *original_name, char *out_name);
void BotDropCharacter(char *in_string, char *out_string);
void BotSwapCharacter(char *in_string, char *out_string);
void BotChatName(char *original_name, char *out_name);
void BotChatText(char *in_text, char *out_text);
void BotChatFillInName(char *bot_say_msg, char *chat_text, char *chat_name, const char *bot_name);
int BotChatGetRandomIndex( int chat_type );

#endif // BOT_H

