#include "Character_Controller.h"

CCharacter_Controller::CCharacter_Controller(PxController* Controller)
{
	m_pController = Controller;
}

CCharacter_Controller::CCharacter_Controller(const CCharacter_Controller& rhs)
{
}

HRESULT CCharacter_Controller::Initialize_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CCharacter_Controller::Initialize(void* pArg)
{
	return E_NOTIMPL;
}

void CCharacter_Controller::Update()
{
	if (m_bUpdated)
	{
		
	}
}

CCharacter_Controller* CCharacter_Controller::Create()
{
	return nullptr;
}

void CCharacter_Controller::Move(PxVec3 Dir, _float fTimeDelta)
{
	// 캐릭터 이동
	if (m_pController)
		PxControllerCollisionFlags flags = m_pController->move(Dir, 0.0f, fTimeDelta, PxControllerFilters());

	//m_BodyCollider->addForce(velocity * 5.f, PxForceMode::eIMPULSE);
}

void CCharacter_Controller::Free()
{
}
