//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//

#ifndef ILOCALIZE_H
#define ILOCALIZE_H

#ifdef _WIN32
#pragma once
#endif

#include "tier1/ilocalize.h"

namespace vgui
{
	class ILocalize : public ::ILocalize { };		// backwards compatability with vgui::ILocalize declarations
}

#endif // ILOCALIZE_H
