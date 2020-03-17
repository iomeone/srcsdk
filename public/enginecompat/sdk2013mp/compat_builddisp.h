class CCoreDispSurface
{
public:

	enum { QUAD_POINT_COUNT = 4 };
	enum { MAX_CORNER_NEIGHBOR_COUNT = 16 };

	CCoreDispSurface();

	//=========================================================================
	//
	// initialization
	//
	void Init( void );

	//=========================================================================
	//
	// parent surface id - index to CMapFace, dface_t, or msurface_t
	//
	inline void SetHandle( int handle );
	inline int GetHandle( void );

	//=========================================================================
	//
	// vertex data - pos, normal, texture, lightmap, alpha, etc...
	//
	inline void SetPointCount( int count );
	inline int GetPointCount( void ) const;

	inline void SetPoint( int index, Vector const &pt );
	inline void GetPoint( int index, Vector& pt ) const;
	inline Vector const& GetPoint( int index ) const;

	inline void SetPointNormal( int index, Vector const &normal );
	inline void GetPointNormal( int index, Vector &normal );
	inline void SetTexCoord( int index, Vector2D const& texCoord );
	inline void GetTexCoord( int index, Vector2D& texCoord ) const;

	inline void SetLuxelCoord( int bumpIndex, int index, Vector2D const& luxelCoord );
	inline void GetLuxelCoord( int bumpIndex, int index, Vector2D& luxelCoord ) const;
	inline void SetLuxelCoords( int bumpIndex, Vector2D const coords[4] );
	inline void GetLuxelCoords( int bumpIndex, Vector2D coords[4] ) const;

	inline void SetLuxelU( int nU )			{ m_nLuxelU = nU; }
	inline int	GetLuxelU( void )			{ return m_nLuxelU; }
	inline void SetLuxelV( int nV )			{ m_nLuxelV = nV; }
	inline int	GetLuxelV( void )			{ return m_nLuxelV; }
	bool		CalcLuxelCoords( int nLuxels, bool bAdjust, const Vector &vecU, const Vector &vecV );

	inline void SetAlpha( int index, float alpha );
	inline float GetAlpha( int const index ) const;

	//=========================================================================
	//
	// utils
	//
	inline void GetNormal( Vector& normal );
	inline void SetFlags( int flag );
	inline int GetFlags( void );
	inline void SetContents( int contents );
	inline int GetContents( void );

	//=========================================================================
	//
	// create utils (texture axis not use anymore but here to support older maps)
	//
	inline void SetSAxis( Vector const &axis );
	inline void GetSAxis( Vector &axis );
	inline void SetTAxis( Vector const &axis );
	inline void GetTAxis( Vector &axis );

	inline void SetPointStartIndex( int index );
	inline int GetPointStartIndex( void );
	inline void SetPointStart( Vector const &pt );
	inline void GetPointStart( Vector &pt );

	// Used by the tools to set the neighbor data from the BSP file.
	void	SetNeighborData( const CDispNeighbor edgeNeighbors[4], const CDispCornerNeighbors cornerNeighbors[4] );

    void	GeneratePointStartIndexFromMappingAxes( Vector const &sAxis, Vector const &tAxis );
	int		GenerateSurfPointStartIndex( void );
	int		FindSurfPointStartIndex( void );
	void	AdjustSurfPointData( void );
	
	// Indexed by CORNER_ defines.
	CDispCornerNeighbors*		GetCornerNeighbors( int iCorner )		{ Assert( iCorner >= 0 && iCorner < ARRAYSIZE( m_CornerNeighbors ) ); return &m_CornerNeighbors[iCorner]; }
	const CDispCornerNeighbors*	GetCornerNeighbors( int iCorner ) const { Assert( iCorner >= 0 && iCorner < ARRAYSIZE( m_CornerNeighbors ) ); return &m_CornerNeighbors[iCorner]; }
	
	// Indexed by CORNER_ defines.
	int							GetCornerNeighborCount( int iCorner ) const				{ return GetCornerNeighbors( iCorner )->m_nNeighbors; }
	int							GetCornerNeighbor( int iCorner, int iNeighbor ) const	{ Assert( iNeighbor >= 0 && iNeighbor < GetCornerNeighbors(iCorner)->m_nNeighbors ); return GetCornerNeighbors( iCorner )->m_Neighbors[iNeighbor]; }
	
	CDispNeighbor*			GetEdgeNeighbor( int iEdge )		{ Assert( iEdge >= 0 && iEdge < ARRAYSIZE( m_EdgeNeighbors ) ); return &m_EdgeNeighbors[iEdge]; }
	const CDispNeighbor*	GetEdgeNeighbor( int iEdge ) const	{ Assert( iEdge >= 0 && iEdge < ARRAYSIZE( m_EdgeNeighbors ) ); return &m_EdgeNeighbors[iEdge]; }


protected:

	// Utility
	bool		LongestInU( const Vector &vecU, const Vector &vecV );


	int			m_Index;																// parent face (CMapFace, dface_t, msurface_t) index "handle"
	
	int			m_PointCount;															// number of points in the face (should be 4!)
	Vector		m_Points[QUAD_POINT_COUNT];												// points
	Vector		m_Normals[QUAD_POINT_COUNT];											// normals at points
	Vector2D	m_TexCoords[QUAD_POINT_COUNT];											// texture coordinates at points
	Vector2D	m_LuxelCoords[NUM_BUMP_VECTS+1][QUAD_POINT_COUNT];						// lightmap coordinates at points
	float		m_Alphas[QUAD_POINT_COUNT];												// alpha at points

	// Luxels sizes
	int					m_nLuxelU;
	int					m_nLuxelV;

	// Straight from the BSP file.	
	CDispNeighbor			m_EdgeNeighbors[4];
	CDispCornerNeighbors	m_CornerNeighbors[4];

    int			m_Flags;																// surface flags - inherited from the "parent" face
	int			m_Contents;																// contents flags - inherited from the "parent" face

	Vector		sAxis;																	// used to generate start disp orientation (old method)
	Vector		tAxis;																	// used to generate start disp orientation (old method)
	int			m_PointStartIndex;														// index to the starting point -- for saving starting point
	Vector		m_PointStart;															// starting point used to determine the orientation of the displacement map on the surface
};

