//
// HPB bot - botman's High Ping Bastard bot
//
// (http://planethalflife.com/botman/)
//
// engine.cpp
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
#include "bot_client.h"
#include "engine.h"
#include "dll.h"


extern enginefuncs_t g_engfuncs;
extern bot_t bots[32];
extern int mod_id;
extern float speed_mod[5];
extern int isFakeClientCommand;
extern int fake_arg_count;
extern char g_argv[1024];

int debug_engine = 0;

void (*botMsgFunction)(void *, edict_t *) = NULL;
void (*botMsgEndFunction)(void *, edict_t *) = NULL;
edict_t *botMsgEdict;
//int botMsgIndex;
extern float g_flVomiting[32];
// messages created in RegUserMsg which will be "caught"
int message_VGUI = 0;
int message_ShowMenu = 0;
int message_WeaponList = 0;
int message_CurWeapon = 0;
int message_AmmoX = 0;
int message_AmmoPickup = 0;
int message_Damage = 0;
int message_DeathMsg = 0;
int message_TextMsg = 0;
int message_ScreenFade = 0;
int message_ItemPickup = 0;
int message_WeapPickup = 0;
int message_Battery = 0;
// Science and Industry specific
int message_TeamCash = 0;
int message_SciCount = 0;
int message_CarryInfo = 0;
int message_StartCarry = 0;
int message_StopCarry = 0;
int message_Goal = 0;
int message_Notice = 0;
int message_VoteInfo = 0;
// events
short g_usVomit;// player vomit event

static FILE *fp;


#ifndef METAMOD_BUILD
   int pfnPrecacheModel(char* s)
   {
	   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnPrecacheModel: %s\n",s); fclose(fp); }
	   return (*g_engfuncs.pfnPrecacheModel)(s);
   }
   int pfnPrecacheSound(char* s)
   {
	   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnPrecacheSound: %s\n",s); fclose(fp); }
	   return (*g_engfuncs.pfnPrecacheSound)(s);
   }
   void pfnSetModel(edict_t *e, const char *m)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnSetModel: edict=%x %s\n",e,m); fclose(fp); }
	   (*g_engfuncs.pfnSetModel)(e, m);
   }
   int pfnModelIndex(const char *m)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnModelIndex: %s\n",m); fclose(fp); }
	   return (*g_engfuncs.pfnModelIndex)(m);
   }
   int pfnModelFrames(int modelIndex)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnModelFrames:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnModelFrames)(modelIndex);
   }
   void pfnSetSize(edict_t *e, const float *rgflMin, const float *rgflMax)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnSetSize: %x\n",e); fclose(fp); }
	   (*g_engfuncs.pfnSetSize)(e, rgflMin, rgflMax);
   }
#endif

void pfnChangeLevel(char* s1, char* s2)
{
	if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnChangeLevel:\n"); fclose(fp); }
	
	// kick any bot off of the server after time/frag limit...
	for (int index = 0; index < 32; index++)
	{
		if (bots[index].is_used)  // is this slot used?
		{
			char cmd[40];
			
			sprintf(cmd, "kick \"%s\"\n", bots[index].name);
			
			bots[index].respawn_state = RESPAWN_NEED_TO_RESPAWN;
			
			SERVER_COMMAND(cmd);  // kick the bot using (kick "name")
		}
	}

   #ifndef METAMOD_BUILD
	   (*g_engfuncs.pfnChangeLevel)(s1, s2);
   #else
      RETURN_META (MRES_IGNORED);
   #endif
}

#ifndef METAMOD_BUILD
   void pfnGetSpawnParms(edict_t *ent)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnGetSpawnParms:\n"); fclose(fp); }
	   (*g_engfuncs.pfnGetSpawnParms)(ent);
   }
   void pfnSaveSpawnParms(edict_t *ent)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnSaveSpawnParms:\n"); fclose(fp); }
	   (*g_engfuncs.pfnSaveSpawnParms)(ent);
   }
   float pfnVecToYaw(const float *rgflVector)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnVecToYaw:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnVecToYaw)(rgflVector);
   }
   void pfnVecToAngles(const float *rgflVectorIn, float *rgflVectorOut)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnVecToAngles:\n"); fclose(fp); }
	   (*g_engfuncs.pfnVecToAngles)(rgflVectorIn, rgflVectorOut);
   }
   void pfnMoveToOrigin(edict_t *ent, const float *pflGoal, float dist, int iMoveType)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnMoveToOrigin:\n"); fclose(fp); }
	   (*g_engfuncs.pfnMoveToOrigin)(ent, pflGoal, dist, iMoveType);
   }
   void pfnChangeYaw(edict_t* ent)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnChangeYaw:\n"); fclose(fp); }
	   (*g_engfuncs.pfnChangeYaw)(ent);
   }
   void pfnChangePitch(edict_t* ent)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnChangePitch:\n"); fclose(fp); }
	   (*g_engfuncs.pfnChangePitch)(ent);
   }
   edict_t* pfnFindEntityByString(edict_t *pEdictStartSearchAfter, const char *pszField, const char *pszValue)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnFindEntityByString: %s\n",pszValue); fclose(fp); }
	   
	   return (*g_engfuncs.pfnFindEntityByString)(pEdictStartSearchAfter, pszField, pszValue);
   }
   int pfnGetEntityIllum(edict_t* pEnt)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnGetEntityIllum:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnGetEntityIllum)(pEnt);
   }
   edict_t* pfnFindEntityInSphere(edict_t *pEdictStartSearchAfter, const float *org, float rad)
   {
	   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnFindEntityInSphere:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnFindEntityInSphere)(pEdictStartSearchAfter, org, rad);
   }
   edict_t* pfnFindClientInPVS(edict_t *pEdict)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnFindClientInPVS:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnFindClientInPVS)(pEdict);
   }
   edict_t* pfnEntitiesInPVS(edict_t *pplayer)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnEntitiesInPVS:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnEntitiesInPVS)(pplayer);
   }
   void pfnMakeVectors(const float *rgflVector)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnMakeVectors:\n"); fclose(fp); }
	   (*g_engfuncs.pfnMakeVectors)(rgflVector);
   }
   void pfnAngleVectors(const float *rgflVector, float *forward, float *right, float *up)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnAngleVectors:\n"); fclose(fp); }
	   (*g_engfuncs.pfnAngleVectors)(rgflVector, forward, right, up);
   }
   edict_t* pfnCreateEntity(void)
   {
	   edict_t *pent = (*g_engfuncs.pfnCreateEntity)();
	   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnCreateEntity: %x\n",pent); fclose(fp); }
	   return pent;
   }
   void pfnRemoveEntity(edict_t* e)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnRemoveEntity: %x\n",e); fclose(fp); }
	   if (debug_engine)
	   {
		   fp=fopen("bot.txt","a");
		   fprintf(fp,"pfnRemoveEntity: %x\n",e);
		   if (e->v.model != 0)
			   fprintf(fp," model=%s\n", STRING(e->v.model));
		   fclose(fp);
	   }
	   
	   (*g_engfuncs.pfnRemoveEntity)(e);
   }
   edict_t* pfnCreateNamedEntity(int className)
   {
	   edict_t *pent = (*g_engfuncs.pfnCreateNamedEntity)(className);
	   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnCreateNamedEntity: edict=%x name=%s\n",pent,STRING(className)); fclose(fp); }
	   return pent;
   }
   void pfnMakeStatic(edict_t *ent)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnMakeStatic:\n"); fclose(fp); }
	   (*g_engfuncs.pfnMakeStatic)(ent);
   }
   int pfnEntIsOnFloor(edict_t *e)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnEntIsOnFloor:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnEntIsOnFloor)(e);
   }
   int pfnDropToFloor(edict_t* e)
   {
	   //  if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnDropToFloor:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnDropToFloor)(e);
   }
   int pfnWalkMove(edict_t *ent, float yaw, float dist, int iMode)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnWalkMove:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnWalkMove)(ent, yaw, dist, iMode);
   }
   void pfnSetOrigin(edict_t *e, const float *rgflOrigin)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnSetOrigin:\n"); fclose(fp); }
	   (*g_engfuncs.pfnSetOrigin)(e, rgflOrigin);
   }
