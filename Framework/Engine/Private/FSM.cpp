#include "FSM.h"
#include "State.h"

CFSM::CFSM()
{
}

HRESULT CFSM::Initialize(class CState* pBaseState)
{
	if (nullptr == pBaseState)
		return E_FAIL;

	m_pCurState = pBaseState;
	m_pCurState->Enter(0.f);
	Safe_AddRef(m_pCurState);

	return S_OK;
}

void CFSM::Change_State(CState* pNextState, _float fTimeDelta)
{
	if (pNextState == m_pCurState)
		return;

	if (m_pCurState != nullptr)
	{
		m_pCurState->Exit(fTimeDelta);
		Safe_Release(m_pCurState);
	}

	m_pCurState = pNextState;
	m_pCurState->Enter(fTimeDelta);
	Safe_AddRef(m_pCurState);
}

void CFSM::Update_State(_float fTimeDelta)
{
	if (nullptr != m_pCurState)
		m_pCurState->Excute(fTimeDelta);
}

CFSM* CFSM::Create(class CState* pBaseState)
{
	CFSM* pInstance = { new CFSM() };
	if (FAILED(pInstance->Initialize(pBaseState)))
	{
		MSG_BOX(TEXT("Failed To Created : CFSM"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFSM::Free()
{
	__super::Free();

	m_pCurState->Exit(0.f);

	Safe_Release(m_pCurState);
}
