//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef DATAMAP_H
#define DATAMAP_H
#ifdef _WIN32
#pragma once
#endif

#ifndef VECTOR_H
#include "mathlib/vector.h"
#endif

#include "tier1/utlvector.h"

#include "tier0/memdbgon.h"

// SINGLE_INHERITANCE restricts the size of CBaseEntity pointers-to-member-functions to 4 bytes
class SINGLE_INHERITANCE CBaseEntity;
struct inputdata_t;

#define INVALID_TIME (FLT_MAX * -1.0) // Special value not rebased on save/load

// See predictioncopy.h for implementation and notes
struct optimized_datamap_t;
struct datamap_t;
class ISaveRestoreOps;

//
// Function prototype for all input handlers.
//
typedef void (CBaseEntity::*inputfunc_t)(inputdata_t &data);

enum
{
	TD_OFFSET_NORMAL = 0,
	TD_OFFSET_PACKED = 1,

	// Must be last
	TD_OFFSET_COUNT,
};

using typedesc_offset_t = int[TD_OFFSET_COUNT];

#include "compat_datamap.h"


//-----------------------------------------------------------------------------
// Field sizes... 
//-----------------------------------------------------------------------------
template <int FIELD_TYPE>
class CDatamapFieldSizeDeducer
{
public:
	enum
	{
		SIZE = 0
	};

	static int FieldSize( )
	{
		return 0;
	}
};

#define DECLARE_FIELD_SIZE( _fieldType, _fieldSize )	\
	template< > class CDatamapFieldSizeDeducer<_fieldType> { public: enum { SIZE = _fieldSize }; static int FieldSize() { return _fieldSize; } };
#define FIELD_SIZE( _fieldType )	CDatamapFieldSizeDeducer<_fieldType>::SIZE
#define FIELD_BITS( _fieldType )	(FIELD_SIZE( _fieldType ) * 8)

DECLARE_FIELD_SIZE( FIELD_FLOAT,		sizeof(float) )
DECLARE_FIELD_SIZE( FIELD_STRING,		sizeof(int) )
DECLARE_FIELD_SIZE( FIELD_VECTOR,		3 * sizeof(float) )
DECLARE_FIELD_SIZE( FIELD_VECTOR2D,		2 * sizeof(float) )
DECLARE_FIELD_SIZE( FIELD_VECTOR4D,		4 * sizeof( float ) )
DECLARE_FIELD_SIZE( FIELD_QUATERNION,	4 * sizeof(float))
DECLARE_FIELD_SIZE( FIELD_INTEGER,		sizeof(int))
DECLARE_FIELD_SIZE( FIELD_INTEGER64,	sizeof(int64))
DECLARE_FIELD_SIZE( FIELD_BOOLEAN,		sizeof(char))
DECLARE_FIELD_SIZE( FIELD_SHORT,		sizeof(short))
DECLARE_FIELD_SIZE( FIELD_CHARACTER,	sizeof(char))
DECLARE_FIELD_SIZE( FIELD_COLOR32,		sizeof(int))
DECLARE_FIELD_SIZE( FIELD_CLASSPTR,		sizeof(int))
DECLARE_FIELD_SIZE( FIELD_EHANDLE,		sizeof(int))
DECLARE_FIELD_SIZE( FIELD_EDICT,		sizeof(int))
DECLARE_FIELD_SIZE( FIELD_POSITION_VECTOR, 	3 * sizeof(float))
DECLARE_FIELD_SIZE( FIELD_TIME,			sizeof(float))
DECLARE_FIELD_SIZE( FIELD_TICK,			sizeof(int))
DECLARE_FIELD_SIZE( FIELD_MODELNAME,	sizeof(int))
DECLARE_FIELD_SIZE( FIELD_SOUNDNAME,	sizeof(int))
DECLARE_FIELD_SIZE( FIELD_INPUT,		sizeof(int))
#ifdef POSIX
// pointer to members under gnuc are 8bytes if you have a virtual func
DECLARE_FIELD_SIZE( FIELD_FUNCTION,		sizeof(uint64))
#else
DECLARE_FIELD_SIZE( FIELD_FUNCTION,		sizeof(int *))
#endif
DECLARE_FIELD_SIZE( FIELD_VMATRIX,		16 * sizeof(float))
DECLARE_FIELD_SIZE( FIELD_VMATRIX_WORLDSPACE,	16 * sizeof(float))
DECLARE_FIELD_SIZE( FIELD_MATRIX3X4_WORLDSPACE,	12 * sizeof(float))
DECLARE_FIELD_SIZE( FIELD_INTERVAL,		2 * sizeof( float) )  // NOTE:  Must match interval.h definition
DECLARE_FIELD_SIZE( FIELD_MODELINDEX,	sizeof(int) )
DECLARE_FIELD_SIZE( FIELD_MATERIALINDEX,	sizeof(int) )


