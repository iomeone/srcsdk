//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef ISERVER_H
#define ISERVER_H
#ifdef _WIN32
#pragma once
#endif

#include <inetmsghandler.h>
#include <bitvec.h>
#include <const.h>

class INetMessage;
class IRecipientFilter;
class IClient;

typedef struct player_info_s player_info_t;

#include "compat_iserver.h"


#endif // ISERVER_H
