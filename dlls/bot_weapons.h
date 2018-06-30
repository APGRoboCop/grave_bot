//
// HPB_bot - botman's High Ping Bastard bot
//
// (http://planethalflife.com/botman/)
//
// bot_weapons.h
//

#ifndef BOT_WEAPONS_H
#define BOT_WEAPONS_H

// weapon ID values for Valve's Half-Life Deathmatch
#define VALVE_WEAPON_UNKNOWN		0
#define VALVE_WEAPON_CROWBAR		1
#define VALVE_WEAPON_GLOCK			2
#define VALVE_WEAPON_PYTHON			3
#define VALVE_WEAPON_MP5			4
#define VALVE_WEAPON_CHAINGUN		5
#define VALVE_WEAPON_CROSSBOW		6
#define VALVE_WEAPON_SHOTGUN		7
#define VALVE_WEAPON_RPG			8
#define VALVE_WEAPON_GAUSS			9
#define VALVE_WEAPON_EGON			10
#define VALVE_WEAPON_HORNETGUN		11
#define VALVE_WEAPON_HANDGRENADE	12
#define VALVE_WEAPON_TRIPMINE		13
#define VALVE_WEAPON_SATCHEL		14
#define VALVE_WEAPON_SNARK			15

#define VALVE_MAX_NORMAL_BATTERY   100

// weapon ID values for Science and Industry
#define SI_WEAPON_BRIEFCASE			1
#define SI_WEAPON_COLT				2
#define SI_WEAPON_VOMIT				3
#define SI_WEAPON_ROCKETPISTOL		4
#define SI_WEAPON_SHOTGUN			5
#define SI_WEAPON_TOMMYGUN			6
#define SI_WEAPON_SNUZI				7
#define SI_WEAPON_CROSSBOW			8
#define SI_WEAPON_GAUSS				9
// WHERE IS WEAPON 10?
#define SI_WEAPON_MINDRAY			11
#define SI_WEAPON_HANDGRENADE		12
#define SI_WEAPON_TRIPMINE			13
#define SI_WEAPON_SATCHEL			14
#define SI_WEAPON_TRANSISTOR		15
#define SI_WEAPON_FLESHGRENADE		16
#define SI_WEAPON_RESOURCE			17
#define SI_WEAPON_CLOAK				18
#define SI_WEAPON_EMPCANNON			19

//-----------------------------------------------------------------------------
// Possible Research Options
//-----------------------------------------------------------------------------
enum ResearchGoal
{
	RESEARCH_NULL = 0,			// starting research option (sets up research), it's also the end-game research
	RESEARCH_ARMOR_25,			// level one armor upgrade
	RESEARCH_ARMOR_50,			// level two armor upgrade
	RESEARCH_A_REGEN,			// player armor regeneration
	RESEARCH_ARMOR_75,			// level three armor upgrade
	RESEARCH_A_REGEN2,			// advanced player armor regeneration
	RESEARCH_ARMOR_100,			// level four armor upgrade
	RESEARCH_H_REGEN,			// player health regeneration
	RESEARCH_H_REGEN2,			// advanced player health regeneration
	RESEARCH_LEGS_1,			// faster player movement
	RESEARCH_AMMO_REPLICATE,	// ammo replication
	RESEARCH_STRENGTH,			// bionic strength
	RESEARCH_SUPERJUMP,			// super jump module
	RESEARCH_OPTICS,			// optical implants
	RESEARCH_LEGS_2,			// long jump power
	RESEARCH_AMMO_REPLICATE2,	// advanced ammo replication
	RESEARCH_STRENGTH2,			// enhanced strength
	RESEARCH_SHOTGUN,			// weapon_shotgun
	RESEARCH_TOMMYGUN,			// weapon_tommygun
	RESEARCH_SNUZI,				// weapon_snUZI
	RESEARCH_GRENADE,			// weapon_grenade
	RESEARCH_FEV,				// flesh eating virus
	RESEARCH_CROSSBOW,			// weapon_crossbow
	RESEARCH_GAUSS,				// weapon_gauss
	RESEARCH_ROCKETPISTOL,		// weapon_rocketpistol
	RESEARCH_BRIEFCASE,			// weapon_briefcase
	RESEARCH_RADIO,				// weapon_transistor
	RESEARCH_CLOAKING,			// cloaking device
	RESEARCH_MINDRAY,			// weapon_mindray
	RESEARCH_EMPCANNON,			// weapon_empcannon
	RESEARCH_VOMIT,				// weapon_vomit
	RESEARCH_SATCHEL,			// weapon_satchel
	RESEARCH_TRIPMINE,			// weapon_tripmine
	RESEARCH_COFFEE,			// quicker wake up for scientists and higher max eff.
	RESEARCH_MANUFACTURING,		// cash reward, incentive for scis (cheaper and quicker replacemnts)
	//RESEARCH_PRODUCTION,		// shortend weapon/ammo spawn times
	RESEARCH_CLONING,			// cheaper/quicker player respawn
	RESEARCH_COLT,				// weapon_colt

