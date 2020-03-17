struct DispNodeInfo_t
{
	enum
	{
		// Indicates if any children at all have triangles
		CHILDREN_HAVE_TRIANGLES = 0x1
	};


	// Indicates which tesselation indices are associated with a node
	unsigned short	m_FirstTesselationIndex;
	unsigned char	m_Count;
	unsigned char	m_Flags;
};
