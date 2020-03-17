//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//

#ifndef FILESYSTEM_H
#define FILESYSTEM_H
#pragma once

#include <limits.h>

#include "tier0/threadtools.h"
#include "tier0/memalloc.h"
#include "tier1/interface.h"
#include "tier1/utlsymbol.h"
#include "tier1/utlstring.h"
#include "appframework/IAppSystem.h"
#include "tier1/checksum_crc.h"
#include "tier1/checksum_md5.h"
#include "tier1/refcount.h"

#ifdef _WIN32
#pragma once
#endif

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------

class CUtlBuffer;
class KeyValues;
class IFileList;
class IThreadPool;
class CMemoryFileBacking;

typedef void * FileHandle_t;
typedef void * FileCacheHandle_t;
typedef int FileFindHandle_t;
typedef void (*FileSystemLoggingFunc_t)( const char *fileName, const char *accessType );
typedef int WaitForResourcesHandle_t;

#ifdef _X360
typedef void* HANDLE;
#endif

#define USE_CRC_FILE_TRACKING 0

// Turn on some extra pure server debug spew in certain builds.
// WARNING: This spew can be used by hackers to locate places to hack
// the code to bypas sv_pure!  Be careful!
#if defined( _DEBUG ) || defined( STAGING_ONLY )
	#define PURE_SERVER_DEBUG_SPEW
#endif

/// How strict will the pure server be for a particular set of files
enum EPureServerFileClass
{
	ePureServerFileClass_Unknown = -1, // dummy debugging value
	ePureServerFileClass_Any = 0,
	ePureServerFileClass_AnyTrusted,
	ePureServerFileClass_CheckHash,
};

class IPureServerWhitelist
{
public:

	// Reference counting
	virtual void AddRef() = 0;
	virtual void Release() = 0;

	// What should we do with a particular file?
	virtual EPureServerFileClass GetFileClass( const char *pszFilename ) = 0;

	// Access list of trusted keys which we will allow to set trusted content
	virtual int GetTrustedKeyCount() const = 0;
	virtual const byte *GetTrustedKey( int iKeyIndex, int *nKeySize ) const = 0;
};

//-----------------------------------------------------------------------------
// Enums used by the interface
//-----------------------------------------------------------------------------

#define FILESYSTEM_MAX_SEARCH_PATHS 128

enum FileSystemSeek_t
{
	FILESYSTEM_SEEK_HEAD	= SEEK_SET,
	FILESYSTEM_SEEK_CURRENT = SEEK_CUR,
	FILESYSTEM_SEEK_TAIL	= SEEK_END,
};

enum
{
	FILESYSTEM_INVALID_FIND_HANDLE = -1
};

enum FileWarningLevel_t
{
	// A problem!
	FILESYSTEM_WARNING = -1,

	// Don't print anything
	FILESYSTEM_WARNING_QUIET = 0,

	// On shutdown, report names of files left unclosed
	FILESYSTEM_WARNING_REPORTUNCLOSED,

	// Report number of times a file was opened, closed
	FILESYSTEM_WARNING_REPORTUSAGE,

	// Report all open/close events to console ( !slow! )
	FILESYSTEM_WARNING_REPORTALLACCESSES,

	// Report all open/close/read events to the console ( !slower! )
	FILESYSTEM_WARNING_REPORTALLACCESSES_READ,

	// Report all open/close/read/write events to the console ( !slower! )
	FILESYSTEM_WARNING_REPORTALLACCESSES_READWRITE,

	// Report all open/close/read/write events and all async I/O file events to the console ( !slower(est)! )
	FILESYSTEM_WARNING_REPORTALLACCESSES_ASYNC,

};

// search path filtering
enum PathTypeFilter_t
{
	FILTER_NONE        = 0,	// no filtering, all search path types match
	FILTER_CULLPACK    = 1,	// pack based search paths are culled (maps and zips)
	FILTER_CULLNONPACK = 2,	// non-pack based search paths are culled
};

