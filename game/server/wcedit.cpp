//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:		Namespace for functions having to do with WC Edit mode
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $Log: $
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "mathlib/mathlib.h"
#include "player.h"
#include "wcedit.h"
#include "ndebugoverlay.h"
#include "editor_sendcommand.h"
#include "movevars_shared.h"
#include "model_types.h"
// UNDONE: Reduce some dependency here!
#include "physics_prop_ragdoll.h"
#include "items.h"
#include "utlsymbol.h"
#include "physobj.h"
#include "fmtstr.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern float			GetFloorZ(const Vector &origin);

//-----------------------------------------------------------------------------
// Purpose: Make sure the version of the map in WC is the same as the map 
//			that's being edited 
// Input  :
// Output :
//-----------------------------------------------------------------------------
bool NWCEdit::IsWCVersionValid(void)
{
	int status = Editor_CheckVersion(STRING(gpGlobals->mapname), gpGlobals->mapversion, false);
	if (!status)
	{
		return true;
	}
	else if (status == Editor_NotRunning)
	{
		Msg("\nAborting map_edit\nWorldcraft not running...\n\n");
		UTIL_CenterPrintAll( "Worldcraft not running..." );
		engine->ServerCommand("disconnect\n");
	}
	else
	{
		Msg("\nAborting map_edit\nWC/Engine map versions different...\n\n");
		UTIL_CenterPrintAll( "WC/Engine map versions different..." );
		engine->ServerCommand("disconnect\n");
	}
	return false;
}

//------------------------------------------------------------------------------
// Purpose : Figure out placement position of air nodes form where player is
//			 looking.  Keep distance from player constant but adjust height
//			 based on viewing angle
// Input   :
// Output  :
//------------------------------------------------------------------------------
Vector NWCEdit::AirNodePlacementPosition( void )
{
	CBasePlayer* pPlayer = UTIL_PlayerByIndex(CBaseEntity::m_nDebugPlayer);

	if (!pPlayer) 
	{
		return vec3_origin;
	}

	Vector pForward;
	pPlayer->EyeVectors( &pForward );
	
	Vector	floorVec = pForward;
	floorVec.z = 0;
	VectorNormalize( floorVec );
	VectorNormalize( pForward );

	float cosAngle = DotProduct(floorVec,pForward);

	float lookDist = 100.0f/cosAngle;
	Vector lookPos = pPlayer->EyePosition()+pForward * lookDist;

	return lookPos;
}

Vector *g_EntityPositions = NULL;
QAngle *g_EntityOrientations = NULL;
string_t *g_EntityClassnames = NULL;

class GlobalCleanUp : public CAutoGameSystem
{
	void Shutdown()
	{
		delete [] g_EntityPositions;
		delete [] g_EntityOrientations;
		delete [] g_EntityClassnames;
		delete this;
	}
};

//-----------------------------------------------------------------------------
// Purpose: Saves the entity's position for future communication with Hammer
//-----------------------------------------------------------------------------
void NWCEdit::RememberEntityPosition( CBaseEntity *pEntity )
{
	if ( !(pEntity->ObjectCaps() & FCAP_WCEDIT_POSITION) )
		return;

	if ( !g_EntityPositions )
	{
		new GlobalCleanUp();
		g_EntityPositions = new Vector[NUM_ENT_ENTRIES];
		g_EntityOrientations = new QAngle[NUM_ENT_ENTRIES];
		// have to save these too because some entities change the classname on spawn (e.g. prop_physics_override, physics_prop)
		g_EntityClassnames = new string_t[NUM_ENT_ENTRIES];
	}
	int index = pEntity->entindex();
	g_EntityPositions[index] = pEntity->GetAbsOrigin();
	g_EntityOrientations[index] = pEntity->GetAbsAngles();
	g_EntityClassnames[index] = pEntity->m_iClassname;
}

