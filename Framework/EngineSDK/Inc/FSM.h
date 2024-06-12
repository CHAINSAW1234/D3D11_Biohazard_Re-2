#pragma once
#include "Component.h"

BEGIN(Engine)
class CFSM_HState;

class ENGINE_DLL CFSM final : public CComponent
{
	typedef unordered_map<_uint, CFSM_HState*>	MAP_FSM_HSTATE;

private:
	CFSM(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFSM(const CFSM& rhs);
	virtual ~CFSM() = default;

public:
	virtual								HRESULT Initialize_Prototype();
	virtual								HRESULT Initialize(void* pArg);
	void								Update(_float fTimeDelta);
	void								Start();

	void								Change_State(_uint iState);
	void								Add_State(_uint iState, CFSM_HState* pInitState);
	CFSM_HState*						Find_State(_uint iState);

private:
	CFSM_HState*						m_pCurrent_State = { nullptr };
	MAP_FSM_HSTATE						m_mapFSM_State;

public:
	static CFSM*						Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent*					Clone(void* pArg);
	virtual void						Free() override;
};

END