#endif

void pfnEmitSound(edict_t *entity, int channel, const char *sample, /*int*/float volume, float attenuation, int fFlags, int pitch)
{
	//if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnEmitSound:\n"); fclose(fp); }
	
	BotListenForSound(entity, sample, volume);

   #ifndef METAMOD_BUILD
	   (*g_engfuncs.pfnEmitSound)(entity, channel, sample, volume, attenuation, fFlags, pitch);
   #else
      RETURN_META (MRES_IGNORED);
   #endif
}
void pfnEmitAmbientSound(edict_t *entity, float *pos, const char *samp, float vol, float attenuation, int fFlags, int pitch)
{
	//if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnEmitAmbientSound:\n"); fclose(fp); }

   BotListenForSound(entity, samp, vol);

   #ifndef METAMOD_BUILD
   	(*g_engfuncs.pfnEmitAmbientSound)(entity, pos, samp, vol, attenuation, fFlags, pitch);
   #else
      RETURN_META (MRES_IGNORED);
   #endif
}

#ifndef METAMOD_BUILD
   void pfnTraceLine(const float *v1, const float *v2, int fNoMonsters, edict_t *pentToSkip, TraceResult *ptr)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnTraceLine:\n"); fclose(fp); }
	   (*g_engfuncs.pfnTraceLine)(v1, v2, fNoMonsters, pentToSkip, ptr);
   }
   void pfnTraceToss(edict_t* pent, edict_t* pentToIgnore, TraceResult *ptr)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnTraceToss:\n"); fclose(fp); }
	   (*g_engfuncs.pfnTraceToss)(pent, pentToIgnore, ptr);
   }
   int pfnTraceMonsterHull(edict_t *pEdict, const float *v1, const float *v2, int fNoMonsters, edict_t *pentToSkip, TraceResult *ptr)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnTraceMonsterHull:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnTraceMonsterHull)(pEdict, v1, v2, fNoMonsters, pentToSkip, ptr);
   }
   void pfnTraceHull(const float *v1, const float *v2, int fNoMonsters, int hullNumber, edict_t *pentToSkip, TraceResult *ptr)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnTraceHull:\n"); fclose(fp); }
	   (*g_engfuncs.pfnTraceHull)(v1, v2, fNoMonsters, hullNumber, pentToSkip, ptr);
   }
   void pfnTraceModel(const float *v1, const float *v2, int hullNumber, edict_t *pent, TraceResult *ptr)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnTraceModel:\n"); fclose(fp); }
	   (*g_engfuncs.pfnTraceModel)(v1, v2, hullNumber, pent, ptr);
   }
   const char *pfnTraceTexture(edict_t *pTextureEntity, const float *v1, const float *v2 )
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnTraceTexture:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnTraceTexture)(pTextureEntity, v1, v2);
   }
   void pfnTraceSphere(const float *v1, const float *v2, int fNoMonsters, float radius, edict_t *pentToSkip, TraceResult *ptr)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnTraceSphere:\n"); fclose(fp); }
	   (*g_engfuncs.pfnTraceSphere)(v1, v2, fNoMonsters, radius, pentToSkip, ptr);
   }
   void pfnGetAimVector(edict_t* ent, float speed, float *rgflReturn)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnGetAimVector:\n"); fclose(fp); }
	   (*g_engfuncs.pfnGetAimVector)(ent, speed, rgflReturn);
   }
   void pfnServerCommand(char* str)
   {
	   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnServerCommand: %s\n",str); fclose(fp); }
	   (*g_engfuncs.pfnServerCommand)(str);
   }
   void pfnServerExecute(void)
   {
	   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnServerExecute:\n"); fclose(fp); }
	   (*g_engfuncs.pfnServerExecute)();
   }
   void pfnClientCommand(edict_t* pEdict, char* szFmt, ...)
   {
	   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnClientCommand=%s\n",szFmt); fclose(fp); }
	   return;
   }
   void pfnParticleEffect(const float *org, const float *dir, float color, float count)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnParticleEffect:\n"); fclose(fp); }
	   (*g_engfuncs.pfnParticleEffect)(org, dir, color, count);
   }
   void pfnLightStyle(int style, char* val)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnLightStyle:\n"); fclose(fp); }
	   (*g_engfuncs.pfnLightStyle)(style, val);
   }
   int pfnDecalIndex(const char *name)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnDecalIndex:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnDecalIndex)(name);
   }
   int pfnPointContents(const float *rgflVector)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnPointContents:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnPointContents)(rgflVector);
   }
#endif

