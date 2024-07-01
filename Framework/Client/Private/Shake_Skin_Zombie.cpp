#include "stdafx.h"
#include "GameInstance.h"
#include "Shake_Skin_Zombie.h"
#include "Player.h"
#include "BlackBoard_Zombie.h"
#include "Zombie.h"
#include "Body_Zombie.h"

CShake_Skin_Zombie::CShake_Skin_Zombie()
	: CTask_Node()
{
}

CShake_Skin_Zombie::CShake_Skin_Zombie(const CShake_Skin_Zombie& rhs)
{
}

HRESULT CShake_Skin_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

void CShake_Skin_Zombie::Enter()
{

}

_bool CShake_Skin_Zombie::Execute(_float fTimeDelta)
{
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion

	m_pBlackBoard->Organize_PreState(this);

	auto pAI = m_pBlackBoard->Get_AI();
	pAI->SetState(MONSTER_STATE::MST_IDLE);
	Change_Animation();

	return true;
}

void CShake_Skin_Zombie::Exit()
{
}

void CShake_Skin_Zombie::Change_Animation()
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel*			pBody_Model = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
	if (nullptr == pBody_Model)
		return;

#pragma region TEST Add Mask Anim

	/*static			_uint iNum = { 3 };

	for (_uint i = 3; i < 9; ++i)
	{
		m_pModelCom->Set_BlendWeight(i, 0.f, 0.f);
	}

	switch (iNum)
	{
	case 3:
	{
		m_pModelCom->Change_Animation(3, TEXT("Add_Leg_L"), static_cast<_uint>(ANIM_ADD_LEG_L::_FRONT));
		m_pModelCom->Set_BoneLayer_Playinnfo(3, BONE_LAYER_L_LEG_TWIST_TAG);
		m_pModelCom->Set_BlendWeight(3, 100.f, 0.f);
		m_pModelCom->Set_Loop(3, true);
		break;
	}
	case 4:
	{
		m_pModelCom->Change_Animation(4, TEXT("Add_Leg_R"), static_cast<_uint>(ANIM_ADD_LEG_R::_FRONT));
		m_pModelCom->Set_BoneLayer_PlayingInfo(4, BONE_LAYER_R_LEG_TWIST_TAG);
		m_pModelCom->Set_BlendWeight(4, 100.f, 0.f);
		m_pModelCom->Set_Loop(4, true);
		break;
	}
	case 5:
	{
		m_pModelCom->Change_Animation(5, TEXT("Add_Arm_L"), static_cast<_uint>(ANIM_ADD_ARM_L::_FRONT));
		m_pModelCom->Set_BoneLayer_PlayingInfo(5, BONE_LAYER_L_ARM_TWIST_TAG);
		m_pModelCom->Set_BlendWeight(5, 100.f, 0.f);
		m_pModelCom->Set_Loop(5, true);
		break;
	}

	case 6:
	{
		m_pModelCom->Change_Animation(6, TEXT("Add_Arm_R"), static_cast<_uint>(ANIM_ADD_ARM_R::_FRONT));
		m_pModelCom->Set_BoneLayer_PlayingInfo(6, BONE_LAYER_R_ARM_TWIST_TAG);
		m_pModelCom->Set_BlendWeight(6, 100.f, 0.f);
		m_pModelCom->Set_Loop(6, true);
		break;
	}

	case 7:
	{
		m_pModelCom->Change_Animation(7, TEXT("Add_Shoulder_L"), static_cast<_uint>(ANIM_ADD_SHOULDER_L::_FRONT));
		m_pModelCom->Set_BoneLayer_PlayingInfo(7, BONE_LAYER_L_SHOULDER_TWIST_TAG);
		m_pModelCom->Set_BlendWeight(7, 100.f, 0.f);
		m_pModelCom->Set_Loop(7, true);
		break;
	}

	case 8:
	{
		m_pModelCom->Change_Animation(8, TEXT("Add_Shoulder_R"), static_cast<_uint>(ANIM_ADD_SHOULDER_R::_FRONT));
		m_pModelCom->Set_BoneLayer_PlayingInfo(8, BONE_LAYER_R_SHOULDER_TWIST_TAG);
		m_pModelCom->Set_BlendWeight(8, 100.f, 0.f);
		m_pModelCom->Set_Loop(8, true);
		break;
	}
	}*/

#pragma endregion
}

CShake_Skin_Zombie* CShake_Skin_Zombie::Create(void* pArg)
{
	CShake_Skin_Zombie* pInstance = { new CShake_Skin_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CShake_Skin_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShake_Skin_Zombie::Free()
{
	__super::Free();
}
