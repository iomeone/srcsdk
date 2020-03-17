//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//

#ifndef ICLIENTRENDERABLE_H
#define ICLIENTRENDERABLE_H
#ifdef _WIN32
#pragma once
#endif

#include "mathlib/mathlib.h"
#include "interface.h"
#include "iclientunknown.h"
#include "client_render_handle.h"
#include "engine/ivmodelrender.h"

struct model_t;
struct matrix3x4_t;

extern void DefaultRenderBoundsWorldspace( IClientRenderable *pRenderable, Vector &absMins, Vector &absMaxs );

//-----------------------------------------------------------------------------
// Handles to a client shadow
//-----------------------------------------------------------------------------
typedef unsigned short ClientShadowHandle_t;

enum
{
	CLIENTSHADOW_INVALID_HANDLE = (ClientShadowHandle_t)~0
};

//-----------------------------------------------------------------------------
// What kind of shadows to render?
//-----------------------------------------------------------------------------
enum ShadowType_t
{
	SHADOWS_NONE = 0,
	SHADOWS_SIMPLE,
	SHADOWS_RENDER_TO_TEXTURE,
	SHADOWS_RENDER_TO_TEXTURE_DYNAMIC,	// the shadow is always changing state
	SHADOWS_RENDER_TO_DEPTH_TEXTURE,
	SHADOWS_RENDER_TO_TEXTURE_DYNAMIC_CUSTOM,	// changing, and entity uses custom rendering code for shadow
};


// This provides a way for entities to know when they've entered or left the PVS.
// Normally, server entities can use NotifyShouldTransmit to get this info, but client-only
// entities can use this. Store a CPVSNotifyInfo in your 
//
// When bInPVS=true, it's being called DURING rendering. It might be after rendering any
// number of views.
//
// If no views had the entity, then it is called with bInPVS=false after rendering.
abstract_class IPVSNotify
{
public:
	virtual void OnPVSStatusChanged( bool bInPVS ) = 0;
};

//-----------------------------------------------------------------------------
// Information needed to draw a model
//-----------------------------------------------------------------------------
struct RenderableInstance_t
{
	uint8 m_nAlpha;
};


// client renderable frame buffer usage flags
#define ERENDERFLAGS_NEEDS_POWER_OF_TWO_FB  1				// needs refract texture
#define ERENDERFLAGS_NEEDS_FULL_FB          2				// needs full framebuffer texture
#define ERENDERFLAGS_REFRACT_ONLY_ONCE_PER_FRAME 4 // even if it needs a the refract texture, don't update it >once/ frame

//-----------------------------------------------------------------------------
// Purpose: All client entities must implement this interface.
//-----------------------------------------------------------------------------

#include "compat_iclientrenderable.h"

//-----------------------------------------------------------------------------
// Purpose: All client renderables supporting the fast-path mdl
// rendering algorithm must inherit from this interface
//-----------------------------------------------------------------------------
enum RenderableLightingModel_t
{
	LIGHTING_MODEL_NONE = -1,
	LIGHTING_MODEL_STANDARD = 0,
	LIGHTING_MODEL_STATIC_PROP,
	LIGHTING_MODEL_PHYSICS_PROP,

	LIGHTING_MODEL_COUNT,
};

enum ModelDataCategory_t
{
	MODEL_DATA_LIGHTING_MODEL,	// data type returned is a RenderableLightingModel_t
	MODEL_DATA_STENCIL,			// data type returned is a ShaderStencilState_t

	MODEL_DATA_CATEGORY_COUNT,
};


abstract_class IClientModelRenderable
{
public:
	virtual bool GetRenderData( void *pData, ModelDataCategory_t nCategory ) = 0;
};