void pfnMessageBegin(int msg_dest, int msg_type, const float *pOrigin, edict_t *ed)
{
	if (gpGlobals->deathmatch)
	{
		int index = -1;
		
		if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnMessageBegin: edict=%x dest=%d type=%d\n",ed,msg_dest,msg_type); fclose(fp); }
		
		if (ed)
		{
			botMsgFunction = NULL;     // no msg function until known otherwise
			botMsgEndFunction = NULL;  // no msg end function until known otherwise
			//botMsgIndex = index;       // index of bot receiving message
			botMsgEdict = ed;

			if (mod_id == CRABBED_DLL || mod_id == VALVE_DLL)
			{	// Crabbed / HLDM message handling
				if (msg_type == message_WeaponList)
					botMsgFunction = BotClient_Valve_WeaponList;
				else if (msg_type == message_CurWeapon)
					botMsgFunction = BotClient_Valve_CurrentWeapon;
				else if (msg_type == message_AmmoX)
					botMsgFunction = BotClient_Valve_AmmoX;
				else if (msg_type == message_AmmoPickup)
					botMsgFunction = BotClient_Valve_AmmoPickup;
				else if (msg_type == message_Damage)
					botMsgFunction = BotClient_Valve_Damage;
				else if (msg_type == message_ScreenFade)
					botMsgFunction = BotClient_Valve_ScreenFade;
				else if (msg_type == message_ItemPickup)
					botMsgFunction = BotClient_Valve_ItemPickup;
				else if (msg_type == message_WeapPickup)
					botMsgFunction = BotClient_Valve_WeapPickup;
				else if (msg_type == message_Battery)
					botMsgFunction = BotClient_Valve_Battery;
			}
			else if (mod_id == SI_DLL)
			{	// Science and Industry message handling
				if (msg_type == message_WeaponList)
					botMsgFunction = BotClient_SI_WeaponList;
				else if (msg_type == message_CurWeapon)
					botMsgFunction = BotClient_SI_CurrentWeapon;
				else if (msg_type == message_AmmoX)
					botMsgFunction = BotClient_SI_AmmoX;
				else if (msg_type == message_AmmoPickup)
					botMsgFunction = BotClient_SI_AmmoPickup;
				else if (msg_type == message_Damage)
					botMsgFunction = BotClient_SI_Damage;
				else if (msg_type == message_ScreenFade)
					botMsgFunction = BotClient_SI_ScreenFade;
				else if (msg_type == message_ItemPickup)
					botMsgFunction = BotClient_SI_ItemPickup;
				else if (msg_type == message_WeapPickup)
					botMsgFunction = BotClient_SI_WeapPickup;
				else if (msg_type == message_Battery)
					botMsgFunction = BotClient_SI_Battery;
				else if (msg_type == message_TeamCash)
					botMsgFunction = BotClient_SI_TeamCash;
				else if (msg_type == message_CarryInfo)
					botMsgFunction = BotClient_SI_CarryInfo;
				else if (msg_type == message_Goal)
					botMsgFunction = BotClient_SI_Goal;
				else if (msg_type == message_Notice)
					botMsgFunction = BotClient_SI_Notice;
				else if (msg_type == message_VoteInfo)
					botMsgFunction = BotClient_SI_VoteInfo;
			}
		}
		else if (msg_dest == MSG_ALL)
		{
			botMsgFunction = NULL;  // no msg function until known otherwise
			//botMsgIndex = -1;       // index of bot receiving message (none)
			botMsgEdict = NULL;

			if (msg_type == message_DeathMsg)
				botMsgFunction = BotClient_Valve_DeathMsg;
		}
		else
		{
			// Steam makes the WeaponList message be sent differently
			
			botMsgFunction = NULL;  // no msg function until known otherwise
			//botMsgIndex = -1;       // index of bot receiving message (none)
			botMsgEdict = NULL;

			if (mod_id == CRABBED_DLL || mod_id == VALVE_DLL)
			{
				if (msg_type == message_WeaponList)
					botMsgFunction = BotClient_Valve_WeaponList;
			}
			else if (mod_id == SI_DLL)
			{
				if (msg_type == message_WeaponList)
					botMsgFunction = BotClient_SI_WeaponList;
			}
		}
	}

   #ifndef METAMOD_BUILD
	   (*g_engfuncs.pfnMessageBegin)(msg_dest, msg_type, pOrigin, ed);
   #else
      RETURN_META (MRES_IGNORED);
   #endif
}

