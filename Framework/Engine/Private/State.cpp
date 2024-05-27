#include "State.h"
#include "GameInstance.h"
#include "GameObject.h"

CState::CState()
	: m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

void CState::Enter(_float fTimeDelta)
{
}

void CState::Excute(_float fTimeDelta)
{
}

void CState::Exit(_float fTimeDelta)
{
}

void CState::Free()
{
	__super::Free();

	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}
