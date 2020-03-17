#define SEND_SOUND_TIME 1

struct SoundInfo_t
{
    Vector			vOrigin;
	Vector			vDirection;
	Vector			vListenerOrigin;
	const char		*pszName;		// UNDONE: Make this a FilenameHandle_t to avoid bugs with arrays of these
	float			fVolume;
	float			fDelay;
	float			fTickTime;			// delay is encoded relative to this tick, fix up if packet is delayed
	int				nSequenceNumber;
	int				nEntityIndex;
	int				nChannel;
	int				nPitch;
	int				nFlags;
	int 			nSoundNum;
	int				nSpeakerEntity;
	soundlevel_t	Soundlevel;
	bool			bIsSentence;
	bool			bIsAmbient;
	bool			bLooping;
	
	//---------------------------------

	SoundInfo_t();

	void Set(int newEntity, int newChannel, const char *pszNewName, const Vector &newOrigin, const Vector& newDirection, 
		float newVolume, soundlevel_t newSoundLevel, bool newLooping, int newPitch, const Vector &vecListenerOrigin, int speakerentity );

	void SetDefault();
	void ClearStopFields();
	void WriteDelta( SoundInfo_t *delta, bf_write &buffer, float finalTickTime );
	void ReadDelta( SoundInfo_t *delta, bf_read &buffer );
};

inline void SoundInfo_t::ReadDelta( SoundInfo_t *delta, bf_read &buffer)
{
    if ( !buffer.ReadOneBit() )
    {
        nEntityIndex = delta->nEntityIndex;
    }
    else
    {
        if ( buffer.ReadOneBit() )
        {
            nEntityIndex = buffer.ReadUBitLong( 5 );
        }
        else
        {
            nEntityIndex = buffer.ReadUBitLong( MAX_EDICT_BITS );
        }
    }

    READ_DELTA_UINT( nSoundNum, MAX_SOUND_INDEX_BITS );

    READ_DELTA_UINT( nFlags, SND_FLAG_BITS_ENCODE );

    READ_DELTA_UINT( nChannel, 3 );

    bIsAmbient = buffer.ReadOneBit() != 0;
    bIsSentence = buffer.ReadOneBit() != 0; // NOTE: SND_STOP behavior is different depending on this flag

    if ( nFlags != SND_STOP )
    {
        if ( buffer.ReadOneBit() != 0 )
        {
            nSequenceNumber = delta->nSequenceNumber;
        }
        else if ( buffer.ReadOneBit() != 0 )
        {
            nSequenceNumber = delta->nSequenceNumber + 1;
        }
        else
        {
            nSequenceNumber = buffer.ReadUBitLong( SOUND_SEQNUMBER_BITS );
        }
            
        if ( buffer.ReadOneBit() != 0 )
        {
            fVolume = (float)buffer.ReadUBitLong( 7 )/127.0f;
        }
        else
        {
            fVolume = delta->fVolume;
        }

        if ( buffer.ReadOneBit() != 0 )
        {
            Soundlevel = (soundlevel_t)buffer.ReadUBitLong( MAX_SNDLVL_BITS );
        }
        else
        {
            Soundlevel = delta->Soundlevel;
        }

        READ_DELTA_UINT( nPitch, 8 );


        if ( buffer.ReadOneBit() != 0 )
        {
#if SEND_SOUND_TIME
            fDelay = buffer.ReadFloat();
#else
            // Up to 4096 msec delay
            fDelay = (float)buffer.ReadSBitLong( MAX_SOUND_DELAY_MSEC_ENCODE_BITS ) / 1000.0f; ;
            
            if ( fDelay < 0 )
            {
                fDelay *= 10.0f;
            }
            // bias results so that we only incur the precision loss on relatively large skipaheads
            fDelay -= SOUND_DELAY_OFFSET;
#endif
        }
        else
        {
            fDelay = delta->fDelay;
        }

        READ_DELTA_SINT_SCALE( vOrigin.x, 8.0f, COORD_INTEGER_BITS - 2 );
        READ_DELTA_SINT_SCALE( vOrigin.y, 8.0f, COORD_INTEGER_BITS - 2 );
        READ_DELTA_SINT_SCALE( vOrigin.z, 8.0f, COORD_INTEGER_BITS - 2 );

        READ_DELTA_SINT( nSpeakerEntity, MAX_EDICT_BITS + 1 );
    }
    else
    {
        ClearStopFields();
    }
}