	NUM_RESEARCH_OPTIONS		// leave this as the last item in the list
};

// -----------------------------------------------------------------------------
// notice_type
//
// Notice types are used with the "Notice" message. Send one of these types as a
// BYTE to the player
// -----------------------------------------------------------------------------
enum notice_type
{
	Notice_None = 0,

	Notice_Kill_Def,
	Notice_Kill_DefNoisy,
	Notice_Kill_DefRecruiter,
	Notice_Kill_Recruiter,
	Notice_Kill_RecruiterResource,
	Notice_Sci_Recruit,
	Notice_Sci_RecruitAssist,
	Notice_Sci_OtherRecruit,
	Notice_Resource_Capture,
	Notice_Resource_OtherCapture,
	Notice_Research_CanSalvage,
	Notice_Research_Salvage,
	Notice_Research_OtherSalvage,
	Notice_Sabotage,
	Notice_Kill_Death,
	Notice_Sci_NewHire,
	Notice_Sci_Kill,
	Notice_Kill_TeamKiller,
	Notice_Sabotage_Opponent,
	Notice_Sabotage_Replace,
	Notice_Award_LlamaOn,
	Notice_Award_LlamaOff,
	Notice_Award_EotMOn,
	Notice_Award_EotMOff,
	Notice_Research_Completed,
	Notice_Research_Start,
	Notice_Research_Future,
	Notice_Research_Espionage,
	Notice_Sci_Missing,
	Notice_Research_Vote,
	Notice_Research_VoteResults,
	Notice_Sci_Defend,
	Notice_Sci_Attack,
	Notice_Sci_AttackStill,
	Notice_Sci_Dead,
	Notice_NoTripmine,
	Notice_NoSatchel,
	Notice_TeamButton,
	Notice_ChangeModel,
	Notice_Research_Castvote,
	Notice_Research_End,
	Notice_Startmatch,

	NUM_NOTICETYPES		// leave this one last
};

typedef struct
{
	bool	researched;
	bool	canidate;
	bool	stolen;
	bool	disabled;
} bot_research_t;

// ammo %s
#define AMMO_NONE		2.0 // no ammo
#define AMMO_MAX		1.0 // full ammo
#define AMMO_LOW		0.2 // low percent
#define AMMO_CRITICAL	0 // not enough to use this weapon

typedef struct
{
   int   iId;						// the weapon ID value
   char  weapon_name[32];			// name of the weapon when selecting it
   char  weapon_model[32];			// model used for third person (p_weapon)
   char  primary_ammo_names[2][32];	// ammo pickup classnames for primary ammo (max of 2)
   char  secondary_ammo_names[2][32];// ammo pickup classnames for primary ammo (max of 2)
   int   skill_level;				// bot skill must be less than or equal to this value
   int   priority;					// how important is this weapon?
   float primary_min_distance;		// 0 = no minimum
   float primary_max_distance;		// 9999 = no maximum
   float secondary_min_distance;	// 0 = no minimum
   float secondary_max_distance;	// 9999 = no maximum
   bool  can_use_underwater;		// can use this weapon underwater
   float reload_delay;				// how long does this weapon take to reload?
   int   primary_fire_percent;		// times out of 100 to use primary fire
   int   min_primary_ammo;			// minimum ammout of primary ammo needed to fire
   int   min_secondary_ammo;		// minimum ammout of seconday ammo needed to fire
   bool  primary_fire_hold;			// hold down primary fire button to use?
   bool  secondary_fire_hold;		// hold down secondary fire button to use?
   bool  primary_fire_charge;		// charge weapon using primary fire?
   bool  secondary_fire_charge;		// charge weapon using secondary fire?
   float primary_charge_delay;		// time to charge weapon
   float secondary_charge_delay;	// time to charge weapon
} bot_weapon_select_t;

typedef struct
{
   int iId;
   float primary_base_delay;
   float primary_min_delay[5];
   float primary_max_delay[5];
   float secondary_base_delay;
   float secondary_min_delay[5];
   float secondary_max_delay[5];
} bot_fire_delay_t;

typedef struct
{
	char	szClassname[24];		// classname (weapon_whatever)
	int		iAmmo1;					// ammo index for primary ammo
	int		iAmmo1Max;				// max primary ammo
	int		iAmmo2;					// ammo index for secondary ammo
	int		iAmmo2Max;				// max secondary ammo
	int		iSlot;					// HUD slot (0 based)
	int		iPosition;				// slot position
	int		iId;					// weapon ID
	int		iFlags;					// flags???
} bot_weapon_t;

bot_weapon_select_t *WeaponGetSelectPointer( void );
bot_fire_delay_t *WeaponGetDelayPointer( void );
int WeaponGetSelectIndex( int iId );
int WeaponGetBest( void );
int WeaponGetWorst( void );
int WeaponGetNextBest( int iId );
int WeaponGetNextWorse( int iId );
int WeaponIdToResearch( int iId );
int WeaponGetAmmoResearchDiff( int iId );
float WeaponProjectileSpeed( int iId );

#endif // BOT_WEAPONS_H

