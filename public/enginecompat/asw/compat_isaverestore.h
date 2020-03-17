abstract_class ISave
{
public:

	//---------------------------------
	// Logging
	virtual void	StartLogging( const char *pszLogName ) = 0;
	virtual void	EndLogging( void ) = 0;

	//---------------------------------
	virtual bool	IsAsync() = 0;

	//---------------------------------

	virtual int		GetWritePos() const = 0;
	virtual void	SetWritePos(int pos) = 0;
	
	//---------------------------------
	// Datamap based writing
	//
	
	virtual int		WriteAll( const void *pLeafObject, datamap_t *pLeafMap ) = 0;
	virtual int		WriteFields( const char *pname, const void *pBaseData, datamap_t *pMap, typedescription_t *pFields, int fieldCount ) = 0;

	template <typename T>
	int WriteAll( const T *pLeafObject )
	{
		return WriteAll( pLeafObject, &pLeafObject->m_DataMap );
	}

	//---------------------------------
	// Block support
	//
	// Using these, one doesn't have to worry about queuing up the read pointer on restore if only
	// a subset of the data is read
	//
	
	virtual void	StartBlock( const char *pszBlockName ) = 0;
	virtual void	StartBlock() = 0;
	virtual void	EndBlock() = 0;
	
	//---------------------------------
	// Primitive types
	//
	
	virtual void	WriteShort( const short *value, int count = 1 ) = 0;
	virtual void	WriteInt( const int *value, int count = 1 ) = 0;		// Save an int
	inline void		WriteInt( const unsigned *value, int count = 1 ) { WriteInt( (int *)value, count );	}
	virtual void	WriteBool( const bool *value, int count = 1 ) = 0;		// Save a bool
	virtual void	WriteFloat( const float *value, int count = 1 ) = 0;	// Save a float
	virtual void	WriteData( const char *pdata, int size ) = 0;		// Save a binary data block
	virtual void	WriteString( const char *pstring ) = 0;			// Save a null-terminated string
	virtual void	WriteString( const string_t *stringId, int count = 1 ) = 0;	// Save a null-terminated string (engine string)
	virtual void	WriteVector( const Vector &value ) = 0;				// Save a vector
	virtual void	WriteVector( const Vector *value, int count = 1 ) = 0;	// Save a vector array
	virtual void	WriteQuaternion( const Quaternion &value ) = 0;				// Save a Quaternion
	virtual void	WriteQuaternion( const Quaternion *value, int count = 1 ) = 0;	// Save a Quaternion array

	// Note: All of the following will write out both a header and the data. On restore,
	// this needs to be cracked
	virtual void	WriteShort( const char *pname, const short *value, int count = 1 ) = 0;
	virtual void	WriteInt( const char *pname, const int *value, int count = 1 ) = 0;		// Save an int
	virtual void	WriteBool( const char *pname, const bool *value, int count = 1 ) = 0;		// Save a bool
	virtual void	WriteFloat( const char *pname, const float *value, int count = 1 ) = 0;	// Save a float
	virtual void	WriteData( const char *pname, int size, const char *pdata ) = 0;		// Save a binary data block
	virtual void	WriteString( const char *pname, const char *pstring ) = 0;			// Save a null-terminated string
	virtual void	WriteString( const char *pname, const string_t *stringId, int count = 1 ) = 0;	// Save a null-terminated string (engine string)
	virtual void	WriteVector( const char *pname, const Vector &value ) = 0;				// Save a vector
	virtual void	WriteVector( const char *pname, const Vector *value, int count = 1 ) = 0;	// Save a vector array
	virtual void	WriteQuaternion( const char *pname, const Quaternion &value ) = 0;				// Save a Quaternion
	virtual void	WriteQuaternion( const char *pname, const Quaternion *value, int count = 1 ) = 0;	// Save a Quaternion array
	
	//---------------------------------
	// Game types
	//
	
	virtual void	WriteTime( const char *pname, const float *value, int count = 1 ) = 0;	// Save a float (timevalue)
	virtual void	WriteTick( const char *pname, const int *value, int count = 1 ) = 0;	// Save a tick (timevalue)
	virtual void	WritePositionVector( const char *pname, const Vector &value ) = 0;		// Offset for landmark if necessary
	virtual void	WritePositionVector( const char *pname, const Vector *value, int count = 1 ) = 0;	// array of pos vectors
	virtual void	WriteFunction( datamap_t *pMap, const char *pname, const int *value, int count = 1 ) = 0; // Save a function pointer

	virtual void	WriteTime( const float *value, int count = 1 ) = 0;	// Save a float (timevalue)
	virtual void	WriteTick( const int *value, int count = 1 ) = 0;	// Save a tick (timevalue)
	virtual void	WritePositionVector( const Vector &value ) = 0;		// Offset for landmark if necessary
	virtual void	WritePositionVector( const Vector *value, int count = 1 ) = 0;	// array of pos vectors

	virtual void	WriteEntityPtr( const char *pname, CBaseEntity **ppEntity, int count = 1 ) = 0;
	virtual void	WriteEdictPtr( const char *pname, edict_t **ppEdict, int count = 1 ) = 0;
	virtual void	WriteEHandle( const char *pname, const EHANDLE *pEHandle, int count = 1 ) = 0;

	virtual void	WriteEntityPtr( CBaseEntity **ppEntity, int count = 1 ) = 0;
	virtual void	WriteEdictPtr( edict_t **ppEdict, int count = 1 ) = 0;
	virtual void	WriteEHandle( const EHANDLE *pEHandle, int count = 1 ) = 0;

	//---------------------------------
	// Back door to support somewhat awkward ownership of game save/restore data
	virtual CGameSaveRestoreInfo *GetGameSaveRestoreInfo() = 0;

protected:
	virtual ~ISave() {};
};