#define ARRAYSIZE2D(p)		(sizeof(p)/sizeof(p[0][0]))
#define SIZE_OF_ARRAY(p)	_ARRAYSIZE(p)

#define _FIELD(name,fieldtype,count,flags,mapname,tolerance)		typedescription_t{ fieldtype, #name, typedesc_offset_t{ offsetof(classNameTypedef, name), 0 }, count, flags, mapname, NULL, NULL, NULL, sizeof( ((classNameTypedef *)0)->name ), NULL, 0, tolerance }
#define DEFINE_FIELD_NULL	typedescription_t{ FIELD_VOID,0, typedesc_offset_t{0,0},0,0,0,0,0,0}
#define DEFINE_FIELD(name,fieldtype)			_FIELD(name, fieldtype, 1,  FTYPEDESC_SAVE, NULL, 0 )
#define DEFINE_FIELD_NOT_SAVED(name,fieldtype)			_FIELD(name, fieldtype, 1, 0, NULL, 0 )
#define DEFINE_KEYFIELD(name,fieldtype, mapname) _FIELD(name, fieldtype, 1,  FTYPEDESC_KEY | FTYPEDESC_SAVE, mapname, 0 )
#define DEFINE_KEYFIELD_NOT_SAVED(name,fieldtype, mapname)_FIELD(name, fieldtype, 1,  FTYPEDESC_KEY, mapname, 0 )
#define DEFINE_AUTO_ARRAY(name,fieldtype)		_FIELD(name, fieldtype, SIZE_OF_ARRAY(((classNameTypedef *)0)->name), FTYPEDESC_SAVE, NULL, 0 )
#define DEFINE_AUTO_ARRAY_KEYFIELD(name,fieldtype,mapname)		_FIELD(name, fieldtype, SIZE_OF_ARRAY(((classNameTypedef *)0)->name), FTYPEDESC_SAVE, mapname, 0 )
#define DEFINE_ARRAY(name,fieldtype, count)		_FIELD(name, fieldtype, count, FTYPEDESC_SAVE, NULL, 0 )
#define DEFINE_ARRAY_NOT_SAVED(name,fieldtype, count)		_FIELD(name, fieldtype, count, 0, NULL, 0 )
#define DEFINE_ENTITY_FIELD(name,fieldtype)			_FIELD(edict_t, name, fieldtype, 1,  FTYPEDESC_KEY | FTYPEDESC_SAVE, #name, 0 )
#define DEFINE_ENTITY_GLOBAL_FIELD(name,fieldtype)	_FIELD(edict_t, name, fieldtype, 1,  FTYPEDESC_KEY | FTYPEDESC_SAVE | FTYPEDESC_GLOBAL, #name, 0 )
#define DEFINE_GLOBAL_FIELD(name,fieldtype)	_FIELD(name, fieldtype, 1,  FTYPEDESC_GLOBAL | FTYPEDESC_SAVE, NULL, 0 )
#define DEFINE_GLOBAL_KEYFIELD(name,fieldtype, mapname)	_FIELD(name, fieldtype, 1,  FTYPEDESC_GLOBAL | FTYPEDESC_KEY | FTYPEDESC_SAVE, mapname, 0 )
#define DEFINE_CUSTOM_FIELD(name,datafuncs)	typedescription_t{ FIELD_CUSTOM, #name, typedesc_offset_t{ offsetof(classNameTypedef, name), 0 }, 1, FTYPEDESC_SAVE, NULL, datafuncs, NULL }
#define DEFINE_CUSTOM_KEYFIELD(name,datafuncs,mapname)	typedescription_t{ FIELD_CUSTOM, #name, typedesc_offset_t{ offsetof(classNameTypedef, name), 0 }, 1, FTYPEDESC_SAVE | FTYPEDESC_KEY, mapname, datafuncs, NULL }
#define DEFINE_AUTO_ARRAY2D(name,fieldtype)		_FIELD(name, fieldtype, ARRAYSIZE2D(((classNameTypedef *)0)->name), FTYPEDESC_SAVE, NULL, 0 )
// Used by byteswap datadescs
#define DEFINE_BITFIELD(name,fieldtype,bitcount)	DEFINE_ARRAY(name,fieldtype,((bitcount+FIELD_BITS(fieldtype)-1)&~(FIELD_BITS(fieldtype)-1)) / FIELD_BITS(fieldtype) )
#define DEFINE_INDEX(name,fieldtype)			_FIELD(name, fieldtype, 1,  FTYPEDESC_INDEX, NULL, 0 )

