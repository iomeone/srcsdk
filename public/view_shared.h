//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef VIEW_SHARED_H
#define VIEW_SHARED_H

#ifdef _WIN32
#pragma once
#endif

#include "convar.h"
#include "mathlib/vector.h"
#include "materialsystem/MaterialSystemUtil.h"


//-----------------------------------------------------------------------------
// Flags passed in with view setup
//-----------------------------------------------------------------------------
enum ClearFlags_t
{
	VIEW_CLEAR_COLOR = 0x1,
	VIEW_CLEAR_DEPTH = 0x2,
	VIEW_CLEAR_FULL_TARGET = 0x4,
	VIEW_NO_DRAW = 0x8,
	VIEW_CLEAR_OBEY_STENCIL = 0x10, // Draws a quad allowing stencil test to clear through portals
	VIEW_CLEAR_STENCIL = 0x20,
};

enum StereoEye_t
{
	STEREO_EYE_MONO = 0,
	STEREO_EYE_LEFT = 1,
	STEREO_EYE_RIGHT = 2,
	STEREO_EYE_MAX = 3,
};

enum MotionBlurMode_t
{
	MOTION_BLUR_DISABLE = 1,
	MOTION_BLUR_GAME = 2,			// Game uses real-time inter-frame data
	MOTION_BLUR_SFM = 3				// Use SFM data passed in CViewSetup structure
};

//-----------------------------------------------------------------------------
// Purpose: Renderer setup data.  
//-----------------------------------------------------------------------------

#include "compat_view_shared.h"

#endif // VIEW_SHARED_H
