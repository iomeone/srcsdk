//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef INETCHANNEL_H
#define INETCHANNEL_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "inetchannelinfo.h"
#include "tier1/bitbuf.h"

class	IDemoRecorder;
class	INetMessage;
class	INetChannelHandler;
class	INetChannelInfo;
typedef struct netpacket_s netpacket_t;
typedef struct netadr_s	netadr_t;

#include "compat_inetchannel.h"


#endif // INETCHANNEL_H
