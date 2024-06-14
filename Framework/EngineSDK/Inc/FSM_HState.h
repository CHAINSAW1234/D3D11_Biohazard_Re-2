#pragma once
#include "Base.h"

BEGIN(Engine)
class CFSM_State;

class ENGINE_DLL CFSM_HState abstract : public CBase
{
	typedef unordered_map<_uint, CFSM_State*> MAP_FSM_STATE;

protected:
	CFSM_HState();
	virtual ~CFSM_HState() = default;

public:
	virtual void						OnStateEnter();
	virtual void						OnStateUpdate(_float fTimeDelta);
	virtual void						OnStateExit();
	virtual void						Start();

	virtual void						Change_State(_uint iState);
	void								Reset_State();
	void								Add_State(_uint iState, CFSM_State* pInitState);
	CFSM_State*							Find_State(_uint iState);

protected:
	class CGameInstance*				m_pGameInstance = { nullptr };
	CFSM_State*							m_pCurrent_State = { nullptr };
	MAP_FSM_STATE						m_mapFSM_State;

public:
	virtual void Free() override;
};

END

