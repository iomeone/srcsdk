struct StripHeader_t
{
	DECLARE_BYTESWAP_DATADESC();
	int numIndices;				// indexOffset offsets into the mesh's index array
	int indexOffset;

	int numVerts;				// vertexOffset offsets into the mesh's vert array
	int vertOffset;

	// Use this to enable/disable skinning.
	// May decide (in optimize.cpp) to put all with 1 bone in a different strip
	// than those that need skinning.
	short numBones;
	
	unsigned char flags;
	
	int numBoneStateChanges;
	int boneStateChangeOffset;
	inline BoneStateChangeHeader_t *pBoneStateChange( int i ) const
	{
		return (BoneStateChangeHeader_t *)(((byte *)this) + boneStateChangeOffset) + i;
	};

	// These go last on purpose!
	int numTopologyIndices;
	int topologyOffset;
};

struct StripGroupHeader_t
{
	DECLARE_BYTESWAP_DATADESC();
	// These are the arrays of all verts and indices for this mesh.  strips index into this.
	int numVerts;
	int vertOffset;
	inline Vertex_t *pVertex( int i ) const 
	{ 
		return (Vertex_t *)(((byte *)this) + vertOffset) + i; 
	};

	int numIndices;
	int indexOffset;
	inline unsigned short *pIndex( int i ) const 
	{ 
		return (unsigned short *)(((byte *)this) + indexOffset) + i; 
	};

	int numStrips;
	int stripOffset;
	inline StripHeader_t *pStrip( int i ) const 
	{ 
		return (StripHeader_t *)(((byte *)this) + stripOffset) + i; 
	};

	unsigned char flags;

	int numTopologyIndices;
	int topologyOffset;
	inline unsigned short *pTopologyIndex( int i ) const 
	{ 
		return (unsigned short *)(((byte *)this) + topologyOffset) + i; 
	};
};

struct FileHeader_t
{
	DECLARE_BYTESWAP_DATADESC();
	// file version as defined by OPTIMIZED_MODEL_FILE_VERSION
	int version;

	// hardware params that affect how the model is to be optimized.
	int vertCacheSize;
	unsigned short maxBonesPerStrip;
	unsigned short maxBonesPerFace;
	int maxBonesPerVert;

	// must match checkSum in the .mdl
	long checkSum;
	
	int numLODs; // garymcthack - this is also specified in ModelHeader_t and should match

	// one of these for each LOD
	int materialReplacementListOffset;
	MaterialReplacementListHeader_t *pMaterialReplacementList( int lodID ) const
	{ 
		MaterialReplacementListHeader_t *pDebug = 
			(MaterialReplacementListHeader_t *)(((byte *)this) + materialReplacementListOffset) + lodID;
		return pDebug;
	}

	int numBodyParts;
	int bodyPartOffset;
	inline BodyPartHeader_t *pBodyPart( int i ) const 
	{
		BodyPartHeader_t *pDebug = (BodyPartHeader_t *)(((byte *)this) + bodyPartOffset) + i;
		return pDebug;
	};	
};