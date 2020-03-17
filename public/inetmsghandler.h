//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $Log: $
//
// $NoKeywords: $
//=============================================================================//
#if !defined( INETMSGHANDLER_H )
#define INETMSGHANDLER_H
#ifdef _WIN32
#pragma once
#endif

class	INetChannel;
typedef struct netpacket_s netpacket_t;

#define PROCESS_NET_MESSAGE( name )	\
	virtual bool Process##name( NET_##name *msg )

#define PROCESS_SVC_MESSAGE( name )	\
	virtual bool Process##name( SVC_##name *msg )

#define PROCESS_CLC_MESSAGE( name )	\
	virtual bool Process##name( CLC_##name *msg )

#define PROCESS_MM_MESSAGE( name )	\
	virtual bool Process##name( MM_##name *msg )


#define REGISTER_NET_MSG( name )				\
	NET_##name * p##name = new NET_##name();	\
	p##name->m_pMessageHandler = this;			\
	chan->RegisterMessage( p##name );			\

#define REGISTER_SVC_MSG( name )				\
	SVC_##name * p##name = new SVC_##name();	\
	p##name->m_pMessageHandler = this;			\
	chan->RegisterMessage( p##name );			\

#define REGISTER_CLC_MSG( name )				\
	CLC_##name * p##name = new CLC_##name();	\
	p##name->m_pMessageHandler = this;			\
	chan->RegisterMessage( p##name );			\

#define REGISTER_MM_MSG( name )					\
	MM_##name * p##name = new MM_##name();		\
	p##name->m_pMessageHandler = this;			\
	chan->RegisterMessage( p##name );			\

class NET_Tick;
class NET_StringCmd;
class NET_SetConVar;
class NET_SignonState;
class NET_SplitScreenUser;

class CLC_ClientInfo;
class CLC_Move;
class CLC_VoiceData;
class CLC_BaselineAck;
class CLC_ListenEvents;
class CLC_RespondCvarValue;
class CLC_FileCRCCheck;
class CLC_FileMD5Check;
class CLC_SaveReplay;
class CLC_CmdKeyValues;
class CLC_LoadingProgress;
class CLC_SplitPlayerConnect;

class SVC_Print;
class SVC_ServerInfo;
class SVC_SendTable;
class SVC_ClassInfo;
class SVC_SetPause;
class SVC_CreateStringTable;
class SVC_UpdateStringTable;
class SVC_VoiceInit;
class SVC_VoiceData;
class SVC_Sounds;
class SVC_SetView;
class SVC_FixAngle;
class SVC_CrosshairAngle;
class SVC_BSPDecal;
class SVC_GameEvent;
class SVC_UserMessage;
class SVC_EntityMessage;
class SVC_PacketEntities;
class SVC_TempEntities;
class SVC_Prefetch;
class SVC_Menu;
class SVC_GameEventList;
class SVC_GetCvarValue;
class SVC_CmdKeyValues;
class SVC_SplitScreen;
class SVC_SetPauseTimed;

class MM_Heartbeat;
class MM_ClientInfo;
class MM_JoinResponse;
class MM_RegisterResponse;
class MM_Migrate;
class MM_Mutelist;
class MM_Checkpoint;

class IMatchmakingMessageHandler : public INetMessageHandler
{
public:
	virtual ~IMatchmakingMessageHandler( void ) {};

	PROCESS_MM_MESSAGE( Heartbeat ) = 0;
	PROCESS_MM_MESSAGE( ClientInfo ) = 0;
	PROCESS_MM_MESSAGE( JoinResponse ) = 0;
	PROCESS_MM_MESSAGE( RegisterResponse ) = 0;
	PROCESS_MM_MESSAGE( Migrate ) = 0;
	PROCESS_MM_MESSAGE( Mutelist ) = 0;
	PROCESS_MM_MESSAGE( Checkpoint) = 0;
};

class IConnectionlessPacketHandler
{
public:
	virtual	~IConnectionlessPacketHandler( void ) {};

	virtual bool ProcessConnectionlessPacket( netpacket_t *packet ) = 0;	// process a connectionless packet
};

#include "compat_inetmsghandler.h"

#endif // INETMSGHANDLER_H
