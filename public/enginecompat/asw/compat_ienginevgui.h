abstract_class IEngineVGui
{
public:
	virtual					~IEngineVGui( void ) { }

	virtual vgui::VPANEL	GetPanel( VGuiPanel_t type ) = 0;

	virtual bool			IsGameUIVisible() = 0;

	virtual void			ActivateGameUI() = 0;
}; 