inline void SoundInfo_t::WriteDelta( SoundInfo_t *delta, bf_write &buffer, float finalTickTime )
{
    if ( nEntityIndex == delta->nEntityIndex )
    {
        buffer.WriteOneBit( 0 );
    }
    else
    {
        buffer.WriteOneBit( 1 );
    
        if ( nEntityIndex <= 31)
        {
            buffer.WriteOneBit( 1 );
            buffer.WriteUBitLong( nEntityIndex, 5 );
        }
        else
        {
            buffer.WriteOneBit( 0 );
            buffer.WriteUBitLong( nEntityIndex, MAX_EDICT_BITS );
        }
    }

    WRITE_DELTA_UINT( nSoundNum, MAX_SOUND_INDEX_BITS );

    WRITE_DELTA_UINT( nFlags, SND_FLAG_BITS_ENCODE );

    WRITE_DELTA_UINT( nChannel, 3 );

    buffer.WriteOneBit( bIsAmbient?1:0 );
    buffer.WriteOneBit( bIsSentence?1:0 ); // NOTE: SND_STOP behavior is different depending on this flag

    if ( nFlags != SND_STOP )
    {
        if ( nSequenceNumber == delta->nSequenceNumber )
        {
            // didn't change, most often case
            buffer.WriteOneBit( 1 );
        }
        else if ( nSequenceNumber == (delta->nSequenceNumber+1) )
        {
            // increased by one
            buffer.WriteOneBit( 0 );
            buffer.WriteOneBit( 1 );
        }
        else
        {
            // send full seqnr
            buffer.WriteUBitLong( 0, 2 ); // 2 zero bits
            buffer.WriteUBitLong( nSequenceNumber, SOUND_SEQNUMBER_BITS ); 
        }
                    
        if ( fVolume == delta->fVolume )
        {
            buffer.WriteOneBit( 0 );
        }
        else
        {
            buffer.WriteOneBit( 1 );
            buffer.WriteUBitLong( (unsigned int)(fVolume*127.0f), 7 );
        }

        WRITE_DELTA_UINT( Soundlevel, MAX_SNDLVL_BITS );

        WRITE_DELTA_UINT( nPitch, 8 );

        float delayValue = fDelay;
        if ( (nFlags & SND_DELAY) && fTickTime != finalTickTime )
        {
            delayValue += fTickTime - finalTickTime;
        }
        if ( delayValue == delta->fDelay )
        {
            buffer.WriteOneBit( 0 );
        }
        else
        {
            buffer.WriteOneBit( 1 );

#if SEND_SOUND_TIME
            buffer.WriteFloat( delayValue );
#else
            // skipahead works in 10 ms increments
            // bias results so that we only incur the precision loss on relatively large skipaheads
            delayValue += SOUND_DELAY_OFFSET;

            // Convert to msecs
            int iDelay = delayValue * 1000.0f;

            iDelay = clamp( iDelay, (int)(-10 * MAX_SOUND_DELAY_MSEC), (int)(MAX_SOUND_DELAY_MSEC) );

            if ( iDelay < 0 )
            {
                iDelay /=10;	
            }
            
            buffer.WriteSBitLong( iDelay , MAX_SOUND_DELAY_MSEC_ENCODE_BITS );
#endif
        }

        // don't transmit sounds with high precision
        WRITE_DELTA_SINT_SCALE( vOrigin.x, 8.0f, COORD_INTEGER_BITS - 2 );
        WRITE_DELTA_SINT_SCALE( vOrigin.y, 8.0f, COORD_INTEGER_BITS - 2  );
        WRITE_DELTA_SINT_SCALE( vOrigin.z, 8.0f, COORD_INTEGER_BITS - 2 );

        WRITE_DELTA_SINT( nSpeakerEntity, MAX_EDICT_BITS + 1 );
    }
    else
    {
        ClearStopFields();
    }
};

inline void SoundInfo_t::SetDefault()
{
    fDelay = DEFAULT_SOUND_PACKET_DELAY;
    fTickTime = 0;
    fVolume = DEFAULT_SOUND_PACKET_VOLUME;
    Soundlevel = SNDLVL_NORM;
    nPitch = DEFAULT_SOUND_PACKET_PITCH;

    nEntityIndex = 0;
    nSpeakerEntity = -1;
    nChannel = CHAN_STATIC;
    nSoundNum = 0;
    nFlags = 0;
    nSequenceNumber = 0;

    pszName = NULL;

    bLooping = false;
    bIsSentence = false;
    bIsAmbient = false;
    
    vOrigin.Init();
    vDirection.Init();
    vListenerOrigin.Init();
}

inline void SoundInfo_t::ClearStopFields()
{
    fVolume = 0;
    Soundlevel = SNDLVL_NONE;
    nPitch = PITCH_NORM;
    pszName = NULL;
    fDelay = 0.0f;
    nSequenceNumber = 0;

    vOrigin.Init();
    nSpeakerEntity = -1;
}