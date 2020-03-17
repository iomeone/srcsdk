//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Interfaces between the client.dll and engine
//
//===========================================================================//

#ifndef CDLL_INT_H
#define CDLL_INT_H
#ifdef _WIN32
#pragma once
#endif

#include "basetypes.h"
#include "interface.h"
#include "mathlib/mathlib.h"
#include "const.h"
#include "checksum_crc.h"
#include "datamap.h"
#include "tier1/bitbuf.h"
#include "inputsystem/ButtonCode.h"
#include "modes.h"

//-----------------------------------------------------------------------------
// forward declarations
//-----------------------------------------------------------------------------
FORWARD_DECLARE_HANDLE( InputContextHandle_t );
class CSteamAPIContext;
struct adsp_auto_params_t;
class ClientClass;
struct model_t;
class CSentence;
struct vrect_t;
struct cmodel_t;
class IMaterial;
class CAudioSource;
class CMeasureSection;
class SurfInfo;
class ISpatialQuery;
struct cache_user_t;
class IMaterialSystem;
class VMatrix;
struct ScreenFade_t;
struct ScreenShake_t;
class CViewSetup;
class CEngineSprite;
class CGlobalVarsBase;
class CPhysCollide;
class CSaveRestoreData;
class INetChannelInfo;
struct datamap_t;
struct typedescription_t;
class CStandardRecvProxies;
struct client_textmessage_t;
class IAchievementMgr;
class CGamestatsData;
class KeyValues;
class IFileList;
class CRenamedRecvTableInfo;
class CMouthInfo;
class IConVar;
using XUSER_CONTEXT = void *;
using XUSER_PROPERTY = void *;
class IMaterialProxy;
struct InputEvent_t;

namespace vgui
{
	// handle to an internal vgui panel
	// this is the only handle to a panel that is valid across dll boundaries
	typedef unsigned int VPANEL;
}

//-----------------------------------------------------------------------------
// Purpose: This data structure is filled in by the engine when the client .dll requests information about
//  other players that the engine knows about
//-----------------------------------------------------------------------------

// Engine player info, no game related infos here
// If you change this, change the two byteswap defintions: 
// cdll_client_int.cpp and cdll_engine_int.cpp



//-----------------------------------------------------------------------------
// Hearing info
//-----------------------------------------------------------------------------
struct AudioState_t
{
	Vector m_Origin;
	QAngle m_Angles;
	bool m_bIsUnderwater;
};


//-----------------------------------------------------------------------------
// Skybox visibility
//-----------------------------------------------------------------------------
enum SkyboxVisibility_t
{
	SKYBOX_NOT_VISIBLE = 0,
	SKYBOX_3DSKYBOX_VISIBLE,
	SKYBOX_2DSKYBOX_VISIBLE,
};

enum EngineInputContextId_t
{
	ENGINE_INPUT_CONTEXT_GAME = 0,
	ENGINE_INPUT_CONTEXT_GAMEUI,
};

//-----------------------------------------------------------------------------
// Skybox materials
//-----------------------------------------------------------------------------
struct SkyBoxMaterials_t
{
	// order: "rt", "bk", "lf", "ft", "up", "dn"
	IMaterial *material[6];
};

//-----------------------------------------------------------------------------
// Purpose: The engine reports to the client DLL what stage it's entering so the DLL can latch events
//  and make sure that certain operations only happen during the right stages.
// The value for each stage goes up as you move through the frame so you can check ranges of values
//  and if new stages get added in-between, the range is still valid.
//-----------------------------------------------------------------------------
enum ClientFrameStage_t
{
	FRAME_UNDEFINED=-1,			// (haven't run any frames yet)
	FRAME_START,

	// A network packet is being recieved
	FRAME_NET_UPDATE_START,
		// Data has been received and we're going to start calling PostDataUpdate
		FRAME_NET_UPDATE_POSTDATAUPDATE_START,
		// Data has been received and we've called PostDataUpdate on all data recipients
		FRAME_NET_UPDATE_POSTDATAUPDATE_END,
	// We've received all packets, we can now do interpolation, prediction, etc..
	FRAME_NET_UPDATE_END,		

