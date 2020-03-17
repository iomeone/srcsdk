/*
cdll_int.cpp -- VClient017 wrap
Copyright (C) 2017 a1batross

Licensed under WTFPL license.
Thanks to Valve for SDK, me for idea. 
Please, don't punish, Mr. Newell. :)
*/ 

#include "client.h"
#include "tier1.h"
#include "inputsystem/ButtonCode.h"

IBaseClientDLL *realClientDLL;
// we don't provide own full implementation of VClient017, so we need wrap it
// we also provide ourselves as "libclient.so", so real library was moved to "libclient_original.so"
GET_INTERFACE_PTR( Client, "libclient_original.so", CLIENT_DLL_INTERFACE_VERSION, &realClientDLL );

IVEngineClient	*engine = NULL;
IFileSystem *filesystem = NULL;
IEngineVGui *enginevgui = NULL;
IGameUIFuncs *gameuifuncs = NULL;
IInputSystem *inputsystem = NULL;
IGameUI *gameui = NULL;

GET_INTERFACE_PTR( IGameUI, "libGameUI.so", GAMEUI_INTERFACE_VERSION, &gameui );

class CWrapClient : public IBaseClientDLL
{
public:
	void **backup;
	size_t backupSize;

	virtual void			PostInit() { realClientDLL->PostInit(); }
	virtual bool			ReplayInit( CreateInterfaceFn replayFactory ) { return realClientDLL->ReplayInit(replayFactory); }
	virtual bool			ReplayPostInit() { return realClientDLL->ReplayPostInit(); }
	virtual void			LevelInitPreEntity( char const* pMapName ) { realClientDLL->LevelInitPreEntity(pMapName); }
	virtual void			LevelInitPostEntity( ) { realClientDLL->LevelInitPostEntity(); }
	virtual void			LevelShutdown( void ) { realClientDLL->LevelShutdown(); }
	virtual ClientClass		*GetAllClasses( void ) { return realClientDLL->GetAllClasses(); }
	virtual int				HudVidInit( void ) { return realClientDLL->HudVidInit(); }
	virtual void			HudUpdate( bool bActive ) { realClientDLL->HudUpdate(bActive); }
	virtual void			HudReset( void ) { realClientDLL->HudReset(); }
	virtual void			HudText( const char * message ) { realClientDLL->HudText(message); }
	virtual void			IN_ActivateMouse( void ) { realClientDLL->IN_ActivateMouse(); }
	virtual void			IN_DeactivateMouse( void ) { realClientDLL->IN_DeactivateMouse(); }
	virtual void			IN_Accumulate (void) { realClientDLL->IN_Accumulate(); }
	virtual void			IN_ClearStates (void) { realClientDLL->IN_ClearStates(); }
	virtual bool			IN_IsKeyDown( const char *name, bool& isdown ) { return realClientDLL->IN_IsKeyDown(name, isdown); }
	virtual void			IN_OnMouseWheeled( int nDelta ) { realClientDLL->IN_OnMouseWheeled(nDelta); }
	virtual int				IN_KeyEvent( int eventcode, ButtonCode_t keynum, const char *pszCurrentBinding ) { return realClientDLL->IN_KeyEvent(eventcode, keynum, pszCurrentBinding); }
	virtual void			CreateMove ( 
								int sequence_number,			// sequence_number of this cmd
								float input_sample_frametime,	// Frametime for mouse input sampling
								bool active ) { realClientDLL->CreateMove(sequence_number, input_sample_frametime, active); }
	virtual void			ExtraMouseSample( float frametime, bool active ) { realClientDLL->ExtraMouseSample(frametime, active); }
	virtual bool			WriteUsercmdDeltaToBuffer( bf_write *buf, int from, int to, bool isnewcommand ) { return realClientDLL->WriteUsercmdDeltaToBuffer(buf, from, to, isnewcommand); }
	virtual void			EncodeUserCmdToBuffer( bf_write& buf, int slot ) { realClientDLL->EncodeUserCmdToBuffer(buf, slot); }
	virtual void			DecodeUserCmdFromBuffer( bf_read& buf, int slot ) { realClientDLL->DecodeUserCmdFromBuffer(buf, slot); }
	virtual void			View_Render( vrect_t *rect ) { realClientDLL->View_Render(rect); }
	virtual void			RenderView( const CViewSetup &view, int nClearFlags, int whatToDraw ) { realClientDLL->RenderView(view, nClearFlags, whatToDraw); }
	virtual void			View_Fade( ScreenFade_t *pSF ) { realClientDLL->View_Fade(pSF); }
	virtual void			SetCrosshairAngle( const QAngle& angle ) { realClientDLL->SetCrosshairAngle(angle); }
	virtual void			InitSprite( CEngineSprite *pSprite, const char *loadname ) { realClientDLL->InitSprite(pSprite, loadname); }
	virtual void			ShutdownSprite( CEngineSprite *pSprite ) { realClientDLL->ShutdownSprite(pSprite); }
	virtual int				GetSpriteSize( void ) const { return realClientDLL->GetSpriteSize(); }
	virtual void			VoiceStatus( int entindex, qboolean bTalking ) { realClientDLL->VoiceStatus(entindex, bTalking); }
	virtual void			InstallStringTableCallback( char const *tableName ) { realClientDLL->InstallStringTableCallback(tableName); }
	virtual void			FrameStageNotify( ClientFrameStage_t curStage ) { realClientDLL->FrameStageNotify(curStage); }
	virtual bool			DispatchUserMessage( int msg_type, bf_read &msg_data ) { return realClientDLL->DispatchUserMessage(msg_type, msg_data); }
	virtual CSaveRestoreData  *SaveInit( int size ) { return realClientDLL->SaveInit(size); }
	virtual void			SaveWriteFields( CSaveRestoreData *a1, const char *a2, void *a3, datamap_t *a4, typedescription_t *a5, int a6 ) { realClientDLL->SaveWriteFields(a1, a2, a3, a4, a5, a6); }
	virtual void			SaveReadFields( CSaveRestoreData *a1, const char *a2, void *a3, datamap_t *a4, typedescription_t *a5, int a6 ) { realClientDLL->SaveReadFields(a1, a2, a3, a4, a5, a6); }
	virtual void			PreSave( CSaveRestoreData *a1 ) { realClientDLL->PreSave(a1); }
	virtual void			Save( CSaveRestoreData *a1 ) { realClientDLL->Save(a1); }
	virtual void			WriteSaveHeaders( CSaveRestoreData *a1 ) { realClientDLL->WriteSaveHeaders(a1); }
	virtual void			ReadRestoreHeaders( CSaveRestoreData *a1 ) { realClientDLL->ReadRestoreHeaders(a1); }
	virtual void			Restore( CSaveRestoreData *a1, bool a2 ) { realClientDLL->Restore(a1, a2); }
	virtual void			DispatchOnRestore() { realClientDLL->DispatchOnRestore(); }
	virtual CStandardRecvProxies* GetStandardRecvProxies() { return realClientDLL->GetStandardRecvProxies(); }
	virtual void			WriteSaveGameScreenshot( const char *pFilename ) { realClientDLL->WriteSaveGameScreenshot(pFilename); }
	virtual void			EmitSentenceCloseCaption( char const *tokenstream ) { realClientDLL->EmitSentenceCloseCaption(tokenstream); }
	virtual void			EmitCloseCaption( char const *captionname, float duration ) { realClientDLL->EmitCloseCaption(captionname, duration); }
	virtual bool			CanRecordDemo( char *errorMsg, int length ) const { return realClientDLL->CanRecordDemo(errorMsg, length); }
	virtual void			OnDemoRecordStart( char const* pDemoBaseName ) { realClientDLL->OnDemoRecordStart(pDemoBaseName); }
	virtual void			OnDemoRecordStop() { realClientDLL->OnDemoRecordStop(); }
	virtual void			OnDemoPlaybackStart( char const* pDemoBaseName ) { realClientDLL->OnDemoPlaybackStart(pDemoBaseName); }
	virtual void			OnDemoPlaybackStop() { realClientDLL->OnDemoPlaybackStop(); }
	virtual bool			ShouldDrawDropdownConsole() { return realClientDLL->ShouldDrawDropdownConsole(); }
	virtual int				GetScreenWidth() { return realClientDLL->GetScreenWidth(); }
	virtual int				GetScreenHeight() { return realClientDLL->GetScreenHeight(); }
	virtual void			WriteSaveGameScreenshotOfSize( const char *pFilename, int width, int height, bool bCreatePowerOf2Padded = false, bool bWriteVTF = false ) { realClientDLL->WriteSaveGameScreenshotOfSize(pFilename, width, height, bCreatePowerOf2Padded, bWriteVTF); }
	virtual bool			GetPlayerView( CViewSetup &playerView ) { return realClientDLL->GetPlayerView(playerView); }
	virtual void			SetupGameProperties( CUtlVector< XUSER_CONTEXT > &contexts, CUtlVector< XUSER_PROPERTY > &properties ) { realClientDLL->SetupGameProperties(contexts, properties); }
	virtual uint			GetPresenceID( const char *pIDName ) { return realClientDLL->GetPresenceID(pIDName); }
	virtual const char		*GetPropertyIdString( const uint id ) { return realClientDLL->GetPropertyIdString(id); }
	virtual void			GetPropertyDisplayString( uint id, uint value, char *pOutput, int nBytes ) { realClientDLL->GetPropertyDisplayString(id, value, pOutput, nBytes); }
	virtual void			InvalidateMdlCache() { realClientDLL->InvalidateMdlCache(); }
	virtual void			IN_SetSampleTime( float frametime ) { realClientDLL->IN_SetSampleTime(frametime); }
	virtual void			ReloadFilesInList( IFileList *pFilesToReload ) { realClientDLL->ReloadFilesInList(pFilesToReload); }
	virtual void			StartStatsReporting( HANDLE handle, bool bArbitrated ) { realClientDLL->StartStatsReporting(handle, bArbitrated); }
	virtual bool			HandleUiToggle() { return realClientDLL->HandleUiToggle(); }
	virtual bool			ShouldAllowConsole() { return realClientDLL->ShouldAllowConsole(); }
	virtual CRenamedRecvTableInfo	*GetRenamedRecvTableInfos() { return realClientDLL->GetRenamedRecvTableInfos(); }
	virtual CMouthInfo		*GetClientUIMouthInfo() { return realClientDLL->GetClientUIMouthInfo(); }
	virtual void			FileReceived( const char * fileName, unsigned int transferID ) { realClientDLL->FileReceived(fileName, transferID); }
	virtual const char* TranslateEffectForVisionFilter( const char *pchEffectType, const char *pchEffectName ) { return realClientDLL->TranslateEffectForVisionFilter(pchEffectType, pchEffectName); }
	virtual void			ClientAdjustStartSoundParams( struct StartSoundParams_t& params ) { realClientDLL->ClientAdjustStartSoundParams(params); }
	virtual bool DisconnectAttempt( void ) { return realClientDLL->DisconnectAttempt(); }
	virtual bool IsConnectedUserInfoChangeAllowed( IConVar *pCvar ) { return realClientDLL->IsConnectedUserInfoChangeAllowed(pCvar); }