// search path querying (bit flags)
enum
{
	PATH_IS_NORMAL      = 0x00, // normal path, not pack based
	PATH_IS_PACKFILE    = 0x01, // path is a pack file
	PATH_IS_MAPPACKFILE = 0x02, // path is a map pack file
	PATH_IS_REMOTE		= 0x04, // path is the remote filesystem
	PATH_IS_DVDDEV		= 0x04, // path is the dvddev cache
};
typedef uint32 PathTypeQuery_t;

#define IS_PACKFILE( n ) ( n & ( PATH_IS_PACKFILE | PATH_IS_MAPPACKFILE ) )
#define IS_REMOTE( n )   ( n & PATH_IS_REMOTE )
#define IS_DVDDEV( n )   ( n & PATH_IS_DVDDEV )

enum DVDMode_t
{
	DVDMODE_OFF    = 0, // not using dvd
	DVDMODE_STRICT = 1, // dvd device only
	DVDMODE_DEV    = 2, // dev mode, mutiple devices ok
	DVDMODE_DEV_VISTA = 3, // dev mode from a vista host, mutiple devices ok
};

// In non-retail builds, enable the file blocking access tracking stuff...
#if defined( TRACK_BLOCKING_IO )
enum FileBlockingWarning_t
{
	// Report how long synchronous i/o took to complete
	FILESYSTEM_BLOCKING_SYNCHRONOUS = 0,
	// Report how long async i/o took to complete if AsyncFileFinished caused it to load via "blocking" i/o
	FILESYSTEM_BLOCKING_ASYNCHRONOUS_BLOCK,
	// Report how long async i/o took to complete
	FILESYSTEM_BLOCKING_ASYNCHRONOUS,
	// Report how long the async "callback" took
	FILESYSTEM_BLOCKING_CALLBACKTIMING,

	FILESYSTEM_BLOCKING_NUMBINS,
};

#pragma pack(1)
class FileBlockingItem
{
public:
	enum
	{
		FB_ACCESS_OPEN = 1,
		FB_ACCESS_CLOSE = 2,
		FB_ACCESS_READ = 3,
		FB_ACCESS_WRITE = 4,
		FB_ACCESS_APPEND = 5,
		FB_ACCESS_SIZE = 6
	};

	FileBlockingItem() :
		m_ItemType( (FileBlockingWarning_t)0 ),
		m_flElapsed( 0.0f ),
		m_nAccessType( 0 )
	{
		SetFileName( NULL );
	}

	FileBlockingItem( int type, char const *filename, float elapsed, int accessType ) :
		m_ItemType( (FileBlockingWarning_t)type ),
		m_flElapsed( elapsed ),
		m_nAccessType( accessType )
	{
		SetFileName( filename );
	}

	void SetFileName( char const *filename )
	{
		if ( !filename )
		{
			m_szFilename[ 0 ] = 0;
			return;
		}

		int len = Q_strlen( filename );
		if ( len >= sizeof( m_szFilename ) )
		{
			Q_strncpy( m_szFilename, &filename[ len - sizeof( m_szFilename ) + 1 ], sizeof( m_szFilename ) );
		}
		else
		{
			Q_strncpy( m_szFilename, filename, sizeof( m_szFilename ) );
		}
	}

	char const *GetFileName() const
	{
		return m_szFilename;
	}

	FileBlockingWarning_t	m_ItemType;
	float					m_flElapsed;
	byte					m_nAccessType;
private:

	char					m_szFilename[ 32 ];
};
#pragma pack()

class IBlockingFileItemList
{
public:
	
	// You can't call any of the below calls without locking first
	virtual void	LockMutex() = 0;
	virtual void	UnlockMutex() = 0;

	virtual int		First() const = 0;
	virtual int		Next( int i ) const = 0;
	virtual int		InvalidIndex() const = 0;

	virtual const	FileBlockingItem& Get( int index ) const = 0;

	virtual void	Reset() = 0;
};

#endif // TRACK_BLOCKING_IO

