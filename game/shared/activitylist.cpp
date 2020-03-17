//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
#include "cbase.h"
#include "activitylist.h"
#include "stringregistry.h"
#include "isaverestore.h"

#include "filesystem.h"
#include <KeyValues.h>
#include "utldict.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern IFileSystem *filesystem;

// NOTE: If CStringRegistry allowed storing arbitrary data, we could just use that.
// in this case we have the "isPrivate" member and the replacement rules 
// (activityIndex can be reused by private activities), so a custom table is necessary
struct activitylist_t
{
	int					activityIndex;
	unsigned short		stringKey;
	short				isPrivate;
};

CUtlVector<activitylist_t> g_ActivityList;

static CUtlDict< CActivityRemapCache, int > m_ActivityRemapDatabase;

// This stores the actual activity names.  Also, the string ID in the registry is simply an index 
// into the g_ActivityList array.
CStringRegistry	g_ActivityStrings;

// this is just here to accelerate adds
static int g_HighestActivity = 0;

int g_nActivityListVersion = 1;


void ActivityList_Init( void )
{
	g_HighestActivity = 0;
}

void ActivityList_Free( void )
{
	g_ActivityStrings.ClearStrings();
	g_ActivityList.Purge();
	m_ActivityRemapDatabase.Purge();

	// So studiohdrs can reindex activity indices
	++g_nActivityListVersion;
}

// add a new activity to the database
activitylist_t *ActivityList_AddActivityEntry( const char *pName, int iActivityIndex, bool isPrivate )
{
	MEM_ALLOC_CREDIT();
	int index = g_ActivityList.AddToTail();
	activitylist_t *pList = &g_ActivityList[index];
	pList->activityIndex = iActivityIndex;
	pList->stringKey = g_ActivityStrings.AddString( pName, index );
	pList->isPrivate = isPrivate;
	
	// UNDONE: This implies that ALL shared activities are added before ANY custom activities
	// UNDONE: Segment these instead?  It's a 32-bit int, how many activities do we need?
	if ( iActivityIndex > g_HighestActivity )
	{
		g_HighestActivity = iActivityIndex;
	}

	return pList;
}

// get the database entry from a string
static activitylist_t *ListFromString( const char *pString )
{
	// just use the string registry to do this search/map
	int stringID = g_ActivityStrings.GetStringID( pString );
	if ( stringID < 0 )
		return NULL;

	return &g_ActivityList[stringID];
}

// Get the database entry for an index
static activitylist_t *ListFromActivity( int activityIndex )
{
	// ugly linear search
	for ( int i = 0; i < g_ActivityList.Size(); i++ )
	{
		if ( g_ActivityList[i].activityIndex == activityIndex )
		{
			return &g_ActivityList[i];
		}
	}

	return NULL;
}

bool ActivityList_RegisterSharedActivity( const char *pszActivityName, int iActivityIndex )
{
	// UNDONE: Do we want to do these checks when not in developer mode? or maybe DEBUG only?
	// They really only matter when you change the list of code controlled activities.  IDs
	// for content controlled activities never collide because they are generated.

	// technically order isn't dependent, but it's too damn easy to forget to add new ACT_'s to all three lists.
	static int lastActivityIndex = -1;
	Assert( iActivityIndex < LAST_SHARED_ACTIVITY && (iActivityIndex == lastActivityIndex + 1 || iActivityIndex == 0) );
	lastActivityIndex = iActivityIndex;

	// first, check to make sure the slot we're asking for is free. It must be for 
	// a shared activity.
	activitylist_t *pList = ListFromString( pszActivityName );
	if ( !pList )
	{
		pList = ListFromActivity( iActivityIndex );
	}

	if ( pList )
	{
		Warning( "***\nShared activity collision! %s<->%s\n***\n", pszActivityName, g_ActivityStrings.GetStringForKey( pList->stringKey ) );
		Assert(0);
		return false;
	}
	// ----------------------------------------------------------------

	ActivityList_AddActivityEntry( pszActivityName, iActivityIndex, false );
	return true;
}


