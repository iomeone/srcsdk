//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//===========================================================================//

#ifndef ICVAR_H
#define ICVAR_H
#ifdef _WIN32
#pragma once
#endif

#include "appframework/IAppSystem.h"
#include "tier1/iconvar.h"

class ConCommandBase;
class ConCommand;
class ConVar;
class Color;


//-----------------------------------------------------------------------------
// ConVars/ComCommands are marked as having a particular DLL identifier
//-----------------------------------------------------------------------------
typedef int CVarDLLIdentifier_t;


//-----------------------------------------------------------------------------
// Used to display console messages
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Purpose: Applications can implement this to modify behavior in ICvar
//-----------------------------------------------------------------------------
#define CVAR_QUERY_INTERFACE_VERSION "VCvarQuery001"
abstract_class ICvarQuery : public IAppSystem
{
public:
	// Can these two convars be aliased?
	virtual bool AreConVarsLinkable( const ConVar *child, const ConVar *parent ) = 0;
};

//-----------------------------------------------------------------------------
// Purpose: DLL interface to ConVars/ConCommands
//-----------------------------------------------------------------------------
#include "compat_icvar.h"

inline ICvar::Iterator::Iterator(ICvar *icvar)
{
	m_pIter = icvar->FactoryInternalIterator();
}

inline ICvar::Iterator::~Iterator( void )
{
	delete m_pIter;
}

inline void ICvar::Iterator::SetFirst( void )
{
	m_pIter->SetFirst();
}

inline void ICvar::Iterator::Next( void )
{
	m_pIter->Next();
}

inline bool ICvar::Iterator::IsValid( void )
{
	return m_pIter->IsValid();
}

inline ConCommandBase * ICvar::Iterator::Get( void )
{
	return m_pIter->Get();
}


//-----------------------------------------------------------------------------
// These global names are defined by tier1.h, duplicated here so you
// don't have to include tier1.h
//-----------------------------------------------------------------------------

// These are marked DLL_EXPORT for Linux.
DECLARE_TIER1_INTERFACE( ICvar, cvar );
DECLARE_TIER1_INTERFACE( ICvar, g_pCVar );


#endif // ICVAR_H
