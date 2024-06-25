#include "GameInstance.h"
#include "Decorator_Node.h"

CDecorator_Node::CDecorator_Node(): CNode()
{

}

CDecorator_Node::CDecorator_Node(const CDecorator_Node& rhs)
{
}

HRESULT CDecorator_Node::Initialize(void* pArg)
{
	return S_OK;
}

CDecorator_Node* CDecorator_Node::Create(void* pArg)
{
	CDecorator_Node* pInstance = new CDecorator_Node();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CDecorator_Node"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDecorator_Node::Free()
{
	Safe_Release(m_pGameInstance);
}
