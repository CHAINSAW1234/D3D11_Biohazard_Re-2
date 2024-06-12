#include "FSM_HState.h"
#include "FSM_State.h"
#include "GameInstance.h"

CFSM_HState::CFSM_HState()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}

void CFSM_HState::OnStateEnter()
{
	if (m_pCurrent_State == nullptr)
		return;

	m_pCurrent_State->OnStateEnter();
}

void CFSM_HState::OnStateUpdate(_float fTimeDelta)
{
	if (m_pCurrent_State == nullptr)
		return;

	m_pCurrent_State->OnStateUpdate(fTimeDelta);

}

void CFSM_HState::OnStateExit()
{
	if (m_pCurrent_State == nullptr)
		return;

	m_pCurrent_State->OnStateExit();
}

void CFSM_HState::Start()
{
	for (auto& mapFSM_State : m_mapFSM_State) {
		mapFSM_State.second->Start();
	}
}

void CFSM_HState::Change_State(_uint iState)
{
	auto pNextState = Find_State(iState);

	if (nullptr == pNextState || m_pCurrent_State == pNextState)
		return;

	// ====== 상태가 변경되었다면 ======
	// 이전 상태 Terminate
	if (m_pCurrent_State != nullptr)
		m_pCurrent_State->OnStateExit();

	// 현재 상태 변경
	Safe_Release(m_pCurrent_State);
	m_pCurrent_State = pNextState;
	Safe_AddRef(m_pCurrent_State);
	// 바뀐 상태의 Execute 
	m_pCurrent_State->OnStateEnter();
}

void CFSM_HState::Add_State(_uint iState, CFSM_State* pInitState)
{
	if (nullptr == pInitState) {
		return;
	}
	m_mapFSM_State.emplace(iState, pInitState);
}

CFSM_State* CFSM_HState::Find_State(_uint iState)
{
	auto iter = m_mapFSM_State.find(iState);
	if (iter == m_mapFSM_State.end()) return nullptr;
	return iter->second;
}

void CFSM_HState::Free()
{
	__super::Free();
	Safe_Release(m_pGameInstance);

	Safe_Release(m_pCurrent_State);
	for (auto& pFSMstate : m_mapFSM_State)
		Safe_Release(pFSMstate.second);

	m_mapFSM_State.clear();
}
