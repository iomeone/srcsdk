//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef ACTIVITYLIST_H
#define ACTIVITYLIST_H
#ifdef _WIN32
#pragma once
#endif

#include <KeyValues.h>

enum Activity : int;

typedef struct activityentry_s activityentry_t;

class CActivityRemap
{
public:

	CActivityRemap()
	{
		pExtraBlock = NULL;
	}

	void SetExtraKeyValueBlock ( KeyValues *pKVBlock )
	{
		pExtraBlock = pKVBlock;
	}

	KeyValues *GetExtraKeyValueBlock ( void ) { return pExtraBlock; }

	Activity 		activity;
	Activity		mappedActivity;

private:

	KeyValues		*pExtraBlock;
};


class CActivityRemapCache
{
public:

	CActivityRemapCache()
	{
	}

	CActivityRemapCache( const CActivityRemapCache& src )
	{
		int c = src.m_cachedActivityRemaps.Count();
		for ( int i = 0; i < c; i++ )
		{
			m_cachedActivityRemaps.AddToTail( src.m_cachedActivityRemaps[ i ] );
		}
	}

	CActivityRemapCache& operator = ( const CActivityRemapCache& src )
	{
		if ( this == &src )
			return *this;

		int c = src.m_cachedActivityRemaps.Count();
		for ( int i = 0; i < c; i++ )
		{
			m_cachedActivityRemaps.AddToTail( src.m_cachedActivityRemaps[ i ] );
		}

		return *this;
	}

	CUtlVector< CActivityRemap > m_cachedActivityRemaps;
};

void UTIL_LoadActivityRemapFile( const char *filename, const char *section, CUtlVector <CActivityRemap> &entries );

//=========================================================
//=========================================================
extern void ActivityList_Init( void );
extern void ActivityList_Free( void );
extern bool ActivityList_RegisterSharedActivity( const char *pszActivityName, int iActivityIndex );
extern Activity ActivityList_RegisterPrivateActivity( const char *pszActivityName );
extern int ActivityList_IndexForName( const char *pszActivityName );
extern const char *ActivityList_NameForIndex( int iActivityIndex );
extern int ActivityList_HighestIndex();

// This macro guarantees that the names of each activity and the constant used to
// reference it in the code are identical.
#define REGISTER_SHARED_ACTIVITY( _n ) ActivityList_RegisterSharedActivity(#_n, _n);
#define REGISTER_PRIVATE_ACTIVITY( _n ) _n = ActivityList_RegisterPrivateActivity( #_n );

// Implemented in shared code
extern void ActivityList_RegisterSharedActivities( void );

class ISaveRestoreOps;
extern ISaveRestoreOps* ActivityDataOps();

#define ACTIVITY_NOT_AVAILABLE		-1

enum Activity : int
{
	ACT_INVALID = -1,			// So we have something more succint to check for than '-1'
	ACT_RESET = 0,				// Set m_Activity to this invalid value to force a reset to m_IdealActivity
	ACT_IDLE,
	ACT_TRANSITION,
	ACT_DIERAGDOLL,
	ACT_DIESIMPLE,
	ACT_DIEBACKWARD,
	ACT_DIEFORWARD,
	ACT_DIEVIOLENT,
	ACT_DIE_HEADSHOT,
	ACT_DIE_CHESTSHOT,
	ACT_DIE_GUTSHOT,
	ACT_DIE_BACKSHOT,
	ACT_DIE_LEFTSIDE,
	ACT_DIE_RIGHTSIDE,
	ACT_DIE_BACKSIDE,
	ACT_DIE_FRONTSIDE,
	ACT_CROUCHIDLE,
	ACT_RUN_CROUCH,
	ACT_WALK,
	ACT_WALK_CROUCH,
	ACT_RUN,
	ACT_RUNTOIDLE,
	ACT_IDLETORUN,
	ACT_TURN,
	ACT_LEAP,
	ACT_HOP,
	ACT_HOVER,
	ACT_SWIM,
	ACT_RANGE_ATTACK1,
	ACT_VM_IDLE,
	ACT_VM_PRIMARYATTACK,
	ACT_VM_SECONDARYATTACK,
	ACT_VM_DRAW,
	ACT_VM_HOLSTER,
	ACT_VM_RELOAD,

	ACT_MP_ATTACK_STAND_PRIMARYFIRE,
	ACT_MP_ATTACK_STAND_SECONDARYFIRE,
	ACT_MP_ATTACK_STAND_GRENADE,
	ACT_MP_RELOAD_CROUCH,
	ACT_MP_RELOAD_SWIM,
	ACT_MP_RELOAD_STAND,
	ACT_MP_RELOAD_CROUCH_LOOP,
	ACT_MP_RELOAD_SWIM_LOOP,
	ACT_MP_RELOAD_STAND_LOOP,
	ACT_MP_RELOAD_CROUCH_END,
	ACT_MP_RELOAD_SWIM_END,
	ACT_MP_RELOAD_STAND_END,
	ACT_MP_GESTURE_FLINCH_CHEST,
	ACT_MP_GESTURE_FLINCH_HEAD,
	ACT_MP_GESTURE_FLINCH_LEFTARM,
	ACT_MP_GESTURE_FLINCH_RIGHTARM,
	ACT_MP_GESTURE_FLINCH_LEFTLEG,
	ACT_MP_GESTURE_FLINCH_RIGHTLEG,
	ACT_MP_VCD,
	ACT_MP_JUMP,
	ACT_MP_CROUCHWALK,
	ACT_MP_CROUCH_IDLE,
	ACT_MP_SWIM,
	ACT_MP_RUN,
	ACT_MP_STAND_IDLE,
	ACT_MP_ATTACK_SWIM_PRIMARYFIRE,
	ACT_MP_ATTACK_SWIM_SECONDARYFIRE,
	ACT_MP_ATTACK_SWIM_GRENADE,
	ACT_MP_DEPLOYED_IDLE,
	ACT_MP_CROUCH_DEPLOYED_IDLE,
	ACT_MP_WALK,
	ACT_MP_SPRINT,

	ACT_PHYSCANNON_ANIMATE,
	ACT_PHYSCANNON_DETACH,
	ACT_PHYSCANNON_ANIMATE_PRE,
	ACT_PHYSCANNON_ANIMATE_POST,

	ACT_OPEN_DOOR,

#ifdef CSTRIKE_DLL
	#include "enum_activity.h"
#endif

	// this is the end of the global activities, private per-monster activities start here.
	LAST_SHARED_ACTIVITY,
};

#endif // ACTIVITYLIST_H
