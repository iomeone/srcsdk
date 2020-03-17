//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Public interfaces to vphysics DLL
//
// $NoKeywords: $
//=============================================================================//

#ifndef VPHYSICS_INTERFACE_H
#define VPHYSICS_INTERFACE_H
#ifdef _WIN32
#pragma once
#endif


#include "tier1/interface.h"
#include "appframework/IAppSystem.h"
#include "mathlib/vector.h"
#include "mathlib/vector4d.h"
#include "vcollide.h"
#include "tier3/tier3.h"

// ------------------------------------------------------------------------------------
// UNITS:
// ------------------------------------------------------------------------------------
// NOTE:  Coordinates are in HL units.  1 unit == 1 inch.  X is east (forward), Y is north (left), Z is up (up)
// QAngle are pitch (around y), Yaw (around Z), Roll (around X)
// AngularImpulse are exponetial maps (an axis in HL units scaled by a "twist" angle in degrees)
//		They can be transformed like normals/covectors and added linearly
// mass is kg, volume is in^3, acceleration is in/s^2, velocity is in/s

// density is kg/m^3 (water ~= 998 at room temperature)
// preferably, these would be in kg/in^3, but the range of those numbers makes them not very human readable
// having water be about 1000 is really convenient for data entry.
// Since volume is in in^3 and density is in kg/m^3: 
//	density = (mass / volume) * CUBIC_METERS_PER_CUBIC_INCH
// Force is applied using impulses (kg*in/s)
// Torque is applied using impulses (kg*degrees/s)
// ------------------------------------------------------------------------------------

#define METERS_PER_INCH					(0.0254f)
#define CUBIC_METERS_PER_CUBIC_INCH		(METERS_PER_INCH*METERS_PER_INCH*METERS_PER_INCH)
// 2.2 lbs / kg
#define POUNDS_PER_KG	(2.2f)
#define KG_PER_POUND	(1.0f/POUNDS_PER_KG)

// convert from pounds to kg
#define lbs2kg(x)		((x)*KG_PER_POUND)
#define kg2lbs(x)		((x)*POUNDS_PER_KG)

const float VPHYSICS_MIN_MASS = 0.1f;
const float VPHYSICS_MAX_MASS = 5e4f;

class IPhysicsObject;
class IPhysicsEnvironment;
class IPhysicsSurfaceProps;
class IPhysicsConstraint;
class IPhysicsConstraintGroup;
class IPhysicsFluidController;
class IPhysicsSpring;
class IPhysicsVehicleController;
class IConvexInfo;
class IPhysicsObjectPairHash;
class IPhysicsCollisionSet;
class IPhysicsPlayerController;
class IPhysicsFrictionSnapshot;

struct Ray_t;
struct constraint_ragdollparams_t;
struct constraint_hingeparams_t;
struct constraint_fixedparams_t;
struct constraint_ballsocketparams_t;
struct constraint_slidingparams_t;
struct constraint_pulleyparams_t;
struct constraint_lengthparams_t;
struct constraint_groupparams_t;

struct vehicleparams_t;
struct matrix3x4_t;

struct fluidparams_t;
struct springparams_t;
struct objectparams_t;
struct debugcollide_t;
class CGameTrace;
typedef CGameTrace trace_t;
struct physics_stats_t;
struct physics_performanceparams_t;
struct virtualmeshparams_t;

//enum PhysInterfaceId_t;
struct physsaveparams_t;
struct physrestoreparams_t;
struct physprerestoreparams_t;

enum PhysInterfaceId_t 
{
	PIID_UNKNOWN,
	PIID_IPHYSICSOBJECT,
	PIID_IPHYSICSFLUIDCONTROLLER,
	PIID_IPHYSICSSPRING,
	PIID_IPHYSICSCONSTRAINTGROUP,
	PIID_IPHYSICSCONSTRAINT,
	PIID_IPHYSICSSHADOWCONTROLLER,
	PIID_IPHYSICSPLAYERCONTROLLER,
	PIID_IPHYSICSMOTIONCONTROLLER,
	PIID_IPHYSICSVEHICLECONTROLLER,
	PIID_IPHYSICSGAMETRACE,

