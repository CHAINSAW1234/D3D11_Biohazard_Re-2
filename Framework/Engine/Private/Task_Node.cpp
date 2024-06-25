#include "GameInstance.h"
#include "Task_Node.h"
#include "Decorator_Node.h"

CTask_Node::CTask_Node(): CNode()
{
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

_bool CTask_Node::Execute()
{
	return true;
}

void CTask_Node::Free()
{
	__super::Free();
}
