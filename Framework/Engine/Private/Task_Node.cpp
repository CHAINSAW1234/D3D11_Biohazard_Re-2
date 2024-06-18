#include "GameInstance.h"
#include "Task_Node.h"

CTask_Node::CTask_Node(): m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

CTask_Node::CTask_Node(const CTask_Node& rhs)
{
}

HRESULT CTask_Node::Initialize_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CTask_Node::Initialize(void* pArg)
{
	return E_NOTIMPL;
}

void CTask_Node::Free()
{
	Safe_Release(m_pGameInstance);
}