	PIID_NUM_TYPES
};


class ISave;
class IRestore;


#define VPHYSICS_DEBUG_OVERLAY_INTERFACE_VERSION	"VPhysicsDebugOverlay001"

abstract_class IVPhysicsDebugOverlay
{
public:
	virtual void AddEntityTextOverlay(int ent_index, int line_offset, float duration, int r, int g, int b, int a, PRINTF_FORMAT_STRING const char *format, ...) = 0;
	virtual void AddBoxOverlay(const Vector& origin, const Vector& mins, const Vector& max, QAngle const& orientation, int r, int g, int b, int a, float duration) = 0;
	virtual void AddTriangleOverlay(const Vector& p1, const Vector& p2, const Vector& p3, int r, int g, int b, int a, bool noDepthTest, float duration) = 0;
	virtual void AddLineOverlay(const Vector& origin, const Vector& dest, int r, int g, int b,bool noDepthTest, float duration) = 0;
	virtual void AddTextOverlay(const Vector& origin, float duration, PRINTF_FORMAT_STRING const char *format, ...) = 0;
	virtual void AddTextOverlay(const Vector& origin, int line_offset, float duration, PRINTF_FORMAT_STRING const char *format, ...) = 0;
	virtual void AddScreenTextOverlay(float flXPos, float flYPos,float flDuration, int r, int g, int b, int a, const char *text) = 0;
	virtual void AddSweptBoxOverlay(const Vector& start, const Vector& end, const Vector& mins, const Vector& max, const QAngle & angles, int r, int g, int b, int a, float flDuration) = 0;
	virtual void AddTextOverlayRGB(const Vector& origin, int line_offset, float duration, float r, float g, float b, float alpha, PRINTF_FORMAT_STRING const char *format, ...) = 0;
};

#define VPHYSICS_INTERFACE_VERSION	"VPhysics031"

abstract_class IPhysics : public IAppSystem
{
public:
	virtual	IPhysicsEnvironment		*CreateEnvironment( void ) = 0;
	virtual void DestroyEnvironment( IPhysicsEnvironment * ) = 0;
	virtual IPhysicsEnvironment		*GetActiveEnvironmentByIndex( int index ) = 0;

	// Creates a fast hash of pairs of objects
	// Useful for maintaining a table of object relationships like pairs that do not collide.
	virtual IPhysicsObjectPairHash		*CreateObjectPairHash() = 0;
	virtual void						DestroyObjectPairHash( IPhysicsObjectPairHash *pHash ) = 0;

	// holds a cache of these by id.  So you can get by id to search for the previously created set
	// UNDONE: Sets are currently limited to 32 elements.  More elements will return NULL in create.
	// NOTE: id is not allowed to be zero.
	virtual IPhysicsCollisionSet		*FindOrCreateCollisionSet( unsigned int id, int maxElementCount ) = 0;
	virtual IPhysicsCollisionSet		*FindCollisionSet( unsigned int id ) = 0;
	virtual void						DestroyAllCollisionSets() = 0;
};


// CPhysConvex is a single convex solid
class CPhysConvex;
// CPhysPolysoup is an abstract triangle soup mesh
class CPhysPolysoup;
class ICollisionQuery;
class IVPhysicsKeyParser;
struct convertconvexparams_t;
class CPackedPhysicsDescription;

class CPolyhedron;

// UNDONE: Find a better place for this?  Should be in collisionutils, but it's needs VPHYSICS' solver.
struct truncatedcone_t
{
	Vector	origin;
	Vector	normal;
	float	h;			// height of the cone (hl units)
	float	theta;		// cone angle (degrees)
};

// this can be used to post-process a collision model
abstract_class ICollisionQuery
{
public:
	virtual ~ICollisionQuery() {}
	// number of convex pieces in the whole solid
	virtual int		ConvexCount( void ) = 0;
	// triangle count for this convex piece
	virtual int		TriangleCount( int convexIndex ) = 0;
	// get the stored game data
	virtual unsigned int GetGameData( int convexIndex ) = 0;
	// Gets the triangle's verts to an array
	virtual void	GetTriangleVerts( int convexIndex, int triangleIndex, Vector *verts ) = 0;
	
	// UNDONE: This doesn't work!!!
	virtual void	SetTriangleVerts( int convexIndex, int triangleIndex, const Vector *verts ) = 0;
	
	// returns the 7-bit material index
	virtual int		GetTriangleMaterialIndex( int convexIndex, int triangleIndex ) = 0;
	// sets a 7-bit material index for this triangle
	virtual void	SetTriangleMaterialIndex( int convexIndex, int triangleIndex, int index7bits ) = 0;
};

