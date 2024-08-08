#include "stdafx.h"
#include "GameInstance.h"
#include "Damage_Lost_Zombie.h"
#include "Player.h"
#include "BlackBoard_Zombie.h"
#include "Zombie.h"
#include "Body_Zombie.h"

CDamage_Lost_Zombie::CDamage_Lost_Zombie()
	: CTask_Node()
{
}

CDamage_Lost_Zombie::CDamage_Lost_Zombie(const CDamage_Lost_Zombie& rhs)
{
}

HRESULT CDamage_Lost_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

void CDamage_Lost_Zombie::Enter()
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel* pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

#ifdef _DEBUG
	cout << "Enter Damage Lost" << endl;
#endif 
}

_bool CDamage_Lost_Zombie::Execute(_float fTimeDelta)
{
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion

	MONSTER_STATE					eMonsterState = { m_pBlackBoard->Get_AI()->Get_Current_MonsterState() };
	if (MONSTER_STATE::MST_DAMAGE_LOST == eMonsterState)
	{
		_bool			isFinsihed = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY)->isFinished(static_cast<_uint>(m_eBasePlayingIndex)) };
		if (true == isFinsihed)
		{
			m_pBlackBoard->Get_AI()->Set_PoseState(CZombie::POSE_STATE::_CREEP);
			m_pBlackBoard->Get_AI()->Set_FaceState(CZombie::FACE_STATE::_DOWN);
			return false;
		}

		Play_DropSound();
	}

	else
	{
		CZombie::POSE_STATE			ePoseState = { m_pBlackBoard->Get_AI()->Get_PoseState() };
		if (CZombie::POSE_STATE::_UP != ePoseState)
			return false;

		if (false == m_pBlackBoard->Is_New_Part_Break())
			return false;

		_int			iBrokenPartType = { m_pBlackBoard->Get_New_Break_PartType() };
		if (BREAK_PART::_L_LOWER_TABIA == static_cast<BREAK_PART>(iBrokenPartType) ||
			BREAK_PART::_L_UPPER_TABIA == static_cast<BREAK_PART>(iBrokenPartType))
		{
			m_eLostType = ZOMBIE_DAMAGE_LOST_TYPE::_B_F_L;
		}

		else if (BREAK_PART::_R_LOWER_TABIA == static_cast<BREAK_PART>(iBrokenPartType) || 
			BREAK_PART::_R_UPPER_TABIA == static_cast<BREAK_PART>(iBrokenPartType))
		{
			m_eLostType = ZOMBIE_DAMAGE_LOST_TYPE::_B_F_R;
		}

		else if (BREAK_PART::_L_LOWER_FEMUR == static_cast<BREAK_PART>(iBrokenPartType) ||
			BREAK_PART::_L_UPPER_FEMUR == static_cast<BREAK_PART>(iBrokenPartType))
		{
			m_eLostType = ZOMBIE_DAMAGE_LOST_TYPE::_C_F_L;
		}

		else if (BREAK_PART::_R_LOWER_FEMUR == static_cast<BREAK_PART>(iBrokenPartType) ||
			BREAK_PART::_R_UPPER_FEMUR == static_cast<BREAK_PART>(iBrokenPartType))
		{
			m_eLostType = ZOMBIE_DAMAGE_LOST_TYPE::_C_F_R;
		}

		else
		{
			return false;
		}

		Change_Animation();

		m_pBlackBoard->Get_AI()->Set_PoseState(CZombie::POSE_STATE::_CREEP);
		m_pBlackBoard->Get_AI()->Set_FaceState(CZombie::FACE_STATE::_DOWN);
	}

	m_pBlackBoard->Organize_PreState(this);

	auto pAI = m_pBlackBoard->Get_AI();
	pAI->Set_State(MONSTER_STATE::MST_DAMAGE_LOST);

	return true;
}


void CDamage_Lost_Zombie::Exit()
{
}

void CDamage_Lost_Zombie::Change_Animation()
{
	_int			iResultAnimIndex = { -1 };

	CModel*			pBody_Model = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
	if (nullptr == pBody_Model)
		return;

	/*if (ZOMBIE_DAMAGE_LOST_TYPE::_A_F_L == m_eLostType)
	{
		iResultAnimIndex = static_cast<_int>(ANIM_DAMAGE_LOST::_A_F_L);
	}

	else if (ZOMBIE_DAMAGE_LOST_TYPE::_A_F_R == m_eLostType)
	{
		iResultAnimIndex = static_cast<_int>(ANIM_DAMAGE_LOST::_A_F_R);
	}

	else */if (ZOMBIE_DAMAGE_LOST_TYPE::_B_F_L == m_eLostType)
	{
		iResultAnimIndex = static_cast<_int>(ANIM_DAMAGE_LOST::_B_F_L);

	}

	else if (ZOMBIE_DAMAGE_LOST_TYPE::_B_F_R == m_eLostType)
	{
		iResultAnimIndex = static_cast<_int>(ANIM_DAMAGE_LOST::_B_F_R);

	}

	else if (ZOMBIE_DAMAGE_LOST_TYPE::_C_F_L == m_eLostType)
	{
		iResultAnimIndex = static_cast<_int>(ANIM_DAMAGE_LOST::_C_F_L);
	}

	else if (ZOMBIE_DAMAGE_LOST_TYPE::_C_F_R == m_eLostType)
	{
		iResultAnimIndex = static_cast<_int>(ANIM_DAMAGE_LOST::_C_F_R);
	}

	pBody_Model->Change_Animation(static_cast<_uint>(m_eBasePlayingIndex), m_strAnimLayerTag, iResultAnimIndex);
	pBody_Model->Set_Loop(static_cast<_uint>(m_eBasePlayingIndex), false);
	pBody_Model->Set_TotalLinearInterpolation(0.2f);
}

void CDamage_Lost_Zombie::Play_DropSound()
{
	CModel* pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	_float fTrackPosition = pBodyModel->Get_TrackPosition(static_cast<_uint>(m_eBasePlayingIndex));

	if (abs(fTrackPosition - 35.f) < 1.3f)
	{
		const wchar_t* str = L"Break_Drop_";
		wchar_t result[32];
		_int inum = m_pGameInstance->GetRandom_Int(10, 12);

		std::swprintf(result, sizeof(result) / sizeof(wchar_t), L"%ls%d.mp3", str, inum);

		m_pGameInstance->Change_Sound_3D(m_pBlackBoard->Get_AI()->Get_Transform(), result, (_uint)ZOMBIE_SOUND_CH::_BITE_DROP);

		if (inum == 11)
		{
			m_pGameInstance->Set_Volume_3D(m_pBlackBoard->Get_AI()->Get_Transform(), (_uint)ZOMBIE_SOUND_CH::_BITE_DROP, 0.4f);
		}
	}
}

CDamage_Lost_Zombie* CDamage_Lost_Zombie::Create(void* pArg)
{
	CDamage_Lost_Zombie* pInstance = { new CDamage_Lost_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CDamage_Lost_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDamage_Lost_Zombie::Free()
{
	__super::Free();
}