void pfnMessageEnd(void)
{
	if (gpGlobals->deathmatch)
	{
		if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnMessageEnd:\n"); fclose(fp); }
		
		if (botMsgEndFunction)
			(*botMsgEndFunction)(NULL, botMsgEdict);
			//(*botMsgEndFunction)(NULL, botMsgIndex);  // NULL indicated msg end
		
		// clear out the bot message function pointers...
		botMsgFunction = NULL;
		botMsgEndFunction = NULL;
		botMsgEdict = NULL;
	}
	
   #ifndef METAMOD_BUILD
   	(*g_engfuncs.pfnMessageEnd)();
   #else
      RETURN_META (MRES_IGNORED);
   #endif
}
void pfnWriteByte(int iValue)
{
	if (gpGlobals->deathmatch)
	{
		if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnWriteByte: %d\n",iValue); fclose(fp); }
		
		// if this message is for a bot, call the client message function...
		if (botMsgFunction)
			(*botMsgFunction)((void *)&iValue, botMsgEdict/*botMsgIndex*/);
	}
	
   #ifndef METAMOD_BUILD
   	(*g_engfuncs.pfnWriteByte)(iValue);
   #else
      RETURN_META (MRES_IGNORED);
   #endif
}
void pfnWriteChar(int iValue)
{
	if (gpGlobals->deathmatch)
	{
		if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnWriteChar: %d\n",iValue); fclose(fp); }
		
		// if this message is for a bot, call the client message function...
		if (botMsgFunction)
			(*botMsgFunction)((void *)&iValue, botMsgEdict/*botMsgIndex*/);
	}
	
   #ifndef METAMOD_BUILD
   	(*g_engfuncs.pfnWriteChar)(iValue);
   #else
      RETURN_META (MRES_IGNORED);
   #endif
}
void pfnWriteShort(int iValue)
{
	if (gpGlobals->deathmatch)
	{
		if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnWriteShort: %d\n",iValue); fclose(fp); }
		
		// if this message is for a bot, call the client message function...
		if (botMsgFunction)
			(*botMsgFunction)((void *)&iValue, botMsgEdict/*botMsgIndex*/);
	}
	
   #ifndef METAMOD_BUILD
   	(*g_engfuncs.pfnWriteShort)(iValue);
   #else
      RETURN_META (MRES_IGNORED);
   #endif
}
void pfnWriteLong(int iValue)
{
	if (gpGlobals->deathmatch)
	{
		if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnWriteLong: %d\n",iValue); fclose(fp); }
		
		// if this message is for a bot, call the client message function...
		if (botMsgFunction)
			(*botMsgFunction)((void *)&iValue, botMsgEdict/*botMsgIndex*/);
	}
	
   #ifndef METAMOD_BUILD
   	(*g_engfuncs.pfnWriteLong)(iValue);
   #else
      RETURN_META (MRES_IGNORED);
   #endif
}
void pfnWriteAngle(float flValue)
{
	if (gpGlobals->deathmatch)
	{
		if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnWriteAngle: %f\n",flValue); fclose(fp); }
		
		// if this message is for a bot, call the client message function...
		if (botMsgFunction)
			(*botMsgFunction)((void *)&flValue, botMsgEdict/*botMsgIndex*/);
	}
	
   #ifndef METAMOD_BUILD
   	(*g_engfuncs.pfnWriteAngle)(flValue);
   #else
      RETURN_META (MRES_IGNORED);
   #endif
}
void pfnWriteCoord(float flValue)
{
	if (gpGlobals->deathmatch)
	{
		if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnWriteCoord: %f\n",flValue); fclose(fp); }
		
		// if this message is for a bot, call the client message function...
		if (botMsgFunction)
			(*botMsgFunction)((void *)&flValue, botMsgEdict/*botMsgIndex*/);
	}
	
   #ifndef METAMOD_BUILD
   	(*g_engfuncs.pfnWriteCoord)(flValue);
   #else
      RETURN_META (MRES_IGNORED);
   #endif
}
void pfnWriteString(const char *sz)
{
	if (gpGlobals->deathmatch)
	{
		if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnWriteString: %s\n",sz); fclose(fp); }
		
		// if this message is for a bot, call the client message function...
		if (botMsgFunction)
			(*botMsgFunction)((void *)sz, botMsgEdict/*botMsgIndex*/);
	}
	
   #ifndef METAMOD_BUILD
   	(*g_engfuncs.pfnWriteString)(sz);
   #else
      RETURN_META (MRES_IGNORED);
   #endif
}
void pfnWriteEntity(int iValue)
{
	if (gpGlobals->deathmatch)
	{
		if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnWriteEntity: %d\n",iValue); fclose(fp); }
		
		// if this message is for a bot, call the client message function...
		if (botMsgFunction)
			(*botMsgFunction)((void *)&iValue, botMsgEdict/*botMsgIndex*/);
	}
	
   #ifndef METAMOD_BUILD
   	(*g_engfuncs.pfnWriteEntity)(iValue);
   #else
      RETURN_META (MRES_IGNORED);
   #endif
}

#ifndef METAMOD_BUILD
   void pfnCVarRegister(cvar_t *pCvar)
   {
	   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnCVarRegister:\n"); fclose(fp); }
	   (*g_engfuncs.pfnCVarRegister)(pCvar);
   }
   float pfnCVarGetFloat(const char *szVarName)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnCVarGetFloat: %s\n",szVarName); fclose(fp); }
	   return (*g_engfuncs.pfnCVarGetFloat)(szVarName);
   }
   const char* pfnCVarGetString(const char *szVarName)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnCVarGetString:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnCVarGetString)(szVarName);
   }
   void pfnCVarSetFloat(const char *szVarName, float flValue)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnCVarSetFloat:\n"); fclose(fp); }
	   (*g_engfuncs.pfnCVarSetFloat)(szVarName, flValue);
   }
   void pfnCVarSetString(const char *szVarName, const char *szValue)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnCVarSetString:\n"); fclose(fp); }
	   (*g_engfuncs.pfnCVarSetString)(szVarName, szValue);
   }
   void pfnEngineFprintf( void *pfile, char *szFmt, ... )
   {
	   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnEngineFprintf:\n"); fclose(fp); }
	   
	   va_list argptr;
	   static char string[1024];
	   
	   va_start (argptr, szFmt);
	   vsprintf (string, szFmt, argptr);
	   va_end (argptr);
	   
	   (*g_engfuncs.pfnEngineFprintf) (pfile, string);
   }
   void pfnAlertMessage( ALERT_TYPE atype, char *szFmt, ... )
   {
	   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnAlertMessage:\n"); fclose(fp); }
	   
	   va_list argptr;
	   static char string[1024];
	   
	   va_start (argptr, szFmt);
	   vsprintf (string, szFmt, argptr);
	   va_end (argptr);
	   
	   (*g_engfuncs.pfnAlertMessage)(atype, string);
   }
   void* pfnPvAllocEntPrivateData(edict_t *pEdict, int32 cb)
   {
	   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnPvAllocEntPrivateData:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnPvAllocEntPrivateData)(pEdict, cb);
   }
   void* pfnPvEntPrivateData(edict_t *pEdict)
   {
	   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnPvEntPrivateData:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnPvEntPrivateData)(pEdict);
   }
   void pfnFreeEntPrivateData(edict_t *pEdict)
   {
	   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnFreeEntPrivateData:\n"); fclose(fp); }
	   (*g_engfuncs.pfnFreeEntPrivateData)(pEdict);
   }
   const char* pfnSzFromIndex(int iString)
   {
	   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnSzFromIndex:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnSzFromIndex)(iString);
   }
   int pfnAllocString(const char *szValue)
   {
	   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnAllocString:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnAllocString)(szValue);
   }
   entvars_t* pfnGetVarsOfEnt(edict_t *pEdict)
   {
	   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnGetVarsOfEnt:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnGetVarsOfEnt)(pEdict);
   }
   edict_t* pfnPEntityOfEntOffset(int iEntOffset)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnPEntityOfEntOffset:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnPEntityOfEntOffset)(iEntOffset);
   }
   int pfnEntOffsetOfPEntity(const edict_t *pEdict)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnEntOffsetOfPEntity: %x\n",pEdict); fclose(fp); }
	   return (*g_engfuncs.pfnEntOffsetOfPEntity)(pEdict);
   }
   int pfnIndexOfEdict(const edict_t *pEdict)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnIndexOfEdict: %x\n",pEdict); fclose(fp); }
	   return (*g_engfuncs.pfnIndexOfEdict)(pEdict);
   }
   edict_t* pfnPEntityOfEntIndex(int iEntIndex)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnPEntityOfEntIndex:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnPEntityOfEntIndex)(iEntIndex);
   }
   edict_t* pfnFindEntityByVars(entvars_t* pvars)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnFindEntityByVars:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnFindEntityByVars)(pvars);
   }
   void* pfnGetModelPtr(edict_t* pEdict)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnGetModelPtr: %x\n",pEdict); fclose(fp); }
	   return (*g_engfuncs.pfnGetModelPtr)(pEdict);
   }