#define DEFINE_EMBEDDED( name )						\
	typedescription_t{ FIELD_EMBEDDED, #name, typedesc_offset_t{ offsetof(classNameTypedef, name), 0 }, 1, FTYPEDESC_SAVE, NULL, NULL, NULL, &(((classNameTypedef *)0)->name.m_DataMap), sizeof( ((classNameTypedef *)0)->name ), NULL, 0, 0.0f }

#define DEFINE_EMBEDDED_OVERRIDE( name, overridetype )	\
	typedescription_t{ FIELD_EMBEDDED, #name, typedesc_offset_t{ offsetof(classNameTypedef, name), 0 }, 1, FTYPEDESC_SAVE, NULL, NULL, NULL, &((overridetype *)0)->m_DataMap, sizeof( ((classNameTypedef *)0)->name ), NULL, 0, 0.0f }

#define DEFINE_EMBEDDEDBYREF( name )					\
	typedescription_t{ FIELD_EMBEDDED, #name, typedesc_offset_t{ offsetof(classNameTypedef, name), 0 }, 1, FTYPEDESC_SAVE | FTYPEDESC_PTR, NULL, NULL, NULL, &(((classNameTypedef *)0)->name->m_DataMap), sizeof( *(((classNameTypedef *)0)->name) ), NULL, 0, 0.0f }

#define DEFINE_EMBEDDED_ARRAY( name, count )			\
	typedescription_t{ FIELD_EMBEDDED, #name, typedesc_offset_t{ offsetof(classNameTypedef, name), 0 }, count, FTYPEDESC_SAVE, NULL, NULL, NULL, &(((classNameTypedef *)0)->name->m_DataMap), sizeof( ((classNameTypedef *)0)->name[0] ), NULL, 0, 0.0f  }

#define DEFINE_EMBEDDED_AUTO_ARRAY( name )			\
	typedescription_t{ FIELD_EMBEDDED, #name, typedesc_offset_t{ offsetof(classNameTypedef, name), 0 }, SIZE_OF_ARRAY( ((classNameTypedef *)0)->name ), FTYPEDESC_SAVE, NULL, NULL, NULL, &(((classNameTypedef *)0)->name->m_DataMap), sizeof( ((classNameTypedef *)0)->name[0] ), NULL, 0, 0.0f  }

#ifndef NO_ENTITY_PREDICTION

#define DEFINE_PRED_TYPEDESCRIPTION( name, fieldtype )						\
	typedescription_t{ FIELD_EMBEDDED, #name, typedesc_offset_t{ offsetof(classNameTypedef, name), 0 }, 1, FTYPEDESC_SAVE, NULL, NULL, NULL, &fieldtype::m_PredMap }

#define DEFINE_PRED_TYPEDESCRIPTION_PTR( name, fieldtype )						\
	typedescription_t{ FIELD_EMBEDDED, #name, typedesc_offset_t{ offsetof(classNameTypedef, name), 0 }, 1, FTYPEDESC_SAVE | FTYPEDESC_PTR, NULL, NULL, NULL, &fieldtype::m_PredMap }

#else

#define DEFINE_PRED_TYPEDESCRIPTION( name, fieldtype )		DEFINE_FIELD_NULL
#define DEFINE_PRED_TYPEDESCRIPTION_PTR( name, fieldtype )	DEFINE_FIELD_NULL

#endif

// Extensions to datamap.h macros for predicted entities only
#define DEFINE_PRED_FIELD(name,fieldtype, flags)			_FIELD(name, fieldtype, 1,  flags, NULL, 0.0f )
#define DEFINE_PRED_ARRAY(name,fieldtype, count,flags)		_FIELD(name, fieldtype, count, flags, NULL, 0.0f )
#define DEFINE_FIELD_NAME(localname,netname,fieldtype)		_FIELD(localname, fieldtype, 1,  0, #netname, 0.0f )
// Predictable macros, which include a tolerance for floating point values...
#define DEFINE_PRED_FIELD_TOL(name,fieldtype, flags,tolerance)			_FIELD(name, fieldtype, 1,  flags, NULL, tolerance )
#define DEFINE_PRED_ARRAY_TOL(name,fieldtype, count,flags,tolerance)		_FIELD(name, fieldtype, count, flags, NULL, tolerance)
#define DEFINE_FIELD_NAME_TOL(localname,netname,fieldtolerance)		_FIELD(localname, fieldtype, 1,  0, #netname, tolerance )

//#define DEFINE_DATA( name, fieldextname, flags ) _FIELD(name, fieldtype, 1,  flags, extname )

// INPUTS
#define DEFINE_INPUT( name, fieldtype, inputname ) typedescription_t{ fieldtype, #name, typedesc_offset_t{ offsetof(classNameTypedef, name), 0 }, 1, FTYPEDESC_INPUT | FTYPEDESC_SAVE | FTYPEDESC_KEY,	inputname, NULL, NULL, NULL, sizeof( ((classNameTypedef *)0)->name ) }
#define DEFINE_INPUTFUNC( fieldtype, inputname, inputfunc ) typedescription_t{ fieldtype, #inputfunc, typedesc_offset_t{ 0, 0 }, 1, FTYPEDESC_INPUT, inputname, NULL, static_cast <inputfunc_t> (&classNameTypedef::inputfunc) }

// OUTPUTS
// the variable 'name' MUST BE derived from CBaseOutput
// we know the output type from the variable itself, so it doesn't need to be specified here
class ISaveRestoreOps;
extern ISaveRestoreOps *eventFuncs;
#define DEFINE_OUTPUT( name, outputname )	typedescription_t{ FIELD_CUSTOM, #name, typedesc_offset_t{ offsetof(classNameTypedef, name), 0 }, 1, FTYPEDESC_OUTPUT | FTYPEDESC_SAVE | FTYPEDESC_KEY, outputname, eventFuncs }

// replaces EXPORT table for portability and non-DLL based systems (xbox)
#define DEFINE_FUNCTION_RAW( function, func_type )			typedescription_t{ FIELD_VOID, nameHolder.GenerateName(#function), typedesc_offset_t{ 0, 0 }, 1, FTYPEDESC_FUNCTIONTABLE, NULL, NULL, (inputfunc_t)((func_type)(&classNameTypedef::function)) }
#define DEFINE_FUNCTION( function )			DEFINE_FUNCTION_RAW( function, inputfunc_t )


#define FTYPEDESC_GLOBAL			0x0001		// This field is masked for global entity save/restore
#define FTYPEDESC_SAVE				0x0002		// This field is saved to disk
#define FTYPEDESC_KEY				0x0004		// This field can be requested and written to by string name at load time
#define FTYPEDESC_INPUT				0x0008		// This field can be written to by string name at run time, and a function called
#define FTYPEDESC_OUTPUT			0x0010		// This field propogates it's value to all targets whenever it changes
#define FTYPEDESC_FUNCTIONTABLE		0x0020		// This is a table entry for a member function pointer
#define FTYPEDESC_PTR				0x0040		// This field is a pointer, not an embedded object
#define FTYPEDESC_OVERRIDE			0x0080		// The field is an override for one in a base class (only used by prediction system for now)

// Flags used by other systems (e.g., prediction system)
#define FTYPEDESC_INSENDTABLE		0x0100		// This field is present in a network SendTable
#define FTYPEDESC_PRIVATE			0x0200		// The field is local to the client or server only (not referenced by prediction code and not replicated by networking)
#define FTYPEDESC_NOERRORCHECK		0x0400		// The field is part of the prediction typedescription, but doesn't get compared when checking for errors

#define FTYPEDESC_MODELINDEX		0x0800		// The field is a model index (used for debugging output)

#define FTYPEDESC_INDEX				0x1000		// The field is an index into file data, used for byteswapping. 

// These flags apply to C_BasePlayer derived objects only
#define FTYPEDESC_VIEW_OTHER_PLAYER		0x2000		// By default you can only view fields on the local player (yourself), 
													//   but if this is set, then we allow you to see fields on other players
#define FTYPEDESC_VIEW_OWN_TEAM			0x4000		// Only show this data if the player is on the same team as the local player
#define FTYPEDESC_VIEW_NEVER			0x8000		// Never show this field to anyone, even the local player (unusual)

#define TD_MSECTOLERANCE		0.001f		// This is a FIELD_FLOAT and should only be checked to be within 0.001 of the networked info

struct typedescription_t;


class ISaveRestoreOps;

struct datamap_t;
struct typedescription_t;

//-----------------------------------------------------------------------------
// Purpose: stores the list of objects in the hierarchy
//			used to iterate through an object's data descriptions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//
// Macros used to implement datadescs
//
#define DECLARE_FRIEND_DATADESC_ACCESS()	\
	template <typename T> friend void DataMapAccess(T *, datamap_t **p); \
	template <typename T> friend datamap_t *DataMapInit(T *);

#define DECLARE_SIMPLE_DATADESC() \
	static datamap_t m_DataMap; \
	static datamap_t *GetBaseMap(); \
	template <typename T> friend void DataMapAccess(T *, datamap_t **p); \
	template <typename T> friend datamap_t *DataMapInit(T *);

#define DECLARE_SIMPLE_DATADESC_INSIDE_NAMESPACE() \
	static datamap_t m_DataMap; \
	static datamap_t *GetBaseMap(); \
	template <typename T> friend void ::DataMapAccess(T *, datamap_t **p); \
	template <typename T> friend datamap_t *::DataMapInit(T *);

#define	DECLARE_DATADESC() \
	DECLARE_SIMPLE_DATADESC() \
	virtual datamap_t *GetDataDescMap( void );

#define BEGIN_DATADESC( className ) \
	datamap_t className::m_DataMap = datamap_t{ 0, 0, #className, NULL }; \
	datamap_t *className::GetDataDescMap( void ) { return &m_DataMap; } \
	datamap_t *className::GetBaseMap() { datamap_t *pResult; DataMapAccess((BaseClass *)NULL, &pResult); return pResult; } \
	BEGIN_DATADESC_GUTS( className )

#define BEGIN_DATADESC_NO_BASE( className ) \
	datamap_t className::m_DataMap = datamap_t{ 0, 0, #className, NULL }; \
	datamap_t *className::GetDataDescMap( void ) { return &m_DataMap; } \
	datamap_t *className::GetBaseMap() { return NULL; } \
	BEGIN_DATADESC_GUTS( className )

#define BEGIN_SIMPLE_DATADESC( className ) \
	datamap_t className::m_DataMap = datamap_t{ 0, 0, #className, NULL }; \
	datamap_t *className::GetBaseMap() { return NULL; } \
	BEGIN_DATADESC_GUTS( className )

#define BEGIN_SIMPLE_DATADESC_( className, BaseClass ) \
	datamap_t className::m_DataMap = datamap_t{ 0, 0, #className, NULL }; \
	datamap_t *className::GetBaseMap() { datamap_t *pResult; DataMapAccess((BaseClass *)NULL, &pResult); return pResult; } \
	BEGIN_DATADESC_GUTS( className )

#define BEGIN_DATADESC_GUTS( className ) \
	template <typename T> datamap_t *DataMapInit(T *); \
	template <> datamap_t *DataMapInit<className>( className * ); \
	namespace className##_DataDescInit \
	{ \
		datamap_t *g_DataMapHolder = DataMapInit( (className *)NULL ); /* This can/will be used for some clean up duties later */ \
	} \
	\
	template <> datamap_t *DataMapInit<className>( className * ) \
	{ \
		typedef className classNameTypedef; \
		static CDatadescGeneratedNameHolder nameHolder(#className); \
		className::m_DataMap.baseMap = className::GetBaseMap(); \
		static typedescription_t dataDesc[] = \
		{ \
		typedescription_t{ FIELD_VOID,0, typedesc_offset_t{0,0},0,0,0,0,0,0}, /* so you can define "empty" tables */

#define BEGIN_DATADESC_GUTS_NAMESPACE( className, nameSpace ) \
	template <typename T> datamap_t *nameSpace::DataMapInit(T *); \
	template <> datamap_t *nameSpace::DataMapInit<className>( className * ); \
	namespace className##_DataDescInit \
	{ \
		datamap_t *g_DataMapHolder = nameSpace::DataMapInit( (className *)NULL ); /* This can/will be used for some clean up duties later */ \
	} \
	\
	template <> datamap_t *nameSpace::DataMapInit<className>( className * ) \
	{ \
		typedef className classNameTypedef; \
		static CDatadescGeneratedNameHolder nameHolder(#className); \
		className::m_DataMap.baseMap = className::GetBaseMap(); \
		static typedescription_t dataDesc[] = \
		{ \
		typedescription_t{ FIELD_VOID,0, typedesc_offset_t{0,0},0,0,0,0,0,0}, /* so you can define "empty" tables */

#define END_DATADESC() \
		}; \
		\
		if ( sizeof( dataDesc ) > sizeof( dataDesc[0] ) ) \
		{ \
			classNameTypedef::m_DataMap.dataNumFields = SIZE_OF_ARRAY( dataDesc ) - 1; \
			classNameTypedef::m_DataMap.dataDesc 	  = &dataDesc[1]; \
		} \
		else \
		{ \
			classNameTypedef::m_DataMap.dataNumFields = 1; \
			classNameTypedef::m_DataMap.dataDesc 	  = dataDesc; \
		} \
		return &classNameTypedef::m_DataMap; \
	}

// used for when there is no data description
#define IMPLEMENT_NULL_SIMPLE_DATADESC( derivedClass ) \
	BEGIN_SIMPLE_DATADESC( derivedClass ) \
	END_DATADESC()

#define IMPLEMENT_NULL_SIMPLE_DATADESC_( derivedClass, baseClass ) \
	BEGIN_SIMPLE_DATADESC_( derivedClass, baseClass ) \
	END_DATADESC()

#define IMPLEMENT_NULL_DATADESC( derivedClass ) \
	BEGIN_DATADESC( derivedClass ) \
	END_DATADESC()

// helps get the offset of a bitfield
#define BEGIN_BITFIELD( name ) \
	union \
	{ \
		char name; \
		struct \
		{

#define END_BITFIELD() \
		}; \
	};

//-----------------------------------------------------------------------------
// Forward compatability with potential seperate byteswap datadescs

#define DECLARE_BYTESWAP_DATADESC() DECLARE_SIMPLE_DATADESC()
#define BEGIN_BYTESWAP_DATADESC(name) BEGIN_SIMPLE_DATADESC(name) 
#define BEGIN_BYTESWAP_DATADESC_(name,base) BEGIN_SIMPLE_DATADESC_(name,base) 
#define END_BYTESWAP_DATADESC() END_DATADESC()

//-----------------------------------------------------------------------------

template <typename T> 
inline void DataMapAccess(T *ignored, datamap_t **p)
{
	*p = &T::m_DataMap;
}

//-----------------------------------------------------------------------------

class CDatadescGeneratedNameHolder
{
public:
	CDatadescGeneratedNameHolder( const char *pszBase )
	 : m_pszBase(pszBase)
	{
		m_nLenBase = strlen( m_pszBase );
	}
	
	~CDatadescGeneratedNameHolder()
	{
		for ( int i = 0; i < m_Names.Count(); i++ )
		{
			delete m_Names[i];
		}
	}
	
	const char *GenerateName( const char *pszIdentifier )
	{
		char *pBuf = new char[m_nLenBase + strlen(pszIdentifier) + 1];
		strcpy( pBuf, m_pszBase );
		strcat( pBuf, pszIdentifier );
		m_Names.AddToTail( pBuf );
		return pBuf;
	}
	
private:
	const char *m_pszBase;
	size_t m_nLenBase;
	CUtlVector<char *> m_Names;
};

//-----------------------------------------------------------------------------

#include "tier0/memdbgoff.h"

#endif // DATAMAP_H
