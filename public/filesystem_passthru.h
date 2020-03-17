//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef FILESYSTEM_PASSTHRU_H
#define FILESYSTEM_PASSTHRU_H
#ifdef _WIN32
#pragma once
#endif


#include "filesystem.h"
#include <stdio.h>
#include <stdarg.h>

#ifdef AsyncRead
#undef AsyncRead
#undef AsyncReadMutiple
#endif

//
// These classes pass all filesystem interface calls through to another filesystem
// interface. They can be used anytime you want to override a couple things in 
// a filesystem. VMPI uses this to override the base filesystem calls while 
// allowing the rest of the filesystem functionality to work on the master.
//

template<class Base>
class CInternalFileSystemPassThru : public Base
{
public:
	CInternalFileSystemPassThru()
	{
		m_pBaseFileSystemPassThru = NULL;
	}
	virtual void InitPassThru( IBaseFileSystem *pBaseFileSystemPassThru )
	{
		m_pBaseFileSystemPassThru = pBaseFileSystemPassThru;
	}
	virtual int				Read( void* pOutput, int size, FileHandle_t file )							{ return m_pBaseFileSystemPassThru->Read( pOutput, size, file ); }
	virtual int				Write( void const* pInput, int size, FileHandle_t file )					{ return m_pBaseFileSystemPassThru->Write( pInput, size, file ); }
	virtual FileHandle_t	Open( const char *pFileName, const char *pOptions, const char *pathID )		{ return m_pBaseFileSystemPassThru->Open( pFileName, pOptions, pathID ); }
	virtual void			Close( FileHandle_t file )													{ m_pBaseFileSystemPassThru->Close( file ); }
	virtual void			Seek( FileHandle_t file, int pos, FileSystemSeek_t seekType )				{ m_pBaseFileSystemPassThru->Seek( file, pos, seekType ); }
	virtual unsigned int	Tell( FileHandle_t file )													{ return m_pBaseFileSystemPassThru->Tell( file ); }
	virtual unsigned int	Size( FileHandle_t file )													{ return m_pBaseFileSystemPassThru->Size( file ); }
	virtual unsigned int	Size( const char *pFileName, const char *pPathID )							{ return m_pBaseFileSystemPassThru->Size( pFileName, pPathID ); }
	virtual void			Flush( FileHandle_t file )													{ m_pBaseFileSystemPassThru->Flush( file ); }
	virtual bool			Precache( const char *pFileName, const char *pPathID )						{ return m_pBaseFileSystemPassThru->Precache( pFileName, pPathID ); }
	virtual bool			FileExists( const char *pFileName, const char *pPathID )					{ return m_pBaseFileSystemPassThru->FileExists( pFileName, pPathID ); }
	virtual bool			IsFileWritable( char const *pFileName, const char *pPathID )					{ return m_pBaseFileSystemPassThru->IsFileWritable( pFileName, pPathID ); }
	virtual bool			SetFileWritable( char const *pFileName, bool writable, const char *pPathID )	{ return m_pBaseFileSystemPassThru->SetFileWritable( pFileName, writable, pPathID ); }
	virtual long			GetFileTime( const char *pFileName, const char *pPathID )						{ return m_pBaseFileSystemPassThru->GetFileTime( pFileName, pPathID ); }
	virtual bool			ReadFile( const char *pFileName, const char *pPath, CUtlBuffer &buf, int nMaxBytes = 0, int nStartingByte = 0, FSAllocFunc_t pfnAlloc = NULL ) { return m_pBaseFileSystemPassThru->ReadFile( pFileName, pPath, buf, nMaxBytes, nStartingByte, pfnAlloc  ); }
	virtual bool			WriteFile( const char *pFileName, const char *pPath, CUtlBuffer &buf )		{  return m_pBaseFileSystemPassThru->WriteFile( pFileName, pPath, buf ); }
	virtual bool			UnzipFile( const char *pFileName, const char *pPath, const char *pDestination )		{  return m_pBaseFileSystemPassThru->UnzipFile( pFileName, pPath, pDestination ); }

protected:
	IBaseFileSystem *m_pBaseFileSystemPassThru;
};


class CBaseFileSystemPassThru : public CInternalFileSystemPassThru<IBaseFileSystem>
{
public:
};


#include "compat_filesystem_passthru.h"


// This is so people who change the filesystem interface are forced to add the passthru wrapper into CFileSystemPassThru,
// so they don't break VMPI.
inline void GiveMeACompileError()
{
	CFileSystemPassThru asdf;
}


#endif // FILESYSTEM_PASSTHRU_H
