<<<<<<< HEAD
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
 
extern int mod_id;

// HLDM
extern bot_weapon_select_t valve_weapon_select[];
extern bot_fire_delay_t valve_fire_delay[];
// SI
extern bot_weapon_select_t si_weapon_select[];
extern bot_fire_delay_t si_fire_delay[];

//////////////////////////////////
// Begin weapon based functions //
//////////////////////////////////

bot_weapon_select_t *WeaponGetSelectPointer( void )
{
//	ALERT(at_console, "WeaponGetSelectPointer\n");
	bot_weapon_select_t *pSelect = NULL;

	if (mod_id == CRABBED_DLL || mod_id == VALVE_DLL)
		pSelect = &valve_weapon_select[0];
	else if (mod_id == SI_DLL)
		pSelect = &si_weapon_select[0];

	return pSelect;
}

bot_fire_delay_t *WeaponGetDelayPointer( void )
{
//	ALERT(at_console, "WeaponGetDelayPointer\n");
	bot_fire_delay_t *pDelay = NULL;

	if (mod_id == CRABBED_DLL || mod_id == VALVE_DLL)
		pDelay = &valve_fire_delay[0];
	else if (mod_id == SI_DLL)
		pDelay = &si_fire_delay[0];

	return pDelay;
}

int WeaponGetSelectIndex( int iId )
{
//	ALERT(at_console, "WeaponGetSelectIndex\n");
	bot_weapon_select_t *pSelect = NULL;
	pSelect = WeaponGetSelectPointer();

	int select_index = 0;

	while (pSelect[select_index].iId)
	{
		if (pSelect[select_index].iId == iId)
			return select_index;

		select_index++;
	}

	return -1;
}

int WeaponGetBest( void )
{
//	ALERT(at_console, "WeaponGetBest\n");
	int weapon = -1;

	bot_weapon_select_t *pSelect = NULL;
	pSelect = WeaponGetSelectPointer();

	int select_index = 0;
	int priority = 32;

	while (pSelect[select_index].iId)
	{
		if (pSelect[select_index].priority < priority)
		{
			weapon = pSelect[select_index].iId;
			priority = pSelect[select_index].priority;
		}

		select_index++;
	}

	return weapon;
}

int WeaponGetWorst( void )
{
//	ALERT(at_console, "WeaponGetWorst\n");
	int weapon = -1;

	bot_weapon_select_t *pSelect = NULL;
	pSelect = WeaponGetSelectPointer();

	int select_index = 0;
	int priority = 0;

	while (pSelect[select_index].iId)
	{
		if (pSelect[select_index].priority > priority)
		{
			weapon = pSelect[select_index].iId;
			priority = pSelect[select_index].priority;
		}

		select_index++;
	}

	return weapon;
}

int WeaponGetNextBest( int iId )
{
//	ALERT(at_console, "WeaponGetNextBest\n");
	int weapon = -1;

	bot_weapon_select_t *pSelect = NULL;
	pSelect = WeaponGetSelectPointer();

	int select_index = 0;
	int priority = pSelect[WeaponGetSelectIndex(iId)].priority;

	while (pSelect[select_index].iId)
	{
		if (pSelect[select_index].priority == (priority-1))
			weapon = pSelect[select_index].iId;

		select_index++;
	}

	return weapon;
}

int WeaponGetNextWorse( int iId )
{
//	ALERT(at_console, "WeaponGetNextWorse\n");
	int weapon = -1;

	bot_weapon_select_t *pSelect = NULL;
	pSelect = WeaponGetSelectPointer();

	int select_index = 0;
	int priority = pSelect[WeaponGetSelectIndex(iId)].priority;

	while (pSelect[select_index].iId)
	{
		if (pSelect[select_index].priority == (priority+1))
			weapon = pSelect[select_index].iId;

		select_index++;
	}

	return weapon;
}

