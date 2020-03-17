//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The application object.
//
//=============================================================================//

#ifndef IHAMMER_H
#define IHAMMER_H

#include "appframework/IAppSystem.h"

typedef struct tagMSG MSG;


class IStudioDataCache;


//-----------------------------------------------------------------------------
// Return values for RequestNewConfig
//-----------------------------------------------------------------------------
enum RequestRetval_t
{
	REQUEST_OK = 0,
	REQUEST_QUIT
};


//-----------------------------------------------------------------------------
// Interface used to drive hammer
//-----------------------------------------------------------------------------
#define INTERFACEVERSION_HAMMER	"Hammer001"

#include "compat_ihammer.h"

#endif // IHAMMER_H
