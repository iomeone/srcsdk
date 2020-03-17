//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//

#ifndef DATATABLE_COMMON_H
#define DATATABLE_COMMON_H

#ifdef _WIN32
#pragma once
#endif	

#include "basetypes.h"
#include "tier0/dbg.h"
#include "tier1/strtools.h"
#include <stddef.h>

#if defined LINUX || defined __MINGW32__
#undef offsetof
#define offsetof(s,m)	(size_t)&(((s *)0)->m)
#endif

// Max number of properties in a datatable and its children.
#define MAX_DATATABLES		1024	// must be a power of 2.
#define MAX_DATATABLE_PROPS	4096

#define MAX_ARRAY_ELEMENTS	2048		// a network array should have more that 1024 elements

#define HIGH_DEFAULT		-121121.121121f

#define BITS_FULLRES	-1	// Use the full resolution of the type being encoded.
#define BITS_WORLDCOORD	-2	// Encode as a world coordinate.

#define DT_MAX_STRING_BITS			9
#define DT_MAX_STRING_BUFFERSIZE	(1<<DT_MAX_STRING_BITS)	// Maximum length of a string that can be sent.

#define STRINGBUFSIZE(className, varName)	sizeof( ((className*)0)->varName )

// Gets the size of a variable in a class.
#define PROPSIZEOF(className, varName)		sizeof(((className*)0)->varName)

#include "compat_dt_common.h"

// SendProp::m_Flags.
#define SPROP_UNSIGNED			(1<<0)	// Unsigned integer data.

#define SPROP_COORD				(1<<1)	// If this is set, the float/vector is treated like a world coordinate.
										// Note that the bit count is ignored in this case.

#define SPROP_NOSCALE			(1<<2)	// For floating point, don't scale into range, just take value as is.

#define SPROP_ROUNDDOWN			(1<<3)	// For floating point, limit high value to range minus one bit unit

#define SPROP_ROUNDUP			(1<<4)	// For floating point, limit low value to range minus one bit unit

#define SPROP_NORMAL			(1<<5)	// If this is set, the vector is treated like a normal (only valid for vectors)
							
#define SPROP_EXCLUDE			(1<<6)	// This is an exclude prop (not excludED, but it points at another prop to be excluded).

#define SPROP_XYZE				(1<<7)	// Use XYZ/Exponent encoding for vectors.

#define SPROP_INSIDEARRAY		(1<<8)	// This tells us that the property is inside an array, so it shouldn't be put into the
										// flattened property list. Its array will point at it when it needs to.

#define SPROP_PROXY_ALWAYS_YES	(1<<9)	// Set for datatable props using one of the default datatable proxies like
										// SendProxy_DataTableToDataTable that always send the data to all clients.

#define SPROP_VARINT					SPROP_NORMAL	// reuse existing flag so we don't break demo. note you want to include SPROP_UNSIGNED if needed, its more efficient

// Used by the SendProp and RecvProp functions to disable debug checks on type sizes.
#define SIZEOF_IGNORE		-1


// Use this to extern send and receive datatables, and reference them.
#define EXTERN_SEND_TABLE(tableName)	namespace tableName {extern SendTable g_SendTable;}
#define EXTERN_RECV_TABLE(tableName)	namespace tableName {extern RecvTable g_RecvTable;}

#define REFERENCE_SEND_TABLE(tableName)	tableName::g_SendTable
#define REFERENCE_RECV_TABLE(tableName)	tableName::g_RecvTable


class SendProp;

#include "compat_dt_common.h"

inline DVariant::DVariant() {m_Type = DPT_Float;}
inline DVariant::DVariant(float val) {m_Type = DPT_Float; m_Float = val;}

// This can be used to set the # of bits used to transmit a number between 0 and nMaxElements-1.
inline int NumBitsForCount( int nMaxElements )
{
	int nBits = 0;
	while ( nMaxElements > 0 )
	{
		++nBits;
		nMaxElements >>= 1;
	}
	return nBits;
}


#endif // DATATABLE_COMMON_H
