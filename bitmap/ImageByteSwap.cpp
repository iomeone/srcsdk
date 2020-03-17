//======= Copyright � 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Image Byte Swapping. Isolate routines to own module to allow librarian
// to ignore xbox 360 dependenices in non-applicable win32 projects.
//
//=============================================================================//

#if defined( _WIN32 )
#include <windows.h>
#include <d3d9types.h>
#define D3D9_TYPES_INCLUDED
#endif
#include "tier0/platform.h"
#include "tier0/dbg.h"
#include "bitmap/imageformat.h"

// Should be last include
#include "tier0/memdbgon.h"

#if defined( _WIN32 )
// the x86 version of the 360 (used by win32 tools)
// It would have been nice to use the 360 D3DFORMAT bit encodings, but the codes
// are different for WIN32, and this routine is used by a WIN32 library to
// manipulate 360 data, so there can be no reliance on WIN32 D3DFORMAT bits
#include <d3d9.h>
#endif

namespace ImageLoader
{

//-----------------------------------------------------------------------------
// Known formats that can be converted.  Used as a trap for 360 formats
// that may occur but have not been validated yet.
//-----------------------------------------------------------------------------
	bool IsFormatValidForConversion( ImageFormat fmt )
	{
		switch ( fmt )
		{
			case IMAGE_FORMAT_RGBA8888: 
			case IMAGE_FORMAT_ABGR8888:
			case IMAGE_FORMAT_RGB888:
			case IMAGE_FORMAT_BGR888:
			case IMAGE_FORMAT_ARGB8888:
			case IMAGE_FORMAT_BGRA8888:
			case IMAGE_FORMAT_BGRX8888:
			case IMAGE_FORMAT_UVWQ8888:
			case IMAGE_FORMAT_RGBA16161616F:
			case IMAGE_FORMAT_RGBA16161616:
			case IMAGE_FORMAT_UVLX8888:
			case IMAGE_FORMAT_DXT1:
			case IMAGE_FORMAT_DXT1_ONEBITALPHA:
			case IMAGE_FORMAT_DXT3:
			case IMAGE_FORMAT_DXT5:
			case IMAGE_FORMAT_UV88:
				return true;

				// untested formats
			default:
			case IMAGE_FORMAT_RGB565: 
			case IMAGE_FORMAT_I8:
			case IMAGE_FORMAT_IA88:
			case IMAGE_FORMAT_A8:
			case IMAGE_FORMAT_RGB888_BLUESCREEN:
			case IMAGE_FORMAT_BGR888_BLUESCREEN:
			case IMAGE_FORMAT_BGR565:
			case IMAGE_FORMAT_BGRX5551:
			case IMAGE_FORMAT_BGRA4444:
			case IMAGE_FORMAT_BGRA5551:
			case IMAGE_FORMAT_ATI1N:
			case IMAGE_FORMAT_ATI2N:
				break;
		}

		return false;
	}


//-----------------------------------------------------------------------------
// Swaps the image element type within the format.
// This is to ensure that >8 bit channels are in the correct endian format
// as expected by the conversion process, which varies according to format,
// input, and output.
//-----------------------------------------------------------------------------
	void PreConvertSwapImageData( unsigned char *pImageData, int nImageSize, ImageFormat imageFormat, int width, int stride )
	{
		Assert( IsFormatValidForConversion( imageFormat ) );
	}

//-----------------------------------------------------------------------------
// Swaps image bytes for use on a big endian platform. This is used after the conversion
// process to match the 360 d3dformats.
//-----------------------------------------------------------------------------
	void PostConvertSwapImageData( unsigned char *pImageData, int nImageSize, ImageFormat imageFormat, int width, int stride )
	{
		Assert( IsFormatValidForConversion( imageFormat ) );
	}

//-----------------------------------------------------------------------------
// Swaps image bytes.
//-----------------------------------------------------------------------------
	void ByteSwapImageData( unsigned char *pImageData, int nImageSize, ImageFormat imageFormat, int width, int stride )
	{
		Assert( IsFormatValidForConversion( imageFormat ) );
	}
	
}
