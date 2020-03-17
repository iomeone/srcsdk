inline void CBaseHandle::Init( int iEntry, int iSerialNumber )
{
	Assert( iEntry >= 0 && iEntry < NUM_ENT_ENTRIES );
	Assert( iSerialNumber >= 0 && iSerialNumber < (1 << NUM_SERIAL_NUM_BITS) );

	m_Index = iEntry | (iSerialNumber << NUM_ENT_ENTRY_BITS);
}

inline int CBaseHandle::GetSerialNumber() const
{
	return m_Index >> NUM_ENT_ENTRY_BITS;
}