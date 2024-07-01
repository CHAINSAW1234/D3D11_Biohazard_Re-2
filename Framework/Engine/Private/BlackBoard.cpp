#include "GameInstance.h"
#include "BlackBoard.h"
#include "Task_Node.h"

CBlackBoard::CBlackBoard()
	: m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

CBlackBoard::CBlackBoard(const CBlackBoard& rhs)
{
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

void CBlackBoard::Free()
{
	Safe_Release(m_pGameInstance);
}
