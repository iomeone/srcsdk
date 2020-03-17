enum MaterialVertexFormat_t
{
	MATERIAL_VERTEX_FORMAT_MODEL_SKINNED		= (VertexFormat_t) VERTEX_POSITION | VERTEX_COLOR | VERTEX_NORMAL | VERTEX_TEXCOORD0_2D | VERTEX_BONEWEIGHT(2) | VERTEX_BONE_INDEX | VERTEX_USERDATA_SIZE(4),
	MATERIAL_VERTEX_FORMAT_MODEL_SKINNED_DX7	= (VertexFormat_t) VERTEX_POSITION | VERTEX_COLOR | VERTEX_NORMAL | VERTEX_TEXCOORD0_2D | VERTEX_BONEWEIGHT(2) | VERTEX_BONE_INDEX,
    MATERIAL_VERTEX_FORMAT_MODEL	            = (VertexFormat_t) VERTEX_POSITION | VERTEX_COLOR_STREAM_1 | VERTEX_NORMAL | VERTEX_TEXCOORD0_2D | VERTEX_USERDATA_SIZE(4),
	MATERIAL_VERTEX_FORMAT_MODEL_DX7			= (VertexFormat_t) VERTEX_POSITION | VERTEX_COLOR | VERTEX_NORMAL | VERTEX_TEXCOORD0_2D,
	MATERIAL_VERTEX_FORMAT_COLOR				= (VertexFormat_t) VERTEX_SPECULAR
}; 

struct ColorMeshInfo_t
{
	// A given color mesh can own a unique Mesh, or it can use a shared Mesh
	// (in which case it uses a sub-range defined by m_nVertOffset and m_nNumVerts)
	IMesh				*	m_pMesh;
	IPooledVBAllocator	*	m_pPooledVBAllocator;
	int						m_nVertOffsetInBytes;
	int						m_nNumVerts;
};

struct DrawModelInfo_t
{
	studiohdr_t		*m_pStudioHdr;
	studiohwdata_t	*m_pHardwareData;
	StudioDecalHandle_t m_Decals;
	int				m_Skin;
	int				m_Body;
	int				m_HitboxSet;
	void			*m_pClientEntity;
	int				m_Lod;
	ColorMeshInfo_t	*m_pColorMeshes;
	bool			m_bStaticLighting;
	MaterialLightingState_t	m_LightingState;

	IMPLEMENT_OPERATOR_EQUAL( DrawModelInfo_t );
};