int WeaponIdToResearch( int iId )
{
//	ALERT(at_console, "WeaponIdToResearch\n");
	if (mod_id != SI_DLL)
		return -1;

	switch (iId)
	{
		case SI_WEAPON_BRIEFCASE:
			return RESEARCH_BRIEFCASE;
		case SI_WEAPON_COLT:
			return RESEARCH_COLT;
		case SI_WEAPON_VOMIT:
			return RESEARCH_VOMIT;
		case SI_WEAPON_ROCKETPISTOL:
			return RESEARCH_ROCKETPISTOL;
		case SI_WEAPON_SHOTGUN:
			return RESEARCH_SHOTGUN;
		case SI_WEAPON_TOMMYGUN:
			return RESEARCH_TOMMYGUN;
		case SI_WEAPON_SNUZI:
			return RESEARCH_SNUZI;
		case SI_WEAPON_CROSSBOW:
			return RESEARCH_CROSSBOW;
		case SI_WEAPON_GAUSS:
			return RESEARCH_GAUSS;
		case SI_WEAPON_MINDRAY:
			return RESEARCH_MINDRAY;
		case SI_WEAPON_HANDGRENADE:
			return RESEARCH_GRENADE;
		case SI_WEAPON_TRIPMINE:
			return RESEARCH_TRIPMINE;
		case SI_WEAPON_SATCHEL:
			return RESEARCH_SATCHEL;
		case SI_WEAPON_TRANSISTOR:
			return RESEARCH_RADIO;
		case SI_WEAPON_FLESHGRENADE:
			return RESEARCH_FEV;
		case SI_WEAPON_RESOURCE:
			return -1;
		case SI_WEAPON_CLOAK:
			return RESEARCH_CLOAKING;
		case SI_WEAPON_EMPCANNON:
			return RESEARCH_EMPCANNON;
	}

	return -1;
}

int WeaponGetAmmoResearchDiff( int iId )
{
//	ALERT(at_console, "WeaponGetAmmoResearchDiff\n");
	if (mod_id != SI_DLL)
		return -1;

	switch (iId)
	{
		case SI_WEAPON_BRIEFCASE:
		case SI_WEAPON_CLOAK:
		case SI_WEAPON_RESOURCE:
		case SI_WEAPON_MINDRAY:
			return 0;
		case SI_WEAPON_COLT:
			return 14;
		case SI_WEAPON_ROCKETPISTOL:
			return 4;
		case SI_WEAPON_SHOTGUN:
			return 8;
		case SI_WEAPON_TOMMYGUN:
		case SI_WEAPON_SNUZI:
			return 50;
		case SI_WEAPON_CROSSBOW:
			return 5;
		case SI_WEAPON_GAUSS:
		case SI_WEAPON_VOMIT:
			return 20;
		case SI_WEAPON_HANDGRENADE:
		case SI_WEAPON_TRIPMINE:
		case SI_WEAPON_SATCHEL:
		case SI_WEAPON_TRANSISTOR:
			return 2;
		case SI_WEAPON_FLESHGRENADE:
		case SI_WEAPON_EMPCANNON:
			return 1;
	}

	return 0;
}

