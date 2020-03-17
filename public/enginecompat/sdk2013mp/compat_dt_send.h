class CStandardSendProxiesV1
{
public:
	CStandardSendProxiesV1();

	SendVarProxyFn m_Int8ToInt32;
	SendVarProxyFn m_Int16ToInt32;
	SendVarProxyFn m_Int32ToInt32;

	SendVarProxyFn m_UInt8ToInt32;
	SendVarProxyFn m_UInt16ToInt32;
	SendVarProxyFn m_UInt32ToInt32;

	SendVarProxyFn m_FloatToFloat;
	SendVarProxyFn m_VectorToVector;
};

class CStandardSendProxies : public CStandardSendProxiesV1
{
public:
	CStandardSendProxies();
	
	SendTableProxyFn m_DataTableToDataTable;
	SendTableProxyFn m_SendLocalDataTable;
	CNonModifiedPointerProxy **m_ppNonModifiedPointerProxies;
};

class CSendProxyRecipients
{
public:
	void	SetAllRecipients();					// Note: recipients are all set by default when each proxy is called.
	void	ClearAllRecipients();

	void	SetRecipient( int iClient );		// Note: these are CLIENT indices, not entity indices (so the first player's index is 0).
	void	ClearRecipient( int iClient );

	// Clear all recipients and set only the specified one.
	void	SetOnly( int iClient );
	// Set all recipients, save for the specified on
	void    ExcludeOnly( int iClient );

public:
	// Make sure we have enough room for the max possible player count
	CBitVec< ABSOLUTE_PLAYER_LIMIT >	m_Bits;
};

class SendProp
{
public:
						SendProp();
	virtual				~SendProp();

	void				Clear();

	int					GetOffset() const;
	void				SetOffset( int i );

	SendVarProxyFn		GetProxyFn() const;
	void				SetProxyFn( SendVarProxyFn f );
	
	SendTableProxyFn	GetDataTableProxyFn() const;
	void				SetDataTableProxyFn( SendTableProxyFn f );
	
	SendTable*			GetDataTable() const;
	void				SetDataTable( SendTable *pTable );

	char const*			GetExcludeDTName() const;
	
	// If it's one of the numbered "000", "001", etc properties in an array, then
	// these can be used to get its array property name for debugging.
	const char*			GetParentArrayPropName() const;
	void				SetParentArrayPropName( char *pArrayPropName );

	const char*			GetName() const;

	bool				IsSigned() const;
	
	bool				IsExcludeProp() const;
	
	bool				IsInsideArray() const;	// Returns true if SPROP_INSIDEARRAY is set.
	void				SetInsideArray();

	// Arrays only.
	void				SetArrayProp( SendProp *pProp );
	SendProp*			GetArrayProp() const;

	// Arrays only.
	void					SetArrayLengthProxy( ArrayLengthSendProxyFn fn );
	ArrayLengthSendProxyFn	GetArrayLengthProxy() const;

	int					GetNumElements() const;
	void				SetNumElements( int nElements );

	// Return the # of bits to encode an array length (must hold GetNumElements()).
	int					GetNumArrayLengthBits() const;

	int					GetElementStride() const;

	SendPropType		GetType() const;

	int					GetFlags() const;
	void				SetFlags( int flags );	

	// Some property types bind more data to the SendProp in here.
	const void*			GetExtraData() const;
	void				SetExtraData( const void *pData );

	byte 				GetPriority() const;
	void				SetPriority( byte priority );

public:

	RecvProp		*m_pMatchingRecvProp;	// This is temporary and only used while precalculating
												// data for the decoders.

	SendPropType	m_Type;
	int				m_nBits;
	float			m_fLowValue;
	float			m_fHighValue;
	
	SendProp		*m_pArrayProp;					// If this is an array, this is the property that defines each array element.
	ArrayLengthSendProxyFn	m_ArrayLengthProxy;	// This callback returns the array length.
	
	int				m_nElements;		// Number of elements in the array (or 1 if it's not an array).
	int				m_ElementStride;	// Pointer distance between array elements.

	const char *m_pExcludeDTName;			// If this is an exclude prop, then this is the name of the datatable to exclude a prop from.
	const char *m_pParentArrayPropName;

	const char		*m_pVarName;
	float			m_fHighLowMul;
	
private:

	int					m_Flags;				// SPROP_ flags.

	SendVarProxyFn		m_ProxyFn;				// NULL for DPT_DataTable.
	SendTableProxyFn	m_DataTableProxyFn;		// Valid for DPT_DataTable.
	
	SendTable			*m_pDataTable;
	
	// SENDPROP_VECTORELEM makes this negative to start with so we can detect that and
	// set the SPROP_IS_VECTOR_ELEM flag.
	int					m_Offset;

	// Extra data bound to this property.
	const void			*m_pExtraData;
};

inline byte SendProp::GetPriority() const
{
	return SENDPROP_DEFAULT_PRIORITY;
}

inline void	SendProp::SetPriority( byte priority )
{
	
}

inline CStandardSendProxiesV1::CStandardSendProxiesV1()
{
	m_Int8ToInt32 = SendProxy_Int8ToInt32;
	m_Int16ToInt32 = SendProxy_Int16ToInt32;
	m_Int32ToInt32 = SendProxy_Int32ToInt32;

	m_UInt8ToInt32 = SendProxy_UInt8ToInt32;
	m_UInt16ToInt32 = SendProxy_UInt16ToInt32;
	m_UInt32ToInt32 = SendProxy_UInt32ToInt32;
	
	m_FloatToFloat = SendProxy_FloatToFloat;
	m_VectorToVector = SendProxy_VectorToVector;
}

inline CStandardSendProxies::CStandardSendProxies()
{	
	m_DataTableToDataTable = SendProxy_DataTableToDataTable;
	m_SendLocalDataTable = SendProxy_SendLocalDataTable;
	m_ppNonModifiedPointerProxies = &s_pNonModifiedPointerProxyHead;
}