enum FilesystemMountRetval_t
{
	FILESYSTEM_MOUNT_OK = 0,
	FILESYSTEM_MOUNT_FAILED,
};

enum SearchPathAdd_t
{
	PATH_ADD_TO_HEAD,		// First path searched
	PATH_ADD_TO_TAIL,		// Last path searched
	PATH_ADD_TO_TAIL_ATINDEX,	// First path searched
};

enum FilesystemOpenExFlags_t
{
	FSOPEN_UNBUFFERED		= (1 << 0),
	FSOPEN_FORCE_TRACK_CRC	= (1 << 1),		// This makes it calculate a CRC for the file (if the file came from disk) regardless 
											// of the IFileList passed to RegisterFileWhitelist.
	FSOPEN_NEVERINPACK	    = (1 << 2),		// 360 only, hint to FS that file is not allowed to be in pack file
};

#define FILESYSTEM_INVALID_HANDLE	( FileHandle_t )0

//-----------------------------------------------------------------------------
// Structures used by the interface
//-----------------------------------------------------------------------------

struct FileSystemStatistics
{
	CInterlockedUInt	nReads,		
						nWrites,		
						nBytesRead,
						nBytesWritten,
						nSeeks;
};

//-----------------------------------------------------------------------------
// File system allocation functions. Client must free on failure
//-----------------------------------------------------------------------------
typedef void *(*FSAllocFunc_t)( const char *pszFilename, unsigned nBytes );


//-----------------------------------------------------------------------------
// Used to display dirty disk error functions
//-----------------------------------------------------------------------------
typedef void (*FSDirtyDiskReportFunc_t)();


//-----------------------------------------------------------------------------
// Asynchronous support types
//-----------------------------------------------------------------------------
DECLARE_POINTER_HANDLE(FSAsyncControl_t);
DECLARE_POINTER_HANDLE(FSAsyncFile_t);
const FSAsyncFile_t FS_INVALID_ASYNC_FILE = (FSAsyncFile_t)(0x0000ffff);


//---------------------------------------------------------
// Async file status
//---------------------------------------------------------
enum FSAsyncStatus_t
{
	FSASYNC_ERR_NOT_MINE     = -8,	// Filename not part of the specified file system, try a different one.  (Used internally to find the right filesystem)
	FSASYNC_ERR_RETRY_LATER  = -7,	// Failure for a reason that might be temporary.  You might retry, but not immediately.  (E.g. Network problems)
	FSASYNC_ERR_ALIGNMENT    = -6,	// read parameters invalid for unbuffered IO
	FSASYNC_ERR_FAILURE      = -5,	// hard subsystem failure
	FSASYNC_ERR_READING      = -4,	// read error on file
	FSASYNC_ERR_NOMEMORY     = -3,	// out of memory for file read
	FSASYNC_ERR_UNKNOWNID    = -2,	// caller's provided id is not recognized
	FSASYNC_ERR_FILEOPEN     = -1,	// filename could not be opened (bad path, not exist, etc)
	FSASYNC_OK               = 0,	// operation is successful
	FSASYNC_STATUS_PENDING,			// file is properly queued, waiting for service
	FSASYNC_STATUS_INPROGRESS,		// file is being accessed
	FSASYNC_STATUS_ABORTED,			// file was aborted by caller
	FSASYNC_STATUS_UNSERVICED,		// file is not yet queued
};

//---------------------------------------------------------
// Async request flags
//---------------------------------------------------------
enum FSAsyncFlags_t
{
	FSASYNC_FLAGS_ALLOCNOFREE		= ( 1 << 0 ),	// do the allocation for dataPtr, but don't free
	FSASYNC_FLAGS_FREEDATAPTR		= ( 1 << 1 ),	// free the memory for the dataPtr post callback
	FSASYNC_FLAGS_SYNC				= ( 1 << 2 ),	// Actually perform the operation synchronously. Used to simplify client code paths
	FSASYNC_FLAGS_NULLTERMINATE		= ( 1 << 3 ),	// allocate an extra byte and null terminate the buffer read in
};

