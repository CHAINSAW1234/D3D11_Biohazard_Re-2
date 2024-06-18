#include "GameInstance.h"
#include "BlackBoard.h"
#include "Task_Node.h"

CBlackBoard::CBlackBoard(): m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

CBlackBoard::CBlackBoard(const CBlackBoard& rhs)
{
}

HRESULT CBlackBoard::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBlackBoard::Initialize(void* pArg)
{
	return S_OK;
}

void CBlackBoard::Organize_PreState(CTask_Node* pCurrentNode)
{
	if (nullptr == pCurrentNode)
		return;

	if (m_pPreTaskNode != pCurrentNode)
	{
		if (nullptr != m_pPreTaskNode)
		{
			m_pPreTaskNode->Exit();
		}

		m_pPreTaskNode = pCurrentNode;

		m_pPreTaskNode->Enter();
	}
}

CBlackBoard* CBlackBoard::Create()
{
	CBlackBoard* pInstance = new CBlackBoard();

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBlackBoard"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBlackBoard::Free()
{
	Safe_Release(m_pGameInstance);
}