struct CoreDispVert_t
{
	Vector			m_FieldVector;						// displacement vector field
	float			m_FieldDistance;					// the distances along the displacement vector normal

	Vector			m_SubdivNormal;
	Vector			m_SubdivPos;						// used the create curvature of displacements

	// generated displacement surface data
	Vector			m_Vert;								// displacement surface vertices
	Vector			m_FlatVert;
	Vector			m_Normal;							// displacement surface normals
	Vector			m_TangentS;							// use in calculating the tangent space axes
	Vector			m_TangentT;							// use in calculating the tangent space axes
	Vector2D		m_TexCoord;							// displacement surface texture coordinates
	Vector2D		m_LuxelCoords[NUM_BUMP_VECTS+1];	// displacement surface lightmap coordinates

	// additional per-vertex data
	float			m_Alpha;							// displacement alpha values (per displacement vertex)
};

class CCoreDispInfo : public CDispUtilsHelper
{
public:

	//
	// tree and displacement surface directions
	//
	enum { WEST      = 0,
		   NORTH     = 1,
		   EAST      = 2,
		   SOUTH     = 3,
		   SOUTHWEST = 4,
		   SOUTHEAST = 5,
		   NORTHWEST = 6,
		   NORTHEAST = 7 };

#if 0
	//
	// building parameters
	//
	enum { BUILD_NORMALS      = 0x1,
		   BUILD_TEXCOORDS    = 0x2,
		   BUILD_LIGHTCOORDS  = 0x4,
		   BUILD_LODTREE      = 0x8,
		   BUILD_COLLISION    = 0x10,
		   BUILD_TANGENTSPACE = 0x20 };
#endif

	//
	// surface info flags
	//
	enum { SURF_BUMPED				= 0x1,  
		   SURF_NOPHYSICS_COLL		= 0x2,
		   SURF_NOHULL_COLL			= 0x4,
		   SURF_NORAY_COLL			= 0x8 };

	enum { MAX_DISP_POWER = MAX_MAP_DISP_POWER };
	enum { MAX_VERT_COUNT = MAX_DISPVERTS };
	enum { MAX_NODE_COUNT = 85 };


// Convert from a CDispUtilsHelper.
public:
	
	static CCoreDispInfo*			FromDispUtils( CDispUtilsHelper *p )	{ return (CCoreDispInfo*)p; }


// CDispUtilsHelper implementation.
public:

	virtual CDispNeighbor*			GetEdgeNeighbor( int index );
	virtual CDispCornerNeighbors*	GetCornerNeighbors( int index );
	virtual const CPowerInfo* 		GetPowerInfo() const;
	virtual CDispUtilsHelper*		GetDispUtilsByIndex( int index );


public:

	//=========================================================================
	//
	// Creation/Destruction
	//
	CCoreDispInfo();
	~CCoreDispInfo();

