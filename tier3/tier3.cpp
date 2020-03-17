//===== Copyright � 2005-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: A higher level link library for general use in the game and tools.
//
//===========================================================================//

#include "tier3/tier3.h"
#include "tier0/dbg.h"
#include "istudiorender.h"
#include "vgui/IVGui.h"
#include "vgui/IInput.h"
#include "vgui/IPanel.h"
#include "vgui/ISurface.h"
#include "vgui/ILocalize.h"
#include "vgui/IScheme.h"
#include "vgui/ISystem.h"
#include "VGuiMatSurface/IMatSystemSurface.h"
#include "datacache/idatacache.h"
#include "datacache/imdlcache.h"
#include "avi/iavi.h"
#include "avi/ibik.h"
#include "movieobjects/idmemakefileutils.h"
#include "vphysics_interface.h"
#include "SoundEmitterSystem/isoundemittersystembase.h"


//-----------------------------------------------------------------------------
// These tier3 libraries must be set by any users of this library.
// They can be set by calling ConnectTier3Libraries.
// It is hoped that setting this, and using this library will be the common mechanism for
// allowing link libraries to access tier3 library interfaces
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Call this to connect to all tier 3 libraries.
// It's up to the caller to check the globals it cares about to see if ones are missing
//-----------------------------------------------------------------------------
void ConnectTier3Libraries( CreateInterfaceFn *pFactoryList, int nFactoryCount )
{
	
}

void DisconnectTier3Libraries()
{
	
}
