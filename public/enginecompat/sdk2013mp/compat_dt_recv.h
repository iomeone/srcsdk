class CStandardRecvProxies
{
public:
	CStandardRecvProxies();

	RecvVarProxyFn m_Int32ToInt8;
	RecvVarProxyFn m_Int32ToInt16;
	RecvVarProxyFn m_Int32ToInt32;
	RecvVarProxyFn m_FloatToFloat;
	RecvVarProxyFn m_VectorToVector;
};

inline CStandardRecvProxies::CStandardRecvProxies()
{
	m_Int32ToInt8 = RecvProxy_Int32ToInt8;
	m_Int32ToInt16 = RecvProxy_Int32ToInt16;
	m_Int32ToInt32 = RecvProxy_Int32ToInt32;
	m_FloatToFloat = RecvProxy_FloatToFloat;
	m_VectorToVector = RecvProxy_VectorToVector;
}