float WeaponProjectileSpeed( int iId )
{
//	ALERT(at_console, "WeaponProjectileSpeed\n");
	if (mod_id == SI_DLL)
	{
		switch (iId)
		{
			case SI_WEAPON_ROCKETPISTOL:
				return 1400;
			case SI_WEAPON_SATCHEL:
				return 274;
			case SI_WEAPON_HANDGRENADE:
				return 500;
			case SI_WEAPON_FLESHGRENADE:
				return 600;
			case SI_WEAPON_EMPCANNON:
				return 2000;
			default:
				return 0;
		}
	}
	else
	{
		switch (iId)
		{
			case VALVE_WEAPON_RPG:
				return 2000;
			case VALVE_WEAPON_HANDGRENADE:
				return 500;
			case VALVE_WEAPON_SATCHEL:
				return 274;
			default:
				return 0;
		}
	}

	return 0;
=======
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
 
extern int mod_id;

// HLDM
extern bot_weapon_select_t valve_weapon_select[];
extern bot_fire_delay_t valve_fire_delay[];
// SI
extern bot_weapon_select_t si_weapon_select[];
extern bot_fire_delay_t si_fire_delay[];

//////////////////////////////////
// Begin weapon based functions //
//////////////////////////////////

bot_weapon_select_t *WeaponGetSelectPointer( void )
{
//	ALERT(at_console, "WeaponGetSelectPointer\n");
	bot_weapon_select_t *pSelect = NULL;

	if (mod_id == CRABBED_DLL || mod_id == VALVE_DLL)
		pSelect = &valve_weapon_select[0];
	else if (mod_id == SI_DLL)
		pSelect = &si_weapon_select[0];

	return pSelect;
}

bot_fire_delay_t *WeaponGetDelayPointer( void )
{
//	ALERT(at_console, "WeaponGetDelayPointer\n");
	bot_fire_delay_t *pDelay = NULL;

	if (mod_id == CRABBED_DLL || mod_id == VALVE_DLL)
		pDelay = &valve_fire_delay[0];
	else if (mod_id == SI_DLL)
		pDelay = &si_fire_delay[0];

	return pDelay;
}

int WeaponGetSelectIndex( int iId )
{
//	ALERT(at_console, "WeaponGetSelectIndex\n");
	bot_weapon_select_t *pSelect = NULL;
	pSelect = WeaponGetSelectPointer();

	int select_index = 0;

	while (pSelect[select_index].iId)
	{
		if (pSelect[select_index].iId == iId)
			return select_index;

		select_index++;
	}

	return -1;
}

int WeaponGetBest( void )
{
//	ALERT(at_console, "WeaponGetBest\n");
	int weapon = -1;

	bot_weapon_select_t *pSelect = NULL;
	pSelect = WeaponGetSelectPointer();

	int select_index = 0;
	int priority = 32;

	while (pSelect[select_index].iId)
	{
		if (pSelect[select_index].priority < priority)
		{
			weapon = pSelect[select_index].iId;
			priority = pSelect[select_index].priority;
		}

		select_index++;
	}

	return weapon;
}

int WeaponGetWorst( void )
{
//	ALERT(at_console, "WeaponGetWorst\n");
	int weapon = -1;

	bot_weapon_select_t *pSelect = NULL;
	pSelect = WeaponGetSelectPointer();

	int select_index = 0;
	int priority = 0;

	while (pSelect[select_index].iId)
	{
		if (pSelect[select_index].priority > priority)
		{
			weapon = pSelect[select_index].iId;
			priority = pSelect[select_index].priority;
		}

		select_index++;
	}

	return weapon;
}

int WeaponGetNextBest( int iId )
{
//	ALERT(at_console, "WeaponGetNextBest\n");
	int weapon = -1;

	bot_weapon_select_t *pSelect = NULL;
	pSelect = WeaponGetSelectPointer();

	int select_index = 0;
	int priority = pSelect[WeaponGetSelectIndex(iId)].priority;

	while (pSelect[select_index].iId)
	{
		if (pSelect[select_index].priority == (priority-1))
			weapon = pSelect[select_index].iId;

		select_index++;
	}

	return weapon;
}

int WeaponGetNextWorse( int iId )
{
//	ALERT(at_console, "WeaponGetNextWorse\n");
	int weapon = -1;

	bot_weapon_select_t *pSelect = NULL;
	pSelect = WeaponGetSelectPointer();

	int select_index = 0;
	int priority = pSelect[WeaponGetSelectIndex(iId)].priority;

	while (pSelect[select_index].iId)
	{
		if (pSelect[select_index].priority == (priority+1))
			weapon = pSelect[select_index].iId;

		select_index++;
	}

	return weapon;
}

int WeaponIdToResearch( int iId )
{
//	ALERT(at_console, "WeaponIdToResearch\n");
	if (mod_id != SI_DLL)
		return -1;

	switch (iId)
	{
		case SI_WEAPON_BRIEFCASE:
			return RESEARCH_BRIEFCASE;
		case SI_WEAPON_COLT:
			return RESEARCH_COLT;
		case SI_WEAPON_VOMIT:
			return RESEARCH_VOMIT;
		case SI_WEAPON_ROCKETPISTOL:
			return RESEARCH_ROCKETPISTOL;
		case SI_WEAPON_SHOTGUN:
			return RESEARCH_SHOTGUN;
		case SI_WEAPON_TOMMYGUN:
			return RESEARCH_TOMMYGUN;
		case SI_WEAPON_SNUZI:
			return RESEARCH_SNUZI;
		case SI_WEAPON_CROSSBOW:
			return RESEARCH_CROSSBOW;
		case SI_WEAPON_GAUSS:
			return RESEARCH_GAUSS;
		case SI_WEAPON_MINDRAY:
			return RESEARCH_MINDRAY;
		case SI_WEAPON_HANDGRENADE:
			return RESEARCH_GRENADE;
		case SI_WEAPON_TRIPMINE:
			return RESEARCH_TRIPMINE;
		case SI_WEAPON_SATCHEL:
			return RESEARCH_SATCHEL;
		case SI_WEAPON_TRANSISTOR:
			return RESEARCH_RADIO;
		case SI_WEAPON_FLESHGRENADE:
			return RESEARCH_FEV;
		case SI_WEAPON_RESOURCE:
			return -1;
		case SI_WEAPON_CLOAK:
			return RESEARCH_CLOAKING;
		case SI_WEAPON_EMPCANNON:
			return RESEARCH_EMPCANNON;
	}

	return -1;
}

int WeaponGetAmmoResearchDiff( int iId )
{
//	ALERT(at_console, "WeaponGetAmmoResearchDiff\n");
	if (mod_id != SI_DLL)
		return -1;

	switch (iId)
	{
		case SI_WEAPON_BRIEFCASE:
		case SI_WEAPON_CLOAK:
		case SI_WEAPON_RESOURCE:
		case SI_WEAPON_MINDRAY:
			return 0;
		case SI_WEAPON_COLT:
			return 14;
		case SI_WEAPON_ROCKETPISTOL:
			return 4;
		case SI_WEAPON_SHOTGUN:
			return 8;
		case SI_WEAPON_TOMMYGUN:
		case SI_WEAPON_SNUZI:
			return 50;
		case SI_WEAPON_CROSSBOW:
			return 5;
		case SI_WEAPON_GAUSS:
		case SI_WEAPON_VOMIT:
			return 20;
		case SI_WEAPON_HANDGRENADE:
		case SI_WEAPON_TRIPMINE:
		case SI_WEAPON_SATCHEL:
		case SI_WEAPON_TRANSISTOR:
			return 2;
		case SI_WEAPON_FLESHGRENADE:
		case SI_WEAPON_EMPCANNON:
			return 1;
	}

	return 0;
}

float WeaponProjectileSpeed( int iId )
{
//	ALERT(at_console, "WeaponProjectileSpeed\n");
	if (mod_id == SI_DLL)
	{
		switch (iId)
		{
			case SI_WEAPON_ROCKETPISTOL:
				return 1400;
			case SI_WEAPON_SATCHEL:
				return 274;
			case SI_WEAPON_HANDGRENADE:
				return 500;
			case SI_WEAPON_FLESHGRENADE:
				return 600;
			case SI_WEAPON_EMPCANNON:
				return 2000;
			default:
				return 0;
		}
	}
	else
	{
		switch (iId)
		{
			case VALVE_WEAPON_RPG:
				return 2000;
			case VALVE_WEAPON_HANDGRENADE:
				return 500;
			case VALVE_WEAPON_SATCHEL:
				return 274;
			default:
				return 0;
		}
	}

	return 0;
>>>>>>> 3f195a42c7b84bf4071b789634ebbe14b6297ebc
}