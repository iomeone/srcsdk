abstract_class IRegistry
{
public:
	// We have to have a virtual destructor since otherwise the derived class destructors
	// will not be called.
	virtual ~IRegistry() {}

	// Init/shutdown
	virtual bool			Init( const char *platformName ) = 0;
	virtual void			Shutdown( void ) = 0;

	// Read/write integers
	virtual int				ReadInt( const char *key, int defaultValue = 0 ) = 0;
	virtual void			WriteInt( const char *key, int value ) = 0;

	// Read/write strings
	virtual const char		*ReadString( const char *key, const char *defaultValue = 0 ) = 0;
	virtual void			WriteString( const char *key, const char *value ) = 0;

	// Read/write helper methods
	virtual int				ReadInt( const char *pKeyBase, const char *pKey, int defaultValue = 0 ) = 0;
	virtual void			WriteInt( const char *pKeyBase, const char *key, int value ) = 0;
	virtual const char		*ReadString( const char *pKeyBase, const char *key, const char *defaultValue ) = 0;
	virtual void			WriteString( const char *pKeyBase, const char *key, const char *value ) = 0;
}; 