//-----------------------------------------------------------------------------
// Purpose: Sends Hammer an update to the current position
//-----------------------------------------------------------------------------
void NWCEdit::UpdateEntityPosition( CBaseEntity *pEntity )
{
	const Vector &newPos = pEntity->GetAbsOrigin();
	const QAngle &newAng = pEntity->GetAbsAngles();

	DevMsg( 1, "%s\n   origin %f %f %f\n   angles %f %f %f\n", pEntity->GetClassname(), newPos.x, newPos.y, newPos.z, newAng.x, newAng.y, newAng.z );
	if ( Ragdoll_IsPropRagdoll(pEntity) )
	{
		char tmp[2048];
		Ragdoll_GetAngleOverrideString( tmp, sizeof(tmp), pEntity );
		DevMsg( 1, "pose: %s\n", tmp );
	}

	if ( !(pEntity->ObjectCaps() & FCAP_WCEDIT_POSITION) )
		return;
	
	// can't do this unless in edit mode
	if ( !engine->IsInEditMode() )
		return;

	int entIndex = pEntity->entindex();
	Vector pos = g_EntityPositions[entIndex];
	EditorSendResult_t result = Editor_BadCommand;
	const char *pClassname = STRING(g_EntityClassnames[entIndex]);

	if ( pEntity->GetModel() && modelinfo->GetModelType(pEntity->GetModel()) == mod_brush )
	{
		QAngle xformAngles;
		RotationDelta( g_EntityOrientations[entIndex], newAng, &xformAngles );
		if ( xformAngles.Length() > 1e-4 )
		{
			result = Editor_RotateEntity( pClassname, pos.x, pos.y, pos.z, xformAngles );
		}
		else
		{
			// don't call through for an identity rotation, may just increase error
			result = Editor_OK;
		}
	}
	else
	{
		if ( Ragdoll_IsPropRagdoll(pEntity) )
		{
			char tmp[2048];
			Ragdoll_GetAngleOverrideString( tmp, sizeof(tmp), pEntity );
			result = Editor_SetKeyValue( pClassname, pos.x, pos.y, pos.z, "angleOverride", tmp );
			if ( result != Editor_OK )
				goto error;
		}
		result = Editor_SetKeyValue( pClassname, pos.x, pos.y, pos.z, "angles", CFmtStr("%f %f %f", newAng.x, newAng.y, newAng.z) );
	}
	if ( result != Editor_OK )
		goto error;

	result = Editor_SetKeyValue( pClassname, pos.x, pos.y, pos.z, "origin", CFmtStr("%f %f %f", newPos.x, newPos.y, newPos.z) );
	if ( result != Editor_OK )
		goto error;

	NDebugOverlay::EntityBounds(pEntity, 0, 255, 0, 0 ,5);
	// save the update
	RememberEntityPosition( pEntity );
	return;

error:
	NDebugOverlay::EntityBounds(pEntity, 255, 0, 0, 0 ,5);
}

/// This is an entity used by the hammer_update_safe_entities command. It allows designers
/// to specify objects that should be ignored. It stores an array of sixteen strings
/// which may correspond to names. Designers may ignore more than sixteen objects by 
/// placing more than one of these in a level.
class CWC_UpdateIgnoreList : public CBaseEntity
{
public:
	DECLARE_CLASS( CWC_UpdateIgnoreList, CBaseEntity );

	enum { MAX_IGNORELIST_NAMES = 16 }; ///< the number of names in the array below

	inline const string_t &GetName( int x ) const { return m_nIgnoredEntityNames[x]; } 

protected:
	// the list of names to ignore
	string_t m_nIgnoredEntityNames[MAX_IGNORELIST_NAMES]; 

public:
	DECLARE_DATADESC();
};


LINK_ENTITY_TO_CLASS( hammer_updateignorelist, CWC_UpdateIgnoreList );

BEGIN_DATADESC( CWC_UpdateIgnoreList )

	// Be still, classcheck!
	//DEFINE_FIELD( m_nIgnoredEntityNames, FIELD_STRING, MAX_IGNORELIST_NAMES ),

	DEFINE_KEYFIELD( m_nIgnoredEntityNames[0], FIELD_STRING, "IgnoredName01" ),
	DEFINE_KEYFIELD( m_nIgnoredEntityNames[1], FIELD_STRING, "IgnoredName02" ),
	DEFINE_KEYFIELD( m_nIgnoredEntityNames[2], FIELD_STRING, "IgnoredName03" ),
	DEFINE_KEYFIELD( m_nIgnoredEntityNames[3], FIELD_STRING, "IgnoredName04" ),
	DEFINE_KEYFIELD( m_nIgnoredEntityNames[4], FIELD_STRING, "IgnoredName05" ),
	DEFINE_KEYFIELD( m_nIgnoredEntityNames[5], FIELD_STRING, "IgnoredName06" ),
	DEFINE_KEYFIELD( m_nIgnoredEntityNames[6], FIELD_STRING, "IgnoredName07" ),
	DEFINE_KEYFIELD( m_nIgnoredEntityNames[7], FIELD_STRING, "IgnoredName08" ),
	DEFINE_KEYFIELD( m_nIgnoredEntityNames[8], FIELD_STRING, "IgnoredName09" ),
	DEFINE_KEYFIELD( m_nIgnoredEntityNames[9], FIELD_STRING, "IgnoredName10" ),
	DEFINE_KEYFIELD( m_nIgnoredEntityNames[10], FIELD_STRING, "IgnoredName11" ),
	DEFINE_KEYFIELD( m_nIgnoredEntityNames[11], FIELD_STRING, "IgnoredName12" ),
	DEFINE_KEYFIELD( m_nIgnoredEntityNames[12], FIELD_STRING, "IgnoredName13" ),
	DEFINE_KEYFIELD( m_nIgnoredEntityNames[13], FIELD_STRING, "IgnoredName14" ),
	DEFINE_KEYFIELD( m_nIgnoredEntityNames[14], FIELD_STRING, "IgnoredName15" ),
	DEFINE_KEYFIELD( m_nIgnoredEntityNames[15], FIELD_STRING, "IgnoredName16" ),

