//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef ENGINE_HLDS_API_H
#define ENGINE_HLDS_API_H
#ifdef _WIN32
#pragma once
#endif

#include "interface.h"
#include "appframework/IAppSystem.h"


#define VENGINE_HLDS_API_VERSION "VENGINE_HLDS_API_VERSION002"


struct ModInfo_t
{
	void *m_pInstance;
	const char *m_pBaseDirectory;	// Executable directory ("c:/program files/half-life 2", for example)
	const char *m_pInitialMod;		// Mod name ("cstrike", for example)
	const char *m_pInitialGame;		// Root game name ("hl2", for example, in the case of cstrike)
	CAppSystemGroup *m_pParentAppSystemGroup;
	bool m_bTextMode;
};


//-----------------------------------------------------------------------------
// Purpose: This is the interface exported by the engine.dll to allow a dedicated server front end
//  application to host it.
//-----------------------------------------------------------------------------

#include "compat_engine_hlds_api.h"

#endif // ENGINE_HLDS_API_H
