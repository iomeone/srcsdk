//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef EVENTLIST_H
#define EVENTLIST_H
#ifdef _WIN32
#pragma once
#endif

#define AE_TYPE_SERVER			( 1 << 0 )
#define AE_TYPE_SCRIPTED		( 1 << 1 )		// see scriptevent.h
#define AE_TYPE_SHARED			( 1 << 2 )
#define AE_TYPE_WEAPON			( 1 << 3 )
#define AE_TYPE_CLIENT			( 1 << 4 )
#define AE_TYPE_FACEPOSER		( 1 << 5 )

#define AE_TYPE_NEWEVENTSYSTEM  ( 1 << 10 ) //Temporary flag.

#define AE_NOT_AVAILABLE		-1

typedef enum
{
	AE_INVALID = -1,			// So we have something more succint to check for than '-1'
	AE_EMPTY,
	AE_NPC_LEFTFOOT, // #define	NPC_EVENT_LEFTFOOT			2050
	AE_NPC_RIGHTFOOT, // #define NPC_EVENT_RIGHTFOOT			2051
	AE_NPC_BODYDROP_LIGHT, //#define NPC_EVENT_BODYDROP_LIGHT	2001
	AE_NPC_BODYDROP_HEAVY, //#define NPC_EVENT_BODYDROP_HEAVY	2002
	AE_NPC_SWISHSOUND, //#define NPC_EVENT_SWISHSOUND		2010
	AE_NPC_180TURN, //#define NPC_EVENT_180TURN			2020
	AE_NPC_ITEM_PICKUP, //#define NPC_EVENT_ITEM_PICKUP					2040
	AE_NPC_WEAPON_DROP, //#define NPC_EVENT_WEAPON_DROP					2041
	AE_NPC_WEAPON_SET_SEQUENCE_NAME, //#define NPC_EVENT_WEAPON_SET_SEQUENCE_NAME		2042
	AE_NPC_WEAPON_SET_SEQUENCE_NUMBER, //#define NPC_EVENT_WEAPON_SET_SEQUENCE_NUMBER	2043
	AE_NPC_WEAPON_SET_ACTIVITY, //#define NPC_EVENT_WEAPON_SET_ACTIVITY			2044
	AE_NPC_HOLSTER,
	AE_NPC_DRAW,
	AE_NPC_WEAPON_FIRE,

	AE_CL_PLAYSOUND, // #define CL_EVENT_SOUND				5004	// Emit a sound 
	AE_SV_PLAYSOUND,
	AE_CL_STOPSOUND,

	AE_START_SCRIPTED_EFFECT,
	AE_STOP_SCRIPTED_EFFECT,

	AE_CLIENT_EFFECT_ATTACH,
	
	AE_MUZZLEFLASH,			// Muzzle flash from weapons held by the player
	AE_NPC_MUZZLEFLASH,		// Muzzle flash from weapons held by NPCs
	
	AE_THUMPER_THUMP,		//Thumper Thump!
	AE_AMMOCRATE_PICKUP_AMMO,	//Ammo crate pick up ammo!

	AE_NPC_RAGDOLL,

	AE_NPC_ADDGESTURE,
	AE_NPC_RESTARTGESTURE,

	AE_NPC_ATTACK_BROADCAST,

	AE_NPC_HURT_INTERACTION_PARTNER,
	AE_NPC_SET_INTERACTION_CANTDIE,

	AE_SV_DUSTTRAIL,

	AE_CL_CREATE_PARTICLE_EFFECT,

	AE_RAGDOLL,

	AE_CL_ENABLE_BODYGROUP,
	AE_CL_DISABLE_BODYGROUP,
	AE_CL_BODYGROUP_SET_VALUE,
	AE_CL_BODYGROUP_SET_VALUE_CMODEL_WPN,

	AE_WPN_PRIMARYATTACK,	// Used by weapons that want their primary attack to occur during an attack anim (i.e. grenade throwing)
	AE_WPN_INCREMENTAMMO,

	AE_WPN_HIDE,		// Used to hide player weapons
	AE_WPN_UNHIDE,		// Used to unhide player weapons

	AE_WPN_PLAYWPNSOUND,	// Play a weapon sound from the weapon script file

	AE_RD_ROBOT_POP_PANELS_OFF,

	AE_TAUNT_ENABLE_MOVE,
	AE_TAUNT_DISABLE_MOVE,

	AE_NPC_RESPONSE,	// Play a response system concept if we're not speaking
	AE_NPC_RESPONSE_FORCED,		// Always play a response system concept

	LAST_SHARED_ANIMEVENT,
} Animevent;