	virtual int						Init( CreateInterfaceFn appSystemFactory, CreateInterfaceFn physicsFactory, CGlobalVarsBase *pGlobals )
	{
		if ( (engine = (IVEngineClient *)appSystemFactory( VENGINE_CLIENT_INTERFACE_VERSION, NULL )) == NULL )
			return false;
		if ( (enginevgui = (IEngineVGui *)appSystemFactory(VENGINE_VGUI_VERSION, NULL )) == NULL )
			return false;
		if ( (filesystem = (IFileSystem *)appSystemFactory(FILESYSTEM_INTERFACE_VERSION, NULL)) == NULL )
			return false;
		if ( (gameuifuncs = (IGameUIFuncs * )appSystemFactory( VENGINE_GAMEUIFUNCS_VERSION, NULL )) == NULL )
			return false;
		if ( (inputsystem = (IInputSystem *)appSystemFactory(INPUTSYSTEM_INTERFACE_VERSION, NULL)) == NULL )
			return false;
		
		ConnectTier1Libraries( &appSystemFactory, 1 );
		
		LogPrintf( "Hello from CWrapClient. Running on %s, ver. %i", engine->GetProductVersionString(), engine->GetEngineBuildNumber() );
		engine->ClientCmd("con_enable 1");
		engine->ClientCmd("developer 1");
		
		g_Touch.Init();
		
		int ret = realClientDLL->Init( appSystemFactory, physicsFactory, pGlobals );
		
		ConVar_Register( FCVAR_CLIENTDLL );
		
		return ret;
	}
	
	virtual void					Shutdown( void )
	{
		g_Touch.Shutdown();
		realClientDLL->Shutdown();
		ConVar_Unregister();
		DisconnectTier1Libraries();
	}
	
	virtual void					HudProcessInput( bool bActive )
	{
		Android_RunEvents(); // called every frame, regardless is active client or not
		g_Touch.Frame();
		return realClientDLL->HudProcessInput( bActive );
	}
} gWrapClient;
IBaseClientDLL *clientdll = &gWrapClient;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CWrapClient, IBaseClientDLL, CLIENT_DLL_INTERFACE_VERSION, gWrapClient );