//-----------------------------------------------------------------------------
// Purpose: Ray traces from game engine.
//-----------------------------------------------------------------------------
abstract_class IPhysicsGameTrace
{
public:
	virtual void VehicleTraceRay( const Ray_t &ray, void *pVehicle, trace_t *pTrace ) = 0;
	virtual	void VehicleTraceRayWithWater( const Ray_t &ray, void *pVehicle, trace_t *pTrace ) = 0;
	virtual bool VehiclePointInWater( const Vector &vecPoint ) = 0;
};

// The caller should implement this to return contents masks per convex on a collide
abstract_class IConvexInfo
{
public:
	virtual unsigned int GetContents( int convexGameData ) = 0;
};

class CPhysicsEventHandler;
abstract_class IPhysicsCollisionData
{
public:
	virtual void GetSurfaceNormal( Vector &out ) = 0;		// normal points toward second object (object index 1)
	virtual void GetContactPoint( Vector &out ) = 0;		// contact point of collision (in world space)
	virtual void GetContactSpeed( Vector &out ) = 0;		// speed of surface 1 relative to surface 0 (in world space)
};


struct vcollisionevent_t
{
	IPhysicsObject	*pObjects[2];
	int				surfaceProps[2];
	bool			isCollision;
	bool			isShadowCollision;
	float			deltaCollisionTime;

	float			collisionSpeed;				// only valid at postCollision
	IPhysicsCollisionData *pInternalData;		// may change pre/post collision
};

abstract_class IPhysicsCollisionEvent
{
public:
	// returns the two objects that collided, time between last collision of these objects
	// and an opaque data block of collision information
	// NOTE: PreCollision/PostCollision ALWAYS come in matched pairs!!!
	virtual void PreCollision( vcollisionevent_t *pEvent ) = 0;
	virtual void PostCollision( vcollisionevent_t *pEvent ) = 0;

	// This is a scrape event.  The object has scraped across another object consuming the indicated energy
	virtual void Friction( IPhysicsObject *pObject, float energy, int surfaceProps, int surfacePropsHit, IPhysicsCollisionData *pData ) = 0;

	virtual void StartTouch( IPhysicsObject *pObject1, IPhysicsObject *pObject2, IPhysicsCollisionData *pTouchData ) = 0;
	virtual void EndTouch( IPhysicsObject *pObject1, IPhysicsObject *pObject2, IPhysicsCollisionData *pTouchData ) = 0;

	virtual void FluidStartTouch( IPhysicsObject *pObject, IPhysicsFluidController *pFluid ) = 0;
	virtual void FluidEndTouch( IPhysicsObject *pObject, IPhysicsFluidController *pFluid ) = 0;

	virtual void PostSimulationFrame() = 0;

	virtual void ObjectEnterTrigger( IPhysicsObject *pTrigger, IPhysicsObject *pObject ) {}
	virtual void ObjectLeaveTrigger( IPhysicsObject *pTrigger, IPhysicsObject *pObject ) {}
};


abstract_class IPhysicsObjectEvent
{
public:
	// these can be used to optimize out queries on sleeping objects
	// Called when an object is woken after sleeping
	virtual void ObjectWake( IPhysicsObject *pObject ) = 0;
	// called when an object goes to sleep (no longer simulating)
	virtual void ObjectSleep( IPhysicsObject *pObject ) = 0;
};

abstract_class IPhysicsConstraintEvent
{
public:
	// the constraint is now inactive, the game code is required to delete it or re-activate it.
	virtual void ConstraintBroken( IPhysicsConstraint * ) = 0;
};

