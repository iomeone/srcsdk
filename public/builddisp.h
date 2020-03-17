//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $Workfile:     $
// $Date:         $
// $NoKeywords: $
//=============================================================================//

#ifndef BUILDDISP_H
#define BUILDDISP_H

#ifdef _WIN32
#pragma once
#endif

#include "commonmacros.h"
#include "tier0/dbg.h"
#include "bspfile.h"
#include "mathlib/mathlib.h"
#include "mathlib/bumpvects.h"
#include "mathlib/vector4d.h"
#include "disp_common.h"
#include "bitvec.h"

#define DISP_ALPHA_PROP_DELTA		382.5f

class CCoreDispInfo;

struct CoreDispBBox_t
{
	Vector vMin, vMax;
};

//=========================================================================
//
// Surface Class - interfacing class (fill in with MapFace, dface_t, and 
//                                    msurface_t)
//
#include "compat_builddisp.h"


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispSurface::SetHandle( int handle )
{
	m_Index = handle;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline int CCoreDispSurface::GetHandle( void )
{
	return m_Index;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispSurface::SetPointCount( int count )
{
	// quad only -- currently!
	if( count != 4 )
		return;
	m_PointCount = count;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline int CCoreDispSurface::GetPointCount( void ) const
{
	return m_PointCount;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispSurface::SetPoint( int index, Vector const &pt )
{
	Assert( index >= 0 );
	Assert( index < QUAD_POINT_COUNT );
	VectorCopy( pt, m_Points[index] );
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispSurface::GetPoint( int index, Vector &pt ) const
{
	Assert( index >= 0 );
	Assert( index < QUAD_POINT_COUNT );
	VectorCopy( m_Points[index], pt );
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline Vector const& CCoreDispSurface::GetPoint( int index ) const
{
	Assert( index >= 0 );
	Assert( index < QUAD_POINT_COUNT );
	return m_Points[index];
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispSurface::SetPointNormal( int index, Vector const &normal )
{
	Assert( index >= 0 );
	Assert( index < QUAD_POINT_COUNT );
	VectorCopy( normal, m_Normals[index] );
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispSurface::GetPointNormal( int index, Vector& normal )
{
	Assert( index >= 0 );
	Assert( index < QUAD_POINT_COUNT );
	VectorCopy( m_Normals[index], normal );
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispSurface::SetTexCoord( int index, Vector2D const& texCoord )
{
	Assert( index >= 0 );
	Assert( index < QUAD_POINT_COUNT );
	Vector2DCopy( texCoord, m_TexCoords[index] );
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispSurface::GetTexCoord( int index, Vector2D& texCoord ) const
{
	Assert( index >= 0 );
	Assert( index < QUAD_POINT_COUNT );
	Vector2DCopy( m_TexCoords[index], texCoord );
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispSurface::SetLuxelCoord( int bumpIndex, int index, Vector2D const& luxelCoord )
{
	Assert( index >= 0 );
	Assert( index < QUAD_POINT_COUNT );
	Assert( bumpIndex >= 0 );
	Assert( bumpIndex < NUM_BUMP_VECTS + 1 );
	Vector2DCopy( luxelCoord, m_LuxelCoords[bumpIndex][index] );
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispSurface::GetLuxelCoord( int bumpIndex, int index, Vector2D& luxelCoord ) const
{
	Assert( index >= 0 );
	Assert( index < QUAD_POINT_COUNT );
	Assert( bumpIndex >= 0 );
	Assert( bumpIndex < NUM_BUMP_VECTS + 1 );
	Vector2DCopy( m_LuxelCoords[bumpIndex][index], luxelCoord );
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispSurface::SetLuxelCoords( int bumpIndex, Vector2D const luxelCoords[4] )
{
	Assert( bumpIndex >= 0 );
	Assert( bumpIndex < NUM_BUMP_VECTS + 1 );
	for( int i=0; i < 4; i++ )
		Vector2DCopy( luxelCoords[i], m_LuxelCoords[bumpIndex][i] );
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispSurface::GetLuxelCoords( int bumpIndex, Vector2D luxelCoords[4] ) const
{
	Assert( bumpIndex >= 0 );
	Assert( bumpIndex < NUM_BUMP_VECTS + 1 );
	for( int i=0; i < 4; i++ )
		Vector2DCopy( m_LuxelCoords[bumpIndex][i], luxelCoords[i] );
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispSurface::SetAlpha( int index, float alpha )
{
	Assert( index >= 0 );
	Assert( index < QUAD_POINT_COUNT );
	m_Alphas[index] = alpha;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline float CCoreDispSurface::GetAlpha( int const index ) const
{
	Assert( index >= 0 );
	Assert( index < QUAD_POINT_COUNT );
	return m_Alphas[index];
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispSurface::SetFlags( int flag )
{
	m_Flags = flag;
}

	
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline int CCoreDispSurface::GetFlags( void )
{
	return m_Flags;
}

	
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispSurface::SetContents( int contents )
{
	m_Contents = contents;
}

	
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline int CCoreDispSurface::GetContents( void )
{
	return m_Contents;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispSurface::SetSAxis( Vector const &axis )
{
	VectorCopy( axis, sAxis );
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispSurface::GetSAxis( Vector& axis )
{
	VectorCopy( sAxis, axis );
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispSurface::SetTAxis( Vector const &axis )
{
	VectorCopy( axis, tAxis );
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispSurface::GetTAxis( Vector& axis )
{
	VectorCopy( tAxis, axis );
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispSurface::SetPointStartIndex( int index )
{
	Assert( index >= 0 );
	Assert( index < QUAD_POINT_COUNT );
	m_PointStartIndex = index;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline int CCoreDispSurface::GetPointStartIndex( void )
{
	return m_PointStartIndex;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispSurface::SetPointStart( Vector const& pt )
{
	VectorCopy( pt, m_PointStart );
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispSurface::GetPointStart( Vector& pt )
{
	VectorCopy( m_PointStart, pt );
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispSurface::GetNormal( Vector& normal )
{
	//
	// calculate the displacement surface normal
	//
	Vector tmp[2];
	VectorSubtract( m_Points[1], m_Points[0], tmp[0] );
	VectorSubtract( m_Points[3], m_Points[0], tmp[1] );
	CrossProduct( tmp[1], tmp[0], normal );
	VectorNormalize( normal );
}


//=========================================================================
//
// Node Class (for displacement quad-tree)
//
class CCoreDispNode
{
public:

	enum { MAX_NEIGHBOR_NODE_COUNT = 4 };
	enum { MAX_NEIGHBOR_VERT_COUNT = 8 };
	enum { MAX_SURF_AT_NODE_COUNT = 8 };

	//=========================================================================
	//
	// Initialization
	//
	void Init( void );

	//=========================================================================
	//
	//
	//
	inline void SetBoundingBox( Vector const& bMin, Vector const& bMax );
	inline void GetBoundingBox( Vector& bMin, Vector& bMax );

	inline void SetErrorTerm( float errorTerm );
	inline float GetErrorTerm( void );

	inline void SetNeighborNodeIndex( int dir, int index );
	inline int GetNeighborNodeIndex( int dir );

	inline void SetCenterVertIndex( int index );
	inline int GetCenterVertIndex( void );
	inline void SetNeighborVertIndex( int dir, int index );
	inline int GetNeighborVertIndex( int dir );

	inline void SetTriBoundingBox( int index, Vector const& bMin, Vector const& bMax );
	inline void GetTriBoundingBox( int index, Vector& bMin, Vector& bMax );
	inline void SetTriPlane( int index, Vector const& normal, float dist );
	inline void GetTriPlane( int index, cplane_t *plane );

	inline void SetRayBoundingBox( int index, Vector const& bMin, Vector const& bMax );
	inline void GetRayBoundingBox( int index, Vector& bMin, Vector& bMax );

	//=========================================================================
	//
	// Node Functions (friend functions)
	//
	friend int GetNodeLevel( int index );
	friend int GetNodeCount( int power );
	friend int GetNodeParent( int index );
	friend int GetNodeChild( int power, int index, int direction );
	friend int GetNodeNeighborNode( int power, int index, int direction, int level );
	friend int GetNodeNeighborNodeFromNeighborSurf( int power, int index, int direction, int level, int neighborOrient );
	friend int GetNodeMinNodeAtLevel( int level );

	friend void GetDispNodeTriVerts( CCoreDispInfo *pDisp, int nodeIndex, int triIndex, float *v1, float *v2, float *v3 );

	friend void GetComponentsFromNodeIndex( int index, int *x, int *y );
	friend int GetNodeIndexFromComponents( int x, int y );

protected:	
		
	Vector		m_BBox[2];											// displacement node bounding box (take into account size of children)
	float		m_ErrorTerm;										// LOD error term (the "precision" of the representation of the surface at this node's level)
	int			m_VertIndex;										// the node's vertex index (center vertex of node)
	int			m_NeighborVertIndices[MAX_NEIGHBOR_VERT_COUNT];		// all other vertex indices in node (maximally creates 8 trianglar surfaces)
	Vector		m_SurfBBoxes[MAX_SURF_AT_NODE_COUNT][2];			// surface bounding boxes - old method
	cplane_t	m_SurfPlanes[MAX_SURF_AT_NODE_COUNT];				// surface plane info - old method

	Vector		m_RayBBoxes[4][2];									// bounding boxes for ray traces
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispNode::SetBoundingBox( Vector const& bMin, Vector const& bMax )
{
	VectorCopy( bMin, m_BBox[0] );
	VectorCopy( bMax, m_BBox[1] );
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispNode::GetBoundingBox( Vector& bMin, Vector& bMax )
{
	VectorCopy( m_BBox[0], bMin );
	VectorCopy( m_BBox[1], bMax );
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispNode::SetErrorTerm( float errorTerm )
{
	m_ErrorTerm = errorTerm;
}

	
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline float CCoreDispNode::GetErrorTerm( void )
{
	return m_ErrorTerm;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispNode::SetCenterVertIndex( int index )
{
	m_VertIndex = index;
}

	
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline int CCoreDispNode::GetCenterVertIndex( void )
{
	return m_VertIndex;
}

	
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispNode::SetNeighborVertIndex( int dir, int index )
{
	Assert( dir >= 0 );
	Assert( dir < MAX_NEIGHBOR_VERT_COUNT );
	m_NeighborVertIndices[dir] = index;
}

	
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline int CCoreDispNode::GetNeighborVertIndex( int dir )
{
	Assert( dir >= 0 );
	Assert( dir < MAX_NEIGHBOR_VERT_COUNT );
	return m_NeighborVertIndices[dir];
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispNode::SetTriBoundingBox( int index, Vector const& bMin, Vector const& bMax )
{
	Assert( index >= 0 );
	Assert( index < MAX_SURF_AT_NODE_COUNT );
	VectorCopy( bMin, m_SurfBBoxes[index][0] );
	VectorCopy( bMax, m_SurfBBoxes[index][1] );
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispNode::GetTriBoundingBox( int index, Vector& bMin, Vector& bMax )
{
	Assert( index >= 0 );
	Assert( index < MAX_SURF_AT_NODE_COUNT );
	VectorCopy( m_SurfBBoxes[index][0], bMin );
	VectorCopy( m_SurfBBoxes[index][1], bMax );
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispNode::SetTriPlane( int index, Vector const &normal, float dist )
{
	Assert( index >= 0 );
	Assert( index < MAX_SURF_AT_NODE_COUNT );
	VectorCopy( normal, m_SurfPlanes[index].normal );
	m_SurfPlanes[index].dist = dist;
}

	
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispNode::GetTriPlane( int index, cplane_t *plane )
{
	Assert( index >= 0 );
	Assert( index < MAX_SURF_AT_NODE_COUNT );
	VectorCopy( m_SurfPlanes[index].normal, plane->normal );
	plane->dist = m_SurfPlanes[index].dist;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispNode::SetRayBoundingBox( int index, Vector const &bMin, Vector const &bMax )
{
	Assert( index >= 0 );
	Assert( index < 4 );
	VectorCopy( bMin, m_RayBBoxes[index][0] );
	VectorCopy( bMax, m_RayBBoxes[index][1] );
}

	
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispNode::GetRayBoundingBox( int index, Vector& bMin, Vector& bMax )
{
	Assert( index >= 0 );
	Assert( index < 4 );
	VectorCopy( m_RayBBoxes[index][0], bMin );
	VectorCopy( m_RayBBoxes[index][1], bMax );
}

	
//=============================================================================
//
// CCoreInfoBuilder - the primary data necessay to derive a displacement surface
//                    used by WorldCraft (CMapFace, CMapDisp), VRAD (dface_t, ddispinto_t),
//                    and the engine (msurface_t, CDispInfo)
//

// New, need to use this at the node level
#define	COREDISPTRI_TAG_WALKABLE				(1<<0)
#define COREDISPTRI_TAG_FORCE_WALKABLE_BIT		(1<<1)
#define COREDISPTRI_TAG_FORCE_WALKABLE_VAL		(1<<2)
#define COREDISPTRI_TAG_BUILDABLE				(1<<3)
#define COREDISPTRI_TAG_FORCE_BUILDABLE_BIT		(1<<4)
#define COREDISPTRI_TAG_FORCE_BUILDABLE_VAL		(1<<5)
#define COREDISPTRI_TAG_FORCE_REMOVE_BIT		(1<<6)

struct CoreDispTri_t
{
	unsigned short  m_iIndex[3];						// the three indices that make up a triangle
	unsigned short	m_uiTags;							// walkable, buildable, etc.
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispInfo::SetPower( int power )
{
	m_Power = power;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline int CCoreDispInfo::GetPower( void ) const
{
	return m_Power;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline int CCoreDispInfo::GetPostSpacing( void )
{
	return ( ( 1 << m_Power ) + 1 );
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline int CCoreDispInfo::GetWidth( void )
{
	return ( ( 1 << m_Power ) + 1 );
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline int CCoreDispInfo::GetHeight( void )
{
	return ( ( 1 << m_Power ) + 1 );
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline int CCoreDispInfo::GetSize( void ) const
{
    return ( ( ( 1 << m_Power ) + 1 ) * ( ( 1 << m_Power ) + 1 ) );
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispInfo::SetVert( int index, Vector const &vert )
{
	Assert( index >= 0 );
	Assert( index < MAX_VERT_COUNT );
	VectorCopy( vert, m_pVerts[index].m_Vert );
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispInfo::GetVert( int index, Vector& vert ) const
{
	Assert( index >= 0 );
	Assert( index < MAX_VERT_COUNT );
	VectorCopy( m_pVerts[index].m_Vert, vert );
}


inline const Vector& CCoreDispInfo::GetVert( int index ) const
{
	Assert( index >= 0 );
	Assert( index < MAX_VERT_COUNT );
	return m_pVerts[index].m_Vert;
}

inline const Vector& CCoreDispInfo::GetVert( const CVertIndex &index ) const
{
	return GetVert( VertIndexToInt( index ) );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispInfo::GetFlatVert( int index, Vector& vert ) const
{
	Assert( index >= 0 );
	Assert( index < MAX_VERT_COUNT );
	VectorCopy( m_pVerts[index].m_FlatVert, vert );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispInfo::SetFlatVert( int index, const Vector &vert )
{
	Assert( index >= 0 );
	Assert( index < MAX_VERT_COUNT );
	VectorCopy( vert, m_pVerts[index].m_FlatVert );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispInfo::SetNormal( int index, Vector const &normal )
{
	Assert( index >= 0 );
	Assert( index < MAX_VERT_COUNT );
	VectorCopy( normal, m_pVerts[index].m_Normal );
}

	
inline void CCoreDispInfo::SetNormal( const CVertIndex &index, Vector const &normal )
{
	SetNormal( VertIndexToInt( index ), normal );
}

	
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispInfo::GetNormal( int index, Vector& normal ) const
{
	Assert( index >= 0 );
	Assert( index < MAX_VERT_COUNT );
	VectorCopy( m_pVerts[index].m_Normal, normal );
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline const Vector& CCoreDispInfo::GetNormal( int index ) const
{
	Assert( index >= 0 );
	Assert( index < MAX_VERT_COUNT );
	return m_pVerts[index].m_Normal;
}


inline const Vector& CCoreDispInfo::GetNormal( const CVertIndex &index ) const
{
	return GetNormal( VertIndexToInt( index ) );
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispInfo::GetTangentS( int index, Vector& tangentS ) const
{
	Assert( index >= 0 );
	Assert( index < GetSize() );
	VectorCopy( m_pVerts[index].m_TangentS, tangentS );
}

inline const Vector &CCoreDispInfo::GetTangentS( int index ) const
{
	Assert( index >= 0 );
	Assert( index < GetSize() );
	return m_pVerts[index].m_TangentS;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispInfo::GetTangentT( int index, Vector& tangentT ) const
{
	Assert( index >= 0 );
	Assert( index < GetSize() );
	VectorCopy( m_pVerts[index].m_TangentT, tangentT );
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispInfo::SetTexCoord( int index, Vector2D const& texCoord )
{
	Assert( index >= 0 );
	Assert( index < GetSize() );
	Vector2DCopy( texCoord, m_pVerts[index].m_TexCoord );
}

	
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispInfo::GetTexCoord( int index, Vector2D& texCoord ) const
{
	Assert( index >= 0 );
	Assert( index < GetSize() );
	Vector2DCopy( m_pVerts[index].m_TexCoord, texCoord );
}

	
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispInfo::SetLuxelCoord( int bumpIndex, int index, Vector2D const& luxelCoord )
{
	Assert( index >= 0 );
	Assert( index < GetSize() );
	Assert( bumpIndex >= 0 );
	Assert( bumpIndex < NUM_BUMP_VECTS + 1 );
	Vector2DCopy( luxelCoord, m_pVerts[index].m_LuxelCoords[bumpIndex] );
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispInfo::GetLuxelCoord( int bumpIndex, int index, Vector2D& luxelCoord ) const 
{
	Assert( index >= 0 );
	Assert( index < MAX_VERT_COUNT );
	Assert( bumpIndex >= 0 );
	Assert( bumpIndex < NUM_BUMP_VECTS + 1 );
	Vector2DCopy( m_pVerts[index].m_LuxelCoords[bumpIndex], luxelCoord );
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispInfo::SetAlpha( int index, float alpha )
{
	Assert( index >= 0 );
	Assert( index < MAX_VERT_COUNT );
	m_pVerts[index].m_Alpha = alpha;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline float CCoreDispInfo::GetAlpha( int index )
{
	Assert( index >= 0 );
	Assert( index < MAX_VERT_COUNT );
	return m_pVerts[index].m_Alpha;
}

inline void CCoreDispInfo::SetMultiBlend( int index, Vector4D &vBlend, Vector4D &vAlphaBlend, Vector &vColor1, Vector &vColor2, Vector &vColor3, Vector &vColor4 )
{
	Assert( index >= 0 );
	Assert( index < MAX_VERT_COUNT );

	m_pVerts[ index ].m_MultiBlend = vBlend;
	m_pVerts[ index ].m_AlphaBlend = vAlphaBlend;
	m_pVerts[ index ].m_vBlendColors[ 0 ] = vColor1;
	m_pVerts[ index ].m_vBlendColors[ 1 ] = vColor2;
	m_pVerts[ index ].m_vBlendColors[ 2 ] = vColor3;
	m_pVerts[ index ].m_vBlendColors[ 3 ] = vColor4;
}


void CCoreDispInfo::SetMultiBlendColor( int index, int nColorIndex, Vector &vColor )
{
	Assert( index >= 0 );
	Assert( index < MAX_VERT_COUNT );

	m_pVerts[ index ].m_vBlendColors[ nColorIndex ] = vColor;
}


inline void CCoreDispInfo::SetMultiBlend( int index, Vector4D &vBlend )
{
	Assert( index >= 0 );
	Assert( index < MAX_VERT_COUNT );

	m_pVerts[ index ].m_MultiBlend = vBlend;
}


inline void CCoreDispInfo::SetAlphaBlend( int index, Vector4D &vAlphaBlend )
{
	Assert( index >= 0 );
	Assert( index < MAX_VERT_COUNT );

	m_pVerts[ index ].m_AlphaBlend = vAlphaBlend;
}


inline void CCoreDispInfo::GetMultiBlend( int index, Vector4D &vBlend, Vector4D &vAlphaBlend, Vector &vColor1, Vector &vColor2, Vector &vColor3, Vector &vColor4 ) const
{
	Assert( index >= 0 );
	Assert( index < MAX_VERT_COUNT );

	vBlend = m_pVerts[ index ].m_MultiBlend;
	vAlphaBlend = m_pVerts[ index ].m_AlphaBlend;
	vColor1 = m_pVerts[ index ].m_vBlendColors[ 0 ];
	vColor2 = m_pVerts[ index ].m_vBlendColors[ 1 ];
	vColor3 = m_pVerts[ index ].m_vBlendColors[ 2 ];
	vColor4 = m_pVerts[ index ].m_vBlendColors[ 3 ];
}


inline void CCoreDispInfo::GetMultiBlend( int index, Vector4D &vBlend ) const
{
	Assert( index >= 0 );
	Assert( index < MAX_VERT_COUNT );

	vBlend = m_pVerts[ index ].m_MultiBlend;
}


inline void CCoreDispInfo::GetAlphaBlend( int index, Vector4D &vAlphaBlend ) const
{
	Assert( index >= 0 );
	Assert( index < MAX_VERT_COUNT );

	vAlphaBlend = m_pVerts[ index ].m_AlphaBlend;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispInfo::SetElevation( float elevation )
{
	m_Elevation = elevation;
}

	
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline float CCoreDispInfo::GetElevation( void )
{
	return m_Elevation;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispInfo::ResetFieldVectors( void )
{
//	Vector normal;
//	m_Surf.GetNormal( normal );

	int size = GetSize();
	for( int i = 0; i < size; i++ )
	{
		m_pVerts[i].m_FieldVector.Init();
//		m_FieldVectors[i] = normal;
	}
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispInfo::SetFieldVector( int index, Vector const &v )
{
	Assert( index >= 0 );
	Assert( index < MAX_VERT_COUNT );
	VectorCopy( v, m_pVerts[index].m_FieldVector ); 
}

	
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispInfo::GetFieldVector( int index, Vector& v )
{
	Assert( index >= 0 );
	Assert( index < MAX_VERT_COUNT );
	VectorCopy( m_pVerts[index].m_FieldVector, v ); 
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispInfo::ResetSubdivPositions( void )
{
	int size = GetSize();
	for( int i = 0; i < size; i++ )
	{
		m_pVerts[i].m_SubdivPos.Init();
	}
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispInfo::SetSubdivPosition( int ndx, Vector const &v )
{
	Assert( ndx >= 0 );
	Assert( ndx < MAX_VERT_COUNT );
	m_pVerts[ndx].m_SubdivPos = v;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispInfo::GetSubdivPosition( int ndx, Vector& v )
{
	Assert( ndx >= 0 );
	Assert( ndx < MAX_VERT_COUNT );
	v = m_pVerts[ndx].m_SubdivPos;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispInfo::ResetSubdivNormals( void )
{
	Vector normal;
	m_Surf.GetNormal( normal );

	int size = GetSize();
	for( int i = 0; i < size; i++ )
	{
		m_pVerts[i].m_SubdivNormal = normal;
	}
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispInfo::SetSubdivNormal( int ndx, Vector const &v )
{
	Assert( ndx >= 0 );
	Assert( ndx < MAX_VERT_COUNT );
	m_pVerts[ndx].m_SubdivNormal = v;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispInfo::GetSubdivNormal( int ndx, Vector &v )
{
	Assert( ndx >= 0 );
	Assert( ndx < MAX_VERT_COUNT );
	v = m_pVerts[ndx].m_SubdivNormal;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispInfo::ResetFieldDistances( void )
{
	int size = GetSize();
	for( int i = 0; i < size; i++ )
	{
		m_pVerts[i].m_FieldDistance = 0.0f;
	}
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispInfo::SetFieldDistance( int index, float dist )
{
	Assert( index >= 0 );
	Assert( index < GetSize() );
	m_pVerts[index].m_FieldDistance = dist;
}

	
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline float CCoreDispInfo::GetFieldDistance( int index )
{
	Assert( index >= 0 );
	Assert( index < GetSize() );
	return m_pVerts[index].m_FieldDistance;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispInfo::SetRenderIndexCount( int count )
{
	m_RenderIndexCount = count;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline int CCoreDispInfo::GetRenderIndexCount( void )
{
	return m_RenderIndexCount;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispInfo::SetRenderIndex( int index, int triIndex )
{
	Assert( index >= 0 );
	Assert( index < ( MAX_VERT_COUNT*2*3) );
	m_RenderIndices[index] = triIndex;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline int CCoreDispInfo::GetRenderIndex( int index )
{
	Assert( index >= 0 );
	Assert( index < ( MAX_VERT_COUNT*2*3) );
	return m_RenderIndices[index];
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline CoreDispVert_t *CCoreDispInfo::GetDispVertList()
{
	return m_pVerts;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline unsigned short *CCoreDispInfo::GetRenderIndexList( void )
{
	return &m_RenderIndices[0];
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void CCoreDispInfo::SetTouched( bool touched )
{
	m_bTouched = touched;
}

	
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline bool CCoreDispInfo::IsTouched( void )
{
	return m_bTouched;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline CCoreDispNode *CCoreDispInfo::GetNode( int index )
{
	Assert( index >= 0 );
	Assert( index < MAX_NODE_COUNT );
	return &m_Nodes[index];
}

bool CalcBarycentricCooefs( Vector const &v0, Vector const &v1, Vector const &v2,
						    Vector const &pt, float &c0, float &c1, float &c2 );

#endif // BUILDDISP_H
