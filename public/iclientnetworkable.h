//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef ICLIENTNETWORKABLE_H
#define ICLIENTNETWORKABLE_H
#ifdef _WIN32
#pragma once
#endif


#include "iclientunknown.h"
#include "tier1/bitbuf.h"


class IClientEntity;
class ClientClass;


enum ShouldTransmitState_t
{
	SHOULDTRANSMIT_START=0,	// The entity is starting to be transmitted (maybe it entered the PVS).
	
	SHOULDTRANSMIT_END		// Called when the entity isn't being transmitted by the server.
							// This signals a good time to hide the entity until next time
							// the server wants to transmit its state.
};

// NOTE: All of these are commented out; NotifyShouldTransmit actually
// has all these in them. Left it as an enum in case we want to go back though
enum DataUpdateType_t
{
	DATA_UPDATE_CREATED = 0,	// indicates it was created +and+ entered the pvs
//	DATA_UPDATE_ENTERED_PVS,
	DATA_UPDATE_DATATABLE_CHANGED,
//	DATA_UPDATE_LEFT_PVS,
//	DATA_UPDATE_DESTROYED,		// FIXME: Could enable this, but it's a little worrying
								// since it changes a bunch of existing code
};

#include "compat_iclientnetworkable.h"


#endif // ICLIENTNETWORKABLE_H