struct hlshadowcontrol_params_t
{
	Vector			targetPosition;
	QAngle			targetRotation;
	float			maxAngular;
	float			maxDampAngular;
	float			maxSpeed;
	float			maxDampSpeed;
	float			dampFactor;
	float			teleportDistance;
};

// UNDONE: At some point allow this to be parameterized using hlshadowcontrol_params_t.
// All of the infrastructure is in place to do that.
abstract_class IPhysicsShadowController
{
public:
	virtual ~IPhysicsShadowController( void ) {}

	virtual void Update( const Vector &position, const QAngle &angles, float timeOffset ) = 0;
	virtual void MaxSpeed( float maxSpeed, float maxAngularSpeed ) = 0;
	virtual void StepUp( float height ) = 0;
	
	// If the teleport distance is non-zero, the object will be teleported to 
	// the target location when the error exceeds this quantity.
	virtual void SetTeleportDistance( float teleportDistance ) = 0;
	virtual bool AllowsTranslation() = 0;
	virtual bool AllowsRotation() = 0;

	// There are two classes of shadow objects:
	// 1) Game physics controlled, shadow follows game physics (this is the default)
	// 2) Physically controlled - shadow position is a target, but the game hasn't guaranteed that the space can be occupied by this object
	virtual void SetPhysicallyControlled( bool isPhysicallyControlled ) = 0;
	virtual bool IsPhysicallyControlled() = 0;
	virtual void GetLastImpulse( Vector *pOut ) = 0;
	virtual void UseShadowMaterial( bool bUseShadowMaterial ) = 0;
	virtual void ObjectMaterialChanged( int materialIndex ) = 0;


	//Basically get the last inputs to IPhysicsShadowController::Update(), returns last input to timeOffset in Update()
	virtual float GetTargetPosition( Vector *pPositionOut, QAngle *pAnglesOut ) = 0;
	
	virtual float GetTeleportDistance( void ) = 0;
	virtual void GetMaxSpeed( float *pMaxSpeedOut, float *pMaxAngularSpeedOut ) = 0;
};

class CPhysicsSimObject;
class IPhysicsMotionController;

// Callback for simulation
class IMotionEvent
{
public:
	// These constants instruct the simulator as to how to apply the values copied to linear & angular
	// GLOBAL/LOCAL refer to the coordinate system of the values, whereas acceleration/force determine whether or not
	// mass is divided out (forces must be divided by mass to compute acceleration)
	enum simresult_e { SIM_NOTHING = 0, SIM_LOCAL_ACCELERATION, SIM_LOCAL_FORCE, SIM_GLOBAL_ACCELERATION, SIM_GLOBAL_FORCE };
	virtual simresult_e	Simulate( IPhysicsMotionController *pController, IPhysicsObject *pObject, float deltaTime, Vector &linear, AngularImpulse &angular ) = 0;
};



abstract_class IPhysicsMotionController
{
public:
	virtual ~IPhysicsMotionController( void ) {}
	virtual void SetEventHandler( IMotionEvent *handler ) = 0;
	virtual void AttachObject( IPhysicsObject *pObject, bool checkIfAlreadyAttached ) = 0;
	virtual void DetachObject( IPhysicsObject *pObject ) = 0;

	// returns the number of objects currently attached to the controller
	virtual int CountObjects( void ) = 0;
	// NOTE: pObjectList is an array with at least CountObjects() allocated
	virtual void GetObjects( IPhysicsObject **pObjectList ) = 0;
	// detaches all attached objects
	virtual void ClearObjects( void ) = 0;
	// wakes up all attached objects
	virtual void WakeObjects( void ) = 0;

	enum priority_t
	{
		LOW_PRIORITY = 0,
		MEDIUM_PRIORITY = 1,
		HIGH_PRIORITY = 2,
	};
	virtual void SetPriority( priority_t priority ) = 0;
};

