inline void CBaseHandle::Init( int iEntry, int iSerialNumber )
{
	Assert( iEntry >= 0 && (iEntry & ENT_ENTRY_MASK) == iEntry);
	Assert( iSerialNumber >= 0 && iSerialNumber < (1 << NUM_SERIAL_NUM_BITS) );

	m_Index = iEntry | (iSerialNumber << NUM_SERIAL_NUM_SHIFT_BITS);
}

inline int CBaseHandle::GetSerialNumber() const
{
	return m_Index >> NUM_SERIAL_NUM_SHIFT_BITS;
}