#endif

int pfnRegUserMsg(const char *pszName, int iSize)
{
	int msg;
	
	msg = (*g_engfuncs.pfnRegUserMsg)(pszName, iSize);
	
	if (gpGlobals->deathmatch)
	{
/*
#ifdef _DEBUG
		fp=fopen("bot.txt","a"); fprintf(fp,"pfnRegUserMsg: pszName=%s msg=%d\n",pszName,msg); fclose(fp);
#endif
*/		
		if (strcmp(pszName, "WeaponList") == 0)
			message_WeaponList = msg;
		else if (strcmp(pszName, "CurWeapon") == 0)
			message_CurWeapon = msg;
		else if (strcmp(pszName, "AmmoX") == 0)
			message_AmmoX = msg;
		else if (strcmp(pszName, "AmmoPickup") == 0)
			message_AmmoPickup = msg;
		else if (strcmp(pszName, "Damage") == 0)
			message_Damage = msg;
		else if (strcmp(pszName, "DeathMsg") == 0)
			message_DeathMsg = msg;
		else if (strcmp(pszName, "ScreenFade") == 0)
			message_ScreenFade = msg;
		else if (strcmp(pszName, "ItemPickup") == 0)
			message_ItemPickup = msg;
		else if (strcmp(pszName, "WeapPickup") == 0)
			message_WeapPickup = msg;
		else if (strcmp(pszName, "Battery") == 0)
			message_Battery = msg;
		
		if (mod_id == SI_DLL)
		{
			if (strcmp(pszName, "TeamCash") == 0)
				message_TeamCash = msg;
			else if (strcmp(pszName, "SciCount") == 0)
				message_SciCount = msg;
			else if (strcmp(pszName, "CarryInfo") == 0)
				message_CarryInfo = msg;
			else if (strcmp(pszName, "Goal") == 0)
				message_Goal = msg;
			else if (strcmp(pszName, "Notice") == 0)
				message_Notice = msg;
			else if (strcmp(pszName, "VoteInfo") == 0)
				message_VoteInfo = msg;
		}
	}

   #ifndef METAMOD_BUILD
	   return msg;
   #else
      RETURN_META_VALUE (MRES_SUPERCEDE, msg);
   #endif
}

#ifndef METAMOD_BUILD
   void pfnAnimationAutomove(const edict_t* pEdict, float flTime)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnAnimationAutomove:\n"); fclose(fp); }
	   (*g_engfuncs.pfnAnimationAutomove)(pEdict, flTime);
   }
   void pfnGetBonePosition(const edict_t* pEdict, int iBone, float *rgflOrigin, float *rgflAngles )
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnGetBonePosition:\n"); fclose(fp); }
	   (*g_engfuncs.pfnGetBonePosition)(pEdict, iBone, rgflOrigin, rgflAngles);
   }
   uint32 pfnFunctionFromName( const char *pName )
   {
	   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnFunctionFromName:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnFunctionFromName)(pName);
   }
   const char *pfnNameForFunction( uint32 function )
   {
	   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnNameForFunction:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnNameForFunction)(function);
   }
   void pfnClientPrintf( edict_t* pEdict, PRINT_TYPE ptype, const char *szMsg )
   {
	   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnClientPrintf:\n"); fclose(fp); }
	   (*g_engfuncs.pfnClientPrintf)(pEdict, ptype, szMsg);
   }
   void pfnServerPrint( const char *szMsg )
   {
	   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnServerPrint: %s\n",szMsg); fclose(fp); }
	   (*g_engfuncs.pfnServerPrint)(szMsg);
   }
   void pfnGetAttachment(const edict_t *pEdict, int iAttachment, float *rgflOrigin, float *rgflAngles )
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnGetAttachment:\n"); fclose(fp); }
	   (*g_engfuncs.pfnGetAttachment)(pEdict, iAttachment, rgflOrigin, rgflAngles);
   }
   void pfnCRC32_Init(CRC32_t *pulCRC)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnCRC32_Init:\n"); fclose(fp); }
	   (*g_engfuncs.pfnCRC32_Init)(pulCRC);
   }
   void pfnCRC32_ProcessBuffer(CRC32_t *pulCRC, void *p, int len)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnCRC32_ProcessBuffer:\n"); fclose(fp); }
	   (*g_engfuncs.pfnCRC32_ProcessBuffer)(pulCRC, p, len);
   }
   void pfnCRC32_ProcessByte(CRC32_t *pulCRC, unsigned char ch)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnCRC32_ProcessByte:\n"); fclose(fp); }
	   (*g_engfuncs.pfnCRC32_ProcessByte)(pulCRC, ch);
   }
   CRC32_t pfnCRC32_Final(CRC32_t pulCRC)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnCRC32_Final:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnCRC32_Final)(pulCRC);
   }
   int32 pfnRandomLong(int32 lLow, int32 lHigh)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnRandomLong: lLow=%d lHigh=%d\n",lLow,lHigh); fclose(fp); }
	   return (*g_engfuncs.pfnRandomLong)(lLow, lHigh);
   }
   float pfnRandomFloat(float flLow, float flHigh)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnRandomFloat:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnRandomFloat)(flLow, flHigh);
   }
   void pfnSetView(const edict_t *pClient, const edict_t *pViewent )
   {
	   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnSetView:\n"); fclose(fp); }
	   (*g_engfuncs.pfnSetView)(pClient, pViewent);
   }
   float pfnTime( void )
   {
	   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnTime:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnTime)();
   }
   void pfnCrosshairAngle(const edict_t *pClient, float pitch, float yaw)
   {
	   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnCrosshairAngle:\n"); fclose(fp); }
	   (*g_engfuncs.pfnCrosshairAngle)(pClient, pitch, yaw);
   }
   byte *pfnLoadFileForMe(char *filename, int *pLength)
   {
	   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnLoadFileForMe: filename=%s\n",filename); fclose(fp); }
	   return (*g_engfuncs.pfnLoadFileForMe)(filename, pLength);
   }
   void pfnFreeFile(void *buffer)
   {
	   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnFreeFile:\n"); fclose(fp); }
	   (*g_engfuncs.pfnFreeFile)(buffer);
   }
   void pfnEndSection(const char *pszSectionName)
   {
	   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnEndSection:\n"); fclose(fp); }
	   (*g_engfuncs.pfnEndSection)(pszSectionName);
   }
   int pfnCompareFileTime(char *filename1, char *filename2, int *iCompare)
   {
	   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnCompareFileTime:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnCompareFileTime)(filename1, filename2, iCompare);
   }
   void pfnGetGameDir(char *szGetGameDir)
   {
	   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnGetGameDir:\n"); fclose(fp); }
	   (*g_engfuncs.pfnGetGameDir)(szGetGameDir);
   }
   void pfnCvar_RegisterVariable(cvar_t *variable)
   {
	   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnCvar_RegisterVariable:\n"); fclose(fp); }
	   (*g_engfuncs.pfnCvar_RegisterVariable)(variable);
   }
   void pfnFadeClientVolume(const edict_t *pEdict, int fadePercent, int fadeOutSeconds, int holdTime, int fadeInSeconds)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnFadeClientVolume:\n"); fclose(fp); }
	   (*g_engfuncs.pfnFadeClientVolume)(pEdict, fadePercent, fadeOutSeconds, holdTime, fadeInSeconds);
   }
