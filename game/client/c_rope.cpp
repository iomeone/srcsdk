//========= Copyright � 1996-2007, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=====================================================================================//
#include "cbase.h"
#include "c_rope.h"
#include "beamdraw.h"
#include "view.h"
#include "env_wind_shared.h"
#include "input.h"
#include "rope_helpers.h"
#include "engine/ivmodelinfo.h"
#include "tier0/vprof.h"
#include "c_te_effect_dispatch.h"
#include "collisionutils.h"
#include <KeyValues.h>
#include <bitbuf.h>
#include "utllinkedlist.h"
#include "tier1/callqueue.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static void PrecacheCable( void* )
{
	PrecacheMaterial( "cable/rope_shadowdepth" );
}
PRECACHE_REGISTER_FN(PrecacheCable);

void RecvProxy_RecomputeSprings( const CRecvProxyData *pData, void *pStruct, void *pOut )
{
	// Have the regular proxy store the data.
	RecvProxy_Int32ToInt32( pData, pStruct, pOut );

	C_RopeKeyframe *pRope = static_cast<C_RopeKeyframe*>( pStruct );
	pRope->RecomputeSprings();
}


IMPLEMENT_CLIENTCLASS_DT_NOBASE( C_RopeKeyframe, DT_RopeKeyframe, CRopeKeyframe )
	RecvPropInt( RECVINFO(m_nChangeCount) ),
	RecvPropInt( RECVINFO(m_iRopeMaterialModelIndex) ),
	RecvPropEHandle( RECVINFO(m_hStartPoint) ),
	RecvPropEHandle( RECVINFO(m_hEndPoint) ),
	RecvPropInt( RECVINFO(m_iStartAttachment) ),
	RecvPropInt( RECVINFO(m_iEndAttachment) ),

	RecvPropInt( RECVINFO(m_fLockedPoints) ),
	RecvPropInt( RECVINFO(m_Slack), 0, RecvProxy_RecomputeSprings ),
	RecvPropInt( RECVINFO(m_RopeLength), 0, RecvProxy_RecomputeSprings ),
	RecvPropInt( RECVINFO(m_RopeFlags) ),
	RecvPropFloat( RECVINFO(m_TextureScale) ),
	RecvPropInt( RECVINFO(m_nSegments) ),
	RecvPropBool( RECVINFO(m_bConstrainBetweenEndpoints) ),
	RecvPropInt( RECVINFO(m_Subdiv) ),

	RecvPropFloat( RECVINFO(m_Width) ),
	RecvPropFloat( RECVINFO(m_flScrollSpeed) ),
	RecvPropVector( RECVINFO_NAME( m_vecNetworkOrigin, m_vecOrigin ) ),
	RecvPropInt( RECVINFO_NAME(m_hNetworkMoveParent, moveparent), 0, RecvProxy_IntToMoveParent ),
	
	RecvPropInt( RECVINFO( m_iParentAttachment ) ),
END_RECV_TABLE()

#define ROPE_IMPULSE_SCALE	20
#define ROPE_IMPULSE_DECAY	0.95

static ConVar rope_shake( "rope_shake", "0" );
static ConVar rope_subdiv( "rope_subdiv", "2", 0, "Rope subdivision amount", true, 0, true, MAX_ROPE_SUBDIVS );
static ConVar rope_collide( "rope_collide", "1", 0, "Collide rope with the world" );
/*
static ConVar rope_smooth( "rope_smooth", "1", 0, "Do an antialiasing effect on ropes" );
static ConVar rope_smooth_enlarge( "rope_smooth_enlarge", "1.4", 0, "How much to enlarge ropes in screen space for antialiasing effect" );

static ConVar rope_smooth_minwidth( "rope_smooth_minwidth", "0.3", 0, "When using smoothing, this is the min screenspace width it lets a rope shrink to" );
static ConVar rope_smooth_minalpha( "rope_smooth_minalpha", "0.2", 0, "Alpha for rope antialiasing effect" );

static ConVar rope_smooth_maxalphawidth( "rope_smooth_maxalphawidth", "1.75" );
static ConVar rope_smooth_maxalpha( "rope_smooth_maxalpha", "0.5", 0, "Alpha for rope antialiasing effect" );
*/
//static ConVar mat_fullbright( "mat_fullbright", "0", FCVAR_CHEAT ); // get it from the engine

static ConVar r_drawropes( "r_drawropes", "1", FCVAR_CHEAT );
static ConVar r_ropetranslucent( "r_ropetranslucent", "1");

static ConVar rope_wind_dist( "rope_wind_dist", "1000", 0, "Don't use CPU applying small wind gusts to ropes when they're past this distance." );
static ConVar rope_averagelight( "rope_averagelight", "1", 0, "Makes ropes use average of cubemap lighting instead of max intensity." );

/*
static ConVar rope_rendersolid( "rope_rendersolid", "1" );

static ConVar rope_solid_minwidth( "rope_solid_minwidth", "0.3" );
static ConVar rope_solid_maxwidth( "rope_solid_maxwidth", "1" );

static ConVar rope_solid_minalpha( "rope_solid_minalpha", "0.0" );
static ConVar rope_solid_maxalpha( "rope_solid_maxalpha", "1" );
*/

static int			g_nRopePointsSimulated;

static CMaterialReference g_pSplineCableShadowdepth;
// Active ropes.
CUtlLinkedList<C_RopeKeyframe*, int> g_Ropes;

// Precalculated info for rope subdivision.
static float g_flLockAmount = 0.1;
static float g_flLockFalloff = 0.3;

class CRopeManager : public IRopeManager
{
public:

	CRopeManager();
	~CRopeManager();

	void ResetRenderCache() OVERRIDE;
	void AddToRenderCache( C_RopeKeyframe *pRope ) OVERRIDE;
	void DrawRenderCache( bool bShadowDepth ) OVERRIDE;

	enum { MAX_ROPE_RENDERCACHE	= 128 };

	void RemoveRopeFromQueuedRenderCaches( C_RopeKeyframe *pRope );
	
private:
	struct RopeRenderData_t
	{
		IMaterial			*m_pSolidMaterial;
		int					m_nCacheCount;
		C_RopeKeyframe		*m_aCache[MAX_ROPE_RENDERCACHE];
	};

	struct RopeQueuedRenderCache_t
	{
		RopeRenderData_t *pCaches;
		int iCacheCount;
		CThreadFastMutex *m_pRopeDataMutex;
		RopeQueuedRenderCache_t() : pCaches( NULL ), iCacheCount( 0 ), m_pRopeDataMutex( NULL ) {}
	};

	void DrawRenderCache_NonQueued( bool bShadowDepth, RopeRenderData_t *pRenderCache, int nRenderCacheCount, const Vector &vCurrentViewForward, const Vector &vCurrentViewOrigin, C_RopeKeyframe::BuildRopeQueuedData_t *pBuildRopeQueuedData, CThreadFastMutex *pRopeDataMutex );

	CUtlVector<RopeRenderData_t>	m_aRenderCache;
	
	//in queued material system mode we need to store off data for later use.
	IMaterial* m_pDepthWriteMaterial;
	CUtlLinkedList<RopeQueuedRenderCache_t> m_RopeQueuedRenderCaches;	
	CThreadFastMutex		m_RopeQueuedRenderCaches_Mutex; //mutex just for changing m_RopeQueuedRenderCaches
};

static CRopeManager s_RopeManager;