// -------------------
// Collision filter function.  Return 0 if objects should not be tested for collisions, nonzero otherwise
// Install with IPhysicsEnvironment::SetCollisionFilter()
// -------------------
abstract_class IPhysicsCollisionSolver
{
public:
	virtual int ShouldCollide( IPhysicsObject *pObj0, IPhysicsObject *pObj1, void *pGameData0, void *pGameData1 ) = 0;
	virtual int ShouldSolvePenetration( IPhysicsObject *pObj0, IPhysicsObject *pObj1, void *pGameData0, void *pGameData1, float dt ) = 0;
	
	// pObject has already done the max number of collisions this tick, should we freeze it to save CPU?
	virtual bool ShouldFreezeObject( IPhysicsObject *pObject ) = 0;

	// The system has already done too many collision checks, performance will suffer.
	// How many more should it do?
	virtual int AdditionalCollisionChecksThisTick( int currentChecksDone ) = 0;

	// This list of objects is in a connected contact graph that is too large to solve quickly
	// return true to freeze the system, false to solve it
	virtual bool ShouldFreezeContacts( IPhysicsObject **pObjectList, int objectCount ) = 0;
};

enum PhysicsTraceType_t
{
	VPHYSICS_TRACE_EVERYTHING = 0,
	VPHYSICS_TRACE_STATIC_ONLY,
	VPHYSICS_TRACE_MOVING_ONLY,
	VPHYSICS_TRACE_TRIGGERS_ONLY,
	VPHYSICS_TRACE_STATIC_AND_MOVING,
};

abstract_class IPhysicsTraceFilter
{
public:
	virtual bool ShouldHitObject( IPhysicsObject *pObject, int contentsMask ) = 0;
	virtual PhysicsTraceType_t	GetTraceType() const = 0;
};

enum callbackflags
{
	CALLBACK_GLOBAL_COLLISION	= 0x0001,
	CALLBACK_GLOBAL_FRICTION	= 0x0002,
	CALLBACK_GLOBAL_TOUCH		= 0x0004,
	CALLBACK_GLOBAL_TOUCH_STATIC = 0x0008,
	CALLBACK_SHADOW_COLLISION	= 0x0010,
	CALLBACK_GLOBAL_COLLIDE_STATIC = 0x0020,
	CALLBACK_IS_VEHICLE_WHEEL	= 0x0040,
	CALLBACK_FLUID_TOUCH		= 0x0100,
	CALLBACK_NEVER_DELETED		= 0x0200,	// HACKHACK: This means this object will never be deleted (set on the world)
	CALLBACK_MARKED_FOR_DELETE	= 0x0400,	// This allows vphysics to skip some work for this object since it will be
											// deleted later this frame. (Set automatically by destroy calls)
	CALLBACK_ENABLING_COLLISION = 0x0800,	// This is active during the time an object is enabling collisions
											// allows us to skip collisions between "new" objects and objects marked for delete
	CALLBACK_DO_FLUID_SIMULATION = 0x1000,  // remove this to opt out of fluid simulations
	CALLBACK_IS_PLAYER_CONTROLLER= 0x2000,	// HACKHACK: Set this on players until player cotrollers are unified with shadow controllers
	CALLBACK_CHECK_COLLISION_DISABLE = 0x4000,
	CALLBACK_MARKED_FOR_TEST	= 0x8000,	// debug -- marked object is being debugged
};

enum collisionhints
{
	COLLISION_HINT_DEBRIS		= 0x0001,
	COLLISION_HINT_STATICSOLID	= 0x0002,
};

abstract_class IPhysicsSpring
{
public:
	virtual ~IPhysicsSpring( void ) {}
	virtual void			GetEndpoints( Vector *worldPositionStart, Vector *worldPositionEnd ) = 0;
	virtual void			SetSpringConstant( float flSpringContant) = 0;
	virtual void			SetSpringDamping( float flSpringDamping) = 0;
	virtual void			SetSpringLength( float flSpringLenght) = 0;

	// Get the starting object
	virtual IPhysicsObject *GetStartObject( void ) = 0;

	// Get the end object
	virtual IPhysicsObject *GetEndObject( void ) = 0;
};


//-----------------------------------------------------------------------------
// Purpose: These properties are defined per-material.  This is accessible at 
//			each triangle in a collision mesh
//-----------------------------------------------------------------------------
struct surfacephysicsparams_t
{
// vphysics physical properties
	float			friction;
	float			elasticity;				// collision elasticity - used to compute coefficient of restitution
	float			density;				// physical density (in kg / m^3)
	float			thickness;				// material thickness if not solid (sheet materials) in inches
	float			dampening;
};