#endif

void pfnSetClientMaxspeed(const edict_t *pEdict, float fNewMaxspeed)
{
	if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnSetClientMaxspeed: edict=%x %f\n",pEdict,fNewMaxspeed); fclose(fp); }
	
	// change our max speed to whatever the mod tells us it is
	bot_t *pBot = UTIL_GetBotPointer(const_cast<edict_t*>(pEdict));
	if (pBot)
		pBot->f_max_speed = fNewMaxspeed;/* * speed_mod[pBot->bot_skill];*/

   #ifndef METAMOD_BUILD
	   (*g_engfuncs.pfnSetClientMaxspeed)(pEdict, fNewMaxspeed);
   #else
      RETURN_META (MRES_IGNORED);
   #endif
}

#ifndef METAMOD_BUILD
   edict_t * pfnCreateFakeClient(const char *netname)
   {
	   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnCreateFakeClient:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnCreateFakeClient)(netname);
   }
   void pfnRunPlayerMove(edict_t *fakeclient, const float *viewangles, float forwardmove, float sidemove, float upmove, unsigned short buttons, byte impulse, byte msec )
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnRunPlayerMove:\n"); fclose(fp); }
	   (*g_engfuncs.pfnRunPlayerMove)(fakeclient, viewangles, forwardmove, sidemove, upmove, buttons, impulse, msec);
   }
   int pfnNumberOfEntities(void)
   {
	   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnNumberOfEntities:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnNumberOfEntities)();
   }
   char* pfnGetInfoKeyBuffer(edict_t *e)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnGetInfoKeyBuffer:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnGetInfoKeyBuffer)(e);
   }
   char* pfnInfoKeyValue(char *infobuffer, char *key)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnInfoKeyValue: %s %s\n",infobuffer,key); fclose(fp); }
	   return (*g_engfuncs.pfnInfoKeyValue)(infobuffer, key);
   }
   void pfnSetKeyValue(char *infobuffer, char *key, char *value)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnSetKeyValue: %s %s\n",key,value); fclose(fp); }
	   (*g_engfuncs.pfnSetKeyValue)(infobuffer, key, value);
   }
   void pfnSetClientKeyValue(int clientIndex, char *infobuffer, char *key, char *value)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnSetClientKeyValue: %s %s\n",key,value); fclose(fp); }
	   (*g_engfuncs.pfnSetClientKeyValue)(clientIndex, infobuffer, key, value);
   }
   int pfnIsMapValid(char *filename)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnIsMapValid:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnIsMapValid)(filename);
   }
   void pfnStaticDecal( const float *origin, int decalIndex, int entityIndex, int modelIndex )
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnStaticDecal:\n"); fclose(fp); }
	   (*g_engfuncs.pfnStaticDecal)(origin, decalIndex, entityIndex, modelIndex);
   }
   int pfnPrecacheGeneric(char* s)
   {
	   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnPrecacheGeneric: %s\n",s); fclose(fp); }
	   return (*g_engfuncs.pfnPrecacheGeneric)(s);
   }
   int pfnGetPlayerUserId(edict_t *e )
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnGetPlayerUserId: %x\n",e); fclose(fp); }
	   return (*g_engfuncs.pfnGetPlayerUserId)(e);
   }
   void pfnBuildSoundMsg(edict_t *entity, int channel, const char *sample, /*int*/float volume, float attenuation, int fFlags, int pitch, int msg_dest, int msg_type, const float *pOrigin, edict_t *ed)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnBuildSoundMsg:\n"); fclose(fp); }
	   (*g_engfuncs.pfnBuildSoundMsg)(entity, channel, sample, volume, attenuation, fFlags, pitch, msg_dest, msg_type, pOrigin, ed);
   }
   int pfnIsDedicatedServer(void)
   {
	   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnIsDedicatedServer:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnIsDedicatedServer)();
   }
   cvar_t* pfnCVarGetPointer(const char *szVarName)
   {
	   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnCVarGetPointer: %s\n",szVarName); fclose(fp); }
	   return (*g_engfuncs.pfnCVarGetPointer)(szVarName);
   }
   unsigned int pfnGetPlayerWONId(edict_t *e)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnGetPlayerWONId: %x\n",e); fclose(fp); }
	   return (*g_engfuncs.pfnGetPlayerWONId)(e);
   }


   // new stuff for SDK 2.0

   void pfnInfo_RemoveKey(char *s, const char *key)
   {
	   //  if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnInfo_RemoveKey:\n"); fclose(fp); }
	   (*g_engfuncs.pfnInfo_RemoveKey)(s, key);
   }
   const char *pfnGetPhysicsKeyValue(const edict_t *pClient, const char *key)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnGetPhysicsKeyValue:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnGetPhysicsKeyValue)(pClient, key);
   }
   void pfnSetPhysicsKeyValue(const edict_t *pClient, const char *key, const char *value)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnSetPhysicsKeyValue:\n"); fclose(fp); }
	   (*g_engfuncs.pfnSetPhysicsKeyValue)(pClient, key, value);
   }
   const char *pfnGetPhysicsInfoString(const edict_t *pClient)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnGetPhysicsInfoString:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnGetPhysicsInfoString)(pClient);
   }
