#define SPROP_CHANGES_OFTEN		(1<<10)	// this is an often changed field, moved to head of sendtable so it gets a small index

#define SPROP_IS_A_VECTOR_ELEM	(1<<11)	// Set automatically if SPROP_VECTORELEM is used.

#define SPROP_COLLAPSIBLE		(1<<12)	// Set automatically if it's a datatable with an offset of 0 that doesn't change the pointer
										// (ie: for all automatically-chained base classes).
										// In this case, it can get rid of this SendPropDataTable altogether and spare the
										// trouble of walking the hierarchy more than necessary.

#define SPROP_COORD_MP					(1<<13) // Like SPROP_COORD, but special handling for multiplayer games
#define SPROP_COORD_MP_LOWPRECISION 	(1<<14) // Like SPROP_COORD, but special handling for multiplayer games where the fractional component only gets a 3 bits instead of 5
#define SPROP_COORD_MP_INTEGRAL			(1<<15) // SPROP_COORD_MP, but coordinates are rounded to integral boundaries

#define SPROP_CELL_COORD				SPROP_COORD_MP // Like SPROP_COORD, but special encoding for cell coordinates that can't be negative, bit count indicate maximum value
#define SPROP_CELL_COORD_LOWPRECISION 	SPROP_COORD_MP_LOWPRECISION // Like SPROP_CELL_COORD, but special handling where the fractional component only gets a 3 bits instead of 5
#define SPROP_CELL_COORD_INTEGRAL		SPROP_COORD_MP_INTEGRAL // SPROP_CELL_COORD, but coordinates are rounded to integral boundaries

#define SPROP_NUMFLAGBITS_NETWORKED		16

// This is server side only, it's used to mark properties whose SendProxy_* functions encode against gpGlobals->tickcount (the only ones that currently do this are
//  m_flAnimTime and m_flSimulationTime.  MODs shouldn't need to mess with this probably
#define SPROP_ENCODED_AGAINST_TICKCOUNT	(1<<16)

// See SPROP_NUMFLAGBITS_NETWORKED for the ones which are networked
#define SPROP_NUMFLAGBITS				17

typedef enum
{
	DPT_Int=0,
	DPT_Float,
	DPT_Vector,
	DPT_VectorXY, // Only encodes the XY of a vector, ignores Z
	DPT_String,
	DPT_Array,	// An array of the base types (can't be of datatables).
	DPT_DataTable,

	DPT_NUMSendPropTypes,

	DPT_Quaternion = DPT_Vector,
	DPT_Int64 = DPT_Int,
} SendPropType;

class DVariant
{
public:
	DVariant();
	DVariant(float val);
	
	const char *ToString();

	union
	{
		float	m_Float;
		int		m_Int;
		const char	*m_pString;
		void	*m_pData;	// For DataTables.
		float	m_Vector[3];
	};
	SendPropType	m_Type;
};

inline const char *DVariant::ToString()
{
	static char text[128];

	switch ( m_Type )
	{
		case DPT_Int : 
			Q_snprintf( text, sizeof(text), "%i", m_Int );
			break;
		case DPT_Float :
			Q_snprintf( text, sizeof(text), "%.3f", m_Float );
			break;
		case DPT_Vector :
			Q_snprintf( text, sizeof(text), "(%.3f,%.3f,%.3f)", 
				m_Vector[0], m_Vector[1], m_Vector[2] );
			break;
		case DPT_VectorXY :
			Q_snprintf( text, sizeof(text), "(%.3f,%.3f)", 
				m_Vector[0], m_Vector[1] );
			break;
		/*case DPT_Quaternion :
			Q_snprintf( text, sizeof(text), "(%.3f,%.3f,%.3f %.3f)", 
				m_Vector[0], m_Vector[1], m_Vector[2], m_Vector[3] );
			break;*/
		case DPT_String : 
			if ( m_pString ) 
				return m_pString;
			else
				return "NULL";
			break;
		case DPT_Array :
			Q_snprintf( text, sizeof(text), "Array" ); 
			break;
		case DPT_DataTable :
			Q_snprintf( text, sizeof(text), "DataTable" ); 
			break;
		default :
			Q_snprintf( text, sizeof(text), "DVariant type %i unknown", m_Type ); 
			break;
	}

	return text;
}