struct surfaceaudioparams_t
{
// sounds / audio data
	float			reflectivity;		// like elasticity, but how much sound should be reflected by this surface
	float			hardnessFactor;	// like elasticity, but only affects impact sound choices
	float			roughnessFactor;	// like friction, but only affects scrape sound choices

// audio thresholds
	float			roughThreshold;	// surface roughness > this causes "rough" scrapes, < this causes "smooth" scrapes
	float			hardThreshold;	// surface hardness > this causes "hard" impacts, < this causes "soft" impacts
	float			hardVelocityThreshold;	// collision velocity > this causes "hard" impacts, < this causes "soft" impacts
									// NOTE: Hard impacts must meet both hardnessFactor AND velocity thresholds
};

struct surfacegameprops_t
{
// game movement data
	float			maxSpeedFactor;			// Modulates player max speed when walking on this surface
	float			jumpFactor;				// Indicates how much higher the player should jump when on the surface
// Game-specific data
	unsigned short	material;
	// Indicates whether or not the player is on a ladder.
	unsigned char	climbable;
	unsigned char	pad;
};

#include "compat_vphysics_interface.h"

//-----------------------------------------------------------------------------
// Purpose: Each different material has an entry like this
//-----------------------------------------------------------------------------
struct surfacedata_t
{
	surfacephysicsparams_t	physics;	// physics parameters
	surfaceaudioparams_t	audio;		// audio parameters
	surfacesoundnames_t		sounds;		// names of linked sounds
	surfacegameprops_t		game;		// Game data / properties

	surfacesoundhandles_t		soundhandles;
};

#define VPHYSICS_SURFACEPROPS_INTERFACE_VERSION	"VPhysicsSurfaceProps001"
abstract_class IPhysicsSurfaceProps
{
public:
	virtual ~IPhysicsSurfaceProps( void ) {}

	// parses a text file containing surface prop keys
	virtual int		ParseSurfaceData( const char *pFilename, const char *pTextfile ) = 0;
	// current number of entries in the database
	virtual int		SurfacePropCount( void ) const = 0;

	virtual int		GetSurfaceIndex( const char *pSurfacePropName ) const = 0;
	virtual void	GetPhysicsProperties( int surfaceDataIndex, float *density, float *thickness, float *friction, float *elasticity ) const = 0;

	virtual surfacedata_t	*GetSurfaceData( int surfaceDataIndex ) = 0;
	virtual const char		*GetString( unsigned short stringTableIndex ) const = 0;


	virtual const char		*GetPropName( int surfaceDataIndex ) const = 0;

	// sets the global index table for world materials
	// UNDONE: Make this per-CPhysCollide
	virtual void	SetWorldMaterialIndexTable( int *pMapArray, int mapSize ) = 0;

	// NOTE: Same as GetPhysicsProperties, but maybe more convenient
	virtual void	GetPhysicsParameters( int surfaceDataIndex, surfacephysicsparams_t *pParamsOut ) const = 0;
};

abstract_class IPhysicsFluidController
{
public:
	virtual ~IPhysicsFluidController( void ) {}

	virtual void	SetGameData( void *pGameData ) = 0;
	virtual void	*GetGameData( void ) const = 0;

	virtual void	GetSurfacePlane( Vector *pNormal, float *pDist ) const = 0;
	virtual float	GetDensity() const = 0;
	virtual void	WakeAllSleepingObjects() = 0;
	virtual int		GetContents() const = 0;
};


//-----------------------------------------------------------------------------
// Purpose: parameter block for creating fluid dynamic motion
// UNDONE: Expose additional fluid model paramters?
//-----------------------------------------------------------------------------
struct fluidparams_t
{
	Vector4D	surfacePlane;	// x,y,z normal, dist (plane constant) fluid surface
	Vector		currentVelocity; // velocity of the current in inches/second
	float		damping;		// damping factor for buoyancy (tweak)
	float		torqueFactor;
	float		viscosityFactor;
	void		*pGameData;
	bool		useAerodynamics;// true if this controller should calculate surface pressure
	int			contents;

