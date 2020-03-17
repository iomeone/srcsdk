#include "cbase.h"
#include "c_baseentity.h"
#include "tier0/memdbgon.h"

class C_TriggerPortal : public C_BaseEntity
{
	private:
		DECLARE_CLASS(C_TriggerPortal, C_BaseEntity)
		DECLARE_CLIENTCLASS()

	public:
		C_TriggerPortal() {}
		virtual ~C_TriggerPortal() {}

	private:
		EHANDLE m_hRemotePortal;
};

IMPLEMENT_CLIENTCLASS(C_TriggerPortal, DT_TriggerPortal, CTriggerPortal)

BEGIN_RECV_TABLE(C_TriggerPortal, DT_TriggerPortal)
	RecvPropEHandle(RECVINFO(m_hRemotePortal)),
END_RECV_TABLE()