Activity ActivityList_RegisterPrivateActivity( const char *pszActivityName )
{
	activitylist_t *pList = ListFromString( pszActivityName );
	if ( pList )
	{
		// this activity is already in the list. If the activity we collided with is also private, 
		// then the collision is OK. Otherwise, it's a bug.
		if ( pList->isPrivate )
		{
			return (Activity)pList->activityIndex;
		}
		else
		{
			// this private activity collides with a shared activity. That is not allowed.
			Warning( "***\nShared<->Private Activity collision!\n***\n" );
			Assert(0);
			return ACT_INVALID;
		}
	}

	pList = ActivityList_AddActivityEntry( pszActivityName, g_HighestActivity+1, true );
	return (Activity)pList->activityIndex;
}

// Get the index for a given activity name
// Done at load time for all models
int ActivityList_IndexForName( const char *pszActivityName )
{
	// this is a fast O(lgn) search (actually does 2 O(lgn) searches)
	activitylist_t *pList = ListFromString( pszActivityName );

	if ( pList )
	{
		return pList->activityIndex;
	}

	return kActivityLookup_Missing;
}

// Get the name for a given index
// This should only be used in debug code, it does a linear search
// But at least it only compares integers
const char *ActivityList_NameForIndex( int activityIndex )
{
	activitylist_t *pList = ListFromActivity( activityIndex );
	if ( pList )
	{
		return g_ActivityStrings.GetStringForKey( pList->stringKey );
	}
	return NULL;
}

