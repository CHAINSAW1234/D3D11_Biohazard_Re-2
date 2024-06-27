#include "GameInstance.h"
#include "Task_Node.h"
#include "Decorator_Node.h"

CTask_Node::CTask_Node(): CNode()
{
}

CTask_Node::CTask_Node(const CTask_Node& rhs)
{
}

HRESULT CTask_Node::Initialize(void* pArg)
{
	return S_OK;
}

_bool CTask_Node::Execute()
{
	return true;
}

void CTask_Node::Free()
{
	__super::Free();
}
