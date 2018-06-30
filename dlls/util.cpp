<<<<<<< HEAD
=======
<<<<<<< HEAD
>>>>>>> 594320fd6db9c6f9b3b7d743a0270568ddbba6bd
/***
*
*  Copyright (c) 1999, Valve LLC. All rights reserved.
*
*  This product contains software technology licensed from Id 
*  Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*  All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/

//
// HPB_bot - botman's High Ping Bastard bot
//
// (http://planethalflife.com/botman/)
//
// util.cpp
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

#include "engine.h"
#include "dll.h"
#include "bot.h"
#include "bot_func.h"
#include "bot_weapons.h"

extern int mod_id;
extern bot_t bots[32];
extern edict_t *pent_info_ctfdetect;
extern char team_names[MAX_TEAMS][MAX_TEAMNAME_LENGTH];
extern int num_teams;

int gmsgTextMsg = 0;
int gmsgSayText = 0;
int gmsgShowMenu = 0;


Vector UTIL_VecToAngles( const Vector &vec )
{
	float rgflVecOut[3];
	VEC_TO_ANGLES(vec, rgflVecOut);
	return Vector(rgflVecOut);
}


// Overloaded to add IGNORE_GLASS
void UTIL_TraceLine( const Vector &vecStart, const Vector &vecEnd, IGNORE_MONSTERS igmon, IGNORE_GLASS ignoreGlass, edict_t *pentIgnore, TraceResult *ptr )
{
	TRACE_LINE( vecStart, vecEnd, (igmon == ignore_monsters ? TRUE : FALSE) | (ignoreGlass?0x100:0), pentIgnore, ptr );
}


void UTIL_TraceLine( const Vector &vecStart, const Vector &vecEnd, IGNORE_MONSTERS igmon, edict_t *pentIgnore, TraceResult *ptr )
{
	TRACE_LINE( vecStart, vecEnd, (igmon == ignore_monsters ? TRUE : FALSE), pentIgnore, ptr );
}

void UTIL_TraceHull(const Vector &vecStart, const Vector &vecEnd, IGNORE_MONSTERS igmon, int hullNumber, edict_t *pentIgnore, TraceResult *ptr)
{
	TRACE_HULL( vecStart, vecEnd, (igmon == ignore_monsters ? TRUE : FALSE), hullNumber, pentIgnore, ptr );
}

edict_t *UTIL_FindEntityInSphere( edict_t *pentStart, const Vector &vecCenter, float flRadius )
{
	edict_t  *pentEntity;
	
	pentEntity = FIND_ENTITY_IN_SPHERE( pentStart, vecCenter, flRadius);
	
	if (!FNullEnt(pentEntity))
		return pentEntity;
	
	return NULL;
}


edict_t *UTIL_FindEntityByString( edict_t *pentStart, const char *szKeyword, const char *szValue )
{
	edict_t *pentEntity;
	
	pentEntity = FIND_ENTITY_BY_STRING( pentStart, szKeyword, szValue );
	
	if (!FNullEnt(pentEntity))
		return pentEntity;
	return NULL;
}

edict_t *UTIL_FindEntityByClassname( edict_t *pentStart, const char *szName )
{
	return UTIL_FindEntityByString( pentStart, "classname", szName );
}

edict_t *UTIL_FindEntityByTargetname( edict_t *pentStart, const char *szName )
{
	return UTIL_FindEntityByString( pentStart, "targetname", szName );
}


void UTIL_SayText( const char *pText, edict_t *pEdict )
{
	if (gmsgSayText == 0)
		gmsgSayText = REG_USER_MSG( "SayText", -1 );
	
	pfnMessageBegin( MSG_ONE, gmsgSayText, NULL, pEdict );
	pfnWriteByte( ENTINDEX(pEdict) );
	pfnWriteString( pText );
	pfnMessageEnd();
}


void UTIL_HostSay( edict_t *pEntity, int teamonly, char *message )
{
	int   j;
	char  text[128];
	char *pc;
	int   sender_team, player_team;
	edict_t *client;
	
	// make sure the text has content
	for ( pc = message; pc != NULL && *pc != 0; pc++ )
	{
		if ( isprint( *pc ) && !isspace( *pc ) )
		{
			pc = NULL;   // we've found an alphanumeric character,  so text is valid
			break;
		}
	}
	
	if ( pc != NULL )
		return;  // no character found, so say nothing
	
	// turn on color set 2  (color on,  no sound)
	if ( teamonly )
		sprintf( text, "%c(TEAM) %s: ", 2, STRING( pEntity->v.netname ) );
	else
		sprintf( text, "%c%s: ", 2, STRING( pEntity->v.netname ) );
	
	j = sizeof(text) - 2 - strlen(text);  // -2 for /n and null terminator
	if ( (int)strlen(message) > j )
		message[j] = 0;
	
	strcat( text, message );
	strcat( text, "\n" );
	
	// loop through all players
	// Start with the first player.
	// This may return the world in single player if the client types something between levels or during spawn
	// so check it, or it will infinite loop
	
	if (gmsgSayText == 0)
		gmsgSayText = REG_USER_MSG( "SayText", -1 );
	
	sender_team = UTIL_GetTeam(pEntity);
	
	client = NULL;
	while ( ((client = UTIL_FindEntityByClassname( client, "player" )) != NULL) &&
		(!FNullEnt(client)) ) 
	{
		if ( client == pEntity )  // skip sender of message
			continue;
		
		player_team = UTIL_GetTeam(client);
		
		if ( teamonly && (sender_team != player_team) )
			continue;
		
		pfnMessageBegin( MSG_ONE, gmsgSayText, NULL, client );
		pfnWriteByte( ENTINDEX(pEntity) );
		pfnWriteString( text );
		pfnMessageEnd();
	}
	
	// print to the sending client
	pfnMessageBegin( MSG_ONE, gmsgSayText, NULL, pEntity );
	pfnWriteByte( ENTINDEX(pEntity) );
	pfnWriteString( text );
	pfnMessageEnd();
	
	// echo to server console
	SERVER_PRINT ( text );
}


bool IsCarryEnt( edict_t *pEntity )
{	// check if classname begins with "carry_", if so, must be carry_scientist or carry_resource
	return (strncmp(STRING(pEntity->v.classname), "carry_", 6) == 0);
}

// return team number 0 through 3 based what MOD uses for team numbers
int UTIL_GetTeam(edict_t *pEntity)
{
	if (mod_id == CRABBED_DLL || mod_id == VALVE_DLL)
	{
		char *infobuffer;
		char model_name[32];
		
		if (team_names[0][0] == 0)
		{
			char *pName;
			char teamlist[MAX_TEAMS*MAX_TEAMNAME_LENGTH];
			int i;
			
			num_teams = 0;
			strcpy(teamlist, CVAR_GET_STRING("mp_teamlist"));
			pName = teamlist;
			pName = strtok(pName, ";");
			
			while (pName != NULL && *pName)
			{
				// check that team isn't defined twice
				for (i=0; i < num_teams; i++)
					if (strcmp(pName, team_names[i]) == 0)
						break;
				if (i == num_teams)
				{
					strcpy(team_names[num_teams], pName);
					num_teams++;
				}
				pName = strtok(NULL, ";");
			}
		}
		
		infobuffer = (*g_engfuncs.pfnGetInfoKeyBuffer)( pEntity );
		strcpy(model_name, (g_engfuncs.pfnInfoKeyValue(infobuffer, "model")));
		
		for (int index=0; index < num_teams; index++)
		{
			if (strcmp(model_name, team_names[index]) == 0)
				return index;
		}
		
		
	}
	else if (mod_id == SI_DLL)
	{
		return (pEntity->v.team);
	}

	return 0;
}


// return class number 0 through N
int UTIL_GetClass(edict_t *pEntity)
{
	return 0;
}

int UTIL_PlayersOnTeam( int team )
{
	int index;
	int count = 0;
	edict_t *pPlayer = NULL;

	// count number of players on this team...
	for (index = 1; index <= gpGlobals->maxClients; index++)
	{
		pPlayer = INDEXENT(index);
		
		if (pPlayer && !pPlayer->free && pPlayer->v.flags & FL_CLIENT)
		{
			if (UTIL_GetTeam(pPlayer) == team)
				count++;
		}
	}

	return count;
}

int UTIL_ClientsOnTeam( int team )
{
	int index;
	int count = 0;
	edict_t *pPlayer = NULL;

	// count number of clients on this team...
	for (index = 1; index <= gpGlobals->maxClients; index++)
	{
		pPlayer = INDEXENT(index);
		
		if (pPlayer && !pPlayer->free && pPlayer->v.flags & FL_CLIENT &&
			!(pPlayer->v.flags & FL_FAKECLIENT))
		{
			if (UTIL_GetTeam(pPlayer) == team)
				count++;
		}
	}

	return count;
}

int UTIL_BotsOnTeam( int team )
{
	int index;
	int count = 0;
	edict_t *pPlayer = NULL;

	// count number of bots on this team...
	for (index = 1; index <= gpGlobals->maxClients; index++)
	{
		pPlayer = INDEXENT(index);
		
		if (pPlayer && !pPlayer->free && pPlayer->v.flags & FL_CLIENT &&
			pPlayer->v.flags & FL_FAKECLIENT)
		{
			if (UTIL_GetTeam(pPlayer) == team)
				count++;
		}
	}

	return count;
}

int UTIL_GetBotIndex(edict_t *pEdict)
{
	int index;
	
	for (index=0; index < 32; index++)
	{
		if (bots[index].pEdict == pEdict)
		{
			return index;
		}
	}
	
	return -1;  // return -1 if edict is not a bot
}

extern int m_spriteTexture;

void UTIL_DrawBeam(edict_t *pEntity, Vector start, Vector end, int life, int width,
					  int noise, int red, int green, int blue, int brightness, int speed)
{
	MESSAGE_BEGIN(MSG_ONE, SVC_TEMPENTITY, NULL, pEntity);
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
	WRITE_BYTE( life ); // life in 0.1's
	WRITE_BYTE( width ); // width
	WRITE_BYTE( noise );  // noise
	
	WRITE_BYTE( red );   // r, g, b
	WRITE_BYTE( green );   // r, g, b
	WRITE_BYTE( blue );   // r, g, b
	
	WRITE_BYTE( brightness );   // brightness
	WRITE_BYTE( speed );    // speed
	MESSAGE_END();
}

bot_t *UTIL_GetBotPointer(edict_t *pEdict)
{
	int index;
	
	for (index=0; index < 32; index++)
	{
		if (bots[index].pEdict == pEdict)
		{
			break;
		}
	}
	
	if (index < 32)
		return (&bots[index]);
	
	return NULL;  // return NULL if edict is not a bot
}


bool IsAlive(edict_t *pEdict)
{
	return ((pEdict->v.deadflag == DEAD_NO) &&
		(pEdict->v.health > 0) && !(pEdict->v.flags & FL_NOTARGET));
}


bool FInViewCone(Vector *pOrigin, edict_t *pEdict)
{
	Vector2D vec2LOS;
	float    flDot;
	
	MAKE_VECTORS ( pEdict->v.angles );
	
	vec2LOS = ( *pOrigin - pEdict->v.origin ).Make2D();
	vec2LOS = vec2LOS.Normalize();
	
	flDot = DotProduct (vec2LOS , gpGlobals->v_forward.Make2D() );
	
	if ( flDot > 0.50 )  // 60 degree field of view 
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool FVisible( const Vector &vecOrigin, edict_t *pEdict )
{
	TraceResult tr;
	Vector      vecLookerOrigin;
	
	// look through caller's eyes
	vecLookerOrigin = pEdict->v.origin + pEdict->v.view_ofs;
	
	int bInWater = (POINT_CONTENTS (vecOrigin) == CONTENTS_WATER);
	int bLookerInWater = (POINT_CONTENTS (vecLookerOrigin) == CONTENTS_WATER);
	
	// don't look through water
	if (bInWater != bLookerInWater)
		return FALSE;
	
	UTIL_TraceLine(vecLookerOrigin, vecOrigin, ignore_monsters, ignore_glass, pEdict, &tr);
	
	if (tr.flFraction != 1.0)
	{
		return FALSE;  // Line of sight is not established
	}
	else
	{
		return TRUE;  // line of sight is valid.
	}
}


bool FHullClear( const Vector &vecOrigin, edict_t *pEdict )
{
	TraceResult tr;
	Vector      vecLookerOrigin;
	
	// look through caller's eyes
	vecLookerOrigin = pEdict->v.origin + pEdict->v.view_ofs;
	
	int bInWater = (POINT_CONTENTS (vecOrigin) == CONTENTS_WATER);
	int bLookerInWater = (POINT_CONTENTS (vecLookerOrigin) == CONTENTS_WATER);
	
	// don't look through water
	if (bInWater != bLookerInWater)
		return FALSE;
	
	UTIL_TraceHull(vecLookerOrigin, vecOrigin, ignore_monsters, head_hull, pEdict, &tr);
	
	if (tr.flFraction != 1.0)
	{
		return FALSE;  // Line of sight is not established
	}
	else
	{
		return TRUE;  // line of sight is valid.
	}
}

Vector GetGunPosition(edict_t *pEdict)
{
	return (pEdict->v.origin + pEdict->v.view_ofs);
}


void UTIL_SelectItem(edict_t *pEdict, char *item_name)
{
	FakeClientCommand(pEdict, item_name, NULL, NULL);
}


Vector UTIL_GetOrigin(edict_t *pEdict)
{
	if (strncmp(STRING(pEdict->v.classname), "func_", 5) == 0)
		return VecBModelOrigin(pEdict);

	return pEdict->v.origin; 
}

Vector VecBModelOrigin(edict_t *pEdict)
{
	return pEdict->v.absmin + (pEdict->v.size * 0.5);
}


bool UpdateSounds(edict_t *pEdict, edict_t *pPlayer)
{
	float distance;
	static bool check_footstep_sounds = TRUE;
	static float footstep_sounds_on;
	float sensitivity = 1.0;
	float volume;
	
	// update sounds made by this player, alert bots if they are nearby...
	
	if (check_footstep_sounds)
	{
		check_footstep_sounds = FALSE;
		footstep_sounds_on = CVAR_GET_FLOAT("mp_footsteps");
	}
	
	if (footstep_sounds_on > 0.0)
	{
		// check if this player is moving fast enough to make sounds...
		if (pPlayer->v.velocity.Length2D() > 220.0)
		{
			volume = 500.0;  // volume of sound being made (just pick something)
			
			Vector v_sound = pPlayer->v.origin - pEdict->v.origin;
			
			distance = v_sound.Length();
			
			// is the bot close enough to hear this sound?
			if (distance < (volume * sensitivity))
			{
				bot_t *pBot = UTIL_GetBotPointer(pEdict);
				if (pBot && pBot->pBotEnemy != NULL && !pBot->b_use_health_station &&
					!pBot->b_use_HEV_station)
				{
					// just use dmg time, does the same thing
					pBot->dmg_origin = v_sound;
					pBot->f_dmg_time = gpGlobals->time + 1.0;
					
					// stop using health or HEV stations...
					pBot->b_use_health_station = FALSE;
					pBot->b_use_HEV_station = FALSE;
					
					return TRUE;
				}
			}
		}
	}
	
	return FALSE;
}


void UTIL_ShowMenu( edict_t *pEdict, int slots, int displaytime, bool needmore, char *pText )
{
	if (gmsgShowMenu == 0)
		gmsgShowMenu = REG_USER_MSG( "ShowMenu", -1 );
	
	pfnMessageBegin( MSG_ONE, gmsgShowMenu, NULL, pEdict );
	
	pfnWriteShort( slots );
	pfnWriteChar( displaytime );
	pfnWriteByte( needmore );
	pfnWriteString( pText );
	
	pfnMessageEnd();
}

void UTIL_BuildFileName(char *filename, char *arg1, char *arg2)
{
	if (mod_id == VALVE_DLL)
		strcpy(filename, "valve/");
	else if (mod_id == CRABBED_DLL)
		strcpy(filename, "crabbed/");
	else if (mod_id == SI_DLL)
		strcpy(filename, "si/");
	
	if ((arg1) && (*arg1) && (arg2) && (*arg2))
	{
		strcat(filename, arg1);
		strcat(filename, "/");
		strcat(filename, arg2);
	}
	else if ((arg1) && (*arg1))
	{
		strcat(filename, arg1);
	}
}

void UTIL_StringToStringArray( char *pVector[], int count, const char *pString )
{
	char *pstr, *pfront, tempString[128];
	int	j;

	strcpy( tempString, pString );
	pstr = pfront = tempString;

	for ( j = 0; j < count; j++ )			// lifted from pr_edict.c
	{
		strcpy(pVector[j], pfront);

		while ( *pstr && *pstr != ' ' )
			pstr++;
		if (!*pstr)
			break;
		pstr++;
		pfront = pstr;
	}

	for ( j++; j < count; j++ )
	{
		pVector[j] = 0;
	}
}
<<<<<<< HEAD
=======
=======
/***
*
*  Copyright (c) 1999, Valve LLC. All rights reserved.
*
*  This product contains software technology licensed from Id 
*  Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*  All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/

//
// HPB_bot - botman's High Ping Bastard bot
//
// (http://planethalflife.com/botman/)
//
// util.cpp
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

#include "engine.h"
#include "dll.h"
#include "bot.h"
#include "bot_func.h"
#include "bot_weapons.h"

extern int mod_id;
extern bot_t bots[32];
extern edict_t *pent_info_ctfdetect;
extern char team_names[MAX_TEAMS][MAX_TEAMNAME_LENGTH];
extern int num_teams;

int gmsgTextMsg = 0;
int gmsgSayText = 0;
int gmsgShowMenu = 0;


Vector UTIL_VecToAngles( const Vector &vec )
{
	float rgflVecOut[3];
	VEC_TO_ANGLES(vec, rgflVecOut);
	return Vector(rgflVecOut);
}


// Overloaded to add IGNORE_GLASS
void UTIL_TraceLine( const Vector &vecStart, const Vector &vecEnd, IGNORE_MONSTERS igmon, IGNORE_GLASS ignoreGlass, edict_t *pentIgnore, TraceResult *ptr )
{
	TRACE_LINE( vecStart, vecEnd, (igmon == ignore_monsters ? TRUE : FALSE) | (ignoreGlass?0x100:0), pentIgnore, ptr );
}


void UTIL_TraceLine( const Vector &vecStart, const Vector &vecEnd, IGNORE_MONSTERS igmon, edict_t *pentIgnore, TraceResult *ptr )
{
	TRACE_LINE( vecStart, vecEnd, (igmon == ignore_monsters ? TRUE : FALSE), pentIgnore, ptr );
}

void UTIL_TraceHull(const Vector &vecStart, const Vector &vecEnd, IGNORE_MONSTERS igmon, int hullNumber, edict_t *pentIgnore, TraceResult *ptr)
{
	TRACE_HULL( vecStart, vecEnd, (igmon == ignore_monsters ? TRUE : FALSE), hullNumber, pentIgnore, ptr );
}

edict_t *UTIL_FindEntityInSphere( edict_t *pentStart, const Vector &vecCenter, float flRadius )
{
	edict_t  *pentEntity;
	
	pentEntity = FIND_ENTITY_IN_SPHERE( pentStart, vecCenter, flRadius);
	
	if (!FNullEnt(pentEntity))
		return pentEntity;
	
	return NULL;
}


edict_t *UTIL_FindEntityByString( edict_t *pentStart, const char *szKeyword, const char *szValue )
{
	edict_t *pentEntity;
	
	pentEntity = FIND_ENTITY_BY_STRING( pentStart, szKeyword, szValue );
	
	if (!FNullEnt(pentEntity))
		return pentEntity;
	return NULL;
}

edict_t *UTIL_FindEntityByClassname( edict_t *pentStart, const char *szName )
{
	return UTIL_FindEntityByString( pentStart, "classname", szName );
}

edict_t *UTIL_FindEntityByTargetname( edict_t *pentStart, const char *szName )
{
	return UTIL_FindEntityByString( pentStart, "targetname", szName );
}


void UTIL_SayText( const char *pText, edict_t *pEdict )
{
	if (gmsgSayText == 0)
		gmsgSayText = REG_USER_MSG( "SayText", -1 );
	
	pfnMessageBegin( MSG_ONE, gmsgSayText, NULL, pEdict );
	pfnWriteByte( ENTINDEX(pEdict) );
	pfnWriteString( pText );
	pfnMessageEnd();
}


void UTIL_HostSay( edict_t *pEntity, int teamonly, char *message )
{
	int   j;
	char  text[128];
	char *pc;
	int   sender_team, player_team;
	edict_t *client;
	
	// make sure the text has content
	for ( pc = message; pc != NULL && *pc != 0; pc++ )
	{
		if ( isprint( *pc ) && !isspace( *pc ) )
		{
			pc = NULL;   // we've found an alphanumeric character,  so text is valid
			break;
		}
	}
	
	if ( pc != NULL )
		return;  // no character found, so say nothing
	
	// turn on color set 2  (color on,  no sound)
	if ( teamonly )
		sprintf( text, "%c(TEAM) %s: ", 2, STRING( pEntity->v.netname ) );
	else
		sprintf( text, "%c%s: ", 2, STRING( pEntity->v.netname ) );
	
	j = sizeof(text) - 2 - strlen(text);  // -2 for /n and null terminator
	if ( (int)strlen(message) > j )
		message[j] = 0;
	
	strcat( text, message );
	strcat( text, "\n" );
	
	// loop through all players
	// Start with the first player.
	// This may return the world in single player if the client types something between levels or during spawn
	// so check it, or it will infinite loop
	
	if (gmsgSayText == 0)
		gmsgSayText = REG_USER_MSG( "SayText", -1 );
	
	sender_team = UTIL_GetTeam(pEntity);
	
	client = NULL;
	while ( ((client = UTIL_FindEntityByClassname( client, "player" )) != NULL) &&
		(!FNullEnt(client)) ) 
	{
		if ( client == pEntity )  // skip sender of message
			continue;
		
		player_team = UTIL_GetTeam(client);
		
		if ( teamonly && (sender_team != player_team) )
			continue;
		
		pfnMessageBegin( MSG_ONE, gmsgSayText, NULL, client );
		pfnWriteByte( ENTINDEX(pEntity) );
		pfnWriteString( text );
		pfnMessageEnd();
	}
	
	// print to the sending client
	pfnMessageBegin( MSG_ONE, gmsgSayText, NULL, pEntity );
	pfnWriteByte( ENTINDEX(pEntity) );
	pfnWriteString( text );
	pfnMessageEnd();
	
	// echo to server console
	SERVER_PRINT ( text );
}


bool IsCarryEnt( edict_t *pEntity )
{	// check if classname begins with "carry_", if so, must be carry_scientist or carry_resource
	return (strncmp(STRING(pEntity->v.classname), "carry_", 6) == 0);
}

// return team number 0 through 3 based what MOD uses for team numbers
int UTIL_GetTeam(edict_t *pEntity)
{
	if (mod_id == CRABBED_DLL || mod_id == VALVE_DLL)
	{
		char *infobuffer;
		char model_name[32];
		
		if (team_names[0][0] == 0)
		{
			char *pName;
			char teamlist[MAX_TEAMS*MAX_TEAMNAME_LENGTH];
			int i;
			
			num_teams = 0;
			strcpy(teamlist, CVAR_GET_STRING("mp_teamlist"));
			pName = teamlist;
			pName = strtok(pName, ";");
			
			while (pName != NULL && *pName)
			{
				// check that team isn't defined twice
				for (i=0; i < num_teams; i++)
					if (strcmp(pName, team_names[i]) == 0)
						break;
				if (i == num_teams)
				{
					strcpy(team_names[num_teams], pName);
					num_teams++;
				}
				pName = strtok(NULL, ";");
			}
		}
		
		infobuffer = (*g_engfuncs.pfnGetInfoKeyBuffer)( pEntity );
		strcpy(model_name, (g_engfuncs.pfnInfoKeyValue(infobuffer, "model")));
		
		for (int index=0; index < num_teams; index++)
		{
			if (strcmp(model_name, team_names[index]) == 0)
				return index;
		}
		
		
	}
	else if (mod_id == SI_DLL)
	{
		return (pEntity->v.team);
	}

	return 0;
}


// return class number 0 through N
int UTIL_GetClass(edict_t *pEntity)
{
	return 0;
}

int UTIL_PlayersOnTeam( int team )
{
	int index;
	int count = 0;
	edict_t *pPlayer = NULL;

	// count number of players on this team...
	for (index = 1; index <= gpGlobals->maxClients; index++)
	{
		pPlayer = INDEXENT(index);
		
		if (pPlayer && !pPlayer->free && pPlayer->v.flags & FL_CLIENT)
		{
			if (UTIL_GetTeam(pPlayer) == team)
				count++;
		}
	}

	return count;
}

int UTIL_ClientsOnTeam( int team )
{
	int index;
	int count = 0;
	edict_t *pPlayer = NULL;

	// count number of clients on this team...
	for (index = 1; index <= gpGlobals->maxClients; index++)
	{
		pPlayer = INDEXENT(index);
		
		if (pPlayer && !pPlayer->free && pPlayer->v.flags & FL_CLIENT &&
			!(pPlayer->v.flags & FL_FAKECLIENT))
		{
			if (UTIL_GetTeam(pPlayer) == team)
				count++;
		}
	}

	return count;
}

int UTIL_BotsOnTeam( int team )
{
	int index;
	int count = 0;
	edict_t *pPlayer = NULL;

	// count number of bots on this team...
	for (index = 1; index <= gpGlobals->maxClients; index++)
	{
		pPlayer = INDEXENT(index);
		
		if (pPlayer && !pPlayer->free && pPlayer->v.flags & FL_CLIENT &&
			pPlayer->v.flags & FL_FAKECLIENT)
		{
			if (UTIL_GetTeam(pPlayer) == team)
				count++;
		}
	}

	return count;
}

int UTIL_GetBotIndex(edict_t *pEdict)
{
	int index;
	
	for (index=0; index < 32; index++)
	{
		if (bots[index].pEdict == pEdict)
		{
			return index;
		}
	}
	
	return -1;  // return -1 if edict is not a bot
}

extern int m_spriteTexture;

void UTIL_DrawBeam(edict_t *pEntity, Vector start, Vector end, int life, int width,
					  int noise, int red, int green, int blue, int brightness, int speed)
{
	MESSAGE_BEGIN(MSG_ONE, SVC_TEMPENTITY, NULL, pEntity);
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
	WRITE_BYTE( life ); // life in 0.1's
	WRITE_BYTE( width ); // width
	WRITE_BYTE( noise );  // noise
	
	WRITE_BYTE( red );   // r, g, b
	WRITE_BYTE( green );   // r, g, b
	WRITE_BYTE( blue );   // r, g, b
	
	WRITE_BYTE( brightness );   // brightness
	WRITE_BYTE( speed );    // speed
	MESSAGE_END();
}

bot_t *UTIL_GetBotPointer(edict_t *pEdict)
{
	int index;
	
	for (index=0; index < 32; index++)
	{
		if (bots[index].pEdict == pEdict)
		{
			break;
		}
	}
	
	if (index < 32)
		return (&bots[index]);
	
	return NULL;  // return NULL if edict is not a bot
}


bool IsAlive(edict_t *pEdict)
{
	return ((pEdict->v.deadflag == DEAD_NO) &&
		(pEdict->v.health > 0) && !(pEdict->v.flags & FL_NOTARGET));
}


bool FInViewCone(Vector *pOrigin, edict_t *pEdict)
{
	Vector2D vec2LOS;
	float    flDot;
	
	MAKE_VECTORS ( pEdict->v.angles );
	
	vec2LOS = ( *pOrigin - pEdict->v.origin ).Make2D();
	vec2LOS = vec2LOS.Normalize();
	
	flDot = DotProduct (vec2LOS , gpGlobals->v_forward.Make2D() );
	
	if ( flDot > 0.50 )  // 60 degree field of view 
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool FVisible( const Vector &vecOrigin, edict_t *pEdict )
{
	TraceResult tr;
	Vector      vecLookerOrigin;
	
	// look through caller's eyes
	vecLookerOrigin = pEdict->v.origin + pEdict->v.view_ofs;
	
	int bInWater = (POINT_CONTENTS (vecOrigin) == CONTENTS_WATER);
	int bLookerInWater = (POINT_CONTENTS (vecLookerOrigin) == CONTENTS_WATER);
	
	// don't look through water
	if (bInWater != bLookerInWater)
		return FALSE;
	
	UTIL_TraceLine(vecLookerOrigin, vecOrigin, ignore_monsters, ignore_glass, pEdict, &tr);
	
	if (tr.flFraction != 1.0)
	{
		return FALSE;  // Line of sight is not established
	}
	else
	{
		return TRUE;  // line of sight is valid.
	}
}


bool FHullClear( const Vector &vecOrigin, edict_t *pEdict )
{
	TraceResult tr;
	Vector      vecLookerOrigin;
	
	// look through caller's eyes
	vecLookerOrigin = pEdict->v.origin + pEdict->v.view_ofs;
	
	int bInWater = (POINT_CONTENTS (vecOrigin) == CONTENTS_WATER);
	int bLookerInWater = (POINT_CONTENTS (vecLookerOrigin) == CONTENTS_WATER);
	
	// don't look through water
	if (bInWater != bLookerInWater)
		return FALSE;
	
	UTIL_TraceHull(vecLookerOrigin, vecOrigin, ignore_monsters, head_hull, pEdict, &tr);
	
	if (tr.flFraction != 1.0)
	{
		return FALSE;  // Line of sight is not established
	}
	else
	{
		return TRUE;  // line of sight is valid.
	}
}

Vector GetGunPosition(edict_t *pEdict)
{
	return (pEdict->v.origin + pEdict->v.view_ofs);
}


void UTIL_SelectItem(edict_t *pEdict, char *item_name)
{
	FakeClientCommand(pEdict, item_name, NULL, NULL);
}


Vector UTIL_GetOrigin(edict_t *pEdict)
{
	if (strncmp(STRING(pEdict->v.classname), "func_", 5) == 0)
		return VecBModelOrigin(pEdict);

	return pEdict->v.origin; 
}

Vector VecBModelOrigin(edict_t *pEdict)
{
	return pEdict->v.absmin + (pEdict->v.size * 0.5);
}


bool UpdateSounds(edict_t *pEdict, edict_t *pPlayer)
{
	float distance;
	static bool check_footstep_sounds = TRUE;
	static float footstep_sounds_on;
	float sensitivity = 1.0;
	float volume;
	
	// update sounds made by this player, alert bots if they are nearby...
	
	if (check_footstep_sounds)
	{
		check_footstep_sounds = FALSE;
		footstep_sounds_on = CVAR_GET_FLOAT("mp_footsteps");
	}
	
	if (footstep_sounds_on > 0.0)
	{
		// check if this player is moving fast enough to make sounds...
		if (pPlayer->v.velocity.Length2D() > 220.0)
		{
			volume = 500.0;  // volume of sound being made (just pick something)
			
			Vector v_sound = pPlayer->v.origin - pEdict->v.origin;
			
			distance = v_sound.Length();
			
			// is the bot close enough to hear this sound?
			if (distance < (volume * sensitivity))
			{
				bot_t *pBot = UTIL_GetBotPointer(pEdict);
				if (pBot && pBot->pBotEnemy != NULL && !pBot->b_use_health_station &&
					!pBot->b_use_HEV_station)
				{
					// just use dmg time, does the same thing
					pBot->dmg_origin = v_sound;
					pBot->f_dmg_time = gpGlobals->time + 1.0;
					
					// stop using health or HEV stations...
					pBot->b_use_health_station = FALSE;
					pBot->b_use_HEV_station = FALSE;
					
					return TRUE;
				}
			}
		}
	}
	
	return FALSE;
}


void UTIL_ShowMenu( edict_t *pEdict, int slots, int displaytime, bool needmore, char *pText )
{
	if (gmsgShowMenu == 0)
		gmsgShowMenu = REG_USER_MSG( "ShowMenu", -1 );
	
	pfnMessageBegin( MSG_ONE, gmsgShowMenu, NULL, pEdict );
	
	pfnWriteShort( slots );
	pfnWriteChar( displaytime );
	pfnWriteByte( needmore );
	pfnWriteString( pText );
	
	pfnMessageEnd();
}

void UTIL_BuildFileName(char *filename, char *arg1, char *arg2)
{
	if (mod_id == VALVE_DLL)
		strcpy(filename, "valve/");
	else if (mod_id == CRABBED_DLL)
		strcpy(filename, "crabbed/");
	else if (mod_id == SI_DLL)
		strcpy(filename, "si/");
	
	if ((arg1) && (*arg1) && (arg2) && (*arg2))
	{
		strcat(filename, arg1);
		strcat(filename, "/");
		strcat(filename, arg2);
	}
	else if ((arg1) && (*arg1))
	{
		strcat(filename, arg1);
	}
}

void UTIL_StringToStringArray( char *pVector[], int count, const char *pString )
{
	char *pstr, *pfront, tempString[128];
	int	j;

	strcpy( tempString, pString );
	pstr = pfront = tempString;

	for ( j = 0; j < count; j++ )			// lifted from pr_edict.c
	{
		strcpy(pVector[j], pfront);

		while ( *pstr && *pstr != ' ' )
			pstr++;
		if (!*pstr)
			break;
		pstr++;
		pfront = pstr;
	}

	for ( j++; j < count; j++ )
	{
		pVector[j] = 0;
	}
}
>>>>>>> 3f195a42c7b84bf4071b789634ebbe14b6297ebc
>>>>>>> 594320fd6db9c6f9b3b7d743a0270568ddbba6bd