void ActivityList_RegisterSharedActivities( void )
{
	REGISTER_SHARED_ACTIVITY( ACT_RESET );
	REGISTER_SHARED_ACTIVITY( ACT_IDLE );
	REGISTER_SHARED_ACTIVITY( ACT_TRANSITION );
	REGISTER_SHARED_ACTIVITY( ACT_DIERAGDOLL );
	REGISTER_SHARED_ACTIVITY( ACT_DIESIMPLE );
	REGISTER_SHARED_ACTIVITY( ACT_DIEBACKWARD );
	REGISTER_SHARED_ACTIVITY( ACT_DIEFORWARD );
	REGISTER_SHARED_ACTIVITY( ACT_DIEVIOLENT );
	REGISTER_SHARED_ACTIVITY( ACT_DIE_HEADSHOT );
	REGISTER_SHARED_ACTIVITY( ACT_DIE_CHESTSHOT );
	REGISTER_SHARED_ACTIVITY( ACT_DIE_GUTSHOT );
	REGISTER_SHARED_ACTIVITY( ACT_DIE_BACKSHOT );
	REGISTER_SHARED_ACTIVITY( ACT_DIE_LEFTSIDE );
	REGISTER_SHARED_ACTIVITY( ACT_DIE_RIGHTSIDE );
	REGISTER_SHARED_ACTIVITY( ACT_DIE_BACKSIDE );
	REGISTER_SHARED_ACTIVITY( ACT_DIE_FRONTSIDE );
	REGISTER_SHARED_ACTIVITY( ACT_CROUCHIDLE );
	REGISTER_SHARED_ACTIVITY( ACT_RUN_CROUCH );
	REGISTER_SHARED_ACTIVITY( ACT_WALK );
	REGISTER_SHARED_ACTIVITY( ACT_WALK_CROUCH );
	REGISTER_SHARED_ACTIVITY( ACT_RUN );
	REGISTER_SHARED_ACTIVITY( ACT_RUNTOIDLE );
	REGISTER_SHARED_ACTIVITY( ACT_IDLETORUN );
	REGISTER_SHARED_ACTIVITY( ACT_TURN );
	REGISTER_SHARED_ACTIVITY( ACT_LEAP );
	REGISTER_SHARED_ACTIVITY( ACT_HOP );
	REGISTER_SHARED_ACTIVITY( ACT_HOVER );
	REGISTER_SHARED_ACTIVITY( ACT_SWIM );
	REGISTER_SHARED_ACTIVITY( ACT_RANGE_ATTACK1 );
	REGISTER_SHARED_ACTIVITY( ACT_VM_IDLE );
	REGISTER_SHARED_ACTIVITY( ACT_VM_PRIMARYATTACK );
	REGISTER_SHARED_ACTIVITY( ACT_VM_SECONDARYATTACK );
	REGISTER_SHARED_ACTIVITY( ACT_VM_DRAW );
	REGISTER_SHARED_ACTIVITY( ACT_VM_HOLSTER );
	REGISTER_SHARED_ACTIVITY( ACT_VM_RELOAD );

	REGISTER_SHARED_ACTIVITY( ACT_MP_ATTACK_STAND_PRIMARYFIRE );
	REGISTER_SHARED_ACTIVITY( ACT_MP_ATTACK_STAND_SECONDARYFIRE );
	REGISTER_SHARED_ACTIVITY( ACT_MP_ATTACK_STAND_GRENADE );
	REGISTER_SHARED_ACTIVITY( ACT_MP_RELOAD_CROUCH );
	REGISTER_SHARED_ACTIVITY( ACT_MP_RELOAD_SWIM );
	REGISTER_SHARED_ACTIVITY( ACT_MP_RELOAD_STAND );
	REGISTER_SHARED_ACTIVITY( ACT_MP_RELOAD_CROUCH_LOOP );
	REGISTER_SHARED_ACTIVITY( ACT_MP_RELOAD_SWIM_LOOP );
	REGISTER_SHARED_ACTIVITY( ACT_MP_RELOAD_STAND_LOOP );
	REGISTER_SHARED_ACTIVITY( ACT_MP_RELOAD_CROUCH_END );
	REGISTER_SHARED_ACTIVITY( ACT_MP_RELOAD_SWIM_END );
	REGISTER_SHARED_ACTIVITY( ACT_MP_RELOAD_STAND_END );
	REGISTER_SHARED_ACTIVITY( ACT_MP_GESTURE_FLINCH_CHEST );
	REGISTER_SHARED_ACTIVITY( ACT_MP_GESTURE_FLINCH_HEAD );
	REGISTER_SHARED_ACTIVITY( ACT_MP_GESTURE_FLINCH_LEFTARM );
	REGISTER_SHARED_ACTIVITY( ACT_MP_GESTURE_FLINCH_RIGHTARM );
	REGISTER_SHARED_ACTIVITY( ACT_MP_GESTURE_FLINCH_LEFTLEG );
	REGISTER_SHARED_ACTIVITY( ACT_MP_GESTURE_FLINCH_RIGHTLEG );
	REGISTER_SHARED_ACTIVITY( ACT_MP_VCD );
	REGISTER_SHARED_ACTIVITY( ACT_MP_JUMP );
	REGISTER_SHARED_ACTIVITY( ACT_MP_CROUCHWALK );
	REGISTER_SHARED_ACTIVITY( ACT_MP_CROUCH_IDLE );
	REGISTER_SHARED_ACTIVITY( ACT_MP_SWIM );
	REGISTER_SHARED_ACTIVITY( ACT_MP_RUN );
	REGISTER_SHARED_ACTIVITY( ACT_MP_STAND_IDLE );
	REGISTER_SHARED_ACTIVITY( ACT_MP_ATTACK_SWIM_PRIMARYFIRE );
	REGISTER_SHARED_ACTIVITY( ACT_MP_ATTACK_SWIM_SECONDARYFIRE );
	REGISTER_SHARED_ACTIVITY( ACT_MP_ATTACK_SWIM_GRENADE );
	REGISTER_SHARED_ACTIVITY( ACT_MP_DEPLOYED_IDLE );
	REGISTER_SHARED_ACTIVITY( ACT_MP_CROUCH_DEPLOYED_IDLE );
	REGISTER_SHARED_ACTIVITY( ACT_MP_WALK );
	REGISTER_SHARED_ACTIVITY( ACT_MP_SPRINT );
	
	REGISTER_SHARED_ACTIVITY( ACT_PHYSCANNON_ANIMATE );
	REGISTER_SHARED_ACTIVITY( ACT_PHYSCANNON_DETACH );
	REGISTER_SHARED_ACTIVITY( ACT_PHYSCANNON_ANIMATE_PRE );
	REGISTER_SHARED_ACTIVITY( ACT_PHYSCANNON_ANIMATE_POST );

	REGISTER_SHARED_ACTIVITY( ACT_OPEN_DOOR );
	
#ifdef CSTRIKE_DLL
	#include "register_activity.h"
#endif

	AssertMsg( g_HighestActivity == LAST_SHARED_ACTIVITY - 1, "Not all activities from activitylist.h registered in activitylist.cpp" ); 
} 

// HACKHACK: Keep backwards compatibility on broken activities temporarily
#define ACTIVITY_FILE_TAG 0x80800000

