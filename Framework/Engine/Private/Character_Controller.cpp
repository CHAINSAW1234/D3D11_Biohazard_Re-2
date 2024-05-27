#include "Character_Controller.h"
#include "Event_Call_Back.h"

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
	QueryFilterCallback filterCallback;
	PxControllerFilters controllerFilters;
	controllerFilters.mFilterCallback = &filterCallback;
	// 캐릭터 이동
	if (m_pController)
		PxControllerCollisionFlags flags = m_pController->move(Dir, 0.0f, fTimeDelta, controllerFilters);

	//m_BodyCollider->addForce(velocity * 5.f, PxForceMode::eIMPULSE);
}

_float4 CCharacter_Controller::GetTranslation()
{
	PxExtendedVec3 position = m_pController->getPosition();

	_float4 Pos;
	Pos.x = static_cast<_float>(position.x);
	Pos.y = static_cast<_float>(position.y)-1.f;
	Pos.z = static_cast<_float>(position.z);
	Pos.w = 1.f;
	return Pos;
}

void CCharacter_Controller::Release_Px()
{
	if (m_pController)
		m_pController->release();
	m_pController = nullptr;
}

void CCharacter_Controller::Free()
{

}