typedef struct evententry_s evententry_t;

//=========================================================
//=========================================================
extern void EventList_Init( void );
extern void EventList_Free( void );
extern bool EventList_RegisterSharedEvent( const char *pszEventName, int iEventIndex, int iType = 0 );
extern Animevent EventList_RegisterPrivateEvent( const char *pszEventName );
extern int EventList_IndexForName( const char *pszEventName );
extern const char *EventList_NameForIndex( int iEventIndex );
Animevent EventList_RegisterPrivateEvent( const char *pszEventName );

// This macro guarantees that the names of each event and the constant used to
// reference it in the code are identical.
#define REGISTER_SHARED_ANIMEVENT( _n, b ) EventList_RegisterSharedEvent(#_n, _n, b );
#define REGISTER_PRIVATE_ANIMEVENT( _n ) _n = EventList_RegisterPrivateEvent( #_n );

// Implemented in shared code
extern void EventList_RegisterSharedEvents( void );
extern int EventList_GetEventType( int eventIndex );

#ifdef CLIENT_DLL
#define CBaseAnimating C_BaseAnimating
#endif
class CBaseAnimating;

struct animevent_t
{
	int				event;
	const char		*options;
	float			cycle;
	float			eventtime;
	int				type;
	CBaseAnimating	*pSource;
};

#define EVENT_SPECIFIC			0
#define EVENT_SCRIPTED			1000		// see scriptevent.h
#define EVENT_SHARED			2000
#define EVENT_WEAPON			3000
#define EVENT_CLIENT			5000

#define NPC_EVENT_BODYDROP_LIGHT	2001
#define NPC_EVENT_BODYDROP_HEAVY	2002

#define NPC_EVENT_SWISHSOUND		2010

#define NPC_EVENT_180TURN			2020

#define NPC_EVENT_ITEM_PICKUP					2040
#define NPC_EVENT_WEAPON_DROP					2041
#define NPC_EVENT_WEAPON_SET_SEQUENCE_NAME		2042
#define NPC_EVENT_WEAPON_SET_SEQUENCE_NUMBER	2043
#define NPC_EVENT_WEAPON_SET_ACTIVITY			2044

#define	NPC_EVENT_LEFTFOOT			2050
#define NPC_EVENT_RIGHTFOOT			2051

#define NPC_EVENT_OPEN_DOOR			2060

// !! DON'T CHANGE TO ORDER OF THESE.  THEY ARE HARD CODED IN THE WEAPON QC FILES (YUCK!) !!
#define EVENT_WEAPON_MELEE_HIT			3001
#define EVENT_WEAPON_SMG1				3002
#define EVENT_WEAPON_MELEE_SWISH		3003
#define EVENT_WEAPON_SHOTGUN_FIRE		3004
#define EVENT_WEAPON_THROW				3005
#define EVENT_WEAPON_AR1				3006
#define EVENT_WEAPON_AR2				3007
#define EVENT_WEAPON_HMG1				3008
#define EVENT_WEAPON_SMG2				3009
#define EVENT_WEAPON_MISSILE_FIRE		3010
#define EVENT_WEAPON_SNIPER_RIFLE_FIRE	3011
#define EVENT_WEAPON_AR2_GRENADE		3012
#define EVENT_WEAPON_THROW2				3013
#define	EVENT_WEAPON_PISTOL_FIRE		3014
#define EVENT_WEAPON_RELOAD				3015
#define EVENT_WEAPON_THROW3				3016
#define EVENT_WEAPON_RELOAD_SOUND		3017		// Use this + EVENT_WEAPON_RELOAD_FILL_CLIP to prevent shooting during the reload animation 
#define EVENT_WEAPON_RELOAD_FILL_CLIP	3018
#define EVENT_WEAPON_SMG1_BURST1		3101		// first round in a 3-round burst
#define EVENT_WEAPON_SMG1_BURSTN		3102		// 2, 3 rounds
#define EVENT_WEAPON_AR2_ALTFIRE		3103

#define EVENT_WEAPON_SEQUENCE_FINISHED	3900

// NOTE: MUST BE THE LAST WEAPON EVENT -- ONLY WEAPON EVENTS BETWEEN EVENT_WEAPON AND THIS
#define EVENT_WEAPON_LAST				3999


#endif // EVENTLIST_H