class CActivityDataOps : public CDefSaveRestoreOps
{
public:
	// save data type interface
	virtual void Save( const SaveRestoreFieldInfo_t &fieldInfo, ISave *pSave ) 
	{
		int activityIndex = *((int *)fieldInfo.pField);
		const char *pActivityName = ActivityList_NameForIndex( activityIndex );
		if ( !pActivityName )
		{
			AssertOnce( activityIndex == -1 ); // FIXME: whatever activity this was, it's now being saved out as ACT_RESET
			pActivityName = ActivityList_NameForIndex( 0 );
		}
		int len = strlen(pActivityName) + 1;
		
		// Use the high 16-bits of this int to signify this file format
		// this makes this backwards compatible.
		// UNDONE: Remove after playtest save files are no longer needed
		len |= ACTIVITY_FILE_TAG;
		pSave->WriteInt( &len );
		pSave->WriteString( pActivityName );
	}

	virtual void Restore( const SaveRestoreFieldInfo_t &fieldInfo, IRestore *pRestore ) 
	{
		char nameBuf[1024];

		int *pActivityIndex = (int *)fieldInfo.pField;

		int nameLen = pRestore->ReadInt();
		if ( (nameLen & 0xFFFF0000) != ACTIVITY_FILE_TAG )
		{
			// old save file, this is an index, not a name
			*pActivityIndex = nameLen;
			return;
		}
		nameLen &= 0xFFFF;
		pRestore->ReadString( nameBuf, sizeof(nameBuf), nameLen );
		*pActivityIndex = ActivityList_IndexForName( nameBuf );
		if ( *pActivityIndex < 0 )
		{
			*pActivityIndex = 0;
		}
	}

	virtual bool IsEmpty( const SaveRestoreFieldInfo_t &fieldInfo ) 
	{ 
		int *pActivityIndex = (int *)fieldInfo.pField;
		return (*pActivityIndex == 0);
	}

	virtual void MakeEmpty( const SaveRestoreFieldInfo_t &fieldInfo ) 
	{
		int *pActivityIndex = (int *)fieldInfo.pField;
		*pActivityIndex = 0;
	}
};

static CActivityDataOps g_ActivityDataOps;

ISaveRestoreOps* ActivityDataOps()
{
	return &g_ActivityDataOps;
}




void UTIL_LoadActivityRemapFile( const char *filename, const char *section, CUtlVector <CActivityRemap> &entries )
{
	int iIndex = m_ActivityRemapDatabase.Find( filename );

	if ( iIndex != m_ActivityRemapDatabase.InvalidIndex() )
	{
		CActivityRemapCache *actRemap = &m_ActivityRemapDatabase[iIndex];
		entries.AddVectorToTail( actRemap->m_cachedActivityRemaps );
		return;
	}

	KeyValues *pkvFile = new KeyValues( section );

	if ( pkvFile->LoadFromFile( filesystem, filename, NULL ) )
	{
		KeyValues *pTestKey = pkvFile->GetFirstSubKey();

		CActivityRemapCache actRemap;

		while ( pTestKey )
		{
			Activity ActBase = (Activity)ActivityList_IndexForName( pTestKey->GetName() );

			if ( ActBase != ACT_INVALID )
			{
				KeyValues *pRemapKey = pTestKey->GetFirstSubKey();

				CActivityRemap actMap;
				actMap.activity = ActBase;

				while ( pRemapKey )
				{
					const char *pKeyName = pRemapKey->GetName();
					const char *pKeyValue = pRemapKey->GetString();

					if ( !stricmp( pKeyName, "remapactivity" ) )
					{
						Activity Act = (Activity)ActivityList_IndexForName( pKeyValue );

						if ( Act == ACT_INVALID )
						{
							actMap.mappedActivity = ActivityList_RegisterPrivateActivity( pKeyValue );
						}
						else
						{
							actMap.mappedActivity = Act;
						}
					}
					else if ( !stricmp( pKeyName, "extra" ) )
					{
						actMap.SetExtraKeyValueBlock( pRemapKey->MakeCopy() );
					}

					pRemapKey = pRemapKey->GetNextKey();
				}

				entries.AddToTail( actMap );
			}

			pTestKey = pTestKey->GetNextKey();
		}

		actRemap.m_cachedActivityRemaps.AddVectorToTail( entries );
		m_ActivityRemapDatabase.Insert( filename, actRemap );
	}
}

int ActivityList_HighestIndex()
{
	return g_HighestActivity;
}