	// We're about to start rendering the scene
	FRAME_RENDER_START,
	// We've finished rendering the scene.
	FRAME_RENDER_END
};

// Used by RenderView
enum RenderViewInfo_t
{
	RENDERVIEW_UNSPECIFIED	 = 0,
	RENDERVIEW_DRAWVIEWMODEL = (1<<0),
	RENDERVIEW_DRAWHUD		 = (1<<1),
	RENDERVIEW_SUPPRESSMONITORRENDERING = (1<<2),
};

// Spectator Movement modes (mods define these i
enum ClientDLLObserverMode_t
{
	CLIENT_DLL_OBSERVER_NONE = 0,	// not in spectator mode

	CLIENT_DLL_OBSERVER_DEATHCAM,	// special mode for death cam animation
	CLIENT_DLL_OBSERVER_FREEZECAM,	// zooms to a target, and freeze-frames on them
	CLIENT_DLL_OBSERVER_FIXED,		// view from a fixed camera position
	CLIENT_DLL_OBSERVER_IN_EYE,	// follow a player in first person view
	CLIENT_DLL_OBSERVER_CHASE,		// follow a player in third person view
	CLIENT_DLL_OBSERVER_ROAMING,	// free roaming

	CLIENT_DLL_OBSERVER_OTHER,
};

//-----------------------------------------------------------------------------
// Lightcache entry handle
//-----------------------------------------------------------------------------
DECLARE_POINTER_HANDLE( LightCacheHandle_t );


//-----------------------------------------------------------------------------
// Occlusion parameters
//-----------------------------------------------------------------------------
struct OcclusionParams_t
{
	float	m_flMaxOccludeeArea;
	float	m_flMinOccluderArea;
};

//-----------------------------------------------------------------------------
// Demo custom data callback type
//-----------------------------------------------------------------------------
typedef void (*pfnDemoCustomDataCallback)( uint8 *pData, size_t iSize );

//-----------------------------------------------------------------------------
// Just an interface version name for the random number interface
// See vstdlib/random.h for the interface definition
// NOTE: If you change this, also change VENGINE_SERVER_RANDOM_INTERFACE_VERSION in eiface.h
//-----------------------------------------------------------------------------
#define VENGINE_CLIENT_RANDOM_INTERFACE_VERSION	"VEngineRandom001"

// change this when the new version is incompatable with the old
#define VENGINE_CLIENT_INTERFACE_VERSION_13		"VEngineClient013"

//-----------------------------------------------------------------------------
// Purpose: Interface exposed from the engine to the client .dll
//-----------------------------------------------------------------------------

#include "compat_cdll_int.h"

//-----------------------------------------------------------------------------
// Purpose: Interface exposed from the client .dll back to the engine
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Purpose: Interface exposed from the client .dll back to the engine for specifying shared .dll IAppSystems (e.g., ISoundEmitterSystem)
//-----------------------------------------------------------------------------
abstract_class IClientDLLSharedAppSystems
{
public:
	virtual int	Count() = 0;
	virtual char const *GetDllName( int idx ) = 0;
	virtual char const *GetInterfaceName( int idx ) = 0;
};

#define CLIENT_DLL_SHARED_APPSYSTEMS		"VClientDllSharedAppSystems001"

//-----------------------------------------------------------------------------
// Purpose: Interface exposed from client back to materialsystem, currently just for recording into tools
//-----------------------------------------------------------------------------
abstract_class IClientMaterialSystem
{
public:
	virtual HTOOLHANDLE GetCurrentRecordingEntity() = 0;
	virtual void PostToolMessage( HTOOLHANDLE hEntity, KeyValues *pMsg ) = 0;
};

#define VCLIENTMATERIALSYSTEM_INTERFACE_VERSION "VCLIENTMATERIALSYSTEM001"

#endif // CDLL_INT_H
