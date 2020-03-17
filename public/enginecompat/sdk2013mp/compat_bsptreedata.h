abstract_class ISpatialQuery
{
public:
	// Returns the number of leaves
	virtual int LeafCount() const = 0;

	// Enumerates the leaves along a ray, box, etc.
	virtual bool EnumerateLeavesAtPoint( Vector const& pt, ISpatialLeafEnumerator* pEnum, int context ) = 0;
	virtual bool EnumerateLeavesInBox( Vector const& mins, Vector const& maxs, ISpatialLeafEnumerator* pEnum, int context ) = 0;
	virtual bool EnumerateLeavesInSphere( Vector const& center, float radius, ISpatialLeafEnumerator* pEnum, int context ) = 0;
	virtual bool EnumerateLeavesAlongRay( Ray_t const& ray, ISpatialLeafEnumerator* pEnum, int context ) = 0;
};

abstract_class IBSPTreeData
{
public:
	// Add a virtual destructor so that the derived class destructors will
	// be called.
	virtual ~IBSPTreeData() {}

	// Initializes, shuts down
	virtual void Init( ISpatialQuery* pBSPTree ) = 0;
	virtual void Shutdown() = 0;

	// Adds and removes data from the leaf lists
	virtual BSPTreeDataHandle_t Insert( int userId, Vector const& mins, Vector const& maxs ) = 0;
	virtual void Remove( BSPTreeDataHandle_t handle ) = 0;

	// Call this when a element moves
	virtual void ElementMoved( BSPTreeDataHandle_t handle, Vector const& mins, Vector const& maxs ) = 0;

	// Enumerate elements in a particular leaf
	virtual bool EnumerateElementsInLeaf( int leaf, IBSPTreeDataEnumerator* pEnum, int context ) = 0;

	// Is the element in any leaves at all?
	virtual bool IsElementInTree( BSPTreeDataHandle_t handle ) const = 0;

	// NOTE: These methods call through to the functions in the attached
	// ISpatialQuery
	// For convenience, enumerates the leaves along a ray, box, etc.
	virtual bool EnumerateLeavesAtPoint( Vector const& pt, ISpatialLeafEnumerator* pEnum, int context ) = 0;
	virtual bool EnumerateLeavesInBox( Vector const& mins, Vector const& maxs, ISpatialLeafEnumerator* pEnum, int context ) = 0;
	virtual bool EnumerateLeavesInSphere( Vector const& center, float radius, ISpatialLeafEnumerator* pEnum, int context ) = 0;
	virtual bool EnumerateLeavesAlongRay( Ray_t const& ray, ISpatialLeafEnumerator* pEnum, int context ) = 0;
};