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

void CCharacter_Controller::Free()
{
}