//---------------------------------------------------------
// Return value for CheckFileCRC.
//---------------------------------------------------------
enum EFileCRCStatus
{
	k_eFileCRCStatus_CantOpenFile,		// We don't have this file. 
	k_eFileCRCStatus_GotCRC,
	k_eFileCRCStatus_FileInVPK
};

// Used in CacheFileCRCs.
enum ECacheCRCType
{
	k_eCacheCRCType_SingleFile,
	k_eCacheCRCType_Directory,
	k_eCacheCRCType_Directory_Recursive
};

//---------------------------------------------------------
// Optional completion callback for each async file serviced (or failed)
// call is not reentrant, async i/o guaranteed suspended until return
// Note: If you change the signature of the callback, you will have to account for it in FileSystemV12 (toml [4/18/2005] )
//---------------------------------------------------------
struct FileAsyncRequest_t;
typedef void (*FSAsyncCallbackFunc_t)(const FileAsyncRequest_t &request, int nBytesRead, FSAsyncStatus_t err);

//-----------------------------------------------------------------------------
// Used to add results from async directory scans
//-----------------------------------------------------------------------------
typedef void (*FSAsyncScanAddFunc_t)( void* pContext, char* pFoundPath, char* pFoundFile );
typedef void (*FSAsyncScanCompleteFunc_t)( void* pContext, FSAsyncStatus_t err );

//---------------------------------------------------------
// Description of an async request
//---------------------------------------------------------
struct FileAsyncRequest_t
{
	FileAsyncRequest_t()	{ memset( this, 0, sizeof(*this) ); hSpecificAsyncFile = FS_INVALID_ASYNC_FILE;	}
	const char *			pszFilename;		// file system name
	void *					pData;				// optional, system will alloc/free if NULL
	int						nOffset;			// optional initial seek_set, 0=beginning
	int						nBytes;				// optional read clamp, -1=exist test, 0=full read
	FSAsyncCallbackFunc_t	pfnCallback;		// optional completion callback
	void *					pContext;			// caller's unique file identifier
	int						priority;			// inter list priority, 0=lowest
	unsigned				flags;				// behavior modifier
	const char *			pszPathID;			// path ID (NOTE: this field is here to remain binary compatible with release HL2 filesystem interface)
	FSAsyncFile_t			hSpecificAsyncFile; // Optional hint obtained using AsyncBeginRead()
	FSAllocFunc_t			pfnAlloc;			// custom allocator. can be null. not compatible with FSASYNC_FLAGS_FREEDATAPTR
};


struct FileHash_t 
{
	enum EFileHashType_t
	{
		k_EFileHashTypeUnknown			= 0,
		k_EFileHashTypeEntireFile		= 1,
		k_EFileHashTypeIncompleteFile	= 2,
	};
	FileHash_t()
	{
		m_eFileHashType = FileHash_t::k_EFileHashTypeUnknown;
		m_cbFileLen = 0;
		m_PackFileID = 0;
		m_nPackFileNumber = 0;
	}
	int m_eFileHashType;
	CRC32_t m_crcIOSequence;
	MD5Value_t m_md5contents;
	int m_cbFileLen;
	int m_PackFileID;
	int m_nPackFileNumber;

	bool operator==( const FileHash_t &src ) const
	{
		return m_crcIOSequence == src.m_crcIOSequence && 
			m_md5contents == src.m_md5contents && 
			m_eFileHashType == src.m_eFileHashType;
	}
	bool operator!=( const FileHash_t &src ) const
	{
		return m_crcIOSequence != src.m_crcIOSequence || 
			m_md5contents != src.m_md5contents || 
			m_eFileHashType != src.m_eFileHashType;
	}

};

class CUnverifiedFileHash
{
public:
	char m_PathID[MAX_PATH];
	char m_Filename[MAX_PATH];
	int m_nFileFraction;
	FileHash_t m_FileHash;
};

class CUnverifiedCRCFile
{
public:
	char m_PathID[MAX_PATH];
	char m_Filename[MAX_PATH];
	CRC32_t m_CRC;
};

