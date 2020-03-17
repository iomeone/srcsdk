//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: INetMessage interface
//
// $NoKeywords: $
//=============================================================================//

#ifndef INETMESSAGE_H
#define INETMESSAGE_H

#include "tier1/bitbuf.h"

class INetMsgHandler;
class INetMessage;
class INetChannel;

// typedef bool (INetMsgHandler::*PROCESSFUNCPTR)(INetMessage*);
// #define CASTPROCPTR( fn ) static_cast <bool (INetMsgHandler::*)(INetMessage*)> (fn)

#include "compat_inetmessage.h"

#endif

