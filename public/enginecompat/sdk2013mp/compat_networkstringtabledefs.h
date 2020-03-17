class INetworkStringTableContainer
{
public:
	
	virtual					~INetworkStringTableContainer( void ) {};
	
	// table creation/destruction
	virtual INetworkStringTable	*CreateStringTable( const char *tableName, int maxentries, int userdatafixedsize = 0, int userdatanetworkbits = 0 ) = 0;
	virtual void				RemoveAllTables( void ) = 0;
	
	// table infos
	virtual INetworkStringTable	*FindTable( const char *tableName ) const = 0;
	virtual INetworkStringTable	*GetTable( TABLEID stringTable ) const = 0;
	virtual int					GetNumTables( void ) const = 0;

	virtual INetworkStringTable	*CreateStringTableEx( const char *tableName, int maxentries, int userdatafixedsize = 0, int userdatanetworkbits = 0, bool bIsFilenames = false ) = 0;
	virtual void				SetAllowClientSideAddString( INetworkStringTable *table, bool bAllowClientSideAddString ) = 0;
}; 
