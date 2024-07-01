#include "stdafx.h"
#include "GameInstance.h"
#include "Hold_Zombie.h"
#include "Player.h"
#include "BlackBoard_Zombie.h"
#include "Zombie.h"
#include "Body_Zombie.h"

CHold_Zombie::CHold_Zombie()
	: CTask_Node()
{
}

CHold_Zombie::CHold_Zombie(const CHold_Zombie& rhs)
{
}

HRESULT CHold_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

void CHold_Zombie::Enter()
{
	CModel* pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	pBodyModel->Set_TotalLinearInterpolation(0.2f);
	pBodyModel->Set_Loop(static_cast<_uint>(m_eBasePlayingIndex), false);

	pBodyModel->Set_BlendWeight(static_cast<_uint>(m_eBlendPlayingIndex), 0.f, 0.f);
	pBodyModel->Reset_PreAnim_CurrentAnim(static_cast<_uint>(m_eBlendPlayingIndex));

	m_isEntry = true;
}

_bool CHold_Zombie::Execute(_float fTimeDelta)
{
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion
	m_pBlackBoard->Organize_PreState(this);

	auto pAI = m_pBlackBoard->Get_AI();
	pAI->SetState(MONSTER_STATE::MST_HOLD);

	if (false == m_isEntry)
	{
		if (true == Is_StateFinished())
			return false;
	}

	Change_Animation();

	m_isEntry = false;

	return true;
}

void CHold_Zombie::Exit()
{
}

void CHold_Zombie::Change_Animation()
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel* pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	if (false == m_isEntry)
	{
		if (true == true == pBodyModel->isFinished(static_cast<_uint>(PLAYING_INDEX::INDEX_0)))
		{
			m_pBlackBoard->Get_AI()->Get_Status_Ptr()->fAccHoldTime = 0.f;
		}
		return;
	}

	m_isEntry = false; 

	DIRECTION		eDirection = { DIRECTION::_END };
	if (false == m_pBlackBoard->Compute_Direction_To_Player_4Direction_Local(&eDirection))
		return;

	_uint			iPlayingIndex = { static_cast<_uint>(PLAYING_INDEX::INDEX_0) };
	_int			iResultAnimationIndex = { -1 };
	wstring			strBoneLayerTag = { BONE_LAYER_DEFAULT_TAG };
	wstring			strAnimLayerTag = { TEXT("Ordinary_Hold") };

	if (DIRECTION::_F == eDirection)
	{
		_int			iRandomAnimIndex = { m_pGameInstance->GetRandom_Int(static_cast<_int>(ANIM_ORDINARY_HOLD::_F1), static_cast<_int>(ANIM_ORDINARY_HOLD::_F2)) };
		
		iResultAnimationIndex = iRandomAnimIndex;
	}

	else if (DIRECTION::_B == eDirection)
	{
		_float3				vDirectionToPlayerLocalFloat3;
		if (false == m_pBlackBoard->Compute_Direction_To_Player_Local(&vDirectionToPlayerLocalFloat3))
		{
			return;
		}
		_vector				vDirectionToPlayerLocal = { XMLoadFloat3(&vDirectionToPlayerLocalFloat3) };
		_bool				isRight = { 0.f < XMVectorGetX(vDirectionToPlayerLocal) };

		if (true == isRight)
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_HOLD::_R180);
		}

		else
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_HOLD::_L180);
		}
	}

	else if (DIRECTION::_R == eDirection)
	{
		iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_HOLD::_R90);
	}

	else if (DIRECTION::_L == eDirection)
	{
		iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_HOLD::_L90);
	}

	if (-1 == iResultAnimationIndex)
		return;

	pBodyModel->Change_Animation(iPlayingIndex, strAnimLayerTag, iResultAnimationIndex);
	pBodyModel->Set_BoneLayer_PlayingInfo(iPlayingIndex, strBoneLayerTag);
}

_bool CHold_Zombie::Is_StateFinished()
{
	_bool					isFinished = { false };
	
	CModel*					pBody_Model = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
	if (nullptr == pBody_Model)
		return isFinished;

	isFinished = pBody_Model->isFinished(static_cast<_uint>(m_eBasePlayingIndex));

	return isFinished;
}

CHold_Zombie* CHold_Zombie::Create(void* pArg)
{
	CHold_Zombie* pInstance = { new CHold_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CHold_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHold_Zombie::Free()
{
	__super::Free();
}