#endif

unsigned short pfnPrecacheEvent(int type, const char *psz)
{
	//  if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnPrecacheEvent:\n"); fclose(fp); }
	unsigned short event = (*g_engfuncs.pfnPrecacheEvent)(type, psz);
	if (mod_id == SI_DLL)
	{
		// is this the vomit message
		if (FStrEq(psz, "events/vomit.sc"))
			g_usVomit = event;
	}

   #ifndef METAMOD_BUILD
	   return event;
   #else
      RETURN_META_VALUE (MRES_SUPERCEDE, event);
   #endif
}
void pfnPlaybackEvent(int flags, const edict_t *pInvoker, unsigned short eventindex, float delay,
					  float *origin, float *angles, float fparam1,float fparam2, int iparam1, int iparam2, int bparam1, int bparam2)
{
	//  if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnPlaybackEvent:\n"); fclose(fp); }
	if (mod_id == SI_DLL)
	{	// vomit event
		if (eventindex == g_usVomit)
		{
			g_flVomiting[ENTINDEX(const_cast<edict_t*>(pInvoker))-1] = gpGlobals->time;
		}
	}

   #ifndef METAMOD_BUILD
	   (*g_engfuncs.pfnPlaybackEvent)(flags, pInvoker, eventindex, delay, origin, angles, fparam1, fparam2, iparam1, iparam2, bparam1, bparam2);
   #else
      RETURN_META (MRES_IGNORED);
   #endif
}

#ifndef METAMOD_BUILD
   unsigned char *pfnSetFatPVS(float *org)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnSetFatPVS:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnSetFatPVS)(org);
   }
   unsigned char *pfnSetFatPAS(float *org)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnSetFatPAS:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnSetFatPAS)(org);
   }
   int pfnCheckVisibility(const edict_t *entity, unsigned char *pset)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnCheckVisibility:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnCheckVisibility)(entity, pset);
   }
   void pfnDeltaSetField(struct delta_s *pFields, const char *fieldname)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnDeltaSetField:\n"); fclose(fp); }
	   (*g_engfuncs.pfnDeltaSetField)(pFields, fieldname);
   }
   void pfnDeltaUnsetField(struct delta_s *pFields, const char *fieldname)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnDeltaUnsetField:\n"); fclose(fp); }
	   (*g_engfuncs.pfnDeltaUnsetField)(pFields, fieldname);
   }
   void pfnDeltaAddEncoder(char *name, void (*conditionalencode)( struct delta_s *pFields, const unsigned char *from, const unsigned char *to))
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnDeltaAddEncoder:\n"); fclose(fp); }
	   (*g_engfuncs.pfnDeltaAddEncoder)(name, conditionalencode);
   }
   int pfnGetCurrentPlayer(void)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnGetCurrentPlayer:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnGetCurrentPlayer)();
   }
   int pfnCanSkipPlayer(const edict_t *player)
   {
	   //  if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnCanSkipPlayer:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnCanSkipPlayer)(player);
   }
   int pfnDeltaFindField(struct delta_s *pFields, const char *fieldname)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnDeltaFindField:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnDeltaFindField)(pFields, fieldname);
   }
   void pfnDeltaSetFieldByIndex(struct delta_s *pFields, int fieldNumber)
   {
	   //  if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnDeltaSetFieldByIndex:\n"); fclose(fp); }
	   (*g_engfuncs.pfnDeltaSetFieldByIndex)(pFields, fieldNumber);
   }
   void pfnDeltaUnsetFieldByIndex(struct delta_s *pFields, int fieldNumber)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnDeltaUnsetFieldByIndex:\n"); fclose(fp); }
	   (*g_engfuncs.pfnDeltaUnsetFieldByIndex)(pFields, fieldNumber);
   }
   void pfnSetGroupMask(int mask, int op)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnSetGroupMask:\n"); fclose(fp); }
	   (*g_engfuncs.pfnSetGroupMask)(mask, op);
   }
   int pfnCreateInstancedBaseline(int classname, struct entity_state_s *baseline)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnCreateInstancedBaseline:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnCreateInstancedBaseline)(classname, baseline);
   }
   void pfnCvar_DirectSet(struct cvar_s *var, char *value)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnCvar_DirectSet:\n"); fclose(fp); }
	   (*g_engfuncs.pfnCvar_DirectSet)(var, value);
   }
   void pfnForceUnmodified(FORCE_TYPE type, float *mins, float *maxs, const char *filename)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnForceUnmodified:\n"); fclose(fp); }
	   (*g_engfuncs.pfnForceUnmodified)(type, mins, maxs, filename);
   }
   void pfnGetPlayerStats(const edict_t *pClient, int *ping, int *packet_loss)
   {
	   //  if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnGetPlayerStats:\n"); fclose(fp); }
	   (*g_engfuncs.pfnGetPlayerStats)(pClient, ping, packet_loss);
   }


   // new stuff for SDK 2.3

   void pfnAddServerCommand( char *cmd_name, void (*function) (void) )
   {
	   //  if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnAddServerCommand:\n"); fclose(fp); }
	   (*g_engfuncs.pfnAddServerCommand)(cmd_name, function);
   }
   qboolean pfnVoice_GetClientListening(int iReceiver, int iSender)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnVoice_GetClientListening:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnVoice_GetClientListening)(iReceiver, iSender);
   }
   qboolean pfnVoice_SetClientListening(int iReceiver, int iSender, qboolean bListen)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnVoice_SetClientListening:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnVoice_SetClientListening)(iReceiver, iSender, bListen);
   }
   const char *pfnGetPlayerAuthId( edict_t *e )
   {
	   //  if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnGetPlayerAuthId:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnGetPlayerAuthId)(e);
   }
   void* pfnSequenceGet( const char* fileName, const char* entryName )
   {
	   //  if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnSequenceGet:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnSequenceGet)(fileName, entryName);
   }
   void* pfnSequencePickSentence( const char* groupName, int pickMethod, int *picked )
   {
	   //  if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnSequencePickSentence:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnSequencePickSentence)(groupName, pickMethod, picked);
   }
   int pfnGetFileSize( char *filename )
   {
	   //  if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnGetFileSize:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnGetFileSize)(filename);
   }
   unsigned int pfnGetApproxWavePlayLen(const char *filepath)
   {
	   //  if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnGetApproxWavePlayLen:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnGetApproxWavePlayLen)(filepath);
   }
   int pfnIsCareerMatch( void )
   {
	   //  if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnIsCareerMatch:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnIsCareerMatch)();
   }
   int pfnGetLocalizedStringLength(const char *label)
   {
	   //  if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnGetLocalizedStringLength:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnGetLocalizedStringLength)(label);
   }
   void pfnRegisterTutorMessageShown(int mid)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnRegisterTutorMessageShown:\n"); fclose(fp); }
	   (*g_engfuncs.pfnRegisterTutorMessageShown)(mid);
   }
   int pfnGetTimesTutorMessageShown(int mid)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnGetTimesTutorMessageShown:\n"); fclose(fp); }
	   return (*g_engfuncs.pfnGetTimesTutorMessageShown)(mid);
   }
   void pfnProcessTutorMessageDecayBuffer(int *buffer, int bufferLength)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnProcessTutorMessageDecayBuffer:\n"); fclose(fp); }
	   (*g_engfuncs.pfnProcessTutorMessageDecayBuffer)(buffer, bufferLength);
   }
   void pfnConstructTutorMessageDecayBuffer(int *buffer, int bufferLength)
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnConstructTutorMessageDecayBuffer:\n"); fclose(fp); }
	   (*g_engfuncs.pfnConstructTutorMessageDecayBuffer)(buffer, bufferLength);
   }
   void pfnResetTutorMessageDecayData( void )
   {
	   //if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnResetTutorMessageDecayData:\n"); fclose(fp); }
	   (*g_engfuncs.pfnResetTutorMessageDecayData)();
   }