END_DATADESC()



CON_COMMAND( hammer_update_entity, "Updates the entity's position/angles when in edit mode" )
{
	if ( !UTIL_IsCommandIssuedByServerAdmin() )
		return;

	if ( args.ArgC() < 2 )
	{
		CBasePlayer *pPlayer = UTIL_GetCommandClient();
		trace_t tr;
		Vector forward;
		pPlayer->EyeVectors( &forward );
		UTIL_TraceLine(pPlayer->EyePosition(), pPlayer->EyePosition() + forward * MAX_COORD_RANGE,
			MASK_SHOT_HULL|CONTENTS_GRATE|CONTENTS_DEBRIS, pPlayer, COLLISION_GROUP_NONE, &tr );

		if ( tr.DidHit() && !tr.DidHitWorld() )
		{
			NWCEdit::UpdateEntityPosition( tr.m_pEnt );
		}
	}
	else
	{
		CBaseEntity *pEnt = NULL;
		while ((pEnt = gEntList.FindEntityGeneric( pEnt, args[1] ) ) != NULL)
		{
			NWCEdit::UpdateEntityPosition( pEnt );
		}
	}
}

CON_COMMAND( hammer_update_safe_entities, "Updates entities in the map that can safely be updated (don't have parents or are affected by constraints). Also excludes entities mentioned in any hammer_updateignorelist objects in this map." )
{
	int iCount = 0;
	CBaseEntity *pEnt = NULL;

	if ( !UTIL_IsCommandIssuedByServerAdmin() )
		return;

	Msg("\n====================================================\nPerforming Safe Entity Update\n" );

	// first look for any exclusion objects -- these are entities that list specific things to be ignored.
	// All the names that are inside them, we store into a hash table (here implemented through a local
	// CUtlSymbolTable)

	CUtlSymbolTable ignoredNames(16,32,true); // grow 16 strings at a time. Case insensitive.
	while ( (pEnt = gEntList.FindEntityByClassname( pEnt, "hammer_updateignorelist" )) != NULL )
	{
		// for each name in each of those strings, add it to the symbol table.
		CWC_UpdateIgnoreList *piglist = static_cast<CWC_UpdateIgnoreList *>(pEnt);
		for (int ii = 0 ; ii < CWC_UpdateIgnoreList::MAX_IGNORELIST_NAMES ; ++ii )
		{
			if (!!piglist->GetName(ii))  // if not null
			{	// add to symtab
				ignoredNames.AddString(piglist->GetName(ii).ToCStr());
			}
		}
	}

	if ( ignoredNames.GetNumStrings() > 0 )
	{
		Msg( "Ignoring %d specified targetnames.\n", ignoredNames.GetNumStrings() );
	}


	// now iterate through everything in the world
	for ( pEnt = gEntList.FirstEnt(); pEnt != NULL; pEnt = gEntList.NextEnt(pEnt) )
	{
		if ( !(pEnt->ObjectCaps() & FCAP_WCEDIT_POSITION) )
			continue;

		// If we have a parent, or any children, we're not safe to update
		if ( pEnt->GetMoveParent() || pEnt->FirstMoveChild() )
			continue;

		IPhysicsObject *pPhysics = pEnt->VPhysicsGetObject();
		if ( !pPhysics )
			continue;
		// If we are affected by any constraints, we're not safe to update
		if ( pPhysics->IsAttachedToConstraint( Ragdoll_IsPropRagdoll(pEnt) )  )
			continue;
		// Motion disabled?
		if ( !pPhysics->IsMoveable() )
			continue;

		// ignore brush models (per bug 61318)
		if ( dynamic_cast<CPhysBox *>(pEnt) )
			continue;

		// explicitly excluded by designer?
		if ( ignoredNames.Find(pEnt->GetEntityName().ToCStr()).IsValid() )
			continue;

		NWCEdit::UpdateEntityPosition( pEnt );
		iCount++;
	}

	Msg("Updated %d entities.\n", iCount);
}
