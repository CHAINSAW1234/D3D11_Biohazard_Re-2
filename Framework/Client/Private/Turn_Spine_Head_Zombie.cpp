#include "stdafx.h"
#include "GameInstance.h"
#include "Turn_Spine_Head_Zombie.h"
#include "Player.h"
#include "BlackBoard_Zombie.h"
#include "Zombie.h"
#include "Body_Zombie.h"

CTurn_Spine_Head_Zombie::CTurn_Spine_Head_Zombie()
	: CTask_Node()
{
}

CTurn_Spine_Head_Zombie::CTurn_Spine_Head_Zombie(const CTurn_Spine_Head_Zombie& rhs)
{
}

HRESULT CTurn_Spine_Head_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

void CTurn_Spine_Head_Zombie::Enter()
{

}

_bool CTurn_Spine_Head_Zombie::Execute(_float fTimeDelta)
{
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion
	/*CZombie* pZombie = { m_pBlackBoard->Get_AI() };
	if (nullptr == pZombie)
		return false;

	Update_BlendWeights();

	HIT_TYPE			eCurrentHitType = { pZombie->Get_Current_HitType() };
	if (HIT_TYPE::HIT_END == eCurrentHitType)
		return false;

	_float3				vHitDirectionLocalFloat3 = {};
	if (false == m_pBlackBoard->Compute_Direction_From_Hit_Local(&vHitDirectionLocalFloat3))
		return false;

	COLLIDER_TYPE		eIntersectCollider = { pZombie->Get_Current_IntersectCollider() };
	if (COLLIDER_TYPE::_END == eIntersectCollider ||
		COLLIDER_TYPE::HEAD == eIntersectCollider ||
		COLLIDER_TYPE::PELVIS == eIntersectCollider)
		return false;

	DIRECTION			eHitDirection = { vHitDirectionLocalFloat3.z > 0.f ? DIRECTION::_F : DIRECTION::_B };

	Add_Blend_Animation(eIntersectCollider, eHitDirection);*/

	return true;
}

void CTurn_Spine_Head_Zombie::Exit()
{
}

CTurn_Spine_Head_Zombie* CTurn_Spine_Head_Zombie::Create(void* pArg)
{
	CTurn_Spine_Head_Zombie* pInstance = { new CTurn_Spine_Head_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CTurn_Spine_Head_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTurn_Spine_Head_Zombie::Free()
{
	__super::Free();
}
