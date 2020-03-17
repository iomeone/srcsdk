struct solid_t
{
	int		index;
	char	name[512];
	char	parent[512];
	char	surfaceprop[512];
	Vector	massCenterOverride;
	objectparams_t params;
};

struct ragdollanimatedfriction_t
{
	float					flFrictionTimeIn;
	float					flFrictionTimeOut;
	float					flFrictionTimeHold;
	int						iMinAnimatedFriction;
	int						iMaxAnimatedFriction;
};

class IVPhysicsKeyParser
{
public:
	virtual ~IVPhysicsKeyParser() {}

	virtual const char *GetCurrentBlockName( void ) = 0;
	virtual bool		Finished( void ) = 0;
	virtual void		ParseSolid( solid_t *pSolid, IVPhysicsKeyHandler *unknownKeyHandler ) = 0;
	virtual void		ParseFluid( fluid_t *pFluid, IVPhysicsKeyHandler *unknownKeyHandler ) = 0;
	virtual void		ParseRagdollConstraint( constraint_ragdollparams_t *pConstraint, IVPhysicsKeyHandler *unknownKeyHandler ) = 0;
	virtual void		ParseSurfaceTable( int *table, IVPhysicsKeyHandler *unknownKeyHandler ) = 0;
	virtual void		ParseCustom( void *pCustom, IVPhysicsKeyHandler *unknownKeyHandler ) = 0;
	virtual void		ParseVehicle( vehicleparams_t *pVehicle, IVPhysicsKeyHandler *unknownKeyHandler ) = 0;
	virtual void		SkipBlock( void ) = 0;
};