abstract_class IStudioRender : public IAppSystem
{
public:
	virtual void BeginFrame( void ) = 0;
	virtual void EndFrame( void ) = 0;

	// Used for the mat_stub console command.
	virtual void Mat_Stub( IMaterialSystem *pMatSys ) = 0;

	// Updates the rendering configuration 
	virtual void UpdateConfig( const StudioRenderConfig_t& config ) = 0;
	virtual void GetCurrentConfig( StudioRenderConfig_t& config ) = 0;

	// Load, unload model data
	virtual bool LoadModel( studiohdr_t *pStudioHdr, void *pVtxData, studiohwdata_t	*pHardwareData ) = 0;
	virtual void UnloadModel( studiohwdata_t *pHardwareData ) = 0;

	// Refresh the studiohdr since it was lost...
	virtual void RefreshStudioHdr( studiohdr_t* pStudioHdr, studiohwdata_t* pHardwareData ) = 0;

	// This is needed to do eyeglint and calculate the correct texcoords for the eyes.
	virtual void SetEyeViewTarget( const studiohdr_t *pStudioHdr, int nBodyIndex, const Vector& worldPosition ) = 0;
		
	// Methods related to lighting state
	// NOTE: SetAmbientLightColors assumes that the arraysize is the same as 
	// returned from GetNumAmbientLightSamples
	virtual int GetNumAmbientLightSamples() = 0;
	virtual const Vector *GetAmbientLightDirections() = 0;
	virtual void SetAmbientLightColors( const Vector4D *pAmbientOnlyColors ) = 0;
	virtual void SetAmbientLightColors( const Vector *pAmbientOnlyColors ) = 0;
	virtual void SetLocalLights( int numLights, const LightDesc_t *pLights ) = 0;

	// Sets information about the camera location + orientation
	virtual void SetViewState( const Vector& viewOrigin, const Vector& viewRight, 
		const Vector& viewUp, const Vector& viewPlaneNormal ) = 0;
	
	// LOD stuff
	virtual int GetNumLODs( const studiohwdata_t &hardwareData ) const = 0;
	virtual float GetLODSwitchValue( const studiohwdata_t &hardwareData, int lod ) const = 0;
	virtual void SetLODSwitchValue( studiohwdata_t &hardwareData, int lod, float switchValue ) = 0;

	// Sets the color/alpha modulation
	virtual void SetColorModulation( float const* pColor ) = 0;
	virtual void SetAlphaModulation( float flAlpha ) = 0;
	
	// Draws the model
	virtual void DrawModel( DrawModelResults_t *pResults, const DrawModelInfo_t& info, 
		matrix3x4_t *pBoneToWorld, float *pFlexWeights, float *pFlexDelayedWeights, const Vector &modelOrigin, int flags = STUDIORENDER_DRAW_ENTIRE_MODEL ) = 0;

	// Methods related to static prop rendering
	virtual void DrawModelStaticProp( const DrawModelInfo_t& drawInfo, const matrix3x4_t &modelToWorld, int flags = STUDIORENDER_DRAW_ENTIRE_MODEL ) = 0;
	virtual void DrawStaticPropDecals( const DrawModelInfo_t &drawInfo, const matrix3x4_t &modelToWorld ) = 0;
	virtual void DrawStaticPropShadows( const DrawModelInfo_t &drawInfo, const matrix3x4_t &modelToWorld, int flags ) = 0;

	// Causes a material to be used instead of the materials the model was compiled with
	virtual void ForcedMaterialOverride( IMaterial *newMaterial, OverrideType_t nOverrideType = OVERRIDE_NORMAL ) = 0;

	// Create, destroy list of decals for a particular model
	virtual StudioDecalHandle_t CreateDecalList( studiohwdata_t *pHardwareData ) = 0;
	virtual void DestroyDecalList( StudioDecalHandle_t handle ) = 0;

	// Add decals to a decal list by doing a planar projection along the ray
	// The BoneToWorld matrices must be set before this is called
	virtual void AddDecal( StudioDecalHandle_t handle, studiohdr_t *pStudioHdr, matrix3x4_t *pBoneToWorld, 
		const Ray_t & ray, const Vector& decalUp, IMaterial* pDecalMaterial, float radius, int body, bool noPokethru = false, int maxLODToDecal = ADDDECAL_TO_ALL_LODS ) = 0;

	// Compute the lighting at a point and normal
	virtual void ComputeLighting( const Vector* pAmbient, int lightCount,
		LightDesc_t* pLights, const Vector& pt, const Vector& normal, Vector& lighting ) = 0;

	// Compute the lighting at a point, constant directional component is passed
	// as flDirectionalAmount
	virtual void ComputeLightingConstDirectional( const Vector* pAmbient, int lightCount,
		LightDesc_t* pLights, const Vector& pt, const Vector& normal, Vector& lighting, float flDirectionalAmount ) = 0;

	// Shadow state (affects the models as they are rendered)
	virtual void AddShadow( IMaterial* pMaterial, void* pProxyData, FlashlightState_t *m_pFlashlightState = NULL, VMatrix *pWorldToTexture = NULL, ITexture *pFlashlightDepthTexture = NULL ) = 0;
	virtual void ClearAllShadows() = 0;

	// Gets the model LOD; pass in the screen size in pixels of a sphere 
	// of radius 1 that has the same origin as the model to get the LOD out...
	virtual int ComputeModelLod( studiohwdata_t* pHardwareData, float unitSphereSize, float *pMetric = NULL ) = 0;

	// Return a number that is usable for budgets, etc.
	// Things that we care about:
	// 1) effective triangle count (factors in batch sizes, state changes, etc)
	// 2) texture memory usage
	// Get Triangles returns the LOD used
	virtual void GetPerfStats( DrawModelResults_t *pResults, const DrawModelInfo_t &info, CUtlBuffer *pSpewBuf = NULL ) const = 0;
	virtual void GetTriangles( const DrawModelInfo_t& info, matrix3x4_t *pBoneToWorld, GetTriangles_Output_t &out ) = 0;

	// Returns materials used by a particular model
	virtual int GetMaterialList( studiohdr_t *pStudioHdr, int count, IMaterial** ppMaterials ) = 0;
	virtual int GetMaterialListFromBodyAndSkin( MDLHandle_t studio, int nSkin, int nBody, int nCountOutputMaterials, IMaterial** ppOutputMaterials ) = 0;

	// no debug modes, just fastest drawing path
	virtual void DrawModelArrayStaticProp( const DrawModelInfo_t& drawInfo, int nInstanceCount, const MeshInstanceData_t *pInstanceData, ColorMeshInfo_t **pColorMeshes ) = 0;

	// draw an array of models with the same state
	virtual void DrawModelArray( const StudioModelArrayInfo_t &drawInfo, int nCount, 
		StudioArrayInstanceData_t *pInstanceData, int nInstanceStride, int flags = STUDIORENDER_DRAW_ENTIRE_MODEL ) = 0;

	// draw an array of models with the same state
	virtual void DrawModelShadowArray( int nCount, StudioArrayData_t *pShadowData, 
		int nInstanceStride, int flags = STUDIORENDER_DRAW_ENTIRE_MODEL ) = 0;

	// draw an array of models with the same state
	virtual void DrawModelArray( const StudioModelArrayInfo2_t &info, int nCount, StudioArrayData_t *pArrayData, 
		int nInstanceStride, int flags = STUDIORENDER_DRAW_ENTIRE_MODEL ) = 0;
};