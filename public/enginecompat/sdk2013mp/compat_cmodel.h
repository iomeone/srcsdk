struct Ray_t
{
	VectorAligned  m_Start;	// starting point, centered within the extents
	VectorAligned  m_Delta;	// direction + length of the ray
	VectorAligned  m_StartOffset;	// Add this to m_Start to get the actual ray start
	VectorAligned  m_Extents;	// Describes an axis aligned box extruded along a ray
	bool	m_IsRay;	// are the extents zero?
	bool	m_IsSwept;	// is delta != 0?

	Ray_t();

	void Init( Vector const& start, Vector const& end );
	void Init( Vector const& start, Vector const& end, Vector const& mins, Vector const& maxs );
	Vector InvDelta() const;

private:
};

inline Ray_t::Ray_t()
{

}

inline void Ray_t::Init( Vector const& start, Vector const& end )
{
	Assert( &end );
	VectorSubtract( end, start, m_Delta );

	m_IsSwept = (m_Delta.LengthSqr() != 0);

	VectorClear( m_Extents );
	m_IsRay = true;

	// Offset m_Start to be in the center of the box...
	VectorClear( m_StartOffset );
	VectorCopy( start, m_Start );
}

inline void Ray_t::Init( Vector const& start, Vector const& end, Vector const& mins, Vector const& maxs )
{
	Assert( &end );
	VectorSubtract( end, start, m_Delta );

	m_IsSwept = (m_Delta.LengthSqr() != 0);

	VectorSubtract( maxs, mins, m_Extents );
	m_Extents *= 0.5f;
	m_IsRay = (m_Extents.LengthSqr() < 1e-6);

	// Offset m_Start to be in the center of the box...
	VectorAdd( mins, maxs, m_StartOffset );
	m_StartOffset *= 0.5f;
	VectorAdd( start, m_StartOffset, m_Start );
	m_StartOffset *= -1.0f;
}