// This class can be used to implement default versions of some of the 
// functions of IClientRenderable.
abstract_class CDefaultClientRenderable : public IClientUnknown, public IClientRenderable
{
public:
	CDefaultClientRenderable()
	{
		m_hRenderHandle = INVALID_CLIENT_RENDER_HANDLE;
	}

	virtual const Vector &			GetRenderOrigin( void ) = 0;
	virtual const QAngle &			GetRenderAngles( void ) = 0;
	virtual const matrix3x4_t &		RenderableToWorldTransform() = 0;
	virtual bool					ShouldDraw( void ) = 0;
	virtual bool					IsTransparent( void ) = 0;
	virtual bool					IsTwoPass( void ) { return false; }
	virtual void					OnThreadedDrawSetup() {}
	virtual bool					UsesPowerOfTwoFrameBufferTexture( void ) { return false; }
	virtual bool					UsesFullFrameBufferTexture( void ) { return false; }
	virtual int                     GetRenderFlags( void ) { return 0; }

	virtual ClientShadowHandle_t	GetShadowHandle() const
	{
		return CLIENTSHADOW_INVALID_HANDLE;
	}

	virtual ClientRenderHandle_t&	RenderHandle()
	{
		return m_hRenderHandle;
	}

	virtual int						GetBody() { return 0; }
	virtual int						GetSkin() { return 0; }
	virtual bool					UsesFlexDelayedWeights() { return false; }

	virtual const model_t*			GetModel( ) const		{ return NULL; }
	virtual int						DrawModel( int flags )	{ return 0; }
	virtual int						DrawModel( int flags, const RenderableInstance_t &instance )	{ return 0; }
	virtual void					ComputeFxBlend( )		{ return; }
	virtual int						GetFxBlend( )			{ return 255; }
	virtual bool					LODTest()				{ return true; }
	virtual bool					SetupBones( matrix3x4_t *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime )	{ return true; }
	virtual void					SetupWeights( const matrix3x4_t *pBoneToWorld, int nFlexWeightCount, float *pFlexWeights, float *pFlexDelayedWeights ) {}
	virtual void					DoAnimationEvents( void )						{}
	virtual IPVSNotify*				GetPVSNotifyInterface() { return NULL; }
	virtual void					GetRenderBoundsWorldspace( Vector& absMins, Vector& absMaxs ) { DefaultRenderBoundsWorldspace( this, absMins, absMaxs ); }

	// Determine the color modulation amount
	virtual void	GetColorModulation( float* color )
	{
		Assert(color);
		color[0] = color[1] = color[2] = 1.0f;
	}

	// Should this object be able to have shadows cast onto it?
	virtual bool	ShouldReceiveProjectedTextures( int flags ) 
	{
		return false;
	}

	// These methods return true if we want a per-renderable shadow cast direction + distance
	virtual bool	GetShadowCastDistance( float *pDist, ShadowType_t shadowType ) const			{ return false; }
	virtual bool	GetShadowCastDirection( Vector *pDirection, ShadowType_t shadowType ) const	{ return false; }

	virtual void	GetShadowRenderBounds( Vector &mins, Vector &maxs, ShadowType_t shadowType )
	{
		GetRenderBounds( mins, maxs );
	}

	virtual bool IsShadowDirty( )			     { return false; }
	virtual void MarkShadowDirty( bool bDirty )  {}
	virtual IClientRenderable *GetShadowParent() { return NULL; }
	virtual IClientRenderable *FirstShadowChild(){ return NULL; }
	virtual IClientRenderable *NextShadowPeer()  { return NULL; }
	virtual ShadowType_t ShadowCastType()		 { return SHADOWS_NONE; }
	virtual void CreateModelInstance()			 {}
	virtual ModelInstanceHandle_t GetModelInstance() { return MODEL_INSTANCE_INVALID; }

	// Attachments
	virtual int LookupAttachment( const char *pAttachmentName ) { return -1; }
	virtual	bool GetAttachment( int number, Vector &origin, QAngle &angles ) { return false; }
	virtual bool GetAttachment( int number, matrix3x4_t &matrix ) {	return false; }

	// Rendering clip plane, should be 4 floats, return value of NULL indicates a disabled render clip plane
	virtual float *GetRenderClipPlane() { return NULL; }

	virtual void RecordToolMessage() {}
	virtual bool IgnoresZBuffer( void ) const { return false; }

	virtual bool	ShouldDrawForSplitScreenUser( int nSlot ) { return true; }
	virtual uint8	OverrideAlphaModulation( uint8 nAlpha ) { return nAlpha; }
	virtual uint8	OverrideShadowAlphaModulation( uint8 nAlpha ) { return nAlpha; }

// IClientUnknown implementation.
public:
	virtual void SetRefEHandle( const CBaseHandle &handle )	{ Assert( false ); }
	virtual const CBaseHandle& GetRefEHandle() const		{ Assert( false ); return *((CBaseHandle*)0); }

	virtual IClientUnknown*		GetIClientUnknown()		{ return this; }
	virtual ICollideable*		GetCollideable()		{ return 0; }
	virtual IClientRenderable*	GetClientRenderable()	{ return this; }
	virtual IClientNetworkable*	GetClientNetworkable()	{ return 0; }
	virtual IClientEntity*		GetIClientEntity()		{ return 0; }
	virtual C_BaseEntity*		GetBaseEntity()			{ return 0; }
	virtual IClientThinkable*	GetClientThinkable()	{ return 0; }
	virtual IClientModelRenderable*	GetClientModelRenderable()	{ return 0; }
	virtual IClientAlphaProperty*	GetClientAlphaProperty() { return 0; }

public:
	ClientRenderHandle_t m_hRenderHandle;
};


#endif // ICLIENTRENDERABLE_H
