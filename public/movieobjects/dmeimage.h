//====== Copyright � 1996-2004, Valve Corporation, All rights reserved. =======
//
// A class representing an image
//
//=============================================================================

#ifndef DMEIMAGE_H
#define DMEIMAGE_H
#ifdef _WIN32
#pragma once
#endif

#include "datamodel/dmelement.h"
#include "datamodel/dmattributevar.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
enum ImageFormat : int;


//-----------------------------------------------------------------------------
// A class representing an image (2d or 3d bitmap)
//-----------------------------------------------------------------------------
class CDmeImage : public CDmElement
{
	DEFINE_ELEMENT( CDmeImage, CDmElement );

public:
	// Methods related to image format
	ImageFormat Format() const;
	const char *FormatName() const;

	// returns a pointer to the image bits buffer
	const void *ImageBits() const;

public:
	CDmaVar<int> m_Width;
	CDmaVar<int> m_Height;
	CDmaVar<int> m_Depth;

private:
	CDmaVar<int> m_Format;
	CDmaBinaryBlock m_Bits;
};


//-----------------------------------------------------------------------------
// returns a pointer to the image bits buffer
//-----------------------------------------------------------------------------
inline const void *CDmeImage::ImageBits() const
{
	return m_Bits.Get();
}


#endif // DMEIMAGE_H
