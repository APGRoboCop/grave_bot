//
// HPB_bot - botman's High Ping Bastard bot
//
// (http://planethalflife.com/botman/)
//
// bot_func.h
//

#ifndef BOT_FUNC_H
#define BOT_FUNC_H


//prototypes of bot functions...

void BotSpawnInit( bot_t *pBot );
void BotCreate( edict_t *pPlayer, const char *arg1, const char *arg2,
                const char *arg3, const char *arg4, const char *arg5 );
void BotStartGame( bot_t *pBot );
int BotInFieldOfView( bot_t *pBot, Vector dest );
bool BotEntityIsVisible( bot_t *pBot, Vector dest );
bool BotEntityIsVisible( bot_t *pBot, edict_t *pEntity );
bool BotEntityIsNotBlocked( bot_t *pBot, Vector dest );
void BotFindItem( bot_t *pBot );
void BotCheckRole( bot_t *pBot );
void BotThink( bot_t *pBot );
void BotListenForFakeSound( bot_t *pBot );
void BotListenForSound(edict_t *pEntity, const char *pszSample, float fVolume);

void BotFixIdealPitch( edict_t *pEdict );
float BotChangePitch( bot_t *pBot, float speed );
void BotFixIdealYaw( edict_t *pEdict );
float BotChangeYaw( bot_t *pBot, float speed );
void BotFixBodyAngles(edict_t *pEdict);
void BotFixViewAngles(edict_t *pEdict);
bool BotFindWaypoint( bot_t *pBot );
bool BotHeadTowardWaypoint( bot_t *pBot );
void BotEvaluateGoal( bot_t *pBot );
void BotEvaluateGoalSI( bot_t *pBot );
int BotFindWaypointGoal( bot_t *pBot );
int BotFindWaypointGoalSI( bot_t *pBot );
void BotOnLadder( bot_t *pBot, float moved_distance );
void BotUnderWater( bot_t *pBot );
void BotUseLift( bot_t *pBot, float moved_distance );
bool BotStuckInCorner( bot_t *pBot );
void BotTurnAtWall( bot_t *pBot, TraceResult *tr );
bool BotCantMoveForward( bot_t *pBot, TraceResult *tr );
bool BotCanJumpUp( bot_t *pBot, bool *bDuckJump );
bool BotCanDuckUnder( bot_t *pBot );
void BotRandomTurn( bot_t *pBot );
bool BotFollowUser( bot_t *pBot );
bool BotCheckWallOnLeft( bot_t *pBot );
bool BotCheckWallOnRight( bot_t *pBot );
void BotAvoidContact( bot_t *pBot );
void BotDodgeCrabs( bot_t *pBot );
bool BotLookForDrop( bot_t *pBot );

void BotCheckTeamplay(void);
edict_t *BotFindEnemy( bot_t *pBot );
int BotGetEnemyWeapon( edict_t *pEnemy );
bool BotShouldEngageEnemy( bot_t *pBot, edict_t *pEnemy );
Vector BotBodyTarget( edict_t *pBotEnemy, bot_t *pBot );
bool BotFireWeapon( Vector v_enemy, bot_t *pBot, int weapon_choice, bool nofire = false);
Vector BotGetLead( bot_t *pBot, edict_t *pEntity, float flProjSpeed);
void BotShootAtEnemy( bot_t *pBot );
void BotAssessGrenades( bot_t *pBot );
bool BotWeaponPrimaryDistance( bot_t *pBot, float distance, int weapon_id );
bool BotWeaponSecondaryDistance( bot_t *pBot, float distance, int weapon_id );
float BotAssessPrimaryAmmo( bot_t *pBot, int weapon_id );
float BotAssessSecondaryAmmo( bot_t *pBot, int weapon_id );

#endif // BOT_FUNC_H

