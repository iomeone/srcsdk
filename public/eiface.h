//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//===========================================================================//

#ifndef EIFACE_H
#define EIFACE_H

#ifdef _WIN32
#pragma once
#endif

#include "convar.h"
#include "icvar.h"
#include "edict.h"
#include "mathlib/vplane.h"
#include "iserverentity.h"
#include "engine/ivmodelinfo.h"
#include "soundflags.h"
#include "bitvec.h"
#include "engine/iserverplugin.h"
#include "tier1/bitbuf.h"

//-----------------------------------------------------------------------------
// forward declarations
//-----------------------------------------------------------------------------
class	SendTable;
class	ServerClass;
class	IMoveHelper;
struct  Ray_t;
class	CGameTrace;
typedef	CGameTrace trace_t;
struct	typedescription_t;
class	CSaveRestoreData;
struct	datamap_t;
class	SendTable;
class	ServerClass;
class	IMoveHelper;
struct  Ray_t;
struct	studiohdr_t;
class	CBaseEntity;
class	CRestore;
class	CSave;
class	variant_t;
struct	vcollide_t;
class	IRecipientFilter;
class	CBaseEntity;
class	ITraceFilter;
struct	client_textmessage_t;
class	INetChannelInfo;
class	ISpatialPartition;
class IScratchPad3D;
class CStandardSendProxies;
class IAchievementMgr;
class CGamestatsData;
class CSteamID;
class IReplayFactory;
class IReplaySystem;
class IServer;
class ISPSharedMemory;
class CGamestatsData;

typedef struct player_info_s player_info_t;

//-----------------------------------------------------------------------------
// defines
//-----------------------------------------------------------------------------

#ifdef _WIN32
#define DLLEXPORT __stdcall
#else
#define DLLEXPORT /* */
#endif

#define INTERFACEVERSION_VENGINESERVER_VERSION_21	"VEngineServer021"
#define INTERFACEVERSION_VENGINESERVER_VERSION_22	"VEngineServer022"

struct bbox_t
{
	Vector mins;
	Vector maxs;
};

//-----------------------------------------------------------------------------
// Purpose: Interface the engine exposes to the game DLL
//-----------------------------------------------------------------------------

#include "compat_eiface.h"

// These only differ in new items added to the end
typedef IVEngineServer IVEngineServer021;
typedef IVEngineServer IVEngineServer022;


#define INTERFACEVERSION_SERVERGAMEDLL_VERSION_8	"ServerGameDLL008"
#define INTERFACEVERSION_SERVERGAMEDLL_VERSION_9	"ServerGameDLL009"

class IServerGCLobby;

//-----------------------------------------------------------------------------
// Purpose: These are the interfaces that the game .dll exposes to the engine
//-----------------------------------------------------------------------------

typedef IServerGameDLL IServerGameDLL008;

//-----------------------------------------------------------------------------
// Just an interface version name for the random number interface
// See vstdlib/random.h for the interface definition
// NOTE: If you change this, also change VENGINE_CLIENT_RANDOM_INTERFACE_VERSION in cdll_int.h
//-----------------------------------------------------------------------------
#define VENGINE_SERVER_RANDOM_INTERFACE_VERSION	"VEngineRandom001"

#define INTERFACEVERSION_SERVERGAMEENTS			"ServerGameEnts001"
//-----------------------------------------------------------------------------
// Purpose: Interface to get at server entities
//-----------------------------------------------------------------------------

#define INTERFACEVERSION_SERVERGAMECLIENTS_VERSION_3	"ServerGameClients003"
#define INTERFACEVERSION_SERVERGAMECLIENTS				"ServerGameClients004"

//-----------------------------------------------------------------------------
// Purpose: Player / Client related functions
//-----------------------------------------------------------------------------

typedef IServerGameClients IServerGameClients003;


#define INTERFACEVERSION_UPLOADGAMESTATS		"ServerUploadGameStats001"

abstract_class IUploadGameStats
{
public:
	// Note that this call will block the server until the upload is completed, so use only at levelshutdown if at all.
	virtual bool UploadGameStats( 
		char const *mapname,				// Game map name
		unsigned int blobversion,			// Version of the binary blob data
		unsigned int blobsize,				// Size in bytes of blob data
		const void *pvBlobData ) = 0;		// Pointer to the blob data.

	// Call when created to init the CSER connection
	virtual void InitConnection( void ) = 0;

	// Call periodically to poll steam for a CSER connection
	virtual void UpdateConnection( void ) = 0;

	// If user has disabled stats tracking, do nothing
	virtual bool IsGameStatsLoggingEnabled() = 0;

	// Gets a non-personally identifiable unique ID for this steam user, used for tracking total gameplay time across
	//  multiple stats sessions, but isn't trackable back to their Steam account or id.
	// Buffer should be 16 bytes, ID will come back as a hexadecimal string version of a GUID
	virtual void GetPseudoUniqueId( char *buf, size_t bufsize ) = 0;

	// For determining general % of users running using cyber cafe accounts...
	virtual bool IsCyberCafeUser( void ) = 0;

	// Only works in single player
	virtual bool IsHDREnabled( void ) = 0;
};

#define INTERFACEVERSION_PLUGINHELPERSCHECK		"PluginHelpersCheck001"

//-----------------------------------------------------------------------------
// Purpose: allows the game dll to control which plugin functions can be run
//-----------------------------------------------------------------------------
abstract_class IPluginHelpersCheck
{
public:
	virtual bool CreateMessage( const char *plugin, edict_t *pEntity, DIALOG_TYPE type, KeyValues *data ) = 0;
};

//-----------------------------------------------------------------------------
// Purpose: Interface exposed from the client .dll back to the engine for specifying shared .dll IAppSystems (e.g., ISoundEmitterSystem)
//-----------------------------------------------------------------------------
abstract_class IServerDLLSharedAppSystems
{
public:
	virtual int	Count() = 0;
	virtual char const *GetDllName( int idx ) = 0;
	virtual char const *GetInterfaceName( int idx ) = 0;
};

#define SERVER_DLL_SHARED_APPSYSTEMS		"VServerDllSharedAppSystems001"

#define INTERFACEVERSION_SERVERGAMETAGS		"ServerGameTags001"

//-----------------------------------------------------------------------------
// Purpose: querying the game dll for Server cvar tags
//-----------------------------------------------------------------------------
abstract_class IServerGameTags
{
public:
	// Get the list of cvars that require tags to show differently in the server browser
	virtual void			GetTaggedConVarList( KeyValues *pCvarTagList ) = 0;
};

//-----------------------------------------------------------------------------
// Purpose: Provide hooks for the GC based lobby system
//-----------------------------------------------------------------------------
abstract_class IServerGCLobby
{
public:
	virtual bool HasLobby() const = 0;
	virtual bool SteamIDAllowedToConnect( const CSteamID &steamId ) const = 0;
	virtual void UpdateServerDetails(void) = 0;
	virtual bool ShouldHibernate() = 0;
};

#endif // EIFACE_H
