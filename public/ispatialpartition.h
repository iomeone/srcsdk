//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $Revision: $
// $NoKeywords: $
//=============================================================================//

#ifndef ISPATIALPARTITION_H
#define ISPATIALPARTITION_H

#include "interface.h"

//-----------------------------------------------------------------------------
// forward declarations
//-----------------------------------------------------------------------------

class Vector;
struct Ray_t;
class IHandleEntity;


#define INTERFACEVERSION_SPATIALPARTITION	"SpatialPartition001"

//-----------------------------------------------------------------------------
// These are the various partition lists. Note some are server only, some
// are client only
//-----------------------------------------------------------------------------

enum
{
	PARTITION_ENGINE_SOLID_EDICTS		= (1 << 0),		// every edict_t that isn't SOLID_TRIGGER or SOLID_NOT (and static props)
	PARTITION_ENGINE_TRIGGER_EDICTS		= (1 << 1),		// every edict_t that IS SOLID_TRIGGER
	PARTITION_CLIENT_SOLID_EDICTS		= (1 << 2),
	PARTITION_CLIENT_RESPONSIVE_EDICTS	= (1 << 3),		// these are client-side only objects that respond to being forces, etc.
	PARTITION_ENGINE_NON_STATIC_EDICTS	= (1 << 4),		// everything in solid & trigger except the static props, includes SOLID_NOTs
	PARTITION_CLIENT_STATIC_PROPS		= (1 << 5),
	PARTITION_ENGINE_STATIC_PROPS		= (1 << 6),
	PARTITION_CLIENT_NON_STATIC_EDICTS	= (1 << 7),		// everything except the static props
	PARTITION_CLIENT_TRIGGER_ENTITIES   = (1 << 8),		// client side prediction related triggers
	PARTITION_CLIENT_IK_ATTACHMENT	    = (1 << 9),		// Can be used as an IK attachment
};

// Use this to look for all client edicts.
#define PARTITION_ALL_CLIENT_EDICTS	(		\
	PARTITION_CLIENT_NON_STATIC_EDICTS |	\
	PARTITION_CLIENT_STATIC_PROPS |			\
	PARTITION_CLIENT_RESPONSIVE_EDICTS |	\
	PARTITION_CLIENT_SOLID_EDICTS |			\
	PARTITION_CLIENT_TRIGGER_ENTITIES \
	)


// These are the only handles in the spatial partition that the game is controlling (everything but static props)
// These masks are used to handle updating the dirty spatial partition list in each game DLL
#define PARTITION_CLIENT_GAME_EDICTS (PARTITION_ALL_CLIENT_EDICTS & ~PARTITION_CLIENT_STATIC_PROPS)
#define PARTITION_SERVER_GAME_EDICTS (PARTITION_ENGINE_SOLID_EDICTS|PARTITION_ENGINE_TRIGGER_EDICTS|PARTITION_ENGINE_NON_STATIC_EDICTS)

//-----------------------------------------------------------------------------
// Clients that want to know about all elements within a particular
// volume must inherit from this
//-----------------------------------------------------------------------------

enum IterationRetval_t
{
	ITERATION_CONTINUE = 0,
	ITERATION_STOP,
};


typedef unsigned short SpatialPartitionHandle_t;

// A combination of the PARTITION_ flags above.
typedef int SpatialPartitionListMask_t;	

typedef int SpatialTempHandle_t;


//-----------------------------------------------------------------------------
// Any search in the CSpatialPartition must use this to filter out entities it doesn't want.
// You're forced to use listMasks because it can filter by listMasks really fast. Any other
// filtering can be done by EnumElement.
//-----------------------------------------------------------------------------

class IPartitionEnumerator
{
public:
	virtual IterationRetval_t EnumElement( IHandleEntity *pHandleEntity ) = 0;
	// XXX(johns): This should have a virtual destructor, but would be ABI breaking (non-versioned interface implemented
	//             by the game)
	// virtual ~IPartitionEnumerator(){}
};


//-----------------------------------------------------------------------------
// Installs a callback to call right before a spatial partition query occurs
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// This is the spatial partition manager, groups objects into buckets
//-----------------------------------------------------------------------------
enum
{
	PARTITION_INVALID_HANDLE = (SpatialPartitionHandle_t)~0
};

#include "compat_ispatialpartition.h"

#endif



