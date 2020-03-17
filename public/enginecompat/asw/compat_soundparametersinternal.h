static SoundChannels g_pChannelNames[] =
{
	{ CHAN_AUTO, "CHAN_AUTO" },
	{ CHAN_WEAPON, "CHAN_WEAPON" },
	{ CHAN_VOICE, "CHAN_VOICE" },
	{ CHAN_ITEM, "CHAN_ITEM" },
	{ CHAN_BODY, "CHAN_BODY" },
	{ CHAN_STREAM, "CHAN_STREAM" },
	{ CHAN_STATIC, "CHAN_STATIC" },
};

CSoundParametersInternal::CSoundParametersInternal()
{
	m_pConvertedNames = m_pSoundNames = NULL;
	m_nConvertedNames = m_nSoundNames = 0;
	channel			= CHAN_AUTO; // 0

	volume.start	= VOL_NORM;  // 1.0f
	volume.range	= 0.0f;

	pitch.start		= PITCH_NORM; // 100
	pitch.range		= 0;

	soundlevel.start = SNDLVL_NORM; // 75dB
	soundlevel.range = 0;
	delay_msec = 0;
	play_to_owner_only = false;
	had_missing_wave_files = false;
	uses_gender_token = false;

	m_pGameData = NULL;
}

CSoundParametersInternal::CSoundParametersInternal( const CSoundParametersInternal& src )
{
	m_pSoundNames = NULL;
	m_pConvertedNames = NULL;
	m_nSoundNames = 0;
	m_nConvertedNames = 0;
	m_pGameData = NULL;
	CopyFrom( src );
}