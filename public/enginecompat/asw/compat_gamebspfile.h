enum
{
	GAMELUMP_DETAIL_PROPS_VERSION = 4,
	GAMELUMP_DETAIL_PROP_LIGHTING_VERSION = 0,
	GAMELUMP_STATIC_PROPS_MIN_VERSION = 4,
	GAMELUMP_STATIC_PROPS_VERSION = 9,
	GAMELUMP_STATIC_PROP_LIGHTING_VERSION = 0,
	GAMELUMP_DETAIL_PROP_LIGHTING_HDR_VERSION = 0,
}; 

enum
{
	STATIC_PROP_NAME_LENGTH  = 128,

	// Flags field
	// These are automatically computed
	STATIC_PROP_FLAG_FADES	= 0x1,
	STATIC_PROP_USE_LIGHTING_ORIGIN	= 0x2,
	STATIC_PROP_NO_DRAW = 0x4,	// computed at run time based on dx level

	// These are set in WC
	STATIC_PROP_IGNORE_NORMALS	= 0x8,
	STATIC_PROP_NO_SHADOW	= 0x10,
	STATIC_PROP_SCREEN_SPACE_FADE	= 0x20,
	STATIC_PROP_UNUSED		= 0x20,

	STATIC_PROP_NO_PER_VERTEX_LIGHTING = 0x40,				// in vrad, compute lighting at
															// lighting origin, not for each vertex
	
	STATIC_PROP_NO_SELF_SHADOWING = 0x80,					// disable self shadowing in vrad

	STATIC_PROP_NO_PER_TEXEL_LIGHTING = 0x100,				// whether we should do per-texel lightmaps in vrad.

	STATIC_PROP_WC_MASK		= 0xd8,						// all flags settable in hammer (?)
};

struct StaticPropLump_t
{
	DECLARE_BYTESWAP_DATADESC();
	Vector			m_Origin;
	QAngle			m_Angles;
	unsigned short	m_PropType;
	unsigned short	m_FirstLeaf;
	unsigned short	m_LeafCount;
	unsigned char	m_Solid;
	unsigned char	m_Flags;
	int				m_Skin;
	float			m_FadeMinDist;
	float			m_FadeMaxDist;
	Vector			m_LightingOrigin;
	float			m_flForcedFadeScale;
	unsigned char	m_nMinCPULevel;
	unsigned char	m_nMaxCPULevel;
	unsigned char	m_nMinGPULevel;
	unsigned char	m_nMaxGPULevel;
	//	int				m_Lighting;			// index into the GAMELUMP_STATIC_PROP_LIGHTING lump
	color32			m_DiffuseModulation;	// per instance color and alpha modulation
	bool			m_bDisableX360;

    StaticPropLump_t& operator=(const StaticPropLumpV4_t& _rhs);
	StaticPropLump_t& operator=(const StaticPropLumpV5_t& _rhs);
	StaticPropLump_t& operator=(const StaticPropLumpV6_t& _rhs);
};

StaticPropLump_t& StaticPropLump_t::operator=(const StaticPropLumpV4_t& _rhs)
{
	m_Origin				= _rhs.m_Origin;
	m_Angles				= _rhs.m_Angles;
	m_PropType				= _rhs.m_PropType;
	m_FirstLeaf				= _rhs.m_FirstLeaf;
	m_LeafCount				= _rhs.m_LeafCount;
	m_Solid					= _rhs.m_Solid;
	m_Flags					= _rhs.m_Flags;
	m_Skin					= _rhs.m_Skin;
	m_FadeMinDist			= _rhs.m_FadeMinDist;
	m_FadeMaxDist			= _rhs.m_FadeMaxDist;
	m_LightingOrigin		= _rhs.m_LightingOrigin;

	// These get potentially set twice--once here and once in the caller.
	// Value judgement: This makes the code easier to work with, so unless it's a perf issue...
	m_flForcedFadeScale		= 1.0f;

	// Older versions don't want this.
	m_Flags					|= STATIC_PROP_NO_PER_TEXEL_LIGHTING;		
	return *this;
}

StaticPropLump_t& StaticPropLump_t::operator=(const StaticPropLumpV6_t& _rhs)
{
	(*this) = reinterpret_cast<const StaticPropLumpV5_t&>(_rhs);

	return *this;
}