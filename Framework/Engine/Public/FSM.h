#pragma once
#include "Component.h"

BEGIN(Engine)
class CFSM_State;

class ENGINE_DLL CFSM final : public CComponent
{
	typedef unordered_map<_uint, CFSM_State*>	MAP_FSM_STATE;

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
	void								Add_State(_uint iState, CFSM_State* pInitState);
	CFSM_State*							Find_State(_uint iState);

private:
	CFSM_State*							m_pCurrent_State = { nullptr };
	MAP_FSM_STATE						m_mapFSM_State;

public:
	static CFSM*						Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent*					Clone(void* pArg);
	virtual void						Free() override;
};

END