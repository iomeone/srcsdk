//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef SOUNDINFO_H
#define SOUNDINFO_H

#ifdef _WIN32
#pragma once
#endif

#include "bitbuf.h"
#include "const.h"
#include "soundflags.h"
#include "coordsize.h"
#include "mathlib/vector.h"


#define WRITE_DELTA_UINT( name, length )	\
	if ( name == delta->name )		\
		buffer.WriteOneBit(0);		\
	else					\
	{					\
		buffer.WriteOneBit(1);		\
		buffer.WriteUBitLong( name, length ); \
	}

#define READ_DELTA_UINT( name, length )			\
	if ( buffer.ReadOneBit() != 0 )			\
	{ name = buffer.ReadUBitLong( length ); }\
	else { name = delta->name; }

#define WRITE_DELTA_SINT( name, length )	\
	if ( name == delta->name )		\
		buffer.WriteOneBit(0);		\
	else					\
	{					\
		buffer.WriteOneBit(1);		\
		buffer.WriteSBitLong( name, length ); \
	}

#define WRITE_DELTA_SINT_SCALE( name, scale, length )	\
	if ( name == delta->name )		\
	buffer.WriteOneBit(0);		\
	else					\
	{					\
	buffer.WriteOneBit(1);		\
	buffer.WriteSBitLong( name / scale, length ); \
	}

#define READ_DELTA_SINT( name, length )			\
	if ( buffer.ReadOneBit() != 0 )			\
	{ name = buffer.ReadSBitLong( length ); }	\
	else { name = delta->name; }

#define READ_DELTA_SINT_SCALE( name, scale, length )			\
	if ( buffer.ReadOneBit() != 0 )			\
	{ name = scale * buffer.ReadSBitLong( length ); }	\
	else { name = delta->name; }

#define SOUND_SEQNUMBER_BITS	10
#define SOUND_SEQNUMBER_MASK	( (1<<SOUND_SEQNUMBER_BITS) - 1 )

// offset sound delay encoding by 60ms since we encode negative sound delays with less precision
// This means any negative sound delay greater than -100ms will get encoded at full precision
#define SOUND_DELAY_OFFSET					(0.100f)

#pragma pack(4)

#include "compat_soundinfo.h"

//-----------------------------------------------------------------------------

inline SoundInfo_t::SoundInfo_t()
{
	SetDefault();
}

inline void SoundInfo_t::Set(int newEntity, int newChannel, const char *pszNewName, const Vector &newOrigin, const Vector& newDirection, 
		float newVolume, soundlevel_t newSoundLevel, bool newLooping, int newPitch, const Vector &vecListenerOrigin, int speakerentity )
{
	nEntityIndex = newEntity;
	nChannel = newChannel;
	pszName = pszNewName;
	vOrigin = newOrigin;
	vDirection = newDirection;
	fVolume = newVolume;
	Soundlevel = newSoundLevel;
	bLooping = newLooping;
	nPitch = newPitch;
	vListenerOrigin = vecListenerOrigin;
	nSpeakerEntity = speakerentity;
}

struct SpatializationInfo_t
{
	typedef enum
	{
		SI_INCREATION = 0,
		SI_INSPATIALIZATION
	} SPATIALIZATIONTYPE;

	// Inputs
	SPATIALIZATIONTYPE	type;
	// Info about the sound, channel, origin, direction, etc.
	SoundInfo_t			info;

	// Requested Outputs ( NULL == not requested )
	Vector				*pOrigin;
	QAngle				*pAngles;
	float				*pflRadius;
};
#pragma pack()

#endif // SOUNDINFO_H
