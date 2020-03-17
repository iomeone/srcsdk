class CGlobalVars : public CGlobalVarsBase
{	
public:

	CGlobalVars( bool bIsClient );

public:
	
	// Current map
	string_t		mapname;
	int				mapversion;
	string_t		startspot;
	MapLoadType_t	eLoadType;		// How the current map was loaded
	bool			bMapLoadFailed;	// Map has failed to load, we need to kick back to the main menu

	// game specific flags
	bool			deathmatch;
	bool			coop;
	bool			teamplay;
	// current maxentities
	int				maxEntities;

	int				serverCount;
	edict_t			*pEdicts;
};

class CBaseEdict
{
public:

	// Returns an IServerEntity if FL_FULLEDICT is set or NULL if this 
	// is a lightweight networking entity.
	IServerEntity*			GetIServerEntity();
	const IServerEntity*	GetIServerEntity() const;

	IServerNetworkable*		GetNetworkable();
	IServerUnknown*			GetUnknown();

	// Set when initting an entity. If it's only a networkable, this is false.
	void				SetEdict( IServerUnknown *pUnk, bool bFullEdict );
	
	int					AreaNum() const;
	const char *		GetClassName() const;
	
	bool				IsFree() const;
	void				SetFree();
	void				ClearFree();

	bool				HasStateChanged() const;
	void				ClearStateChanged();
	void				StateChanged();
	void				StateChanged( unsigned short offset );

	void				ClearTransmitState();
	
	void SetChangeInfo( unsigned short info );
	void SetChangeInfoSerialNumber( unsigned short sn );
	unsigned short	 GetChangeInfo() const;
	unsigned short	 GetChangeInfoSerialNumber() const;

public:

	// NOTE: this is in the edict instead of being accessed by a virtual because the engine needs fast access to it.
	// NOTE: YOU CAN'T CHANGE THE LAYOUT OR SIZE OF CBASEEDICT AND REMAIN COMPATIBLE WITH HL2_VC6!!!!!
	int	m_fStateFlags;	

	// NOTE: this is in the edict instead of being accessed by a virtual because the engine needs fast access to it.
	int m_NetworkSerialNumber;

	// NOTE: this is in the edict instead of being accessed by a virtual because the engine needs fast access to it.
	IServerNetworkable	*m_pNetworkable;

protected:
	IServerUnknown		*m_pUnk;		


public:

	IChangeInfoAccessor *GetChangeAccessor(); // The engine implements this and the game .dll implements as
	const IChangeInfoAccessor *GetChangeAccessor() const; // The engine implements this and the game .dll implements as
	// as callback through to the engine!!!

	// NOTE: YOU CAN'T CHANGE THE LAYOUT OR SIZE OF CBASEEDICT AND REMAIN COMPATIBLE WITH HL2_VC6!!!!!
	// This breaks HL2_VC6!!!!!
	// References a CEdictChangeInfo with a list of modified network props.
	//unsigned short m_iChangeInfo;
	//unsigned short m_iChangeInfoSerialNumber;

	friend void InitializeEntityDLLFields( edict_t *pEdict );
};

struct edict_t : public CBaseEdict
{
public:
	ICollideable *GetCollideable();
};