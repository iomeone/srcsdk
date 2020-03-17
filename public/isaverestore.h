//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//

#ifndef ISAVERESTORE_H
#define ISAVERESTORE_H

#include "string_t.h"
#include "datamap.h"
#include "mathlib/vmatrix.h"

#if defined( _WIN32 )
#pragma once
#endif

#ifndef CLIENT_DLL
class SINGLE_INHERITANCE CBaseEntity;
#endif

class Vector;
class VMatrix;
struct edict_t;
template< class T > class CHandle;
typedef CHandle<CBaseEntity> EHANDLE;
struct matrix3x4_t;

class CSaveRestoreData;
class CGameSaveRestoreInfo;

class ISave;
class IRestore;

//-----------------------------------------------------------------------------

#pragma pack(push,1)

struct SaveRestoreRecordHeader_t
{
	unsigned short		size;
	unsigned short		symbol;
};


#pragma pack(pop)

//-----------------------------------------------------------------------------
//
// ISaveRestoreBlockHandler
//
//-----------------------------------------------------------------------------

const int MAX_BLOCK_NAME_LEN = 31;
const int SIZE_BLOCK_NAME_BUF = 31 + 1;

//-------------------------------------

abstract_class ISaveRestoreBlockHandler
{
public:
	virtual const char *GetBlockName() = 0;

	virtual void PreSave( CSaveRestoreData * ) = 0;					// Called immediately prior to save, generally used to set up any necessary tables
	virtual void Save( ISave * ) = 0;
	virtual void WriteSaveHeaders( ISave * ) = 0;					// Called after save to allow the writing out of any dictionaries/tables/indexes generated during save
	virtual void PostSave() = 0;
	
	virtual void PreRestore() = 0;
	virtual void ReadRestoreHeaders( IRestore * ) = 0;				// Called prior to Restore()
	virtual void Restore( IRestore *, bool fCreatePlayers ) = 0;
	virtual void PostRestore() = 0;
};

//-------------------------------------

abstract_class ISaveRestoreBlockSet : public ISaveRestoreBlockHandler
{
public:
	virtual void AddBlockHandler( ISaveRestoreBlockHandler *pHandler ) = 0;
	virtual void RemoveBlockHandler( ISaveRestoreBlockHandler *pHandler ) = 0;
	virtual void CallBlockHandlerRestore( ISaveRestoreBlockHandler *pHandler, int baseFilePos, IRestore *pRestore, bool fCreatePlayers ) = 0;
};

extern ISaveRestoreBlockSet *g_pGameSaveRestoreBlockSet;

//-------------------------------------

abstract_class CDefSaveRestoreBlockHandler : public ISaveRestoreBlockHandler
{
	virtual const char *GetBlockName() = 0;

	virtual void PreSave( CSaveRestoreData * ) {}
	virtual void Save( ISave * ) {}
	virtual void WriteSaveHeaders( ISave * ) {}
	virtual void PostSave() {}
	
	virtual void PreRestore() {}
	virtual void ReadRestoreHeaders( IRestore * ) {}
	virtual void Restore( IRestore *, bool fCreatePlayers ) {}
	virtual void PostRestore() {}
};

//-----------------------------------------------------------------------------
// Purpose: The operations necessary to save and restore custom types (FIELD_CUSTOM)
//
//

struct SaveRestoreFieldInfo_t
{
	void *			   pField;

	// Note that it is legal for the following two fields to be NULL,
	// though it may be disallowed by implementors of ISaveRestoreOps
	void *			   pOwner;
	typedescription_t *pTypeDesc;
};

abstract_class ISaveRestoreOps
{
public:
	// save data type interface
	virtual void Save( const SaveRestoreFieldInfo_t &fieldInfo, ISave *pSave ) = 0;
	virtual void Restore( const SaveRestoreFieldInfo_t &fieldInfo, IRestore *pRestore ) = 0;

	virtual bool IsEmpty( const SaveRestoreFieldInfo_t &fieldInfo ) = 0;
	virtual void MakeEmpty( const SaveRestoreFieldInfo_t &fieldInfo ) = 0;
	virtual bool Parse( const SaveRestoreFieldInfo_t &fieldInfo, char const* szValue ) = 0;

	//---------------------------------

	void Save( void *pField, ISave *pSave )				{ SaveRestoreFieldInfo_t fieldInfo = { pField, NULL, NULL }; Save( fieldInfo, pSave ); }
	void Restore( void *pField, IRestore *pRestore )	{ SaveRestoreFieldInfo_t fieldInfo = { pField, NULL, NULL }; Restore( fieldInfo, pRestore ); }

	bool IsEmpty( void *pField)							{ SaveRestoreFieldInfo_t fieldInfo = { pField, NULL, NULL }; return IsEmpty( fieldInfo ); }
	void MakeEmpty( void *pField)						{ SaveRestoreFieldInfo_t fieldInfo = { pField, NULL, NULL }; MakeEmpty( fieldInfo ); }
	bool Parse( void *pField, char const *pszValue )	{ SaveRestoreFieldInfo_t fieldInfo = { pField, NULL, NULL }; return Parse( fieldInfo, pszValue ); }
};

//-------------------------------------

class CDefSaveRestoreOps : public ISaveRestoreOps
{
public:
	// save data type interface
	virtual void Save( const SaveRestoreFieldInfo_t &fieldInfo, ISave *pSave ) {}
	virtual void Restore( const SaveRestoreFieldInfo_t &fieldInfo, IRestore *pRestore ) {}

	virtual bool IsEmpty( const SaveRestoreFieldInfo_t &fieldInfo ) { return false; }
	virtual void MakeEmpty( const SaveRestoreFieldInfo_t &fieldInfo ) {}
	virtual bool Parse( const SaveRestoreFieldInfo_t &fieldInfo, char const* szValue ) { return false; }
};


//-----------------------------------------------------------------------------
// Used by ops that deal with pointers
//-----------------------------------------------------------------------------
class CClassPtrSaveRestoreOps : public CDefSaveRestoreOps
{
public:
	virtual bool IsEmpty( const SaveRestoreFieldInfo_t &fieldInfo )
	{
		void **ppClassPtr = (void **)fieldInfo.pField;
		int nObjects = fieldInfo.pTypeDesc->fieldSize;
		for ( int i = 0; i < nObjects; i++ )
		{
			if ( ppClassPtr[i] != NULL )
				return false;
		}
		return true;
	}

	virtual void MakeEmpty( const SaveRestoreFieldInfo_t &fieldInfo )
	{
		memset( fieldInfo.pField, 0, fieldInfo.pTypeDesc->fieldSize * sizeof( void * ) );
	}
};

#include "compat_isaverestore.h"

//=============================================================================

#endif // ISAVERESTORE_H
