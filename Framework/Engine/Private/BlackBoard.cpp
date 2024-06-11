#include "GameInstance.h"
#include "BlackBoard.h"

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
