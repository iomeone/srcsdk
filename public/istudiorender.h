//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//

#ifndef ISTUDIORENDER_H
#define ISTUDIORENDER_H
#ifdef _WIN32
#pragma once
#endif

#include "tier1/interface.h"
#include "mathlib/vector.h"
#include "mathlib/vector4d.h"
#include "tier1/utlbuffer.h"
#include "tier1/utlvector.h"
#include "materialsystem/imaterial.h"
#include "materialsystem/imaterialsystem.h"
#include "appframework/IAppSystem.h"
#include "datacache/imdlcache.h"
#include "studio.h"


//-----------------------------------------------------------------------------
// forward declarations
//-----------------------------------------------------------------------------
struct studiohdr_t;
struct studiomeshdata_t;
class Vector;
struct LightDesc_t;
class IMaterial;
struct studiohwdata_t;
struct Ray_t;
class Vector4D;
class IMaterialSystem;
struct matrix3x4_t;
class IMesh;
struct vertexFileHeader_t;
struct FlashlightState_t;
class VMatrix;
namespace OptimizedModel { struct FileHeader_t; }
class IPooledVBAllocator;
struct MeshInstanceData_t;
struct ShaderStencilState_t;

// undone: what's the standard for function type naming?
typedef void (*StudioRender_Printf_t)( PRINTF_FORMAT_STRING const char *fmt, ... );

struct StudioRenderConfig_t
{
	float fEyeShiftX;	// eye X position
	float fEyeShiftY;	// eye Y position
	float fEyeShiftZ;	// eye Z position
	float fEyeSize;		// adjustment to iris textures
	float fEyeGlintPixelWidthLODThreshold;

	int maxDecalsPerModel;
	int drawEntities;
	int skin;
	int fullbright;

	bool bEyeMove : 1;		// look around
	bool bSoftwareSkin : 1;
	bool bNoHardware : 1;
	bool bNoSoftware : 1;
	bool bTeeth : 1;
	bool bEyes : 1;
	bool bFlex : 1;
	bool bWireframe : 1;
	bool bDrawNormals : 1;
	bool bDrawTangentFrame : 1;
	bool bDrawZBufferedWireframe : 1;
	bool bSoftwareLighting : 1;
	bool bShowEnvCubemapOnly : 1;
	bool bWireframeDecals : 1;

	// Reserved for future use
	int m_nReserved[4];
};



//-----------------------------------------------------------------------------
// Studio render interface
//-----------------------------------------------------------------------------
DECLARE_POINTER_HANDLE( StudioDecalHandle_t );
#define STUDIORENDER_DECAL_INVALID  ( (StudioDecalHandle_t)0 )

enum
{
	ADDDECAL_TO_ALL_LODS = -1
};


//-----------------------------------------------------------------------------
// DrawModel flags
//-----------------------------------------------------------------------------
enum
{
	STUDIORENDER_DRAW_ENTIRE_MODEL		= 0,
	STUDIORENDER_DRAW_OPAQUE_ONLY		= 0x01,
	STUDIORENDER_DRAW_TRANSLUCENT_ONLY	= 0x02,
	STUDIORENDER_DRAW_GROUP_MASK		= 0x03,

	STUDIORENDER_DRAW_NO_FLEXES			= 0x04,
	STUDIORENDER_DRAW_STATIC_LIGHTING	= 0x08,

	STUDIORENDER_DRAW_ACCURATETIME		= 0x10,		// Use accurate timing when drawing the model.
	STUDIORENDER_DRAW_NO_SHADOWS		= 0x20,
	STUDIORENDER_DRAW_GET_PERF_STATS	= 0x40,

	STUDIORENDER_DRAW_WIREFRAME			= 0x80,

	STUDIORENDER_DRAW_ITEM_BLINK		= 0x100,

	STUDIORENDER_SHADOWDEPTHTEXTURE		= 0x200,

	STUDIORENDER_NO_SKIN				= 0x400,

	STUDIORENDER_SKIP_DECALS			= 0x800,

	STUDIORENDER_SSAODEPTHTEXTURE				= 0x1000,

	STUDIORENDER_GENERATE_STATS					= 0x8000,
};


//-----------------------------------------------------------------------------
// Standard model vertex formats
//-----------------------------------------------------------------------------
// FIXME: remove these (materials/shaders should drive vertex format). Need to
//        list required forcedmaterialoverrides in models/bsps (rather than
//        all models supporting all possible overrides, as they do currently).
#define VERTEX_TEXCOORD0_2D ( ( (uint64) 2 ) << ( TEX_COORD_SIZE_BIT + ( 3*0 ) ) )


//-----------------------------------------------------------------------------
// What kind of material override is it?
//-----------------------------------------------------------------------------
enum OverrideType_t
{
	OVERRIDE_NORMAL = 0,
	OVERRIDE_BUILD_SHADOWS,
	OVERRIDE_DEPTH_WRITE,
	OVERRIDE_SSAO_DEPTH_WRITE,
};


//-----------------------------------------------------------------------------
// DrawModel info
//-----------------------------------------------------------------------------

