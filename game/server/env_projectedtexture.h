#ifndef ENV_PROJECTEDTEXTURE_H
#define ENV_PROJECTEDTEXTURE_H
#ifdef _WIN32
#pragma once
#endif

#define ENV_PROJECTEDTEXTURE_STARTON			(1<<0)
#define ENV_PROJECTEDTEXTURE_ALWAYSUPDATE		(1<<2)
#define ENV_PROJECTEDTEXTURE_VOLUMETRICS_START_ON	( 1 << 3 )
#define ENV_PROJECTEDTEXTURE_UBERLIGHT				( 1 << 4 )

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CEnvProjectedTexture : public CPointEntity
{
	DECLARE_CLASS( CEnvProjectedTexture, CPointEntity );
public:
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();

	CEnvProjectedTexture();
	bool KeyValue( const char *szKeyName, const char *szValue );
	virtual bool GetKeyValue( const char *szKeyName, char *szValue, int iMaxLen );

	// Always transmit to clients
	virtual int UpdateTransmitState();
	virtual void Spawn( void );
	virtual void Activate( void );

	void	SetParent( CBaseEntity* pNewParent, int iAttachment = -1 );

	void InputTurnOn( inputdata_t &inputdata );
	void InputTurnOff( inputdata_t &inputdata );
	void InputAlwaysUpdateOn( inputdata_t &inputdata );
	void InputAlwaysUpdateOff( inputdata_t &inputdata );
	void InputSetFOV( inputdata_t &inputdata );
	void InputSetVerFOV( inputdata_t &inputdata );
	void InputSetHorFOV( inputdata_t &inputdata );
	void InputSetTarget( inputdata_t &inputdata );
	void InputSetCameraSpace( inputdata_t &inputdata );
	void InputSetUseAtten( inputdata_t &inputdata );
	void InputSetLightOnlyTarget( inputdata_t &inputdata );
	void InputSetLightWorld( inputdata_t &inputdata );
	void InputSetEnableShadows( inputdata_t &inputdata );
	void InputSetLightColor( inputdata_t &inputdata );
	void InputSetBrightness( inputdata_t &inputdata );
	void InputSetSpotlightTexture( inputdata_t &inputdata );
	void InputSetSpotlightDX8Texture( inputdata_t &inputdata );
	void InputSetAmbient( inputdata_t &inputdata );
	void InputSetAtten( inputdata_t &inputdata );
	void InputSetSpotlightFrame( inputdata_t &inputdata );
	void InputSetColorTransitionTime( inputdata_t &inputdata );
	void InputSetConstant( inputdata_t &inputdata ) { m_flConstantAtten = CorrectConstantAtten(inputdata.value.Float()); }
	void InputSetLinear( inputdata_t &inputdata ) { m_flLinearAtten = CorrectLinearAtten(inputdata.value.Float()); }
	void InputSetQuadratic( inputdata_t &inputdata ) { m_flQuadraticAtten = CorrectQuadraticAtten(inputdata.value.Float()); }
	void InputSetShadowAtten( inputdata_t &inputdata ) { m_flShadowAtten = inputdata.value.Float(); }
	void InputSetNearZ( inputdata_t &inputdata );
	void InputSetFarZ( inputdata_t &inputdata );
	void InputAlwaysDrawOn( inputdata_t &inputdata ) { m_bAlwaysDraw = true; }
	void InputAlwaysDrawOff( inputdata_t &inputdata ) { m_bAlwaysDraw = false; }
	void InputStopFollowingTarget( inputdata_t &inputdata ) { m_bDontFollowTarget = true; }
	void InputStartFollowingTarget( inputdata_t &inputdata ) { m_bDontFollowTarget = false; }
	void InputSetEnableVolumetrics( inputdata_t &inputdata );
	void InputEnableUberLight( inputdata_t &inputdata );
	void InputDisableUberLight( inputdata_t &inputdata );

	// Corrects keyvalue/input attenuation for internal FlashlightEffect_t attenuation.
	float CorrectConstantAtten( float fl ) { return fl * 0.5f; }
	float CorrectLinearAtten( float fl ) { return fl * 100.0f; }
	float CorrectQuadraticAtten( float fl ) { return fl * 10000.0f; }

	void InitialThink( void );
	void FlickerThink( void );

	CNetworkHandle( CBaseEntity, m_hTargetEntity );
	CNetworkVar( bool, m_bDontFollowTarget );

private:

	CNetworkVar( bool, m_bState );
	CNetworkVar( bool, m_bAlwaysUpdate );
	CNetworkVar( float, m_flLightFOV );
	CNetworkVar( float, m_flLightHorFOV );
	CNetworkVar( bool, m_bEnableShadows );
	CNetworkVar( bool, m_bLightOnlyTarget );
	CNetworkVar( bool, m_bLightWorld );
	CNetworkVar( bool, m_bCameraSpace );
	CNetworkVar( bool, m_bAtten );
	CNetworkColor32( m_LightColor );
	CNetworkVar( float, m_flBrightnessScale );
	CNetworkVar( float, m_flColorTransitionTime );
	CNetworkVar( float, m_flAmbient );
	CNetworkString( m_SpotlightTextureName, MAX_PATH );
	CNetworkString( m_SpotlightDX8TextureName, MAX_PATH );
	CNetworkVar( int, m_nSpotlightTextureFrame );
	CNetworkVar( float, m_flNearZ );
	CNetworkVar( float, m_flFarZ );
	CNetworkVar( float, m_flAtten );
	CNetworkVar( float, m_flQuadratic );
	CNetworkVar( int, m_nShadowQuality );
	CNetworkVar( float, m_flProjectionSize );
	CNetworkVar( float, m_flRotation );
	CNetworkVar( float, m_flConstantAtten );
	CNetworkVar( float, m_flLinearAtten );
	CNetworkVar( float, m_flQuadraticAtten );
	CNetworkVar( float, m_flShadowAtten );

	CNetworkVector( m_LinearFloatLightColor );
	Vector m_LinearFloatLightColorCopy;
	CNetworkVar( bool, m_bFlicker );

	CNetworkVar( bool, m_bEnableVolumetrics );
	CNetworkVar( bool, m_bEnableVolumetricsLOD );
	CNetworkVar( float, m_flVolumetricsFadeDistance );
	CNetworkVar( int, m_iVolumetricsQuality );
	CNetworkVar( float, m_flVolumetricsQualityBias );
	CNetworkVar( float, m_flVolumetricsMultiplier );

	CNetworkVar( bool, m_bUberlight );
	CNetworkVar( float, m_fNearEdge );
	CNetworkVar( float, m_fFarEdge );
	CNetworkVar( float, m_fCutOn );
	CNetworkVar( float, m_fCutOff );
	CNetworkVar( float, m_fShearx );
	CNetworkVar( float, m_fSheary );
	CNetworkVar( float, m_fWidth );
	CNetworkVar( float, m_fWedge );
	CNetworkVar( float, m_fHeight );
	CNetworkVar( float, m_fHedge );
	CNetworkVar( float, m_fRoundness );

	CNetworkVar( bool, m_bAlwaysDraw );

	// 1 = New projected texture
	// 0 = Non-Mapbase projected texture, e.g. one that uses the VDC parenting fix instead of the spawnflag
	// Not needed on the client right now, change to CNetworkVar when it actually is needed
	bool m_bProjectedTextureVersion;
};


#endif	// ENV_PROJECTEDTEXTURE_H
