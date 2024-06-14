#include "stdafx.h"
#include "GameInstance.h"
#include "Is_Character_In_Range_Zombie.h"

CIs_Character_In_Range_Zombie::CIs_Character_In_Range_Zombie()
	: CDecorator_Node()
{

}

CIs_Character_In_Range_Zombie::CIs_Character_In_Range_Zombie(const CIs_Character_In_Range_Zombie& rhs)
{
}

HRESULT CIs_Character_In_Range_Zombie::Initialize_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CIs_Character_In_Range_Zombie::Initialize(void* pArg)
{
	return E_NOTIMPL;
}

CIs_Character_In_Range_Zombie* CIs_Character_In_Range_Zombie::Create()
{
	CIs_Character_In_Range_Zombie* pInstance = new CIs_Character_In_Range_Zombie();

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CIs_Character_In_Range_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CIs_Character_In_Range_Zombie::Free()
{
	__super::Free();
}
