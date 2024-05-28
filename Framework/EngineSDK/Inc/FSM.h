#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CFSM final : public CBase
{
private:
	CFSM();
	virtual ~CFSM() = default;

public:
	HRESULT								Initialize(class CState* pBaseState);

public:
	void								Change_State(class CState* pNextState, _float fTimeDelta);
	void								Update_State(_float fTimeDelta);

private:
	class CState*						m_pCurState = { nullptr };

public:
	static CFSM*						Create(class CState* pBaseState);
	virtual void						Free() override;
};

END