	void InitSurf( int parentIndex, Vector points[4], Vector normals[4],
		           Vector2D texCoords[4], Vector2D lightCoords[4][4], int contents, int flags,
				   bool bGenerateSurfPointStart, Vector& startPoint, 
				   bool bHasMappingAxes, Vector& uAxis, Vector& vAxis );

	void InitDispInfo( int power, int minTess, float smoothingAngle, 
		               float *alphas, Vector *dispVectorField, float *dispDistances, int nFlags, const CDispMultiBlend *pvMultiBlends );

	// This just unpacks the contents of the verts into arrays and calls InitDispInfo.
	void InitDispInfo( int power, int minTess, float smoothingAngle, const CDispVert *pVerts, const CDispTri *pTris, int nFlags, const CDispMultiBlend *pvMultiBlends );
					   
//	bool Create( int creationFlags );
	bool Create( void );
	bool CreateWithoutLOD( void );

	//=========================================================================
	//
	// Parameter "Wrappers"
	//
	CCoreDispSurface*		GetSurface()		{ return &m_Surf; }
	const CCoreDispSurface*	GetSurface() const	{ return &m_Surf; }
	
	inline CCoreDispNode *GetNode( int index );

	inline void SetPower( int power );
	inline int GetPower( void ) const;
	inline int GetPostSpacing( void );
	inline int GetWidth( void );
	inline int GetHeight( void );
	inline int GetSize( void ) const;
	inline int GetFlags( void ) const;

	// Use this disp as a CDispUtils.
	void SetDispUtilsHelperInfo( CCoreDispInfo **ppListBase, int listSize );

	void SetNeighborData( const CDispNeighbor edgeNeighbors[4], const CDispCornerNeighbors cornerNeighbors[4] ) { GetSurface()->SetNeighborData( edgeNeighbors, cornerNeighbors ); }

	// Get a corner point. Indexed by the CORNER_ defines.
	const CVertIndex&	GetCornerPointIndex( int index ) const	{ return GetPowerInfo()->GetCornerPointIndex( index ); }
	const Vector&		GetCornerPoint( int index ) const		{ return GetVert( VertIndexToInt( GetCornerPointIndex( index ) ) ); }

	inline void SetVert( int index, Vector const& vert );
	inline void GetVert( int index, Vector& vert ) const;
	
	inline const Vector& GetVert( int index ) const;
	inline const Vector& GetVert( const CVertIndex &index ) const;

	inline void GetFlatVert( int index, Vector& vert ) const;
	inline void SetFlatVert( int index, const Vector &vert );
	
	inline void GetNormal( int index, Vector& normal ) const;
	inline const Vector& GetNormal( int index ) const;
	inline const Vector& GetNormal( const CVertIndex &index ) const;
	inline void SetNormal( int index, Vector const& normal );
	inline void SetNormal( const CVertIndex &index, Vector const& normal );
	
	inline void GetTangentS( int index, Vector& tangentS ) const;
	inline const Vector &GetTangentS( int index ) const;
	inline const Vector &GetTangentS( const CVertIndex &index ) const { return GetTangentS(VertIndexToInt(index)); }
	inline void GetTangentT( int index, Vector& tangentT ) const;
	inline void SetTangentS( int index, Vector const& vTangentS ) { m_pVerts[index].m_TangentS = vTangentS; }
	inline void SetTangentT( int index, Vector const& vTangentT ) { m_pVerts[index].m_TangentT = vTangentT; }

	inline void SetTexCoord( int index, Vector2D const& texCoord );
	inline void GetTexCoord( int index, Vector2D& texCoord ) const;
	
	inline void SetLuxelCoord( int bumpIndex, int index, Vector2D const& luxelCoord );
	inline void GetLuxelCoord( int bumpIndex, int index, Vector2D& luxelCoord ) const;

	inline void SetAlpha( int index, float alpha );
	inline float GetAlpha( int index );

	inline void SetMultiBlend( int index, Vector4D &vBlend, Vector4D &vAlphaBlend, Vector &vColor1, Vector &vColor2, Vector &vColor3, Vector &vColor4 );
	inline void SetMultiBlendColor( int index, int nColorIndex, Vector &vColor );
	inline void SetMultiBlend( int index, Vector4D &vBlend );
	inline void SetAlphaBlend( int index, Vector4D &vAlphaBlend );
	inline void GetMultiBlend( int index, Vector4D &vBlend, Vector4D &vAlphaBlend, Vector &vColor1, Vector &vColor2, Vector &vColor3, Vector &vColor4 ) const;
	inline void GetMultiBlend( int index, Vector4D &vBlend ) const;
	inline void GetAlphaBlend( int index, Vector4D &vAlphaBlend ) const;