	fluidparams_t() {}
	fluidparams_t( fluidparams_t const& src )
	{
		Vector4DCopy( src.surfacePlane, surfacePlane );
		VectorCopy( src.currentVelocity, currentVelocity );
		damping = src.damping;
		torqueFactor = src.torqueFactor;
		viscosityFactor = src.viscosityFactor;
		contents = src.contents;
	}
};

//-----------------------------------------------------------------------------
// Purpose: parameter block for creating linear springs
// UNDONE: Expose additional spring model paramters?
//-----------------------------------------------------------------------------
struct springparams_t
{
	springparams_t()
	{
		memset( this, 0, sizeof(*this) );
	}
	float	constant;		// spring constant
	float	naturalLength;// relaxed length
	float	damping;		// damping factor
	float	relativeDamping;	// relative damping (damping proportional to the change in the relative position of the objects)
	Vector	startPosition;
	Vector	endPosition;
	bool	useLocalPositions;	// start & end Position are in local space to start and end objects if this is true
	bool	onlyStretch;		// only apply forces when the length is greater than the natural length
};

//-----------------------------------------------------------------------------
// Purpose: parameter block for creating polygonal objects
//-----------------------------------------------------------------------------
struct objectparams_t
{
	Vector		*massCenterOverride;
	float		mass;
	float		inertia;
	float		damping;
	float		rotdamping;
	float		rotInertiaLimit;
	const char	*pName;				// used only for debugging
	void		*pGameData;
	float		volume;
	float		dragCoefficient;
	bool		enableCollisions;
};

//-----------------------------------------------------------------------------
// Physics interface IDs
//
// Note that right now the order of the enum also defines the order of save/load


//-----------------------------------------------------------------------------
// Purpose: parameter blocks for save and load operations
//-----------------------------------------------------------------------------
struct physsaveparams_t
{
	ISave 				*pSave;
	void 				*pObject;
	PhysInterfaceId_t 	type;
};

struct physrestoreparams_t
{
	IRestore 			*pRestore;
	void 				**ppObject;
	PhysInterfaceId_t 	type;
	void 				*pGameData;
	const char			*pName;				// used only for debugging
	const CPhysCollide 	*pCollisionModel;
	IPhysicsEnvironment *pEnvironment;
	IPhysicsGameTrace	*pGameTrace;
};

struct physrecreateparams_t
{
	void *pOldObject;
	void *pNewObject;
};

struct physprerestoreparams_t
{
	int recreatedObjectCount;
	physrecreateparams_t recreatedObjectList[1];
};

//-------------------------------------

#define DEFINE_PIID( type, enumval ) \
	template <> inline PhysInterfaceId_t GetPhysIID<type>( type ** ) { return enumval; }

template <class PHYSPTR> inline PhysInterfaceId_t GetPhysIID(PHYSPTR **); // will get link error if no match

DEFINE_PIID( IPhysicsObject, 			PIID_IPHYSICSOBJECT );
DEFINE_PIID( IPhysicsFluidController, 	PIID_IPHYSICSFLUIDCONTROLLER );
DEFINE_PIID( IPhysicsSpring, 			PIID_IPHYSICSSPRING );
DEFINE_PIID( IPhysicsConstraintGroup, 	PIID_IPHYSICSCONSTRAINTGROUP );
DEFINE_PIID( IPhysicsConstraint, 		PIID_IPHYSICSCONSTRAINT );
DEFINE_PIID( IPhysicsShadowController, 	PIID_IPHYSICSSHADOWCONTROLLER );
DEFINE_PIID( IPhysicsPlayerController,	PIID_IPHYSICSPLAYERCONTROLLER );
DEFINE_PIID( IPhysicsMotionController,	PIID_IPHYSICSMOTIONCONTROLLER );
DEFINE_PIID( IPhysicsVehicleController,	PIID_IPHYSICSVEHICLECONTROLLER );
DEFINE_PIID( IPhysicsGameTrace,			PIID_IPHYSICSGAMETRACE );

//-----------------------------------------------------------------------------

#endif // VPHYSICS_INTERFACE_H
