abstract_class IFileSystem : public IAppSystem, public IBaseFileSystem
{
public:
	//--------------------------------------------------------
	// Steam operations
	//--------------------------------------------------------

	virtual bool			IsSteam() const = 0;

	// Supplying an extra app id will mount this app in addition 
	// to the one specified in the environment variable "steamappid"
	// 
	// If nExtraAppId is < -1, then it will mount that app ID only.
	// (Was needed by the dedicated server b/c the "SteamAppId" env var only gets passed to steam.dll
	// at load time, so the dedicated couldn't pass it in that way).
	virtual	FilesystemMountRetval_t MountSteamContent( int nExtraAppId = -1 ) = 0;

	//--------------------------------------------------------
	// Search path manipulation
	//--------------------------------------------------------

	// Add paths in priority order (mod dir, game dir, ....)
	// If one or more .pak files are in the specified directory, then they are
	//  added after the file system path
	// If the path is the relative path to a .bsp file, then any previous .bsp file 
	//  override is cleared and the current .bsp is searched for an embedded PAK file
	//  and this file becomes the highest priority search path ( i.e., it's looked at first
	//   even before the mod's file system path ).
	virtual void			AddSearchPath( const char *pPath, const char *pathID, SearchPathAdd_t addType = PATH_ADD_TO_TAIL ) = 0;
	virtual bool			RemoveSearchPath( const char *pPath, const char *pathID = 0 ) = 0;

	// Remove all search paths (including write path?)
	virtual void			RemoveAllSearchPaths( void ) = 0;

	// Remove search paths associated with a given pathID
	virtual void			RemoveSearchPaths( const char *szPathID ) = 0;

	// This is for optimization. If you mark a path ID as "by request only", then files inside it
	// will only be accessed if the path ID is specifically requested. Otherwise, it will be ignored.
	// If there are currently no search paths with the specified path ID, then it will still
	// remember it in case you add search paths with this path ID.
	virtual void			MarkPathIDByRequestOnly( const char *pPathID, bool bRequestOnly ) = 0;

	// converts a partial path into a full path
	// Prefer using the RelativePathToFullPath_safe template wrapper to calling this directly
	virtual const char		*RelativePathToFullPath( const char *pFileName, const char *pPathID, OUT_Z_CAP(maxLenInChars) char *pDest, int maxLenInChars, PathTypeFilter_t pathFilter = FILTER_NONE, PathTypeQuery_t *pPathType = NULL ) = 0;
	template <size_t maxLenInChars> const char *RelativePathToFullPath_safe( const char *pFileName, const char *pPathID, OUT_Z_ARRAY char (&pDest)[maxLenInChars], PathTypeFilter_t pathFilter = FILTER_NONE, PathTypeQuery_t *pPathType = NULL )
	{
		return RelativePathToFullPath( pFileName, pPathID, pDest, (int)maxLenInChars, pathFilter, pPathType );
	}

	// Returns the search path, each path is separated by ;s. Returns the length of the string returned
	// Prefer using the GetSearchPath_safe template wrapper to calling this directly
	virtual int				GetSearchPath( const char *pathID, bool bGetPackFiles, OUT_Z_CAP(maxLenInChars) char *pDest, int maxLenInChars ) = 0;
	template <size_t maxLenInChars> int GetSearchPath_safe( const char *pathID, bool bGetPackFiles, OUT_Z_ARRAY char (&pDest)[maxLenInChars] )
	{
		return GetSearchPath( pathID, bGetPackFiles, pDest, (int)maxLenInChars );
	}

	// interface for custom pack files > 4Gb
	virtual bool			AddPackFile( const char *fullpath, const char *pathID ) = 0;

	//--------------------------------------------------------
	// File manipulation operations
	//--------------------------------------------------------

	// Deletes a file (on the WritePath)
	virtual void			RemoveFile( char const* pRelativePath, const char *pathID = 0 ) = 0;

	// Renames a file (on the WritePath)
	virtual bool			RenameFile( char const *pOldPath, char const *pNewPath, const char *pathID = 0 ) = 0;

	// create a local directory structure
	virtual void			CreateDirHierarchy( const char *path, const char *pathID = 0 ) = 0;

	// File I/O and info
	virtual bool			IsDirectory( const char *pFileName, const char *pathID = 0 ) = 0;

	virtual void			FileTimeToString( char* pStrip, int maxCharsIncludingTerminator, long fileTime ) = 0;

	//--------------------------------------------------------
	// Open file operations
	//--------------------------------------------------------

	virtual void			SetBufferSize( FileHandle_t file, unsigned nBytes ) = 0;

	virtual bool			IsOk( FileHandle_t file ) = 0;

	virtual bool			EndOfFile( FileHandle_t file ) = 0;

	virtual char			*ReadLine( char *pOutput, int maxChars, FileHandle_t file ) = 0;
	virtual int				FPrintf( FileHandle_t file, PRINTF_FORMAT_STRING const char *pFormat, ... ) FMTFUNCTION( 3, 4 ) = 0;

	//--------------------------------------------------------
	// Dynamic library operations
	//--------------------------------------------------------

	// load/unload modules
	virtual CSysModule 		*LoadModule( const char *pFileName, const char *pPathID = 0, bool bValidatedDllOnly = true ) = 0;
	virtual void			UnloadModule( CSysModule *pModule ) = 0;

	//--------------------------------------------------------
	// File searching operations
	//--------------------------------------------------------

	// FindFirst/FindNext. Also see FindFirstEx.
	virtual const char		*FindFirst( const char *pWildCard, FileFindHandle_t *pHandle ) = 0;
	virtual const char		*FindNext( FileFindHandle_t handle ) = 0;
	virtual bool			FindIsDirectory( FileFindHandle_t handle ) = 0;
	virtual void			FindClose( FileFindHandle_t handle ) = 0;

	// Same as FindFirst, but you can filter by path ID, which can make it faster.
	virtual const char		*FindFirstEx( 
		const char *pWildCard, 
		const char *pPathID,
		FileFindHandle_t *pHandle
		) = 0;

	//--------------------------------------------------------
	// File name and directory operations
	//--------------------------------------------------------

	// FIXME: This method is obsolete! Use RelativePathToFullPath instead!
	// converts a partial path into a full path
	// Prefer using the GetLocalPath_safe template wrapper to calling this directly
	virtual const char		*GetLocalPath( const char *pFileName, OUT_Z_CAP(maxLenInChars) char *pDest, int maxLenInChars ) = 0;
	template <size_t maxLenInChars> const char *GetLocalPath_safe( const char *pFileName, OUT_Z_ARRAY char (&pDest)[maxLenInChars] )
	{
		return GetLocalPath( pFileName, pDest, (int)maxLenInChars );
	}

	// Returns true on success ( based on current list of search paths, otherwise false if 
	//  it can't be resolved )
	// Prefer using the FullPathToRelativePath_safe template wrapper to calling this directly
	virtual bool			FullPathToRelativePath( const char *pFullpath, OUT_Z_CAP(maxLenInChars) char *pDest, int maxLenInChars ) = 0;
	template <size_t maxLenInChars> bool FullPathToRelativePath_safe( const char *pFullpath, OUT_Z_ARRAY char (&pDest)[maxLenInChars] )
	{
		return FullPathToRelativePath( pFullpath, pDest, (int)maxLenInChars );
	}

	// Gets the current working directory
	virtual bool			GetCurrentDirectory( char* pDirectory, int maxlen ) = 0;

	//--------------------------------------------------------
	// Filename dictionary operations
	//--------------------------------------------------------

	virtual FileNameHandle_t	FindOrAddFileName( char const *pFileName ) = 0;
	virtual bool				String( const FileNameHandle_t& handle, char *buf, int buflen ) = 0;

	//--------------------------------------------------------
	// Asynchronous file operations
	//--------------------------------------------------------

	//------------------------------------
	// Global operations
	//------------------------------------
			FSAsyncStatus_t	AsyncRead( const FileAsyncRequest_t &request, FSAsyncControl_t *phControl = NULL )	{ return AsyncReadMultiple( &request, 1, phControl ); 	}
	virtual FSAsyncStatus_t	AsyncReadMultiple( const FileAsyncRequest_t *pRequests, int nRequests,  FSAsyncControl_t *phControls = NULL ) = 0;
	virtual FSAsyncStatus_t	AsyncAppend(const char *pFileName, const void *pSrc, int nSrcBytes, bool bFreeMemory, FSAsyncControl_t *pControl = NULL ) = 0;
	virtual FSAsyncStatus_t	AsyncAppendFile(const char *pAppendToFileName, const char *pAppendFromFileName, FSAsyncControl_t *pControl = NULL ) = 0;
	virtual void			AsyncFinishAll( int iToPriority = 0 ) = 0;
	virtual void			AsyncFinishAllWrites() = 0;
	virtual FSAsyncStatus_t	AsyncFlush() = 0;
	virtual bool			AsyncSuspend() = 0;
	virtual bool			AsyncResume() = 0;

	//------------------------------------
	// Functions to hold a file open if planning on doing mutiple reads. Use is optional,
	// and is taken only as a hint
	//------------------------------------
	virtual FSAsyncStatus_t	AsyncBeginRead( const char *pszFile, FSAsyncFile_t *phFile ) = 0;
	virtual FSAsyncStatus_t	AsyncEndRead( FSAsyncFile_t hFile ) = 0;

	//------------------------------------
	// Request management
	//------------------------------------
	virtual FSAsyncStatus_t	AsyncFinish( FSAsyncControl_t hControl, bool wait = true ) = 0;
	virtual FSAsyncStatus_t	AsyncGetResult( FSAsyncControl_t hControl, void **ppData, int *pSize ) = 0;
	virtual FSAsyncStatus_t	AsyncAbort( FSAsyncControl_t hControl ) = 0;
	virtual FSAsyncStatus_t	AsyncStatus( FSAsyncControl_t hControl ) = 0;
	// set a new priority for a file already in the queue
	virtual FSAsyncStatus_t	AsyncSetPriority(FSAsyncControl_t hControl, int newPriority) = 0;
	virtual void			AsyncAddRef( FSAsyncControl_t hControl ) = 0;
	virtual void			AsyncRelease( FSAsyncControl_t hControl ) = 0;

	//--------------------------------------------------------
	// Remote resource management
	//--------------------------------------------------------

	// starts waiting for resources to be available
	// returns FILESYSTEM_INVALID_HANDLE if there is nothing to wait on
	virtual WaitForResourcesHandle_t WaitForResources( const char *resourcelist ) = 0;
	// get progress on waiting for resources; progress is a float [0, 1], complete is true on the waiting being done
	// returns false if no progress is available
	// any calls after complete is true or on an invalid handle will return false, 0.0f, true
	virtual bool			GetWaitForResourcesProgress( WaitForResourcesHandle_t handle, float *progress /* out */ , bool *complete /* out */ ) = 0;
	// cancels a progress call
	virtual void			CancelWaitForResources( WaitForResourcesHandle_t handle ) = 0;

	// hints that a set of files will be loaded in near future
	// HintResourceNeed() is not to be confused with resource precaching.
	virtual int				HintResourceNeed( const char *hintlist, int forgetEverything ) = 0;
	// returns true if a file is on disk
	virtual bool			IsFileImmediatelyAvailable(const char *pFileName) = 0;
	
	// copies file out of pak/bsp/steam cache onto disk (to be accessible by third-party code)
	virtual void			GetLocalCopy( const char *pFileName ) = 0;

	//--------------------------------------------------------
	// Debugging operations
	//--------------------------------------------------------

	// Dump to printf/OutputDebugString the list of files that have not been closed
	virtual void			PrintOpenedFiles( void ) = 0;
	virtual void			PrintSearchPaths( void ) = 0;

	// output
	virtual void			SetWarningFunc( void (*pfnWarning)( PRINTF_FORMAT_STRING const char *fmt, ... ) ) = 0;
	virtual void			SetWarningLevel( FileWarningLevel_t level ) = 0;
	virtual void			AddLoggingFunc( void (*pfnLogFunc)( const char *fileName, const char *accessType ) ) = 0;
	virtual void			RemoveLoggingFunc( FileSystemLoggingFunc_t logFunc ) = 0;

	// Returns the file system statistics retreived by the implementation.  Returns NULL if not supported.
	virtual const FileSystemStatistics *GetFilesystemStatistics() = 0;

	//--------------------------------------------------------
	// Start of new functions after Lost Coast release (7/05)
	//--------------------------------------------------------

	virtual FileHandle_t	OpenEx( const char *pFileName, const char *pOptions, unsigned flags = 0, const char *pathID = 0, char **ppszResolvedFilename = NULL ) = 0;

	// Extended version of read provides more context to allow for more optimal reading
	virtual int				ReadEx( void* pOutput, int sizeDest, int size, FileHandle_t file ) = 0;
	virtual int				ReadFileEx( const char *pFileName, const char *pPath, void **ppBuf, bool bNullTerminate = false, bool bOptimalAlloc = false, int nMaxBytes = 0, int nStartingByte = 0, FSAllocFunc_t pfnAlloc = NULL ) = 0;

	virtual FileNameHandle_t	FindFileName( char const *pFileName ) = 0;

#if defined( TRACK_BLOCKING_IO )
	virtual void			EnableBlockingFileAccessTracking( bool state ) = 0;
	virtual bool			IsBlockingFileAccessEnabled() const = 0;

	virtual IBlockingFileItemList *RetrieveBlockingFileAccessInfo() = 0;
#endif

	virtual void SetupPreloadData() = 0;
	virtual void DiscardPreloadData() = 0;

	// Fixme, we could do these via a string embedded into the compiled data, etc...
	enum KeyValuesPreloadType_t
	{
		TYPE_VMT,
		TYPE_SOUNDEMITTER,
		TYPE_SOUNDSCAPE,
		NUM_PRELOAD_TYPES
	};

	// If the "PreloadedData" hasn't been purged, then this'll try and instance the KeyValues using the fast path of compiled keyvalues loaded during startup.
	// Otherwise, it'll just fall through to the regular KeyValues loading routines
	virtual KeyValues	*LoadKeyValues( KeyValuesPreloadType_t type, char const *filename, char const *pPathID = 0 ) = 0;
	virtual bool		LoadKeyValues( KeyValues& head, KeyValuesPreloadType_t type, char const *filename, char const *pPathID = 0 ) = 0;

	virtual FSAsyncStatus_t	AsyncWrite(const char *pFileName, const void *pSrc, int nSrcBytes, bool bFreeMemory, bool bAppend = false, FSAsyncControl_t *pControl = NULL ) = 0;
	virtual FSAsyncStatus_t	AsyncWriteFile(const char *pFileName, const CUtlBuffer *pSrc, int nSrcBytes, bool bFreeMemory, bool bAppend = false, FSAsyncControl_t *pControl = NULL ) = 0;
	// Async read functions with memory blame
	FSAsyncStatus_t			AsyncReadCreditAlloc( const FileAsyncRequest_t &request, const char *pszFile, int line, FSAsyncControl_t *phControl = NULL )	{ return AsyncReadMultipleCreditAlloc( &request, 1, pszFile, line, phControl ); 	}
	virtual FSAsyncStatus_t	AsyncReadMultipleCreditAlloc( const FileAsyncRequest_t *pRequests, int nRequests, const char *pszFile, int line, FSAsyncControl_t *phControls = NULL ) = 0;

	virtual FSAsyncStatus_t AsyncDirectoryScan( const char* pSearchSpec, bool recurseFolders,  void* pContext, FSAsyncScanAddFunc_t pfnAdd, FSAsyncScanCompleteFunc_t pfnDone, FSAsyncControl_t *pControl = NULL ) = 0;

	virtual bool			GetFileTypeForFullPath( char const *pFullPath, OUT_Z_BYTECAP(bufSizeInBytes) wchar_t *buf, size_t bufSizeInBytes ) = 0;

	//--------------------------------------------------------
	//--------------------------------------------------------
	virtual bool		ReadToBuffer( FileHandle_t hFile, CUtlBuffer &buf, int nMaxBytes = 0, FSAllocFunc_t pfnAlloc = NULL ) = 0;

	//--------------------------------------------------------
	// Optimal IO operations
	//--------------------------------------------------------
	virtual bool		GetOptimalIOConstraints( FileHandle_t hFile, unsigned *pOffsetAlign, unsigned *pSizeAlign, unsigned *pBufferAlign ) = 0;
	inline unsigned		GetOptimalReadSize( FileHandle_t hFile, unsigned nLogicalSize );
	virtual void		*AllocOptimalReadBuffer( FileHandle_t hFile, unsigned nSize = 0, unsigned nOffset = 0 ) = 0;
	virtual void		FreeOptimalReadBuffer( void * ) = 0;

	//--------------------------------------------------------
	//
	//--------------------------------------------------------
	virtual void		BeginMapAccess() = 0;
	virtual void		EndMapAccess() = 0;

	// Returns true on success, otherwise false if it can't be resolved
	// Prefer using the FullPathToRelativePathEx_safe template wrapper to calling this directly
	virtual bool		FullPathToRelativePathEx( const char *pFullpath, const char *pPathId, OUT_Z_CAP(maxLenInChars) char *pDest, int maxLenInChars ) = 0;
	template <size_t maxLenInChars> bool FullPathToRelativePathEx_safe( const char *pFullpath, OUT_Z_ARRAY char (&pDest)[maxLenInChars] )
	{
		return FullPathToRelativePathEx( pFullpath, pDest, (int)maxLenInChars );
	}

	virtual int			GetPathIndex( const FileNameHandle_t &handle ) = 0;
	virtual long		GetPathTime( const char *pPath, const char *pPathID ) = 0;

	virtual DVDMode_t	GetDVDMode() = 0;

	//--------------------------------------------------------
	// Whitelisting for pure servers.
	//--------------------------------------------------------

	// This should be called ONCE at startup. Multiplayer games (gameinfo.txt does not contain singleplayer_only)
	// want to enable this so sv_pure works.
	virtual void			EnableWhitelistFileTracking( bool bEnable ) = 0;

	// This is called when the client connects to a server using a pure_server_whitelist.txt file.
	virtual void			RegisterFileWhitelist( IFileList *pWantCRCList, IFileList *pAllowFromDiskList, IFileList **pFilesToReload ) = 0;

	// Called when the client logs onto a server. Any files that came off disk should be marked as 
	// unverified because this server may have a different set of files it wants to guarantee.
	virtual void			MarkAllCRCsUnverified() = 0;

	// As the server loads whitelists when it transitions maps, it calls this to calculate CRCs for any files marked
	// with check_crc.   Then it calls CheckCachedFileCRC later when it gets client requests to verify CRCs.
	virtual void			CacheFileCRCs( const char *pPathname, ECacheCRCType eType, IFileList *pFilter ) = 0;
	virtual EFileCRCStatus	CheckCachedFileCRC( const char *pPathID, const char *pRelativeFilename, CRC32_t *pCRC ) = 0;

	// Fills in the list of files that have been loaded off disk and have not been verified.
	// Returns the number of files filled in (between 0 and nMaxFiles).
	//
	// This also removes any files it's returning from the unverified CRC list, so they won't be
	// returned from here again.
	// The client sends batches of these to the server to verify.
	virtual int				GetUnverifiedCRCFiles( CUnverifiedCRCFile *pFiles, int nMaxFiles ) = 0;
	
	// Control debug message output.
	// Pass a combination of WHITELIST_SPEW_ flags.
	virtual int				GetWhitelistSpewFlags() = 0;
	virtual void			SetWhitelistSpewFlags( int flags ) = 0;

	// Installs a callback used to display a dirty disk dialog
	virtual void			InstallDirtyDiskReportFunc( FSDirtyDiskReportFunc_t func ) = 0;

	virtual bool			IsLaunchedFromXboxHDD() = 0;
	virtual bool			IsInstalledToXboxHDDCache() = 0;
	virtual bool			IsDVDHosted() = 0;
	virtual bool			IsInstallAllowed() = 0;

	virtual int				GetSearchPathID( char *pPath, int nMaxLen ) = 0;
	virtual bool			FixupSearchPathsAfterInstall() = 0;
	
	virtual FSDirtyDiskReportFunc_t		GetDirtyDiskReportFunc() = 0;

	virtual void AddVPKFile( char const *pszName, SearchPathAdd_t addType = PATH_ADD_TO_TAIL ) = 0;
	virtual void RemoveVPKFile( char const *pszName ) = 0;
	virtual void GetVPKFileNames( CUtlVector<CUtlString> &destVector ) = 0;
	virtual void			RemoveAllMapSearchPaths() = 0;
	virtual void			SyncDvdDevCache() = 0;

	virtual bool			GetStringFromKVPool( CRC32_t poolKey, unsigned int key, char *pOutBuff, int buflen ) = 0;

	virtual bool			DiscoverDLC( int iController ) = 0;
	virtual int				IsAnyDLCPresent( bool *pbDLCSearchPathMounted = NULL ) = 0;
	virtual bool			GetAnyDLCInfo( int iDLC, unsigned int *pLicenseMask, wchar_t *pTitleBuff, int nOutTitleSize ) = 0;
	virtual int				IsAnyCorruptDLC() = 0;
	virtual bool			GetAnyCorruptDLCInfo( int iCorruptDLC, wchar_t *pTitleBuff, int nOutTitleSize ) = 0;
	virtual bool			AddDLCSearchPaths() = 0;
	virtual bool			IsSpecificDLCPresent( unsigned int nDLCPackage ) = 0;

	// call this to look for CPU-hogs during loading processes. When you set this, a breakpoint
	// will be issued whenever the indicated # of seconds go by without an i/o request.  Passing
	// 0.0 will turn off the functionality.
	virtual void            SetIODelayAlarm( float flThreshhold ) = 0;
}; 
