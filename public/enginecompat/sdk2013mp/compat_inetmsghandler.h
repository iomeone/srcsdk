class INetChannelHandler
{
public:
	virtual	~INetChannelHandler( void ) {};

	virtual void ConnectionStart(INetChannel *chan) = 0;	// called first time network channel is established

	virtual void ConnectionClosing(const char *reason) = 0; // network channel is being closed by remote site

	virtual void ConnectionCrashed(const char *reason) = 0; // network error occured

	virtual void PacketStart(int incoming_sequence, int outgoing_acknowledged) = 0;	// called each time a new packet arrived

	virtual void PacketEnd( void ) = 0; // all messages has been parsed

	virtual void FileRequested(const char *fileName, unsigned int transferID ) = 0; // other side request a file for download

	virtual void FileReceived(const char *fileName, unsigned int transferID ) = 0; // we received a file
	
	virtual void FileDenied(const char *fileName, unsigned int transferID ) = 0;	// a file request was denied by other side

	virtual void FileSent(const char *fileName, unsigned int transferID ) = 0;	// we sent a file
};

class INetMessageHandler 
{
public:
	virtual ~INetMessageHandler( void ) {};

	PROCESS_NET_MESSAGE( Tick ) = 0;
	PROCESS_NET_MESSAGE( StringCmd ) = 0;
	PROCESS_NET_MESSAGE( SetConVar ) = 0;
	PROCESS_NET_MESSAGE( SignonState ) = 0;
};

class IClientMessageHandler : public INetMessageHandler
{
public:
	virtual ~IClientMessageHandler( void ) {};

	PROCESS_CLC_MESSAGE( ClientInfo ) = 0;
	PROCESS_CLC_MESSAGE( Move ) = 0;
	PROCESS_CLC_MESSAGE( VoiceData ) = 0;
	PROCESS_CLC_MESSAGE( BaselineAck ) = 0;
	PROCESS_CLC_MESSAGE( ListenEvents ) = 0;
	PROCESS_CLC_MESSAGE( RespondCvarValue ) = 0;
	PROCESS_CLC_MESSAGE( FileCRCCheck ) = 0;
	PROCESS_CLC_MESSAGE( FileMD5Check ) = 0;
#if defined( ENGINE_REPLAY_ENABLED )
	PROCESS_CLC_MESSAGE( SaveReplay ) = 0;
#endif
	PROCESS_CLC_MESSAGE( CmdKeyValues ) = 0;
};

#if TARGET_ENGINE != TARGET_ENGINE_TF2
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
};
#endif