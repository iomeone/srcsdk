class CMouthInfo
{
public:
	// 0 = mouth closed, 255 = mouth agape
	byte					mouthopen;		
	// counter for running average
	byte					sndcount;		
	// running average
	int						sndavg;			

public:
							CMouthInfo( void ) { m_nVoiceSources = 0; m_needsEnvelope = false; }
	    					~CMouthInfo( void ) { ClearVoiceSources(); }

	int						GetNumVoiceSources( void );
	CVoiceData				*GetVoiceSource( int number );

	void					ClearVoiceSources( void );
	int						GetIndexForSource( CAudioSource *source );
	bool					IsSourceReferenced( CAudioSource *source );

	CVoiceData				*AddSource( CAudioSource *source, bool bIgnorePhonemes );

	void					RemoveSource( CAudioSource *source );
	void					RemoveSourceByIndex( int index );

	bool					IsActive( void );
	bool					NeedsEnvelope() { return m_needsEnvelope != 0; }
	void					ActivateEnvelope() { m_needsEnvelope = true; }

private:
	enum
	{
		MAX_VOICE_DATA = 4
	};

	short					m_nVoiceSources;
	short					m_needsEnvelope; 
	CVoiceData				m_VoiceSources[ MAX_VOICE_DATA ];
}; 
