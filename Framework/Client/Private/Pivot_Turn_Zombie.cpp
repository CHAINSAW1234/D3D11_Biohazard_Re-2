#include "stdafx.h"
#include "Pivot_Turn_Zombie.h"

CPivot_Turn_Zombie::CPivot_Turn_Zombie()
{
}

CPivot_Turn_Zombie::CPivot_Turn_Zombie(const CPivot_Turn_Zombie& rhs)
{
}

HRESULT CPivot_Turn_Zombie::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPivot_Turn_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

void CPivot_Turn_Zombie::Enter()
{
}

void CPivot_Turn_Zombie::Execute()
{
}

void CPivot_Turn_Zombie::Exit()
{
}

void CPivot_Turn_Zombie::Change_Animation()
{
}

void CPivot_Turn_Zombie::Turn()
{
}

_bool CPivot_Turn_Zombie::Is_CurrentAnim_StartAnim(PLAYING_INDEX eIndex)
{
	return _bool();
}

_bool CPivot_Turn_Zombie::Is_CurrentAnim_LoopAnim(PLAYING_INDEX eIndex)
{
	return _bool();
}

_int CPivot_Turn_Zombie::Compute_Base_Anim()
{
	return _int();
}

CPivot_Turn_Zombie* CPivot_Turn_Zombie::Create()
{
	CPivot_Turn_Zombie* pInstance = new CPivot_Turn_Zombie();

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPivot_Turn_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPivot_Turn_Zombie::Free()
{
}
