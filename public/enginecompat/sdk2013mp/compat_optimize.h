struct StripHeader_t
{
	DECLARE_BYTESWAP_DATADESC();
	// indexOffset offsets into the mesh's index array.
	int numIndices;
	int indexOffset;

	// vertexOffset offsets into the mesh's vert array.
	int numVerts;
	int vertOffset;

	// use this to enable/disable skinning.  
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
};

struct FileHeader_t
{
	DECLARE_BYTESWAP_DATADESC();
	// file version as defined by OPTIMIZED_MODEL_FILE_VERSION
	int version;

	// hardware params that affect how the model is to be optimized.
	int vertCacheSize;
	unsigned short maxBonesPerStrip;
	unsigned short maxBonesPerTri;
	int maxBonesPerVert;

	// must match checkSum in the .mdl
	int checkSum;
	
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