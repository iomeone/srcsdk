#include "sdk2013mp/compat_inetmsghandler.h"

class IServerMessageHandler : public INetMessageHandler
{
public:
	virtual ~IServerMessageHandler( void ) {};

	// Returns dem file protocol version, or, if not playing a demo, just returns PROTOCOL_VERSION
	virtual int GetDemoProtocolVersion() const = 0;

	PROCESS_SVC_MESSAGE( Print ) = 0;
	PROCESS_SVC_MESSAGE( ServerInfo ) = 0;
	PROCESS_SVC_MESSAGE( SendTable ) = 0;
	PROCESS_SVC_MESSAGE( ClassInfo ) = 0;
	PROCESS_SVC_MESSAGE( SetPause ) = 0;
	PROCESS_SVC_MESSAGE( CreateStringTable ) = 0;
	PROCESS_SVC_MESSAGE( UpdateStringTable ) = 0;
	PROCESS_SVC_MESSAGE( VoiceInit ) = 0;
	PROCESS_SVC_MESSAGE( VoiceData ) = 0;
	PROCESS_SVC_MESSAGE( Sounds ) = 0;
	PROCESS_SVC_MESSAGE( SetView ) = 0;
	PROCESS_SVC_MESSAGE( FixAngle ) = 0;
	PROCESS_SVC_MESSAGE( CrosshairAngle ) = 0;
	PROCESS_SVC_MESSAGE( BSPDecal ) = 0;
	PROCESS_SVC_MESSAGE( GameEvent ) = 0;
	PROCESS_SVC_MESSAGE( UserMessage ) = 0;
	PROCESS_SVC_MESSAGE( EntityMessage ) = 0;
	PROCESS_SVC_MESSAGE( PacketEntities ) = 0;
	PROCESS_SVC_MESSAGE( TempEntities ) = 0;
	PROCESS_SVC_MESSAGE( Prefetch ) = 0;
	PROCESS_SVC_MESSAGE( Menu ) = 0;
	PROCESS_SVC_MESSAGE( GameEventList ) = 0;
	PROCESS_SVC_MESSAGE( GetCvarValue ) = 0;
	PROCESS_SVC_MESSAGE( CmdKeyValues ) = 0;
	PROCESS_SVC_MESSAGE( SetPauseTimed ) = 0;
};