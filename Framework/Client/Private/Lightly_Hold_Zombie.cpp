#include "stdafx.h"
#include "GameInstance.h"
#include "Lightly_Hold_Zombie.h"
#include "Player.h"
#include "BlackBoard_Zombie.h"
#include "Zombie.h"
#include "Body_Zombie.h"

CLightly_Hold_Zombie::CLightly_Hold_Zombie()
	: CTask_Node()
{
}

CLightly_Hold_Zombie::CLightly_Hold_Zombie(const CLightly_Hold_Zombie& rhs)
{
}

HRESULT CLightly_Hold_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

void CLightly_Hold_Zombie::Enter()
{
	CModel* pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	pBodyModel->Set_TotalLinearInterpolation(0.2f);
	pBodyModel->Set_Loop(static_cast<_uint>(PLAYING_INDEX::INDEX_0), false);

	pBodyModel->Set_BlendWeight(static_cast<_uint>(PLAYING_INDEX::INDEX_1), 0.f, 0.f);
	pBodyModel->Reset_PreAnim_CurrentAnim(static_cast<_uint>(PLAYING_INDEX::INDEX_1));

	m_isEntry = true;
}

_bool CLightly_Hold_Zombie::Execute(_float fTimeDelta)
{
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion

	m_pBlackBoard->Organize_PreState(this);

	auto pAI = m_pBlackBoard->Get_AI();
	pAI->SetState(MONSTER_STATE::MST_LIGHTLY_HOLD);

	Change_Animation();

	return true;
}

void CLightly_Hold_Zombie::Exit()
{
}

void CLightly_Hold_Zombie::Change_Animation()
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
			m_pBlackBoard->Get_AI()->Get_Status_Ptr()->fAccLightlyHoldTime = 0.f;
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
			return;

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

	//for (_uint i = 0; i < static_cast<_uint>(ANIM_ORDINARY_HOLD::_END); ++i)
	//{
	//	pBodyModel->Set_TickPerSec(TEXT("Ordinary_Hold"), i, 5.f);

	//}
	pBodyModel->Change_Animation(iPlayingIndex, strAnimLayerTag, iResultAnimationIndex);
	pBodyModel->Set_BoneLayer_PlayingInfo(iPlayingIndex, strBoneLayerTag);
}

CLightly_Hold_Zombie* CLightly_Hold_Zombie::Create(void* pArg)
{
	CLightly_Hold_Zombie* pInstance = { new CLightly_Hold_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CLightly_Hold_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLightly_Hold_Zombie::Free()
{
	__super::Free();
}