class CUnverifiedMD5File
{
public:
	char m_PathID[MAX_PATH];
	char m_Filename[MAX_PATH];
	unsigned char bits[MD5_DIGEST_LENGTH];
};

// Spew flags for SetWhitelistSpewFlags (set with the fs_whitelist_spew_flags cvar).
// Update the comment for the fs_whitelist_spew_flags cvar if you change these.
#define WHITELIST_SPEW_WHILE_LOADING		0x0001	// list files as they are added to the CRC tracker
#define WHITELIST_SPEW_RELOAD_FILES			0x0002	// show files the filesystem is telling the engine to reload
#define WHITELIST_SPEW_DONT_RELOAD_FILES	0x0004	// show files the filesystem is NOT telling the engine to reload

//-----------------------------------------------------------------------------
// Interface to fetch a file asynchronously from any source.  This is used
// as a hook
//-----------------------------------------------------------------------------

abstract_class IAsyncFileFetch {
public:
	typedef void *Handle;

	/// Initiate a request.  Returns error status, or on success
	/// returns an opaque handle used to terminate the job
	///
	/// Should return FSASYNC_ERR_NOT_MINE if the filename isn't
	/// handled by this interface
	///
	/// The callback is required, and is the only mechanism to communicate
	/// status.  (No polling.)  The request is automatically destroyed anytime
	/// after the callback is executed.
	virtual FSAsyncStatus_t Start( const FileAsyncRequest_t &request, Handle *pOutHandle, IThreadPool *pThreadPool ) = 0;

	/// Attempt to complete any active work, returning status.  The callback WILL
	/// be executed (this is necessary in case we allocated the buffer).
	/// Afterwards, the request is automatically destroyed.
	virtual FSAsyncStatus_t FinishSynchronous( Handle hControl ) = 0;

	/// Terminate any active work and destroy all resources and bookkeeping info.
	/// The callback will NOT be executed.
	virtual FSAsyncStatus_t Abort( Handle hControl ) = 0;
};

// This interface is for VPK files to communicate with FileTracker
abstract_class IThreadedFileMD5Processor
{
public:
	virtual int				SubmitThreadedMD5Request( uint8 *pubBuffer, int cubBuffer, int PackFileID, int nPackFileNumber, int nPackFileFraction ) = 0;
	virtual bool			BlockUntilMD5RequestComplete( int iRequest, MD5Value_t *pMd5ValueOut ) = 0;
	virtual bool			IsMD5RequestComplete( int iRequest, MD5Value_t *pMd5ValueOut ) = 0;
};

// DLC license mask flags is 32 publisher defined bits
// MSW 16 bits in 8.8: Type.SubVersion
// LSW 16 bits: Flags

// return id component
#define DLC_LICENSE_ID( x )				( ( ( (unsigned int)( x ) ) >> 24 ) & 0x000000FF )
// returns minor version component (not generally used, i.e. we dont rev dlc's yet)
#define DLC_LICENSE_MINORVERSION( x )	( ( ( (unsigned int)( x ) ) >> 16 ) & 0x000000FF )
// returns license flags
#define DLC_LICENSE_FLAGS( x )			( ( ( (unsigned int)( x ) ) & 0x0000FFFF ) )

#define DLCFLAGS_PRESENCE_ONLY			0x0001	// causes no search path loadout

//-----------------------------------------------------------------------------
// Base file system interface
//-----------------------------------------------------------------------------

// This is the minimal interface that can be implemented to provide access to
// a named set of files.
#define BASEFILESYSTEM_INTERFACE_VERSION		"VBaseFileSystem011"

