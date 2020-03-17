abstract_class IDedicatedExports : public IAppSystem
{
public:
	virtual void Sys_Printf( char *text ) = 0;
	virtual void RunServer() = 0;
	virtual bool IsGuiDedicatedServer() = 0;
};
