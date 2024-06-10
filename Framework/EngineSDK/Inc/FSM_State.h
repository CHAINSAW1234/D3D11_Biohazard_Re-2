#pragma once
#include "Base.h"


BEGIN(Engine)

class ENGINE_DLL CFSM_State abstract : public CBase
{
protected:
	CFSM_State();
	virtual ~CFSM_State() = default;

public:
	// 상태 진입했을 때 처음만 호출
	virtual void OnStateEnter() = 0;
	// 상태 진입되어 있는 상태에서 매 tick마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) = 0;
	// 상태가 변경될 때 호출
	virtual void OnStateExit() = 0;
	// 상태가 렌더링되는 특이 경우에 대비해서 새로 만듬
	virtual void Start();

protected:
	class CGameInstance* m_pGameInstance = { nullptr };

public:
	virtual void Free() override;
};

END

