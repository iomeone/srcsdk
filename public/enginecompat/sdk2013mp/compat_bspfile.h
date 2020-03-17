#define BSPVERSION 20
#define	MAX_MAP_ENTITIES				8192

struct lump_t
{
	DECLARE_BYTESWAP_DATADESC();
	int		fileofs, filelen;
	int		version;		// default to zero
	// this field was char fourCC[4] previously, but was unused, favoring the LUMP IDs above instead. It has been
	// repurposed for compression.  0 implies the lump is not compressed.
	int		uncompressedSize; // default to zero
};

struct dbrushside_t
{
	DECLARE_BYTESWAP_DATADESC();
	unsigned short	planenum;		// facing out of the leaf
	short			texinfo;
	short			dispinfo;		// displacement info (BSPVERSION 7)
	short			bevel;			// is the side a bevel plane? (BSPVERSION 7)
};

struct dworldlight_t
{
	DECLARE_BYTESWAP_DATADESC();
	Vector		origin;
	Vector		intensity;
	Vector		normal;			// for surfaces and spotlights
	int			cluster;
	emittype_t	type;
    int			style;
	float		stopdot;		// start of penumbra for emit_spotlight
	float		stopdot2;		// end of penumbra for emit_spotlight
	float		exponent;		// 
	float		radius;			// cutoff distance
	// falloff for emit_spotlight + emit_point: 
	// 1 / (constant_attn + linear_attn * dist + quadratic_attn * dist^2)
	float		constant_attn;	
	float		linear_attn;
	float		quadratic_attn;
	int			flags;			// Uses a combination of the DWL_FLAGS_ defines.
	int			texinfo;		// 
	int			owner;			// entity that this light it relative to
};