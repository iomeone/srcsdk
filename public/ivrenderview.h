//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $Workfile:     $
// $NoKeywords: $
//===========================================================================//
#if !defined( IVRENDERVIEW_H )
#define IVRENDERVIEW_H
#ifdef _WIN32
#pragma once
#endif

#include "basetypes.h"
#include "mathlib/vplane.h"
#include "interface.h"
#include "materialsystem/imaterialsystem.h"
#include "const.h"
#include "tier1/refcount.h"


//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CViewSetup;
class CEngineSprite;
class IClientEntity;
class IMaterial;
struct model_t;
class IClientRenderable;


//-----------------------------------------------------------------------------
// Flags used by DrawWorldLists
//-----------------------------------------------------------------------------
enum
{
	DRAWWORLDLISTS_DRAW_STRICTLYABOVEWATER		= 0x001,
	DRAWWORLDLISTS_DRAW_STRICTLYUNDERWATER		= 0x002,
	DRAWWORLDLISTS_DRAW_INTERSECTSWATER			= 0x004,
	DRAWWORLDLISTS_DRAW_WATERSURFACE			= 0x008,
	DRAWWORLDLISTS_DRAW_SKYBOX					= 0x010,
	DRAWWORLDLISTS_DRAW_CLIPSKYBOX				= 0x020,
	DRAWWORLDLISTS_DRAW_SHADOWDEPTH				= 0x040,
	DRAWWORLDLISTS_DRAW_REFRACTION				= 0x080,
	DRAWWORLDLISTS_DRAW_REFLECTION				= 0x100,
	DRAWWORLDLISTS_DRAW_WORLD_GEOMETRY			= 0x200,
	DRAWWORLDLISTS_DRAW_DECALS_AND_OVERLAYS		= 0x400,
	DRAWWORLDLISTS_DRAW_SSAO					= 0x800,
};

enum
{
	MAT_SORT_GROUP_STRICTLY_ABOVEWATER = 0,
	MAT_SORT_GROUP_STRICTLY_UNDERWATER,
	MAT_SORT_GROUP_INTERSECTS_WATER_SURFACE,
	MAT_SORT_GROUP_WATERSURFACE,

	MAX_MAT_SORT_GROUPS
};

enum ERenderDepthMode
{
	DEPTH_MODE_NORMAL = 0,
	DEPTH_MODE_SHADOW = 1,
	DEPTH_MODE_SSA0 = 2,
	DEPTH_MODE_OVERRIDE = 3,

	DEPTH_MODE_MAX
}; 

typedef VPlane Frustum[FRUSTUM_NUMPLANES];


//-----------------------------------------------------------------------------
// Leaf index
//-----------------------------------------------------------------------------
typedef unsigned short LeafIndex_t;
typedef short LeafFogVolume_t;
enum
{
	INVALID_LEAF_INDEX = (LeafIndex_t)~0
};

struct WorldListLeafData_t
{
	LeafIndex_t	leafIndex;	// 16 bits
	int16	waterData;
	uint16 	firstTranslucentSurface;	// engine-internal list index
	uint16	translucentSurfaceCount;	// count of translucent surfaces+disps
};

//-----------------------------------------------------------------------------
// Describes the leaves to be rendered this view, set by BuildWorldLists
//-----------------------------------------------------------------------------

class IWorldRenderList : public IRefCounted
{
};

//-----------------------------------------------------------------------------
// Describes the fog volume for a particular point
//-----------------------------------------------------------------------------
struct VisibleFogVolumeInfo_t
{
	int		m_nVisibleFogVolume;
	int		m_nVisibleFogVolumeLeaf;
	bool	m_bEyeInFogVolume;
	float	m_flDistanceToWater;
	float	m_flWaterHeight;
	IMaterial *m_pFogVolumeMaterial;
};


//-----------------------------------------------------------------------------
// Vertex format for brush models
//-----------------------------------------------------------------------------
struct BrushVertex_t
{
	Vector		m_Pos;
	Vector		m_Normal;
	Vector		m_TangentS;
	Vector		m_TangentT;
	Vector2D	m_TexCoord;
	Vector2D	m_LightmapCoord;

private:
	BrushVertex_t( const BrushVertex_t& src );
};

//-----------------------------------------------------------------------------
// Visibility data for area portal culling
//-----------------------------------------------------------------------------
struct VisOverrideData_t
{
	Vector		m_vecVisOrigin;					// The point to to use as the viewpoint for area portal backface cull checks.
	float		m_fDistToAreaPortalTolerance;	// The distance from an area portal before using the full screen as the viewable portion.
};


//-----------------------------------------------------------------------------
// interface for asking about the Brush surfaces from the client DLL
//-----------------------------------------------------------------------------

class IBrushSurface
{
public:
	// Computes texture coordinates + lightmap coordinates given a world position
	virtual void ComputeTextureCoordinate( Vector const& worldPos, Vector2D& texCoord ) = 0;
	virtual void ComputeLightmapCoordinate( Vector const& worldPos, Vector2D& lightmapCoord ) = 0;

	// Gets the vertex data for this surface
	virtual int  GetVertexCount() const = 0;
	virtual void GetVertexData( BrushVertex_t* pVerts ) = 0;

	// Gets at the material properties for this surface
	virtual IMaterial* GetMaterial() = 0;
};


//-----------------------------------------------------------------------------
// interface for installing a new renderer for brush surfaces
//-----------------------------------------------------------------------------

class IBrushRenderer
{
public:
	// Draws the surface; returns true if decals should be rendered on this surface
	virtual bool RenderBrushModelSurface( IClientEntity* pBaseEntity, IBrushSurface* pBrushSurface ) = 0;
};


#define MAX_VIS_LEAVES	32
//-----------------------------------------------------------------------------
// Purpose: Interface to client .dll to set up a rendering pass over world
//  The client .dll can call Render multiple times to overlay one or more world
//  views on top of one another
//-----------------------------------------------------------------------------
enum DrawBrushModelMode_t
{
	DBM_DRAW_ALL = 0,
	DBM_DRAW_OPAQUE_ONLY,
	DBM_DRAW_TRANSLUCENT_ONLY,
};

#include "compat_ivrenderview.h"

#if defined(_STATIC_LINKED) && defined(CLIENT_DLL)
namespace Client
{
extern IVRenderView *render;
}
#else
extern IVRenderView *render;
#endif

#endif // IVRENDERVIEW_H
