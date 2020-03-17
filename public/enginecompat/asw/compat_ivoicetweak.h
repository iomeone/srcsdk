typedef struct IVoiceTweak_s
{
	// These turn voice tweak mode on and off. While in voice tweak mode, the user's voice is echoed back
	// without sending to the server. 
	int				(*StartVoiceTweakMode)();	// Returns 0 on error.
	void			(*EndVoiceTweakMode)();
	
	// Get/set control values.
	void			(*SetControlFloat)(VoiceTweakControl iControl, float value);
	float			(*GetControlFloat)(VoiceTweakControl iControl);

	bool			(*IsStillTweaking)(); // This can return false if the user restarts the sound system during voice tweak mode

	bool			(*IsControlFound)(VoiceTweakControl iControl);
} IVoiceTweak; 
