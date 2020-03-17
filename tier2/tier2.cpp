//===== Copyright � 2005-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: A higher level link library for general use in the game and tools.
//
//===========================================================================//

#include <tier2/tier2.h>
#include "tier0/dbg.h"
#include "filesystem.h"
#include "materialsystem/imaterialsystem.h"
#include "materialsystem/imaterialsystemhardwareconfig.h"
#include "materialsystem/IColorCorrection.h"
#include "materialsystem/idebugtextureinfo.h"
#include "materialsystem/ivballoctracker.h"
#include "inputsystem/iinputsystem.h"
#include "networksystem/inetworksystem.h"
#include "mdllib/mdllib.h"
#include "filesystem/IQueuedLoader.h"


//-----------------------------------------------------------------------------
// These tier2 libraries must be set by any users of this library.
// They can be set by calling ConnectTier2Libraries or InitDefaultFileSystem.
// It is hoped that setting this, and using this library will be the common mechanism for
// allowing link libraries to access tier2 library interfaces
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Call this to connect to all tier 2 libraries.
// It's up to the caller to check the globals it cares about to see if ones are missing
//-----------------------------------------------------------------------------
void ConnectTier2Libraries( CreateInterfaceFn *pFactoryList, int nFactoryCount )
{
	
}

void DisconnectTier2Libraries()
{
	
}


