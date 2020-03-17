//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef IVOICETWEAK_H
#define IVOICETWEAK_H
#ifdef _WIN32
#pragma once
#endif

// These provide access to the voice controls.
typedef enum
{
	MicrophoneVolume=0,			// values 0-1.
	OtherSpeakerScale,			// values 0-1. Scales how loud other players are.
	MicBoost,
	SpeakingVolume,				// values 0-1.  Current voice volume received through Voice Tweak mode

} VoiceTweakControl;

#include "compat_ivoicetweak.h"


#endif // IVOICETWEAK_H