IRopeManager *RopeManager()
{
	return &s_RopeManager;
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CRopeManager::CRopeManager()
{
	m_aRenderCache.Purge();
	m_pDepthWriteMaterial = NULL;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CRopeManager::~CRopeManager()
{
	m_aRenderCache.Purge();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CRopeManager::ResetRenderCache()
{
	const int nRenderCacheCount = m_aRenderCache.Count();
	for ( int iRenderCache = 0; iRenderCache < nRenderCacheCount; ++iRenderCache )
	{
		m_aRenderCache[iRenderCache].m_nCacheCount = 0;
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CRopeManager::AddToRenderCache( C_RopeKeyframe *pRope )
{
	if( !pRope->GetSolidMaterial() )
	{
		return;
	}
	
	// Find the current rope list.
	int iRenderCache = 0;
	const int nRenderCacheCount = m_aRenderCache.Count();
	for ( ; iRenderCache < nRenderCacheCount; ++iRenderCache )
	{
		if ( pRope->GetSolidMaterial() == m_aRenderCache[iRenderCache].m_pSolidMaterial )
			break;
	}

	// A full rope list should have been generate in CreateRenderCache
	// If we didn't find one, then allocate the mofo.
	if ( iRenderCache == nRenderCacheCount )
	{
		iRenderCache = m_aRenderCache.AddToTail();
		m_aRenderCache[iRenderCache].m_pSolidMaterial = pRope->GetSolidMaterial();
		m_aRenderCache[iRenderCache].m_nCacheCount = 0;
	}

	if ( m_aRenderCache[iRenderCache].m_nCacheCount >= MAX_ROPE_RENDERCACHE )
	{
		Warning( "CRopeManager::AddToRenderCache count to large for cache!\n" );
		return;
	}

	m_aRenderCache[iRenderCache].m_aCache[m_aRenderCache[iRenderCache].m_nCacheCount] = pRope;
	++m_aRenderCache[iRenderCache].m_nCacheCount;
}

#define OUTPUT_2SPLINE_VERTS( t, u )									\
	meshBuilder.Color4ub( nRed, nGreen, nBlue, nAlpha );	\
	meshBuilder.Position3f( (t), u, 0 );					\
	meshBuilder.TexCoord4fv( 0, vecP0.Base() );				\
	meshBuilder.TexCoord4fv( 1, vecP1.Base() );				\
	meshBuilder.TexCoord4fv( 2, vecP2.Base() );				\
	meshBuilder.TexCoord4fv( 3, vecP3.Base() );				\
	meshBuilder.AdvanceVertexF<VTX_HAVEPOS | VTX_HAVECOLOR, 5>(); \
	meshBuilder.Color4ub( nRed, nGreen, nBlue, nAlpha );	\
	meshBuilder.Position3f( (t), u, 1 );					\
	meshBuilder.TexCoord4fv( 0, vecP0.Base() );				\
	meshBuilder.TexCoord4fv( 1, vecP1.Base() );				\
	meshBuilder.TexCoord4fv( 2, vecP2.Base() );				\
	meshBuilder.TexCoord4fv( 3, vecP3.Base() );				\
	meshBuilder.AdvanceVertexF<VTX_HAVEPOS | VTX_HAVECOLOR, 5>(); 


void CRopeManager::DrawRenderCache_NonQueued( bool bShadowDepth, RopeRenderData_t *pRenderCache, int nRenderCacheCount, const Vector &vCurrentViewForward, const Vector &vCurrentViewOrigin, C_RopeKeyframe::BuildRopeQueuedData_t *pBuildRopeQueuedData, CThreadFastMutex *pRopeDataMutex )
{
	VPROF_BUDGET( "CRopeManager::DrawRenderCache", VPROF_BUDGETGROUP_ROPES );

	CThreadFastMutex dummyMutex;
	if( pRopeDataMutex == NULL )
		pRopeDataMutex = &dummyMutex;

	if ( bShadowDepth && !m_pDepthWriteMaterial && g_pMaterialSystem )
	{
		KeyValues *pVMTKeyValues = new KeyValues( "DepthWrite" );
		pVMTKeyValues->SetInt( "$no_fullbright", 1 );
		pVMTKeyValues->SetInt( "$alphatest", 0 );
		pVMTKeyValues->SetInt( "$nocull", 1 );
		m_pDepthWriteMaterial = g_pMaterialSystem->FindProceduralMaterial( "__DepthWrite01", TEXTURE_GROUP_OTHER, pVMTKeyValues );
	}

	CMatRenderContextPtr pRenderContext( materials );
	{
		AUTO_LOCK_FM( *pRopeDataMutex );
		const int defaultSubdiv = rope_subdiv.GetInt();
		for ( int iRenderCache = 0; iRenderCache < nRenderCacheCount; ++iRenderCache )
		{
			const int nCacheCount = pRenderCache[iRenderCache].m_nCacheCount;

			int nTotalVerts = 0;
			int nTotalIndices = 0;
			for ( int iCache = 0; iCache < nCacheCount; ++iCache )
			{
				C_RopeKeyframe *pRope = pRenderCache[iRenderCache].m_aCache[iCache];
				if ( pRope )
				{
					const int segs =  pRope->m_RopePhysics.NumNodes()-1;
					const int nSubdivCount = (pRope->m_Subdiv != 255 ? pRope->m_Subdiv : defaultSubdiv) + 1;
					nTotalVerts += ((2 * nSubdivCount) * segs) + 2;
					nTotalIndices += (6 * nSubdivCount) * segs;
				}
			}
			if ( nTotalVerts == 0 )
				continue;		

			IMaterial *pMaterial = bShadowDepth ? g_pSplineCableShadowdepth : pRenderCache[iRenderCache].m_pSolidMaterial;

			// Need to make sure that all rope materials use the cable shader since there are a lot of assumptions about how the shader interfaces with this code.
		//	AssertOnce( V_stricmp( pMaterial->GetShaderName(), "cable" ) == 0 );

			pRenderContext->Bind( pMaterial );

			int nMaxVertices = pRenderContext->GetMaxVerticesToRender( pMaterial );
			int nMaxIndices = pRenderContext->GetMaxIndicesToRender();

			IMesh* pMesh = pRenderContext->GetDynamicMesh( true );
			CMeshBuilder meshBuilder;
			int meshVertCount = MIN(nTotalVerts, nMaxVertices);
			int meshIndexCount = MIN(nTotalIndices, nMaxIndices);
			meshBuilder.Begin( pMesh, MATERIAL_TRIANGLES, meshVertCount, meshIndexCount );
			int nCurIDX = 0;

			int availableVerts = meshVertCount;
			int availableIndices = meshIndexCount;
			float flLastU = 1.0f;

		for ( int iCache = 0; iCache < nCacheCount; ++iCache )
		{
			C_RopeKeyframe *pRope = pRenderCache[iRenderCache].m_aCache[iCache];
			if ( pRope )
			{
				CSimplePhysics::CNode *pNode = pRope->m_RopePhysics.GetFirstNode();
				int nSegmentsToRender = pRope->m_RopePhysics.NumNodes()-1;
				if ( !nSegmentsToRender )
					continue;
				
				const int nParticles = pRope->m_RopePhysics.NumNodes();
				const int nSubdivCount = (pRope->m_Subdiv != 255 ? pRope->m_Subdiv : defaultSubdiv) + 1;

				const int nNumIndicesPerSegment = 6 * nSubdivCount;
				const int nNumVerticesPerSegment = 2 * nSubdivCount;

				int nSegmentsAvailableInBuffer = MIN( ( availableVerts - 2 ) / nNumVerticesPerSegment,
					( availableIndices ) / nNumIndicesPerSegment );

				const int segmentsInBuffer = MIN(nSegmentsAvailableInBuffer,nSegmentsToRender);
				availableIndices -= nNumIndicesPerSegment * segmentsInBuffer;
				availableVerts -= 2 + (nNumVerticesPerSegment * segmentsInBuffer);

				const float width = pRope->m_Width;
				const Vector& vModColor = pRope->m_vColorMod;
				Vector *pColors = pRope->m_LightValues;
					
				// Figure out texture scale.
				const float flPixelsPerInch = 4.0f / pRope->m_TextureScale;
				// This is the total number of texels for the length of the whole rope.
				const float flTotalTexCoord = flPixelsPerInch * ( pRope->m_RopeLength + pRope->m_Slack + ROPESLACK_FUDGEFACTOR );
				const int nTotalPoints = (nSegmentsToRender * (nSubdivCount-1)) + 1;
				const float flDU = ( flTotalTexCoord / nTotalPoints ) / static_cast<float>( pRope->m_TextureHeight );
				float flU = pRope->m_flCurScroll;
				const float m_flTStep = 1.0f / float(nSubdivCount);

				bool bFirstPoint = true;

				// initialize first spline segment
				Vector4D vecP1;
				Vector4D vecP2;
				vecP1.Init( pNode[0].m_vPredicted, pRope->m_Width );
				vecP2.Init( pNode[1].m_vPredicted, pRope->m_Width );
				Vector4D vecP0 = vecP1;

				uint8 nRed = 0;
				uint8 nGreen = 0;
				uint8 nBlue = 0;
				const uint8 nAlpha = 255;

				Vector4D vecDelta = vecP2;
				vecDelta -= vecP1;
				vecP0 -= vecDelta;

				Vector4D vecP3;

				if ( nParticles < 3 )
				{
					vecP3 = vecP2;
					vecP3 += vecDelta;
				}
				else
				{
					vecP3.Init( pNode[2].m_vPredicted, width );
				}
				int nPnt = 3;
				int nColor = 1;
				Vector vColor0( pColors[0].x * vModColor.x, pColors[0].y * vModColor.y, pColors[0].z * vModColor.z );
				Vector vColor1( pColors[1].x * vModColor.x, pColors[1].y * vModColor.y, pColors[1].z * vModColor.z );
				//to unify the BuildRope code, emulate the queued data
				float flT = 0;
				do
				{
					if ( ! nSegmentsAvailableInBuffer )
					{
						meshBuilder.End();
						pMesh->Draw();
						nTotalVerts -= (meshVertCount - availableVerts);
						nTotalIndices -= (meshIndexCount - availableIndices);
						meshVertCount = MIN(nTotalVerts, nMaxVertices);
						meshIndexCount = MIN(nTotalIndices, nMaxIndices);
						meshBuilder.Begin( pMesh, MATERIAL_TRIANGLES, meshVertCount, meshIndexCount );
						availableVerts = meshVertCount;
						availableIndices = meshIndexCount;
					
						OUTPUT_2SPLINE_VERTS( flT, flLastU );

						nSegmentsAvailableInBuffer = MIN( ( availableVerts - 2 ) / nNumVerticesPerSegment,
							availableIndices / nNumIndicesPerSegment );

						nCurIDX = 0;
					}
					nSegmentsAvailableInBuffer--;
					flT = 0.;
					for( int nSlice = 0 ; nSlice < nSubdivCount; nSlice++ )
					{
						const float omt = 1.0f - flT;
						nRed = FastFToC( (vColor0.x * omt) + (vColor1.x*flT) );
						nGreen = FastFToC( (vColor0.y * omt) + (vColor1.y*flT) );
						nBlue = FastFToC( (vColor0.z * omt) + (vColor1.z*flT) );
						OUTPUT_2SPLINE_VERTS( flT, flU );
						flT += m_flTStep;
						flU += flDU;
						if ( ! bFirstPoint )
						{
							meshBuilder.FastIndex( nCurIDX );
							meshBuilder.FastIndex( nCurIDX+1 );
							meshBuilder.FastIndex( nCurIDX+2 );
							meshBuilder.FastIndex( nCurIDX+1 );
							meshBuilder.FastIndex( nCurIDX+3 );
							meshBuilder.FastIndex( nCurIDX+2 );
							nCurIDX += 2;
						}
						bFirstPoint = false;
					}

					vColor0 = vColor1;
					if ( nColor < nParticles-1 )
					{
						nColor++;
						vColor1.Init( pColors[nColor].x * vModColor.x, pColors[nColor].y * vModColor.y, pColors[nColor].z * vModColor.z );
					}
					if ( nSegmentsToRender > 1 )
					{
						vecP0 = vecP1;
						vecP1 = vecP2;
						vecP2 = vecP3;

						if ( nPnt < nParticles )
						{
							vecP3.AsVector3D() = pNode[nPnt].m_vPredicted;
							nPnt++;
						}
						else
						{

							vecP3 += vecP2;
							vecP3 -= vecP1;
						}
					}
				} while( --nSegmentsToRender );
				// Render the solid portion of the ropes.
				// output last piece
				OUTPUT_2SPLINE_VERTS( 1.0, flU );
				meshBuilder.FastIndex( nCurIDX );
				meshBuilder.FastIndex( nCurIDX+1 );
				meshBuilder.FastIndex( nCurIDX+2 );
				meshBuilder.FastIndex( nCurIDX+1 );
				meshBuilder.FastIndex( nCurIDX+3 );
				meshBuilder.FastIndex( nCurIDX+2 );
				nCurIDX += 4;
				flLastU = flU;
				}
			}
			meshBuilder.End();
			pMesh->Draw();
		}
	}

	m_RopeQueuedRenderCaches_Mutex.Lock();
	if( pBuildRopeQueuedData && (m_RopeQueuedRenderCaches.Count() != 0) )
	{
		const unsigned short iHeadIndex = m_RopeQueuedRenderCaches.Head();
		delete m_RopeQueuedRenderCaches[iHeadIndex].m_pRopeDataMutex;
		m_RopeQueuedRenderCaches.Remove( iHeadIndex );
	}
	m_RopeQueuedRenderCaches_Mutex.Unlock();
}

ConVar r_queued_ropes( "r_queued_ropes", "1" );

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CRopeManager::DrawRenderCache( bool bShadowDepth )
{
	// Check to see if we want to render the ropes.
	if( !r_drawropes.GetBool() )
		return;

	const int iRenderCacheCount = m_aRenderCache.Count();

	if( iRenderCacheCount == 0 )
		return;

	const Vector& vForward = CurrentViewForward();
	const Vector& vOrigin = CurrentViewOrigin();

	CMatRenderContextPtr pRenderContext(materials);
	ICallQueue *pCallQueue;
	if( r_queued_ropes.GetBool() && (pCallQueue = pRenderContext->GetCallQueue()) != NULL )
	{
		//material queue available and desired
		CRopeManager::RopeRenderData_t *pRenderCache = m_aRenderCache.Base();
		
		int iRopeCount = 0;
		int iNodeCount = 0;
		for( int i = 0; i != iRenderCacheCount; ++i )
		{
			CRopeManager::RopeRenderData_t *pCache = &pRenderCache[i];
			const int iCacheCount = pCache->m_nCacheCount;
			iRopeCount += iCacheCount;
			for( int j = 0; j != iCacheCount; ++j )
			{
				C_RopeKeyframe *pRope = pCache->m_aCache[j];
				if( pRope )
					iNodeCount += pRope->m_RopePhysics.NumNodes();
				else
					--iRopeCount;
			}
		}

		if( iRopeCount == 0 )
			return; //nothing to draw

		const size_t iMemoryNeeded = (iRenderCacheCount * sizeof(CRopeManager::RopeRenderData_t)) + 
								(iRopeCount * sizeof(C_RopeKeyframe::BuildRopeQueuedData_t)) +
								(iNodeCount * (sizeof(Vector) * 2));

		CMatRenderData< byte > rd(pRenderContext, iMemoryNeeded);
		void *pMemory = rd.Base();

		CRopeManager::RopeRenderData_t *pRenderCachesStart = static_cast<CRopeManager::RopeRenderData_t *>( pMemory );
		C_RopeKeyframe::BuildRopeQueuedData_t *pBuildRopeQueuedDataStart = reinterpret_cast<C_RopeKeyframe::BuildRopeQueuedData_t *>( pRenderCachesStart + iRenderCacheCount );
		Vector *pVectorDataStart = reinterpret_cast<Vector *>( pBuildRopeQueuedDataStart + iRopeCount );
		
		//memcpy( pRenderCachesStart, m_aRenderCache.Base(), iRenderCacheCount * sizeof( CRopeManager::RopeRenderData_t ) );

		RopeQueuedRenderCache_t cache;
		cache.pCaches = pRenderCachesStart;
		cache.iCacheCount = iRenderCacheCount;
		cache.m_pRopeDataMutex = new CThreadFastMutex;
		
		C_RopeKeyframe::BuildRopeQueuedData_t *pWriteRopeQueuedData = pBuildRopeQueuedDataStart;
		Vector *pVectorWrite = pVectorDataStart;

		//Setup the rest of our data. This writes to two separate areas of memory at the same time. One area for the C_RopeKeyframe::BuildRopeQueuedData_t array, the other for mini-arrays of vector data
		for( int i = 0; i != iRenderCacheCount; ++i )
		{
			CRopeManager::RopeRenderData_t *pReadCache = &pRenderCache[i];
			CRopeManager::RopeRenderData_t *pWriteCache = &pRenderCachesStart[i];
			const int iCacheCount = pReadCache->m_nCacheCount;
			pWriteCache->m_nCacheCount = 0;
			pWriteCache->m_pSolidMaterial = pReadCache->m_pSolidMaterial;
			for( int j = 0; j != iCacheCount; ++j )
			{
				C_RopeKeyframe *pRope = pReadCache->m_aCache[j];
				if( pRope == NULL )
					continue;

				pWriteCache->m_aCache[pWriteCache->m_nCacheCount] = pRope;
				++pWriteCache->m_nCacheCount;

				const int iNodes = pRope->m_RopePhysics.NumNodes();

				//setup the C_RopeKeyframe::BuildRopeQueuedData_t struct
				pWriteRopeQueuedData->m_iNodeCount = pRope->m_RopePhysics.NumNodes();
				pWriteRopeQueuedData->m_vColorMod = pRope->m_vColorMod;
				pWriteRopeQueuedData->m_RopeLength = pRope->m_RopeLength;
				pWriteRopeQueuedData->m_Slack = pRope->m_Slack;
				pWriteRopeQueuedData->m_pPredictedPositions = pVectorWrite;
				pWriteRopeQueuedData->m_pLightValues = pVectorWrite + iNodes;
				++pWriteRopeQueuedData;

				//make two arrays, one of predicted positions followed immediately by light values
				for( int k = 0; k != iNodes; ++k )
				{					
					pVectorWrite[0] = pRope->m_RopePhysics.GetNode( k )->m_vPredicted;
					pVectorWrite[iNodes] = pRope->m_LightValues[k];
					++pVectorWrite;
				}
				pVectorWrite += iNodes; //so we don't overwrite the light values with the next rope's predicted positions
			}
		}

		m_RopeQueuedRenderCaches_Mutex.Lock();
		const unsigned short iLLIndex = m_RopeQueuedRenderCaches.AddToTail( cache );
		CThreadFastMutex *pRopeDataMutex = m_RopeQueuedRenderCaches[iLLIndex].m_pRopeDataMutex;
		m_RopeQueuedRenderCaches_Mutex.Unlock();
//-----------------------------------------------------------------------------
		Assert( ((void *)pVectorWrite == (void *)(((uint8 *)pMemory) + iMemoryNeeded)) && ((void *)pWriteRopeQueuedData == (void *)pVectorDataStart));		
		pCallQueue->QueueCall( this, &CRopeManager::DrawRenderCache_NonQueued, bShadowDepth, pRenderCachesStart, iRenderCacheCount, vForward, vOrigin, pBuildRopeQueuedDataStart, pRopeDataMutex );
	}
	else
	{
		DrawRenderCache_NonQueued( bShadowDepth, m_aRenderCache.Base(), iRenderCacheCount, vForward, vOrigin, NULL, NULL );
	}
}


void CRopeManager::RemoveRopeFromQueuedRenderCaches( C_RopeKeyframe *pRope )
{
	AUTO_LOCK_FM( m_RopeQueuedRenderCaches_Mutex );

	int index = m_RopeQueuedRenderCaches.Head();
	while( m_RopeQueuedRenderCaches.IsValidIndex( index ) )
	{
		RopeQueuedRenderCache_t &RenderCacheData = m_RopeQueuedRenderCaches[index];
		for( int i = 0; i != RenderCacheData.iCacheCount; ++i )
		{
			RopeRenderData_t *pCache = &RenderCacheData.pCaches[i];
			for( int j = 0; j != pCache->m_nCacheCount; ++j )
			{
				if( pCache->m_aCache[j] == pRope )
				{
					RenderCacheData.m_pRopeDataMutex->Lock();
					pCache->m_aCache[j] = NULL;
					RenderCacheData.m_pRopeDataMutex->Unlock();
				}
			}
		}

		index = m_RopeQueuedRenderCaches.Next( index );
	}	
}

//=============================================================================

// ------------------------------------------------------------------------------------ //
// Global functions.
// ------------------------------------------------------------------------------------ //

void Rope_ResetCounters()
{
	g_nRopePointsSimulated = 0;
}


// ------------------------------------------------------------------------------------ //
// This handles the rope shake command.
// ------------------------------------------------------------------------------------ //

void ShakeRopesCallback( const CEffectData &data )
{
	const Vector& vCenter = data.m_vOrigin;
	const float flRadius = data.m_flRadius;
	const float flMagnitude = data.m_flMagnitude;

	// Now find any nearby ropes and shake them.
	FOR_EACH_LL( g_Ropes, i )
	{
		C_RopeKeyframe *pRope = g_Ropes[i];
	
		pRope->ShakeRope( vCenter, flRadius, flMagnitude );
	}
}

DECLARE_CLIENT_EFFECT( "ShakeRopes", ShakeRopesCallback );




// ------------------------------------------------------------------------------------ //
// C_RopeKeyframe::CPhysicsDelegate
// ------------------------------------------------------------------------------------ //
#define WIND_FORCE_FACTOR 10

void C_RopeKeyframe::CPhysicsDelegate::GetNodeForces( CSimplePhysics::CNode *pNodes, int iNode, Vector *pAccel )
{
	// Gravity.
	if ( !( m_pKeyframe->GetRopeFlags() & ROPE_NO_GRAVITY ) )
	{
		pAccel->Init( ROPE_GRAVITY );
	}

	if( !m_pKeyframe->m_LinksTouchingSomething[iNode] && m_pKeyframe->m_bApplyWind)
	{
		Vector vecWindVel = GetWindspeedAtLocation( m_pKeyframe->m_RopePhysics.GetNode( iNode )->m_vPos );

		if ( vecWindVel.LengthSqr() > 0 )
		{
			VectorMA( *pAccel, WIND_FORCE_FACTOR, vecWindVel, *pAccel );
		}
		else
		{
			if ( ( m_pKeyframe->m_flCurrentGustLifetime != 0.0f ) && ( m_pKeyframe->m_flCurrentGustTimer < m_pKeyframe->m_flCurrentGustLifetime ) )
			{
				const float div = m_pKeyframe->m_flCurrentGustTimer / m_pKeyframe->m_flCurrentGustLifetime;
				const float scale = 1 - cos( div * M_PI );

				*pAccel += m_pKeyframe->m_vWindDir * scale;
			}
		}
	}

	// HACK.. shake the rope around.
	static float scale=15000;
	if( rope_shake.GetInt() )
	{
		*pAccel += RandomVector( -scale, scale );
	}

	// Apply any instananeous forces and reset
	//*pAccel += ROPE_IMPULSE_SCALE * m_pKeyframe->m_vecImpulse;
	*pAccel += ROPE_IMPULSE_SCALE * pNodes[iNode].m_vAbsImpulse;
	m_pKeyframe->m_vecImpulse *= ROPE_IMPULSE_DECAY;
	pNodes[iNode].m_vAbsImpulse *= ROPE_IMPULSE_DECAY;
	if ( m_pKeyframe->m_vecImpulse.LengthSqr() < 0.1f )
	{
		m_pKeyframe->m_vecImpulse = vec3_origin;
	}
	if ( pNodes[iNode].m_vAbsImpulse.LengthSqr() < 0.1f )
	{
		pNodes[iNode].m_vAbsImpulse = vec3_origin;
	}
}


void LockNodeDirection( 
	CSimplePhysics::CNode *pNodes, 
	int parity, 
	int nFalloffNodes,
	float flLockAmount,
	float flLockFalloff,
	const Vector &vIdealDir ) 
{
	for ( int i=0; i < nFalloffNodes; i++ )
	{
		Vector &v0 = pNodes[i*parity].m_vPos;
		Vector &v1 = pNodes[(i+1)*parity].m_vPos;

		Vector vDir = v1 - v0;
		const float len = vDir.Length();
		if ( len > 0.0001f )
		{
			vDir /= len;

			Vector vActual;
			VectorLerp( vDir, vIdealDir, flLockAmount, vActual );
			v1 = v0 + vActual * len;

			flLockAmount *= flLockFalloff;
		}
	}
}


void C_RopeKeyframe::CPhysicsDelegate::ApplyConstraints( CSimplePhysics::CNode *pNodes, int nNodes )
{
	VPROF( "CPhysicsDelegate::ApplyConstraints" );

	// Collide with the world.
	if( ((m_pKeyframe->m_RopeFlags & ROPE_COLLIDE) && 
		rope_collide.GetInt()) || 
		(rope_collide.GetInt() == 2) )
	{
		CTraceFilterWorldOnly traceFilter;

		for( int i=0; i < nNodes; i++ )
		{
			CSimplePhysics::CNode *pNode = &pNodes[i];

			int iIteration;
			const int nIterations = 10;
			for( iIteration=0; iIteration < nIterations; iIteration++ )
			{
				trace_t trace;
				UTIL_TraceHull( pNode->m_vPrevPos, pNode->m_vPos, 
					Vector(-2,-2,-2), Vector(2,2,2), MASK_SOLID_BRUSHONLY, &traceFilter, &trace );

				if( trace.fraction == 1 )
					break;

				if( trace.fraction == 0 || trace.allsolid || trace.startsolid )
				{
					m_pKeyframe->m_LinksTouchingSomething[i] = true;
					pNode->m_vPos = pNode->m_vPrevPos;
					break;
				}

				// Apply some friction.
				const float flSlowFactor = 0.3f;
				pNode->m_vPos -= (pNode->m_vPos - pNode->m_vPrevPos) * flSlowFactor;

				// Move it out along the face normal.
				const float distBehind = trace.plane.normal.Dot( pNode->m_vPos ) - trace.plane.dist;
				pNode->m_vPos += trace.plane.normal * (-distBehind + 2.2);
				m_pKeyframe->m_LinksTouchingSomething[i] = true;
			}

			if( iIteration == nIterations )
				pNodes[i].m_vPos = pNodes[i].m_vPrevPos;
		}
	}

	// Lock the endpoints.
	QAngle angles;
	if( m_pKeyframe->m_fLockedPoints & ROPE_LOCK_START_POINT )
	{
		m_pKeyframe->GetEndPointAttachment( 0, pNodes[0].m_vPos, angles );
		if (( m_pKeyframe->m_fLockedPoints & ROPE_LOCK_START_DIRECTION ) && (nNodes > 3))
		{
			Vector forward;
			AngleVectors( angles, &forward );

			const int parity = 1;
			const int nFalloffNodes = min( 2, nNodes - 2 );
			LockNodeDirection( pNodes, parity, nFalloffNodes, g_flLockAmount, g_flLockFalloff, forward );
		}
	}

	if( m_pKeyframe->m_fLockedPoints & ROPE_LOCK_END_POINT )
	{
		m_pKeyframe->GetEndPointAttachment( 1, pNodes[nNodes-1].m_vPos, angles );
		if( m_pKeyframe->m_fLockedPoints & ROPE_LOCK_END_DIRECTION && (nNodes > 3))
		{
			Vector forward;
			AngleVectors( angles, &forward );
			
			const int parity = -1;
			const int nFalloffNodes = min( 2, nNodes - 2 );
			LockNodeDirection( &pNodes[nNodes-1], parity, nFalloffNodes, g_flLockAmount, g_flLockFalloff, forward );
		}
	}
}


// ------------------------------------------------------------------------------------ //
// C_RopeKeyframe
// ------------------------------------------------------------------------------------ //

C_RopeKeyframe::C_RopeKeyframe()
{
	m_bEndPointAttachmentPositionsDirty = true;
	m_bEndPointAttachmentAnglesDirty = true;
	m_PhysicsDelegate.m_pKeyframe = this;
	m_pMaterial = NULL;
	m_bPhysicsInitted = false;
	m_RopeFlags = 0;
	m_TextureHeight = 1;
	m_hStartPoint = m_hEndPoint = NULL;
	m_iStartAttachment = m_iEndAttachment = 0;
	m_vColorMod.Init( 1, 1, 1 );
	m_nLinksTouchingSomething = 0;
	m_Subdiv = 255; // default to using the cvar
	m_flCurrentGustLifetime = 0.0f;
	m_flCurrentGustTimer = 0.0f;
	
	m_fLockedPoints = 0;
	m_fPrevLockedPoints = 0;
	
	m_iForcePointMoveCounter = 0;
	m_flCurScroll = m_flScrollSpeed = 0;
	m_TextureScale = 4;	// 4:1
	m_vecImpulse.Init();

	g_Ropes.AddToTail( this );
}


C_RopeKeyframe::~C_RopeKeyframe()
{
	s_RopeManager.RemoveRopeFromQueuedRenderCaches( this );	
	g_Ropes.FindAndRemove( this );
}


C_RopeKeyframe* C_RopeKeyframe::Create(
	C_BaseEntity *pStartEnt,
	C_BaseEntity *pEndEnt,
	int iStartAttachment,
	int iEndAttachment,
	float ropeWidth,
	const char *pMaterialName,
	int numSegments,
	int ropeFlags
	)
{
	C_RopeKeyframe *pRope = new C_RopeKeyframe;

	pRope->InitializeAsClientEntity( NULL, RENDER_GROUP_OPAQUE_ENTITY );
	
	if ( pStartEnt )
	{
		pRope->m_hStartPoint = pStartEnt;
		pRope->m_fLockedPoints |= ROPE_LOCK_START_POINT;
	}

	if ( pEndEnt )
	{
		pRope->m_hEndPoint = pEndEnt;
		pRope->m_fLockedPoints |= ROPE_LOCK_END_POINT;
	}

	pRope->m_iStartAttachment = iStartAttachment;
	pRope->m_iEndAttachment = iEndAttachment;
	pRope->m_Width = ropeWidth;
	pRope->m_nSegments = clamp( numSegments, 2, ROPE_MAX_SEGMENTS );
	pRope->m_RopeFlags = ropeFlags;

	pRope->FinishInit( pMaterialName );
	return pRope;
}


C_RopeKeyframe* C_RopeKeyframe::CreateFromKeyValues( C_BaseAnimating *pEnt, KeyValues *pValues )
{
	C_RopeKeyframe *pRope = C_RopeKeyframe::Create( 
		pEnt,
		pEnt,
		pEnt->LookupAttachment( pValues->GetString( "StartAttachment" ) ),
		pEnt->LookupAttachment( pValues->GetString( "EndAttachment" ) ),
		pValues->GetFloat( "Width", 0.5 ),
		pValues->GetString( "Material" ),
		pValues->GetInt( "NumSegments" ),
		0 );

	if ( pRope )
	{
		if ( pValues->GetInt( "Gravity", 1 ) == 0 )
		{
			pRope->m_RopeFlags |= ROPE_NO_GRAVITY;
		}

		// Model ropes need wind to move
		pRope->m_RopeFlags |= ROPE_USE_WIND;

		pRope->m_RopeLength = pValues->GetInt( "Length" );
		pRope->m_TextureScale = pValues->GetFloat( "TextureScale", pRope->m_TextureScale );
		pRope->m_Slack = 0;
		pRope->m_RopeFlags |= ROPE_SIMULATE;
	}

	return pRope;
}


int C_RopeKeyframe::GetRopesIntersectingAABB( C_RopeKeyframe **pRopes, int nMaxRopes, const Vector &vAbsMin, const Vector &vAbsMax )
{
	if ( nMaxRopes == 0 )
		return 0;
	
	int nRopes = 0;
	FOR_EACH_LL( g_Ropes, i )
	{
		C_RopeKeyframe *pRope = g_Ropes[i];
	
		Vector v1, v2;
		if ( pRope->GetEndPointPos( 0, v1 ) && pRope->GetEndPointPos( 1, v2 ) )
		{
			if ( IsBoxIntersectingRay( v1, v2-v1, vAbsMin, vAbsMax, 0.1f ) )
			{
				pRopes[nRopes++] = pRope;
				if ( nRopes == nMaxRopes )
					break;
			}
		}
	}

	return nRopes;
}


void C_RopeKeyframe::SetSlack( int slack )
{
	m_Slack = slack;
	RecomputeSprings();
}


void C_RopeKeyframe::SetRopeFlags( int flags )
{
	m_RopeFlags = flags;
	UpdateVisibility();
}

	
int C_RopeKeyframe::GetRopeFlags() const
{
	return m_RopeFlags;
}


void C_RopeKeyframe::SetupHangDistance( float flHangDist )
{
	C_BaseEntity *pEnt1 = m_hStartPoint;
	C_BaseEntity *pEnt2 = m_hEndPoint;
	if ( !pEnt1 || !pEnt2 )
		return;

	QAngle dummyAngles;

	// Calculate starting conditions so we can force it to hang down N inches.
	Vector v1 = pEnt1->GetAbsOrigin();
	pEnt1->GetAttachment( m_iStartAttachment, v1, dummyAngles );
		
	Vector v2 = pEnt2->GetAbsOrigin();
	pEnt2->GetAttachment( m_iEndAttachment, v2, dummyAngles );

	float flSlack, flLen;
	CalcRopeStartingConditions( v1, v2, ROPE_MAX_SEGMENTS, flHangDist, &flLen, &flSlack );

	m_RopeLength = static_cast<int>( flLen );
	m_Slack = static_cast<int>( flSlack );

	RecomputeSprings();
}


void C_RopeKeyframe::SetStartEntity( C_BaseEntity *pEnt )
{
	m_hStartPoint = pEnt;
}


void C_RopeKeyframe::SetEndEntity( C_BaseEntity *pEnt )
{
	m_hEndPoint = pEnt;
}


C_BaseEntity* C_RopeKeyframe::GetStartEntity() const
{
	return m_hStartPoint;
}


C_BaseEntity* C_RopeKeyframe::GetEndEntity() const
{
	return m_hEndPoint;
}


CSimplePhysics::IHelper* C_RopeKeyframe::HookPhysics( CSimplePhysics::IHelper *pHook )
{
	m_RopePhysics.SetDelegate( pHook );
	return &m_PhysicsDelegate;
}


void C_RopeKeyframe::SetColorMod( const Vector &vColorMod )
{
	m_vColorMod = vColorMod;
}


void C_RopeKeyframe::RecomputeSprings()
{
	m_RopePhysics.ResetSpringLength( ( m_RopeLength + m_Slack + ROPESLACK_FUDGEFACTOR ) / static_cast<float>( m_RopePhysics.NumNodes() - 1 ) );
}


void C_RopeKeyframe::ShakeRope( const Vector &vCenter, float flRadius, float flMagnitude )
{
	// Sum up whatever it would apply to all of our points.
	bool bWantsThink = false;
	for ( int i=0; i < m_nSegments; i++ )
	{
		CSimplePhysics::CNode *pNode = m_RopePhysics.GetNode( i );

		Vector dir = pNode->m_vPos - vCenter;

		const float flDist = dir.Length();
	
		const float flShakeAmount = 1.0f - flDist / flRadius;
		if ( flShakeAmount >= 0 )
		{
			dir.NormalizeInPlace();
			pNode->m_vAbsImpulse = ( flShakeAmount * flMagnitude ) * dir;
			m_vecImpulse.z += flShakeAmount * flMagnitude;
			bWantsThink = true;
		}
	}

	if ( bWantsThink )
	{
		SetNextClientThink( CLIENT_THINK_ALWAYS );
	}
}


void C_RopeKeyframe::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );

	m_bNewDataThisFrame = true;
	SetNextClientThink( CLIENT_THINK_ALWAYS );

	if( updateType != DATA_UPDATE_CREATED )
		return;

	// Figure out the material name.
	char str[512];
	const model_t *pModel = modelinfo->GetModel( m_iRopeMaterialModelIndex );
	if ( pModel )
	{
		Q_strncpy( str, modelinfo->GetModelName( pModel ), sizeof( str ) );

		// Get rid of the extension because the material system doesn't want it.
		char *pExt = Q_stristr( str, ".vmt" );
		if ( pExt )
			pExt[0] = 0;
	}
	else
	{
		Q_strncpy( str, "missing_rope_material", sizeof( str ) );
	}
	
	FinishInit( str );
}


void C_RopeKeyframe::FinishInit( const char *pMaterialName )
{
	// Get the material from the material system.	
	m_pMaterial = materials->FindMaterial( pMaterialName, TEXTURE_GROUP_OTHER );

	if ( !g_pSplineCableShadowdepth.IsValid() )
	{
		g_pSplineCableShadowdepth.Init( "cable/rope_shadowdepth", TEXTURE_GROUP_OTHER );
	}

	if( m_pMaterial )
		m_TextureHeight = m_pMaterial->GetMappingHeight();
	else
		m_TextureHeight = 1;

	// Init rope physics.
	m_nSegments = clamp( m_nSegments, 2, ROPE_MAX_SEGMENTS );
	m_RopePhysics.SetNumNodes( m_nSegments );

	SetCollisionBounds( Vector( -10, -10, -10 ), Vector( 10, 10, 10 ) );

	// We want to think every frame.
	SetNextClientThink( CLIENT_THINK_ALWAYS );
}

void C_RopeKeyframe::RunRopeSimulation( float flSeconds )
{
	// First, forget about links touching things.
	for ( int i=0; i < m_nSegments; i++ )
		m_LinksTouchingSomething[i] = false;

	// Simulate, and it will mark which links touched things.
	m_RopePhysics.Simulate( flSeconds );

	// Now count how many links touched something.
	m_nLinksTouchingSomething = 0;
	for ( int i=0; i < m_nSegments; i++ )
	{
		if ( m_LinksTouchingSomething[i] )
			++m_nLinksTouchingSomething;
	}
}

Vector C_RopeKeyframe::ConstrainNode( const Vector &vNormal, const Vector &vNodePosition, const Vector &vMidpiont, float fNormalLength )
{
	// Get triangle edges formed
	Vector vMidpointToNode = vNodePosition - vMidpiont;
	Vector vMidpointToNodeProjected = vMidpointToNode.Dot( vNormal ) * vNormal;
	const float fMidpointToNodeLengh = VectorNormalize( vMidpointToNode );
	const float fMidpointToNodeProjectedLengh = VectorNormalize( vMidpointToNodeProjected );

	// See if it's past an endpoint
	if ( fMidpointToNodeProjectedLengh < fNormalLength + 1.0f )
		return vNodePosition;

	// Apply the ratio between the triangles
	return vMidpiont + vMidpointToNode * fMidpointToNodeLengh * ( fNormalLength / fMidpointToNodeProjectedLengh );
}

void C_RopeKeyframe::ConstrainNodesBetweenEndpoints()
{
	if ( !m_bConstrainBetweenEndpoints )
		return;

	// Get midpoint and normals
	const Vector& vMidpiont = ( m_vCachedEndPointAttachmentPos[ 0 ] + m_vCachedEndPointAttachmentPos[ 1 ] ) / 2.0f;
	Vector vNormal = vMidpiont - m_vCachedEndPointAttachmentPos[ 0 ];
	const float fNormalLength = VectorNormalize( vNormal );

	// Loop through all the middle segments and ensure their positions are constrained between the endpoints
	for ( int i = 1; i < m_RopePhysics.NumNodes() - 1; ++i )
	{
		// Fix the current position
		m_RopePhysics.GetNode( i )->m_vPos = ConstrainNode( vNormal, m_RopePhysics.GetNode( i )->m_vPos, vMidpiont, fNormalLength );

		// Fix the predicted position
		m_RopePhysics.GetNode( i )->m_vPredicted = ConstrainNode( vNormal, m_RopePhysics.GetNode( i )->m_vPredicted, vMidpiont, fNormalLength );
	}
}

void C_RopeKeyframe::ClientThink()
{
	// Only recalculate the endpoint attachments once per frame.
	m_bEndPointAttachmentPositionsDirty = true;
	m_bEndPointAttachmentAnglesDirty = true;
	
	if( !r_drawropes.GetBool() )
		return;

	if( !InitRopePhysics() ) // init if not already
		return;

	if ( DetectRestingState( m_bApplyWind ) )
	{
		if ( ( m_RopeFlags & ROPE_USE_WIND ) == 0 )
		{
			SetNextClientThink( CLIENT_THINK_NEVER );
		}
		return;
	}
		// Update the simulation.
		
		RunRopeSimulation( gpGlobals->frametime );

		g_nRopePointsSimulated += m_RopePhysics.NumNodes();

		m_bNewDataThisFrame = false;

		// Setup a new wind gust?
	if ( m_bApplyWind )
	{
		m_flCurrentGustTimer += gpGlobals->frametime;
		m_flTimeToNextGust -= gpGlobals->frametime;
		if( m_flTimeToNextGust <= 0 )
		{
			m_vWindDir = RandomVector( -1, 1 );
			VectorNormalize( m_vWindDir );

			static float basicScale = 50;
			m_vWindDir *= basicScale;
			m_vWindDir *= RandomFloat( -1.0f, 1.0f );
			
			m_flCurrentGustTimer = 0;
			m_flCurrentGustLifetime = RandomFloat( 2.0f, 3.0f );

			m_flTimeToNextGust = RandomFloat( 3.0f, 4.0f );
		}
	}

	UpdateBBox();
}


int C_RopeKeyframe::DrawModel( int flags )
{
	VPROF_BUDGET( "C_RopeKeyframe::DrawModel", VPROF_BUDGETGROUP_ROPES );
	if( !InitRopePhysics() )
		return 0;

	if ( !m_bReadyToDraw )
		return 0;

	// Resize the rope
	if( m_RopeFlags & ROPE_RESIZE )
	{
		RecomputeSprings();
	}

	// If our start & end entities have models, but are nodraw, then we don't draw
	if ( m_hStartPoint && m_hStartPoint->IsDormant() && m_hEndPoint && m_hEndPoint->IsDormant() )
	{
		// Check models because rope endpoints are point entities
		if ( m_hStartPoint->GetModelIndex() && m_hEndPoint->GetModelIndex() )
			return 0;
	}

	ConstrainNodesBetweenEndpoints();

	RopeManager()->AddToRenderCache( this );
	return 1;
}

bool C_RopeKeyframe::ShouldDraw()
{
	if( !r_ropetranslucent.GetBool() ) 
		return false;

	if( !(m_RopeFlags & ROPE_SIMULATE) )
		return false;

	return true;
}

const Vector& C_RopeKeyframe::WorldSpaceCenter( ) const
{
	return GetAbsOrigin();
}

bool C_RopeKeyframe::GetAttachment( int number, matrix3x4_t &matrix )
{
	const int nNodes = m_RopePhysics.NumNodes();
	if ( (number != ROPE_ATTACHMENT_START_POINT && number != ROPE_ATTACHMENT_END_POINT) || nNodes < 2 )
		return false;

	// Now setup the orientation based on the last segment.
	Vector vForward, origin;
	if ( number == ROPE_ATTACHMENT_START_POINT )
	{
		origin = m_RopePhysics.GetNode( 0 )->m_vPredicted;
		vForward = m_RopePhysics.GetNode( 0 )->m_vPredicted - m_RopePhysics.GetNode( 1 )->m_vPredicted;
	}
	else
	{
		origin = m_RopePhysics.GetNode( nNodes-1 )->m_vPredicted;
		vForward = m_RopePhysics.GetNode( nNodes-1 )->m_vPredicted - m_RopePhysics.GetNode( nNodes-2 )->m_vPredicted;
	}
	VectorMatrix( vForward, matrix );
	PositionMatrix( origin, matrix );
	return true;
}

bool C_RopeKeyframe::GetAttachment( int number, Vector &origin )
{
	const int nNodes = m_RopePhysics.NumNodes();
	if ( (number != ROPE_ATTACHMENT_START_POINT && number != ROPE_ATTACHMENT_END_POINT) || nNodes < 2 )
		return false;

	// Now setup the orientation based on the last segment.
	if ( number == ROPE_ATTACHMENT_START_POINT )
	{
		origin = m_RopePhysics.GetNode( 0 )->m_vPredicted;
	}
	else
	{
		origin = m_RopePhysics.GetNode( nNodes-1 )->m_vPredicted;
	}
	return true;
}

bool C_RopeKeyframe::GetAttachmentVelocity( int number, Vector &originVel, Quaternion &angleVel )
{
	Assert(0);
	return false;
}

bool C_RopeKeyframe::GetAttachment( int number, Vector &origin, QAngle &angles )
{
	const int nNodes = m_RopePhysics.NumNodes();
	if ( (number == ROPE_ATTACHMENT_START_POINT || number == ROPE_ATTACHMENT_END_POINT) && nNodes >= 2 )
	{
		// Now setup the orientation based on the last segment.
		Vector vForward;
		if ( number == ROPE_ATTACHMENT_START_POINT )
		{
			origin = m_RopePhysics.GetNode( 0 )->m_vPredicted;
			vForward = m_RopePhysics.GetNode( 0 )->m_vPredicted - m_RopePhysics.GetNode( 1 )->m_vPredicted;
		}
		else
		{
			origin = m_RopePhysics.GetNode( nNodes-1 )->m_vPredicted;
			vForward = m_RopePhysics.GetNode( nNodes-1 )->m_vPredicted - m_RopePhysics.GetNode( nNodes-2 )->m_vPredicted;
		}
		VectorAngles( vForward, angles );

		return true;
	}
	
	return false;
}

bool C_RopeKeyframe::AnyPointsMoved()
{
	const int nNodeCount = m_RopePhysics.NumNodes();
	for( int i=0; i < nNodeCount; i++ )
	{
		CSimplePhysics::CNode *pNode = m_RopePhysics.GetNode( i );
		const float flMoveDistSqr = pNode->m_vPos.DistToSqr( pNode->m_vPrevPos );
		if( flMoveDistSqr > 0.25f )
		{
			if ( m_iForcePointMoveCounter < 5 )
			{
				m_iForcePointMoveCounter = 5;
			}
			return true;
		}
	}

	if( m_iForcePointMoveCounter >= 0 )
	{
		--m_iForcePointMoveCounter;
		return true;
	}

	return false;
}


inline bool C_RopeKeyframe::DidEndPointMove( int iPt )
{
	// If this point isn't locked anyway, just break out.
	if( !( m_fLockedPoints & (1 << iPt) ) )
		return false;

	const bool bOld = m_bPrevEndPointPos[iPt];
	const Vector vOld = m_vPrevEndPointPos[iPt];

	m_bPrevEndPointPos[iPt] = GetEndPointPos( iPt, m_vPrevEndPointPos[iPt] );
	
	// If it wasn't and isn't attached to anything, don't register a change.
	if( !bOld && !m_bPrevEndPointPos[iPt] )
		return true;

	// Register a change if the endpoint moves.
	if( !VectorsAreEqual( vOld, m_vPrevEndPointPos[iPt], 0.1 ) )
		return true;

	return false;
}


bool C_RopeKeyframe::DetectRestingState( bool &bApplyWind )
{
	bApplyWind = false;

	if( m_fPrevLockedPoints != m_fLockedPoints )
	{
		// Force it to move the points for some number of frames when they get detached or
		// after we get new data. This allows them to accelerate from gravity.
		m_iForcePointMoveCounter = 10; 
		m_fPrevLockedPoints = m_fLockedPoints;
		return false;
	}

	if( m_bNewDataThisFrame )
	{
		// Simulate if anything about us changed this frame, such as our position due to hierarchy.
		// FIXME: this won't work when hierarchy is client side
		return false;
	}

	// Make sure our attachment points haven't moved.
	if( DidEndPointMove( 0 ) || DidEndPointMove( 1 ) )
		return false;

	// See how close we are to the line.
	Vector &vEnd1 = m_RopePhysics.GetFirstNode()->m_vPos;
	Vector &vEnd2 = m_RopePhysics.GetLastNode()->m_vPos;
	
	if ( m_RopeFlags & ROPE_USE_WIND )
	{
		// Don't apply wind if more than half of the nodes are touching something.
		if( m_nLinksTouchingSomething < (m_RopePhysics.NumNodes() >> 1) )
		{
			bApplyWind = CalcDistanceToLineSegment( MainViewOrigin(), vEnd1, vEnd2 ) < rope_wind_dist.GetFloat();
		}
	}

	if ( m_vecPreviousImpulse != m_vecImpulse )
	{
		m_vecPreviousImpulse = m_vecImpulse;
		return false;
	}

	return !AnyPointsMoved() && !bApplyWind && !rope_shake.GetInt();
}

// simple struct to precompute basis for catmull rom splines for faster evaluation
struct catmull_t
{
	Vector t3;
	Vector t2;
	Vector t;
	Vector c;
};

// bake out the terms of the catmull rom spline
void Catmull_Rom_Spline_Matrix( const Vector &p1, const Vector &p2, const Vector &p3, const Vector &p4, catmull_t &output )
{
	output.t3 = 0.5f * ((-1*p1) + (3*p2) + (-3*p3) + p4);	// 0.5 t^3 * [ (-1*p1) + ( 3*p2) + (-3*p3) + p4 ]
	output.t2 = 0.5f * ((2*p1) + (-5*p2) + (4*p3) - p4);		// 0.5 t^2 * [ ( 2*p1) + (-5*p2) + ( 4*p3) - p4 ]
	output.t = 0.5f * ((-1*p1) + p3);						// 0.5 t * [ (-1*p1) + p3 ]
	output.c = p2;											// p2
}

// evaluate one point on the spline, t is a vector of (t, t^2, t^3)
inline void Catmull_Rom_Eval( const catmull_t &spline, const Vector &t, Vector &output )
{
	Assert(spline.c.IsValid());
	Assert(spline.t.IsValid());
	Assert(spline.t2.IsValid());
	Assert(spline.t3.IsValid());
	output = spline.c + (t.x * spline.t) + (t.y*spline.t2) + (t.z * spline.t3);
}


void C_RopeKeyframe::UpdateBBox()
{
	Vector &vStart = m_RopePhysics.GetFirstNode()->m_vPos;
	Vector &vEnd   = m_RopePhysics.GetLastNode()->m_vPos;

	Vector mins, maxs;

	VectorMin( vStart, vEnd, mins );
	VectorMax( vStart, vEnd, maxs );

	for( int i=1; i < m_RopePhysics.NumNodes()-1; i++ )
	{
		const Vector &vPos = m_RopePhysics.GetNode(i)->m_vPos;
		AddPointToBounds( vPos, mins, maxs );
	}
	
	mins -= GetAbsOrigin();
	maxs -= GetAbsOrigin();
	SetCollisionBounds( mins, maxs );
}


bool C_RopeKeyframe::InitRopePhysics()
{
	if( !(m_RopeFlags & ROPE_SIMULATE) )
		return false;

	if( m_bPhysicsInitted )
	{
		return true;
	}

	// Must have both entities to work.
	m_bPrevEndPointPos[0] = GetEndPointPos( 0, m_vPrevEndPointPos[0] );
	if( !m_bPrevEndPointPos[0] )
		return false;

	// They're allowed to not have an end attachment point so the rope can dangle.
	m_bPrevEndPointPos[1] = GetEndPointPos( 1, m_vPrevEndPointPos[1] );
	if( !m_bPrevEndPointPos[1] )
		m_vPrevEndPointPos[1] = m_vPrevEndPointPos[0];

	const Vector &vStart = m_vPrevEndPointPos[0];
	const Vector &vAttached = m_vPrevEndPointPos[1];

	m_RopePhysics.SetupSimulation( 0, &m_PhysicsDelegate );
	RecomputeSprings();
	m_RopePhysics.Restart();

	// Initialize the positions of the nodes.
	for( int i=0; i < m_RopePhysics.NumNodes(); i++ )
	{
		CSimplePhysics::CNode *pNode = m_RopePhysics.GetNode( i );
		const float t = static_cast<float>( i ) / (m_RopePhysics.NumNodes() - 1);
		
		VectorLerp( vStart, vAttached, t, pNode->m_vPos );
		pNode->m_vPrevPos = pNode->m_vPos;
	}

	// Simulate for a bit to let it sag.
	if ( m_RopeFlags & ROPE_INITIAL_HANG )
	{
		RunRopeSimulation( 5 );
	}

	CalcLightValues();

	// Set our bounds for visibility.
	UpdateBBox();

	m_flTimeToNextGust = RandomFloat( 1.0f, 3.0f );
	m_bPhysicsInitted = true;
	
	return true;
}


bool C_RopeKeyframe::CalculateEndPointAttachment( C_BaseEntity *pEnt, int iAttachment, Vector &vPos, QAngle *pAngles )
{
	VPROF_BUDGET( "C_RopeKeyframe::CalculateEndPointAttachment", VPROF_BUDGETGROUP_ROPES );

	if( !pEnt )
		return false;

	if ( m_RopeFlags & ROPE_PLAYER_WPN_ATTACH )
	{
		C_BasePlayer *pPlayer = ToBasePlayer( pEnt );
		if ( pPlayer )
		{
			C_BaseAnimating *pModel = pPlayer->GetRenderedWeaponModel();
			if ( !pModel )
				return false;

			const int iAttachment = pModel->LookupAttachment( "buff_attach" );
			if ( pAngles )
				return pModel->GetAttachment( iAttachment, vPos, *pAngles );
			return pModel->GetAttachment( iAttachment, vPos );
		}
	}

	if( iAttachment > 0 )
	{
		bool bOk;
		if ( pAngles )
		{
			bOk = pEnt->GetAttachment( iAttachment, vPos, *pAngles );
		}
		else
		{
			bOk = pEnt->GetAttachment( iAttachment, vPos );
		}
		if ( bOk )
			return true;
	}

	vPos = pEnt->WorldSpaceCenter( );
	if ( pAngles )
	{
		*pAngles = pEnt->GetAbsAngles();
	}
	return true;
}

bool C_RopeKeyframe::GetEndPointPos( int iPt, Vector &vPos )
{
	// By caching the results here, we avoid doing this a bunch of times per frame.
	if ( m_bEndPointAttachmentPositionsDirty )
	{
		CalculateEndPointAttachment( m_hStartPoint, m_iStartAttachment, m_vCachedEndPointAttachmentPos[0], NULL );
		CalculateEndPointAttachment( m_hEndPoint, m_iEndAttachment, m_vCachedEndPointAttachmentPos[1], NULL );
		m_bEndPointAttachmentPositionsDirty = false;
	}

	Assert( iPt == 0 || iPt == 1 );
	vPos = m_vCachedEndPointAttachmentPos[iPt];
	return true;
}

bool C_RopeKeyframe::GetEndPointAttachment( int iPt, Vector &vPos, QAngle &angle )
{
	// By caching the results here, we avoid doing this a bunch of times per frame.
	if ( m_bEndPointAttachmentPositionsDirty || m_bEndPointAttachmentAnglesDirty )
	{
		CalculateEndPointAttachment( m_hStartPoint, m_iStartAttachment, m_vCachedEndPointAttachmentPos[0], &m_vCachedEndPointAttachmentAngle[0] );
		CalculateEndPointAttachment( m_hEndPoint, m_iEndAttachment, m_vCachedEndPointAttachmentPos[1], &m_vCachedEndPointAttachmentAngle[1] );
		m_bEndPointAttachmentPositionsDirty = false;
		m_bEndPointAttachmentAnglesDirty = false;
	}

	Assert( iPt == 0 || iPt == 1 );
	vPos = m_vCachedEndPointAttachmentPos[iPt];
	angle = m_vCachedEndPointAttachmentAngle[iPt];
	return true;
}


void C_RopeKeyframe::CalcLightValues()
{
	Vector boxColors[6];

	for( int i=0; i < m_RopePhysics.NumNodes(); i++ )
	{
		const Vector &vPos = m_RopePhysics.GetNode(i)->m_vPredicted;
		engine->ComputeLighting( vPos, NULL, true, m_LightValues[i], boxColors );

		if ( !rope_averagelight.GetInt() )
		{
			// The engine averages the lighting across the 6 box faces, but we would rather just get the MAX intensity
			// since we do our own half-lambert lighting in the rope shader to simulate directionality.
			//
			// So here, we take the average of all the incoming light, and scale it to use the max intensity of all the box sides.
			float flMaxIntensity = 0;
			for ( int iSide=0; iSide < 6; iSide++ )
			{
				float flLen = boxColors[iSide].Length();
				flMaxIntensity = max( flMaxIntensity, flLen );
			}

			VectorNormalize( m_LightValues[i] );
			m_LightValues[i] *= flMaxIntensity;
			const float flMax = max( m_LightValues[i].x, max( m_LightValues[i].y, m_LightValues[i].z ) );
			if ( flMax > 1 )
				m_LightValues[i] /= flMax;
		}
	}
}

//------------------------------------------------------------------------------
// Purpose :
// Input   :
// Output  :
//------------------------------------------------------------------------------
void C_RopeKeyframe::ReceiveMessage( int classID, bf_read &msg )
{
	if ( classID != GetClientClass()->m_ClassID )
	{
		// message is for subclass
		BaseClass::ReceiveMessage( classID, msg );
		return;
	}

	// Read instantaneous fore data
	m_vecImpulse.x   = msg.ReadFloat();
	m_vecImpulse.y   = msg.ReadFloat();
	m_vecImpulse.z   = msg.ReadFloat();
}