#endif


const char *Cmd_Args( void )
{
	if (isFakeClientCommand)
	{
      #ifndef METAMOD_BUILD
			return &g_argv[0];
      #else
         RETURN_META_VALUE (MRES_SUPERCEDE, &g_argv[0]);
      #endif
	}
	else
	{
      #ifndef METAMOD_BUILD
		   return (*g_engfuncs.pfnCmd_Args)();
      #else
         RETURN_META_VALUE (MRES_IGNORED, NULL);
      #endif
	}
}
const char *Cmd_Argv( int argc )
{
	if (isFakeClientCommand)
	{
		if (argc == 0)
		{
         #ifndef METAMOD_BUILD
			   return &g_argv[64];
         #else
            RETURN_META_VALUE (MRES_SUPERCEDE, &g_argv[64]);
         #endif
		}
		else if (argc == 1)
		{
         #ifndef METAMOD_BUILD
			   return &g_argv[128];
         #else
            RETURN_META_VALUE (MRES_SUPERCEDE, &g_argv[128]);
         #endif
		}
		else if (argc == 2)
		{
         #ifndef METAMOD_BUILD
			   return &g_argv[192];
         #else
            RETURN_META_VALUE (MRES_SUPERCEDE, &g_argv[192]);
         #endif
		}
		else
		{
         #ifndef METAMOD_BUILD
			   return NULL;
         #else
            RETURN_META_VALUE (MRES_SUPERCEDE, NULL);
         #endif
		}
	}
	else
	{
      #ifndef METAMOD_BUILD
   		return (*g_engfuncs.pfnCmd_Argv)(argc);
      #else
         RETURN_META_VALUE (MRES_IGNORED, NULL);
      #endif
	}
}
int Cmd_Argc( void )
{
	if (isFakeClientCommand)
	{
      #ifndef METAMOD_BUILD
		   return fake_arg_count;
      #else
         RETURN_META_VALUE (MRES_SUPERCEDE, fake_arg_count);
      #endif
	}
	else
	{
      #ifndef METAMOD_BUILD
		   return (*g_engfuncs.pfnCmd_Argc)();
      #else
         RETURN_META_VALUE (MRES_IGNORED, 0);
      #endif
	}
}


#ifdef METAMOD_BUILD
   C_DLLEXPORT int GetEngineFunctions (enginefuncs_t *pengfuncsFromEngine, int *interfaceVersion)
   {
      // This exported function declares to metamod which functions from the engine interface our
      // DLL will use, by linking each used slot in the meta_engfuncs structure to the address of
      // our actual functions. This way, when metamod will bind any function in any slot of meta_
      // engfuncs, it will land directly in our functions here, just by following the pointer.

      // These functions will be called by metamod BEFORE calling the actual engine function.

      // cleanup the function table
      memset (&meta_engfuncs, 0, sizeof (meta_engfuncs));

      // keep track of pointers to the functions we use in the engine API
      meta_engfuncs.pfnChangeLevel = pfnChangeLevel;
      meta_engfuncs.pfnEmitSound = pfnEmitSound;
      meta_engfuncs.pfnEmitAmbientSound = pfnEmitAmbientSound;
      meta_engfuncs.pfnMessageBegin = pfnMessageBegin;
      meta_engfuncs.pfnWriteByte = pfnWriteByte;
      meta_engfuncs.pfnWriteChar = pfnWriteChar;
      meta_engfuncs.pfnWriteShort = pfnWriteShort;
      meta_engfuncs.pfnWriteLong = pfnWriteLong;
      meta_engfuncs.pfnWriteAngle = pfnWriteAngle;
      meta_engfuncs.pfnWriteCoord = pfnWriteCoord;
      meta_engfuncs.pfnWriteString = pfnWriteString;
      meta_engfuncs.pfnWriteEntity = pfnWriteEntity;
      meta_engfuncs.pfnRegUserMsg = pfnRegUserMsg;
      meta_engfuncs.pfnPrecacheEvent = pfnPrecacheEvent;
      meta_engfuncs.pfnPlaybackEvent = pfnPlaybackEvent;
      meta_engfuncs.pfnSetClientMaxspeed = pfnSetClientMaxspeed;
      meta_engfuncs.pfnCmd_Args = Cmd_Args;
      meta_engfuncs.pfnCmd_Argv = Cmd_Argv;
      meta_engfuncs.pfnCmd_Argc = Cmd_Argc;

      // copy the whole table for metamod to know which functions we are using here
      memcpy (pengfuncsFromEngine, &meta_engfuncs, sizeof (enginefuncs_t));
      return (1); // alright
   }
#endif