abstract_class IBaseFileSystem
{
public:
	virtual int				Read( void* pOutput, int size, FileHandle_t file ) = 0;
	virtual int				Write( void const* pInput, int size, FileHandle_t file ) = 0;

	// if pathID is NULL, all paths will be searched for the file
	virtual FileHandle_t	Open( const char *pFileName, const char *pOptions, const char *pathID = 0 ) = 0;
	virtual void			Close( FileHandle_t file ) = 0;


	virtual void			Seek( FileHandle_t file, int pos, FileSystemSeek_t seekType ) = 0;
	virtual unsigned int	Tell( FileHandle_t file ) = 0;
	virtual unsigned int	Size( FileHandle_t file ) = 0;
	virtual unsigned int	Size( const char *pFileName, const char *pPathID = 0 ) = 0;

	virtual void			Flush( FileHandle_t file ) = 0;
	virtual bool			Precache( const char *pFileName, const char *pPathID = 0 ) = 0;

	virtual bool			FileExists( const char *pFileName, const char *pPathID = 0 ) = 0;
	virtual bool			IsFileWritable( char const *pFileName, const char *pPathID = 0 ) = 0;
	virtual bool			SetFileWritable( char const *pFileName, bool writable, const char *pPathID = 0 ) = 0;

	virtual long			GetFileTime( const char *pFileName, const char *pPathID = 0 ) = 0;

	//--------------------------------------------------------
	// Reads/writes files to utlbuffers. Use this for optimal read performance when doing open/read/close
	//--------------------------------------------------------
	virtual bool			ReadFile( const char *pFileName, const char *pPath, CUtlBuffer &buf, int nMaxBytes = 0, int nStartingByte = 0, FSAllocFunc_t pfnAlloc = NULL ) = 0;
	virtual bool			WriteFile( const char *pFileName, const char *pPath, CUtlBuffer &buf ) = 0;
	virtual bool			UnzipFile( const char *pFileName, const char *pPath, const char *pDestination ) = 0;
};


//-----------------------------------------------------------------------------
// Main file system interface
//-----------------------------------------------------------------------------

#include "compat_filesystem.h"

//-----------------------------------------------------------------------------
// Memory file backing, which you can use to fake out the filesystem, caching data
// in memory and have it associated with a file
//-----------------------------------------------------------------------------
class CMemoryFileBacking : public CRefCounted<CRefCountServiceMT>
{
public:
	CMemoryFileBacking( IFileSystem* pFS ) : m_pFS( pFS ), m_nRegistered( 0 ), m_pFileName( NULL ), m_pData( NULL ), m_nLength( 0 ) { }
	~CMemoryFileBacking() { free( (char*) m_pFileName ); if ( m_pData ) m_pFS->FreeOptimalReadBuffer( (char*) m_pData ); }

	IFileSystem* m_pFS;
	int m_nRegistered;
	const char* m_pFileName;
	const char* m_pData;
	int m_nLength;

private:
	CMemoryFileBacking( const CMemoryFileBacking& ); // not defined
	CMemoryFileBacking& operator=( const CMemoryFileBacking& ); // not defined
};

//-----------------------------------------------------------------------------

#define SetLastProfileFileRead( s ) ((void)0)
#define GetLastProfileFileRead() NULL

#define DISK_INTENSIVE() ((void)0)

//-----------------------------------------------------------------------------

inline unsigned IFileSystem::GetOptimalReadSize( FileHandle_t hFile, unsigned nLogicalSize ) 
{ 
	unsigned align; 
	if ( GetOptimalIOConstraints( hFile, &align, NULL, NULL ) ) 
		return AlignValue( nLogicalSize, align );
	else
		return nLogicalSize;
}

//-----------------------------------------------------------------------------

// We include this here so it'll catch compile errors in VMPI early.
#include "filesystem_passthru.h"

//-----------------------------------------------------------------------------
// Async memory tracking
//-----------------------------------------------------------------------------

#if (defined(_DEBUG) || defined(USE_MEM_DEBUG))
#define AsyncRead( a, b ) AsyncReadCreditAlloc( a, __FILE__, __LINE__, b )
#define AsyncReadMutiple( a, b, c ) AsyncReadMultipleCreditAlloc( a, b, __FILE__, __LINE__, c )
#endif

extern IFileSystem *g_pFullFileSystem;

#endif // FILESYSTEM_H
