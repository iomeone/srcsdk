class IDedicatedServerAPI : public IAppSystem
{
// Functions
public:
	// Initialize the engine with the specified base directory and interface factories
	virtual bool		ModInit( ModInfo_t &info ) = 0;
	// Shutdown the engine
	virtual void		ModShutdown( void ) = 0;
	// Run a frame
	virtual bool		RunFrame( void ) = 0;
	// Insert text into console
	virtual void		AddConsoleText( char *text ) = 0;
	// Get current status to display in the hlds UI (console window title bar, e.g. )
	virtual void		UpdateStatus(float *fps, int *nActive, int *nMaxPlayers, char *pszMap, int maxlen ) = 0;
	// Get current Hostname to display in the hlds UI (console window title bar, e.g. )
	virtual void		UpdateHostname(char *pszHostname, int maxlen) = 0;

	// for the multi-processed fork() server, set the server instance number (1..)
	virtual void        SetSubProcessID( int nID, int nSocketHandle ) = 0;
};