	int GetTriCount( void );
	void GetTriIndices( int iTri, unsigned short &v1, unsigned short &v2, unsigned short &v3 );
	void SetTriIndices( int iTri, unsigned short v1, unsigned short v2, unsigned short v3 );
	void GetTriPos( int iTri, Vector &v1, Vector &v2, Vector &v3 );
	inline void SetTriTag( int iTri, unsigned short nTag )		{ m_pTris[iTri].m_uiTags |= nTag; }
	inline void ResetTriTag( int iTri, unsigned short nTag )	{ m_pTris[iTri].m_uiTags &= ~nTag; }
	inline void ToggleTriTag( int iTri, unsigned short nTag )   { m_pTris[iTri].m_uiTags ^= nTag; }
	inline bool IsTriTag( int iTri, unsigned short nTag )       { return ( ( m_pTris[iTri].m_uiTags & nTag ) != 0 ); }
	inline unsigned short GetTriTagValue( int iTri )			{ return m_pTris[iTri].m_uiTags; }
	inline void SetTriTagValue( int iTri, unsigned short nVal ) { m_pTris[iTri].m_uiTags = nVal; }

	bool IsTriWalkable( int iTri );
	bool IsTriBuildable( int iTri );
	bool IsTriRemove( int iTri );

	inline void SetElevation( float elevation );
	inline float GetElevation( void );

	inline void ResetFieldVectors( void );
	inline void SetFieldVector( int index, Vector const &v );
	inline void GetFieldVector( int index, Vector& v );
	inline void ResetFieldDistances( void );
	inline void SetFieldDistance( int index, float dist );
	inline float GetFieldDistance( int index );

	inline void ResetSubdivPositions( void );
	inline void SetSubdivPosition( int ndx, Vector const &v );
	inline void GetSubdivPosition( int ndx, Vector& v );

	inline void ResetSubdivNormals( void );
	inline void SetSubdivNormal( int ndx, Vector const &v );
	inline void GetSubdivNormal( int ndx, Vector &v );

	inline void SetRenderIndexCount( int count );
	inline int GetRenderIndexCount( void );
	inline void SetRenderIndex( int index, int triIndex );
	inline int GetRenderIndex( int index );

	inline CoreDispVert_t *GetDispVert( int iVert )					{ return &m_pVerts[iVert]; }
	inline CoreDispVert_t *GetDispVertList();
	inline unsigned short *GetRenderIndexList( void );

	inline void SetTouched( bool touched );
	inline bool IsTouched( void );

	void CalcDispSurfCoords( bool bLightMap, int lightmapID );
	void GetPositionOnSurface( float u, float v, Vector &vPos, Vector *pNormal, float *pAlpha );

	void DispUVToSurf( Vector2D const &dispUV, Vector &vecPoint, Vector *pNormal, float *pAlpha );
	void BaseFacePlaneToDispUV( Vector const &planePt, Vector2D &dispUV );
	bool SurfToBaseFacePlane( Vector const &surfPt, Vector &planePt );

	const CDispCornerNeighbors*	GetCornerNeighbors( int iCorner ) const	{ return GetSurface()->GetCornerNeighbors( iCorner ); }
	const CDispNeighbor*	GetEdgeNeighbor( int iEdge ) const	{ return GetSurface()->GetEdgeNeighbor( iEdge ); }

	void SetListIndex( int nIndex )		{ m_nListIndex = nIndex; } 
	int GetListIndex( void )			{ return m_nListIndex; }

	CBitVec<MAX_DISPVERTS>&			GetAllowedVerts()		{ return m_AllowedVerts; }
	const CBitVec<MAX_DISPVERTS>&	GetAllowedVerts() const	{ return m_AllowedVerts; }
	void AllowedVerts_Clear( void )							{ m_AllowedVerts.SetAll(); }
	int	 AllowedVerts_GetNumDWords() const					{ return m_AllowedVerts.GetNumDWords(); }
	unsigned long AllowedVerts_GetDWord(int i) const        { return m_AllowedVerts.GetDWord( i ); }
	void AllowedVerts_SetDWord(int i, unsigned long val)    { m_AllowedVerts.SetDWord( i, val ); }


	void Position_Update( int iVert, Vector vecPos );

	//=========================================================================
	//
	// friend functions
	//
	friend void SmoothNeighboringDispSurfNormals( CCoreDispInfo **ppCoreDispInfoList, int listSize );

private:
																				// be changed to match the paint normal next pass)
	// LOD/collision node data
	CCoreDispNode		*m_Nodes;		// LOD quad-tree nodes

