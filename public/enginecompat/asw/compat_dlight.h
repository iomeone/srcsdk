struct dlight_t
{
	int		flags;
	Vector	origin;
	float	radius;
	ColorRGBExp32	color;		// Light color with exponent
	float	die;				// stop lighting after this time
	float	decay;				// drop this each second
	float	minlight;			// don't add when contributing less
	int		key;
	int		style;				// lightstyle

	// For spotlights. Use m_OuterAngle == 0 for point lights
	Vector	m_Direction;		// center of the light cone
	float	m_InnerAngle;
	float	m_OuterAngle;

	// If this ptr is set, the dlight will only affect this particular client renderable 
	const IClientRenderable* m_pExclusiveLightReceiver;

	dlight_t();

	float GetRadius() const;
	float GetRadiusSquared() const;
	float IsRadiusGreaterThanZero() const;
};

inline dlight_t::dlight_t()
	: m_pExclusiveLightReceiver( NULL )
{
	
}
