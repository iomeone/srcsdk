//========= Copyright 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef C_ENVPROJECTED_TEXTURE_H
#define C_ENVPROJECTED_TEXTURE_H
#ifdef _WIN32
#pragma once
#endif

#include "c_baseentity.h"
#include "basetypes.h"
#include "../../materialsystem/stdshaders/IShaderExtension.h"

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class C_EnvProjectedTexture : public C_BaseEntity
{
	DECLARE_CLASS( C_EnvProjectedTexture, C_BaseEntity );
public:
	DECLARE_CLIENTCLASS();

	void SetMaterial( IMaterial *pMaterial );
	void SetLightColor( byte r, byte g, byte b, byte a );
	void SetSize( float flSize );
	void SetRotation( float flRotation );

	virtual void OnDataChanged( DataUpdateType_t updateType );
	void	ShutDownLightHandle( void );

	virtual void Simulate();

	void	UpdateLight( bool bForceUpdate );

// GSTRINGMIGRATION
	virtual void UpdateOnRemove()
	{
		if ( m_pVolmetricMesh != NULL )
		{
			CMatRenderContextPtr pRenderContext( materials );
			pRenderContext->DestroyStaticMesh( m_pVolmetricMesh );
			m_pVolmetricMesh = NULL;
		}
		BaseClass::UpdateOnRemove();
	}

	virtual bool					IsTransparent() { return true; }
	virtual bool					IsTwoPass() { return false; }

	virtual void GetRenderBoundsWorldspace( Vector& mins, Vector& maxs )
	{
		if ( m_bEnableVolumetrics )
		{
			mins = m_vecRenderBoundsMin;
			maxs = m_vecRenderBoundsMax;
		}
		else
		{
			BaseClass::GetRenderBoundsWorldspace( mins, maxs );
		}
	}
	virtual bool ShouldDraw() { return true; }
	virtual int DrawModel( int flags );

	virtual bool ShouldReceiveProjectedTextures( int flags ) { return false; }

	void ClearVolumetricsMesh();

	C_EnvProjectedTexture();
	~C_EnvProjectedTexture();

	static void SetVisibleBBoxMinHeight( float flVisibleBBoxMinHeight ) { m_flVisibleBBoxMinHeight = flVisibleBBoxMinHeight; }
	static float GetVisibleBBoxMinHeight( void ) { return m_flVisibleBBoxMinHeight; }
	static C_EnvProjectedTexture *Create( );

private:

	inline bool IsBBoxVisible( void );
	bool IsBBoxVisible( Vector vecExtentsMin,
						Vector vecExtentsMax );

	void RebuildVolumetricMesh();
	void GetShadowViewSetup( CViewSetup &setup );

	CTextureReference m_SpotlightTexture;

	IMesh	*m_pVolmetricMesh;
	CMaterialReference m_matVolumetricsMaterial;

	ClientShadowHandle_t m_LightHandle;
	bool m_bForceUpdate;

	EHANDLE	m_hTargetEntity;
	bool m_bDontFollowTarget;

	bool	m_bState;
	bool	m_bAlwaysUpdate;
	float	m_flLightFOV;
	float		m_flLightHorFOV;
	bool	m_bEnableShadows;
	bool	m_bLightOnlyTarget;
	bool	m_bLightWorld;
	bool	m_bCameraSpace;
	float		m_flBrightnessScale;
	color32		m_LightColor;
	Vector		m_CurrentLinearFloatLightColor;
	float		m_flCurrentLinearFloatLightAlpha;
	Vector	m_LinearFloatLightColor;
	float		m_flCurrentBrightnessScale;
	float		m_flColorTransitionTime;
	float	m_flAmbient;
	float	m_flNearZ;
	float	m_flFarZ;
	char	m_SpotlightTextureName[MAX_PATH];
	int		m_nSpotlightTextureFrame;
	int		m_nShadowQuality;

	float		m_flConstantAtten;
	float		m_flLinearAtten;
	float		m_flQuadraticAtten;
	float		m_flShadowAtten;

	bool		m_bAlwaysDraw;
	//bool		m_bProjectedTextureVersion;

	FlashlightState_t	m_FlashlightState;
	UberlightState_t	m_UberlightState;
	Vector m_vecRenderBoundsMin, m_vecRenderBoundsMax;
	Vector	m_vecExtentsMin;
	Vector	m_vecExtentsMax;

	bool m_bEnableVolumetrics;
	bool m_bEnableVolumetricsLOD;
	float m_flVolumetricsFadeDistance;
	int m_iVolumetricsQuality;
	float m_flVolumetricsMultiplier;
	float m_flVolumetricsQualityBias;

	float m_flLastFOV;
	int m_iCurrentVolumetricsSubDiv;

	static float m_flVisibleBBoxMinHeight;
};

#endif // C_ENV_PROJECTED_TEXTURE_H