	float				m_Elevation;	// distance along the subdivision normal (should

	// defines the size of the displacement surface
	int					m_Power;		// "size" of the displacement map

	// base surface data
	CCoreDispSurface	m_Surf;			// surface containing displacement data
	                                    // be changed to match the paint normal next pass)
	// Vertex data..
	CoreDispVert_t		*m_pVerts;

	// Triangle data..
	CoreDispTri_t		*m_pTris;

	// render specific data
	int					m_RenderIndexCount;		// number of indices used in rendering
	unsigned short		*m_RenderIndices;		// rendering index list (list of triangles)
	int					m_RenderCounter;		// counter to verify surfaces are renderered/collided with only once per frame

	// utility data
	bool				m_bTouched;				// touched flag
	CCoreDispInfo		*m_pNext;				// used for chaining

	// The list that this disp is in (used for CDispUtils::IHelper implementation).
	CCoreDispInfo		**m_ppListBase;
	int					m_ListSize;

	CBitVec<MAX_DISPVERTS>	m_AllowedVerts;		// Built in VBSP. Defines which verts are allowed to exist based on what the neighbors are.

	int					m_nListIndex;

	//=========================================================================
	//
	// Creation Functions
	//

	void GenerateDispSurf( void );
	void GenerateDispSurfNormals( void );
	void GenerateDispSurfTangentSpaces( void );
	bool DoesEdgeExist( int indexRow, int indexCol, int direction, int postSpacing );
	void CalcNormalFromEdges( int indexRow, int indexCol, bool bIsEdge[4], Vector& normal );
	void CalcDispSurfAlphas( void );
	void GenerateLODTree( void );
	void CalcVertIndicesAtNodes( int nodeIndex );
	int GetNodeVertIndexFromParentIndex( int level, int parentVertIndex, int direction );
	void CalcNodeInfo( int nodeIndex, int terminationLevel );
	void CalcNeighborVertIndicesAtNode( int nodeIndex, int level );
	void CalcNeighborNodeIndicesAtNode( int nodeIndex, int level );
	void CalcErrorTermAtNode( int nodeIndex, int level );
	float GetMaxErrorFromChildren( int nodeIndex, int level );
	void CalcBoundingBoxAtNode( int nodeIndex );
	void CalcMinMaxBoundingBoxAtNode( int nodeIndex, Vector& bMin, Vector& bMax );
	void CalcTriSurfInfoAtNode( int nodeIndex );
	void CalcTriSurfIndices( int nodeIndex, int indices[8][3] );
	void CalcTriSurfBoundingBoxes( int nodeIndex, int indices[8][3] );
	void CalcRayBoundingBoxes( int nodeIndex, int indices[8][3] );
	void CalcTriSurfPlanes( int nodeIndex, int indices[8][3] );
	void GenerateCollisionData( void );
	void GenerateCollisionSurface( void );

	void CreateBoundingBoxes( CoreDispBBox_t *pBBox, int count );

	void DispUVToSurf_TriTLToBR( Vector &vecPoint, Vector *pNormal, float *pAlpha, float flU, float flV, const Vector &vecIntersectPoint );
	void DispUVToSurf_TriBLToTR( Vector &vecPoint, Vector *pNormal, float *pAlpha, float flU, float flV, const Vector &vecIntersectPoint );
	void DispUVToSurf_TriTLToBR_1( const Vector &vecIntersectPoint, int nSnapU, int nNextU, int nSnapV, int nNextV, Vector &vecPoint, Vector *pNormal, float *pAlpha, bool bBackup );
	void DispUVToSurf_TriTLToBR_2( const Vector &vecIntersectPoint, int nSnapU, int nNextU, int nSnapV, int nNextV, Vector &vecPoint, Vector *pNormal, float *pAlpha, bool bBackup );
	void DispUVToSurf_TriBLToTR_1( const Vector &vecIntersectPoint, int nSnapU, int nNextU, int nSnapV, int nNextV, Vector &vecPoint, Vector *pNormal, float *pAlpha, bool bBackup );
	void DispUVToSurf_TriBLToTR_2( const Vector &vecIntersectPoint, int nSnapU, int nNextU, int nSnapV, int nNextV, Vector &vecPoint, Vector *pNormal, float *pAlpha, bool bBackup );

	void GetTriangleIndicesForDispBBox( int nIndex, int nTris[2][3] );

	void BuildTriTLtoBR( int ndx );
	void BuildTriBLtoTR( int ndx );

	void InitTris( void );
	void CreateTris( void );
};

inline int CCoreDispInfo::GetFlags( void ) const
{
	return 0;
}