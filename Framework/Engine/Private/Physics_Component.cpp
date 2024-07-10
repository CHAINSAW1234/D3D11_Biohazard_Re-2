#include "Physics_Component.h"
#include "GameInstance.h"

CPhysics_Component::CPhysics_Component()
{
	m_pGameInstance = CGameInstance::Get_Instance();
}

CPhysics_Component::CPhysics_Component(const CPhysics_Component& rhs)
{
}

HRESULT CPhysics_Component::Initialize_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CPhysics_Component::Initialize(void* pArg)
{
	return E_NOTIMPL;
}

CPhysics_Component* CPhysics_Component::Create()
{
	return nullptr;
}

void CPhysics_Component::Free()
{
}