abstract_class IRestore
{
public:

	//---------------------------------

	virtual int		GetReadPos() const = 0;
	virtual void	SetReadPos( int pos ) = 0;

	//---------------------------------
	// Datamap based reading
	//
	
	virtual int		ReadAll( void *pLeafObject, datamap_t *pLeafMap ) = 0;
	
	virtual int		ReadFields( const char *pname, void *pBaseData, datamap_t *pMap, typedescription_t *pFields, int fieldcount = 1 ) = 0;
	virtual void	EmptyFields( void *pBaseData, typedescription_t *pFields, int fieldcount = 1 ) = 0;

	template <typename T>
	int ReadAll( T *pLeafObject )
	{
		return ReadAll( pLeafObject, &pLeafObject->m_DataMap );
	}
	
	//---------------------------------
	// Block support
	//
	
	virtual void	StartBlock( SaveRestoreRecordHeader_t *pHeader ) = 0;
	virtual void	StartBlock( char szBlockName[SIZE_BLOCK_NAME_BUF] ) = 0;
	virtual void	StartBlock() = 0;
	virtual void	EndBlock() = 0;
	
	//---------------------------------
	// Field header cracking
	//
	
	virtual void	ReadHeader( SaveRestoreRecordHeader_t *pheader ) = 0;
	virtual int		SkipHeader() = 0; // skips the header, but returns the size of the field
	virtual const char *StringFromHeaderSymbol( int symbol ) = 0;

	//---------------------------------
	// Primitive types
	//
	
	virtual short	ReadShort( void ) = 0;
	virtual int		ReadShort( short *pValue, int count = 1, int nBytesAvailable = 0 ) = 0;
	virtual int		ReadInt( int *pValue, int count = 1, int nBytesAvailable = 0 ) = 0;
	inline  int		ReadInt( unsigned *pValue, int count = 1, int nBytesAvailable = 0 ) { return ReadInt( (int *)pValue, count, nBytesAvailable ); }
	virtual int		ReadInt( void ) = 0;
	virtual int		ReadBool( bool *pValue, int count = 1, int nBytesAvailable = 0 ) = 0;
	virtual int		ReadFloat( float *pValue, int count = 1, int nBytesAvailable = 0 ) = 0;
	virtual int		ReadData( char *pData, int size, int nBytesAvailable ) = 0;
	virtual void	ReadString( char *pDest, int nSizeDest, int nBytesAvailable ) = 0;			// A null-terminated string
	virtual int		ReadString( string_t *pString, int count = 1, int nBytesAvailable = 0 ) = 0;
	virtual int		ReadVector( Vector *pValue ) = 0;
	virtual int		ReadVector( Vector *pValue, int count = 1, int nBytesAvailable = 0 ) = 0;
	virtual int		ReadQuaternion( Quaternion *pValue ) = 0;
	virtual int		ReadQuaternion( Quaternion *pValue, int count = 1, int nBytesAvailable = 0 ) = 0;
	
	//---------------------------------
	// Game types
	//
	
	virtual int		ReadTime( float *pValue, int count = 1, int nBytesAvailable = 0 ) = 0;
	virtual int		ReadTick( int *pValue, int count = 1, int nBytesAvailable = 0 ) = 0;
	virtual int		ReadPositionVector( Vector *pValue ) = 0;
	virtual int		ReadPositionVector( Vector *pValue, int count = 1, int nBytesAvailable = 0 ) = 0;
	virtual int		ReadFunction( datamap_t *pMap, void **pValue, int count = 1, int nBytesAvailable = 0) = 0;
	
	virtual int		ReadEntityPtr( CBaseEntity **ppEntity, int count = 1, int nBytesAvailable = 0 ) = 0;
	virtual int		ReadEdictPtr( edict_t **ppEdict, int count = 1, int nBytesAvailable = 0 ) = 0;
	virtual int		ReadEHandle( EHANDLE *pEHandle, int count = 1, int nBytesAvailable = 0 ) = 0;
	virtual int		ReadVMatrix( VMatrix *pValue, int count = 1, int nBytesAvailable = 0) = 0;
	virtual int		ReadVMatrixWorldspace( VMatrix *pValue, int count = 1, int nBytesAvailable = 0) = 0;
	virtual int		ReadMatrix3x4Worldspace( matrix3x4_t *pValue, int nElems = 1, int nBytesAvailable = 0 ) = 0;

	// Used by Foundry to restore a certain entity's data (with Foundry data) while loading a savegame.
	// Returns -1 if not found.
	virtual int		ScanAheadForHammerID() = 0;
	virtual void	SkipEntityData() = 0;		// This skips the current entity's data.

	//---------------------------------

	virtual bool	GetPrecacheMode( void ) = 0;

	//---------------------------------
	// Back door to support somewhat awkward ownership of game save/restore data
	virtual CGameSaveRestoreInfo *GetGameSaveRestoreInfo() = 0;

protected:
	virtual ~IRestore() {};
};