// Special flag for studio models that have a compiled in shadow lod version
// It's negative 2 since positive numbers == use a regular slot and -1 means
//  have studiorender compute a value instead
enum
{
	USESHADOWLOD = -2,
};

// beyond this number of materials, you won't get info back from DrawModel
#define MAX_DRAW_MODEL_INFO_MATERIALS 8

struct DrawModelResults_t
{
	int m_ActualTriCount; 
	int m_TextureMemoryBytes;
	int m_NumHardwareBones;
	int m_NumBatches;
	int m_NumMaterials;
	int m_nLODUsed;
	int m_flLODMetric;
	CFastTimer m_RenderTime;
	CUtlVectorFixed<IMaterial *,MAX_DRAW_MODEL_INFO_MATERIALS> m_Materials;
};

struct ColorTexelsInfo_t
{
	int						m_nWidth;
	int						m_nHeight;
	int						m_nMipmapCount;
	ImageFormat				m_ImageFormat;
	int						m_nByteCount;
	byte*					m_pTexelData;
};

enum
{
	// This is because we store which flashlights are on which model 
	// in a 32-bit field (see ModelArrayInstanceData_t::m_nFlashlightUsage)
	MAX_FLASHLIGHTS_PER_INSTANCE_DRAW_CALL = 32
};

struct FlashlightInstance_t
{
	IMaterial *m_pDebugMaterial;
	FlashlightState_t m_FlashlightState;
	VMatrix m_WorldToTexture;
	ITexture *m_pFlashlightDepthTexture;
};

struct StudioModelArrayInfo2_t
{
	int						m_nFlashlightCount;
	FlashlightInstance_t	*m_pFlashlights;	// NOTE: Can have at most MAX_FLASHLIGHTS_PER_INSTANCE_DRAW_CALL of these
};

struct StudioModelArrayInfo_t  : public StudioModelArrayInfo2_t
{
	studiohdr_t				*m_pStudioHdr;
	studiohwdata_t			*m_pHardwareData;
};

struct StudioArrayData_t
{
	studiohdr_t				*m_pStudioHdr;
	studiohwdata_t			*m_pHardwareData;
	void					*m_pInstanceData; // See StudioShadowArrayInstanceData_t or StudioArrayInstanceData_t
	int						m_nCount;
};

struct StudioShadowArrayInstanceData_t 
{
	int m_nLOD;
	int m_nBody;
	int m_nSkin;
	matrix3x4a_t *m_pPoseToWorld;
	float *m_pFlexWeights;
	float *m_pDelayedFlexWeights;
};

struct MaterialLightingState_t;
struct ColorMeshInfo_t;

struct StudioArrayInstanceData_t : public StudioShadowArrayInstanceData_t 
{
	MaterialLightingState_t *m_pLightingState;
	MaterialLightingState_t *m_pDecalLightingState;
	ITexture *m_pEnvCubemapTexture;
	StudioDecalHandle_t m_Decals;
	uint32 m_nFlashlightUsage;	// Mask indicating which flashlights to use.
	ShaderStencilState_t *m_pStencilState;
	ColorMeshInfo_t *m_pColorMeshInfo;
	Vector4D m_DiffuseModulation;
};

struct GetTriangles_Vertex_t
{
	Vector m_Position;
	Vector m_Normal;
	Vector4D m_TangentS;
	Vector2D m_TexCoord;
	Vector4D m_BoneWeight;
	int m_BoneIndex[4];
	int m_NumBones;

	IMPLEMENT_OPERATOR_EQUAL( GetTriangles_Vertex_t );
};

struct GetTriangles_MaterialBatch_t
{
	IMaterial *m_pMaterial;
	CUtlVector<GetTriangles_Vertex_t> m_Verts;
	CUtlVector<int> m_TriListIndices;
};

struct GetTriangles_Output_t
{
	CUtlVector<GetTriangles_MaterialBatch_t> m_MaterialBatches;
	matrix3x4_t m_PoseToWorld[MAXSTUDIOBONES];

	DISALLOW_OPERATOR_EQUAL( GetTriangles_Output_t );
};


struct model_array_instance_t 
{
	matrix3x4_t		modelToWorld;

	// UNDONE: Per instance lighting values?
};

//-----------------------------------------------------------------------------
// Cache Callback Function
// implementation can either statically persist data (tools) or lru cache (engine) it.
// caller returns base pointer to resident data.
// code expectes data to be dynamic and invokes cache callback prior to iterative access.
// virtualModel is member passed in via studiohdr_t and passed back for model identification.
//-----------------------------------------------------------------------------
#define STUDIO_DATA_CACHE_INTERFACE_VERSION "VStudioDataCache005"
 
abstract_class IStudioDataCache : public IAppSystem
{
public:
	virtual bool VerifyHeaders( studiohdr_t *pStudioHdr ) = 0;
	virtual vertexFileHeader_t *CacheVertexData( studiohdr_t *pStudioHdr ) = 0;
};

#include "compat_istudiorender.h"

//-----------------------------------------------------------------------------
// Studio render interface
//-----------------------------------------------------------------------------

DECLARE_TIER3_INTERFACE( IStudioRender, g_pStudioRender );

#endif // ISTUDIORENDER_H
