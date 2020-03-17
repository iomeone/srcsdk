typedef enum _fieldtypes
{
	FIELD_VOID = 0,			// No type or value
	FIELD_FLOAT,			// Any floating point value
	FIELD_STRING,			// A string ID (return from ALLOC_STRING)
	FIELD_VECTOR,			// Any vector, QAngle, or AngularImpulse
	FIELD_QUATERNION,		// A quaternion
	FIELD_INTEGER,			// Any integer or enum
	FIELD_BOOLEAN,			// boolean, implemented as an int, I may use this as a hint for compression
	FIELD_SHORT,			// 2 byte integer
	FIELD_CHARACTER,		// a byte
	FIELD_COLOR32,			// 8-bit per channel r,g,b,a (32bit color)
	FIELD_EMBEDDED,			// an embedded object with a datadesc, recursively traverse and embedded class/structure based on an additional typedescription
	FIELD_CUSTOM,			// special type that contains function pointers to it's read/write/parse functions

	FIELD_CLASSPTR,			// CBaseEntity *
	FIELD_EHANDLE,			// Entity handle
	FIELD_EDICT,			// edict_t *

	FIELD_POSITION_VECTOR,	// A world coordinate (these are fixed up across level transitions automagically)
	FIELD_TIME,				// a floating point time (these are fixed up automatically too!)
	FIELD_TICK,				// an integer tick count( fixed up similarly to time)
	FIELD_MODELNAME,		// Engine string that is a model name (needs precache)
	FIELD_SOUNDNAME,		// Engine string that is a sound name (needs precache)

	FIELD_INPUT,			// a list of inputed data fields (all derived from CMultiInputVar)
	FIELD_FUNCTION,			// A class function pointer (Think, Use, etc)

	FIELD_VMATRIX,			// a vmatrix (output coords are NOT worldspace)

	// NOTE: Use float arrays for local transformations that don't need to be fixed up.
	FIELD_VMATRIX_WORLDSPACE,// A VMatrix that maps some local space to world space (translation is fixed up on level transitions)
	FIELD_MATRIX3X4_WORLDSPACE,	// matrix3x4_t that maps some local space to world space (translation is fixed up on level transitions)

	FIELD_INTERVAL,			// a start and range floating point interval ( e.g., 3.2->3.6 == 3.2 and 0.4 )
	FIELD_MODELINDEX,		// a model index
	FIELD_MATERIALINDEX,	// a material index (using the material precache string table)
	
	FIELD_VECTOR2D,			// 2 floats
	FIELD_INTEGER64,		// 64bit integer

	FIELD_VECTOR4D,			// 4 floats

	FIELD_TYPECOUNT,		// MUST BE LAST
} fieldtype_t;

enum
{
	PC_NON_NETWORKED_ONLY = 0,
	PC_NETWORKED_ONLY,
	PC_COPYTYPE_COUNT,
	PC_EVERYTHING = PC_COPYTYPE_COUNT,
};

struct typedescription_t
{
	typedescription_t(
		fieldtype_t _fieldType = FIELD_VOID,
		const char *_fieldName = NULL,
		typedesc_offset_t _fieldOffset = {0},
		unsigned short _fieldSize = 0,
		short _flags = 0,
		const char *_externalName = NULL,
		ISaveRestoreOps *_pSaveRestoreOps = NULL,
		inputfunc_t _inputFunc = NULL,
		datamap_t *_td = NULL,
		int	 _fieldSizeInBytes = 0,
		struct typedescription_t *_override_field = NULL,
		int	 _override_count = 0,
		float _fieldTolerance = 0.0f);

	fieldtype_t			fieldType;
	const char			*fieldName;
	int					fieldOffset; // 0 == normal, 1 == packed offset
	unsigned short		fieldSize;
	short				flags;
	// the name of the variable in the map/fgd data, or the name of the action
	const char			*externalName;	
	// pointer to the function set for save/restoring of custom data types
	ISaveRestoreOps		*pSaveRestoreOps; 
	// for associating function with string names
	inputfunc_t			inputFunc; 
	// For embedding additional datatables inside this one
	datamap_t			*td;

	// Stores the actual member variable size in bytes
	int					fieldSizeInBytes;

	// FTYPEDESC_OVERRIDE point to first baseclass instance if chains_validated has occurred
	struct typedescription_t *override_field;

	// Used to track exclusion of baseclass fields
	int					override_count;
  
	// Tolerance for field errors for float fields
	float				fieldTolerance;

	// For raw fields (including children of embedded stuff) this is the flattened offset
	int					flatOffset[ TD_OFFSET_COUNT ];
	unsigned short		flatGroup;
};

inline typedescription_t::typedescription_t(
	fieldtype_t _fieldType,
	const char *_fieldName,
	typedesc_offset_t _fieldOffset,
	unsigned short _fieldSize,
	short _flags,
	const char *_externalName,
	ISaveRestoreOps *_pSaveRestoreOps,
	inputfunc_t _inputFunc,
	datamap_t *_td,
	int	 _fieldSizeInBytes,
	struct typedescription_t *_override_field,
	int	 _override_count,
	float _fieldTolerance)
{
	fieldType = _fieldType;
	fieldName = _fieldName;
	fieldOffset = _fieldOffset[0];
	fieldSize = _fieldSize;
	flags = _flags;
	externalName = _externalName;
	pSaveRestoreOps = _pSaveRestoreOps;
	inputFunc = _inputFunc;
	td = _td;
	fieldSizeInBytes = _fieldSizeInBytes;
	override_field = _override_field;
	override_count = _override_count;
	fieldTolerance = _fieldTolerance;
	flatOffset[0] = 0;
	flatOffset[1] = 0;
	flatGroup = 0;
}

struct datamap_t
{
	datamap_t(
		typedescription_t *_dataDesc = NULL,
		int					_dataNumFields = 0,
		char const *_dataClassName = NULL,
		datamap_t *_baseMap = NULL,
		bool				_chains_validated = false,
		bool				_packed_offsets_computed = false,
		int					_packed_size = 0,
	#if defined( _DEBUG )
		bool				_bValidityChecked = true
	#else
		bool				_bValidityChecked = false
	#endif
	);

	typedescription_t	*dataDesc;
	int					dataNumFields;
	char const			*dataClassName;
	datamap_t			*baseMap;

	int					m_nPackedSize;
	optimized_datamap_t	*m_pOptimizedDataMap;

#if defined( _DEBUG )
	bool				bValidityChecked;
#endif // _DEBUG
};

inline datamap_t::datamap_t(
	typedescription_t *_dataDesc,
	int					_dataNumFields,
	char const *_dataClassName,
	datamap_t *_baseMap,
	bool				_chains_validated,
	bool				_packed_offsets_computed,
	int					_packed_size,
	bool				_bValidityChecked
) {
	dataDesc = _dataDesc;
	dataNumFields = _dataNumFields;
	dataClassName = _dataClassName;
	baseMap = _baseMap;
	m_nPackedSize = _packed_size;
	m_pOptimizedDataMap = NULL;
#if defined( _DEBUG )
	bValidityChecked = _bValidityChecked;
#endif
}