#include "stdafx.h"
#include "GameInstance.h"
#include "Down_Zombie.h"
#include "Player.h"
#include "BlackBoard_Zombie.h"
#include "Zombie.h"
#include "Body_Zombie.h"

CDown_Zombie::CDown_Zombie()
	: CTask_Node()
{
}

CDown_Zombie::CDown_Zombie(const CDown_Zombie& rhs)
{
}

HRESULT CDown_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

void CDown_Zombie::Enter()
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel* pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	pBodyModel->Set_TotalLinearInterpolation(0.2f);
	pBodyModel->Set_Loop(static_cast<_uint>(PLAYING_INDEX::INDEX_0), false);
	m_eCurrentHitCollider = m_pBlackBoard->GetAI()->Get_Current_IntersectCollider();

	m_isEntry = true;
}

_bool CDown_Zombie::Execute(_float fTimeDelta)
{
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion

	m_pBlackBoard->Organize_PreState(this);

	auto pAI = m_pBlackBoard->GetAI();
	pAI->SetState(MONSTER_STATE::MST_DAMAGE);

	//	cout << "Wait" << endl;
	Change_Animation();

	return true;
}

void CDown_Zombie::Exit()
{
}

void CDown_Zombie::Change_Animation()
{
	if (nullptr == m_pBlackBoard)
		return;

	if (true == m_isEntry)
	{
		m_isEntry = false;
	}

	else
	{
		return;
	}

	CModel*			pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	_uint			iPlayingIndex = { static_cast<_uint>(PLAYING_INDEX::INDEX_0) };
	_int			iResultAnimationIndex = { -1 };
	wstring			strBoneLayerTag = { BONE_LAYER_DEFAULT_TAG };
	wstring			strAnimLayerTag = { TEXT("Damage_Down") };

	_float3			vDirectionFromHitFloat3;
	if (false == m_pBlackBoard->Compute_Direction_From_Hit_Local(&vDirectionFromHitFloat3))
		return;

	_vector			vDirectionFromHit = { XMLoadFloat3(&vDirectionFromHitFloat3) };
	_vector			vDirectionFromHitXZPlane = XMVectorSetY(vDirectionFromHit, 0.f);

	_bool			isFromRight = { 0.f > XMVectorGetX(vDirectionFromHitXZPlane) };
	_bool			isFromFront = { 0.f > XMVectorGetZ(vDirectionFromHitXZPlane) };
	_bool			isBigXAxis = { fabsf(XMVectorGetX(vDirectionFromHitXZPlane)) > fabsf(XMVectorGetZ(vDirectionFromHitXZPlane)) };

	if (m_eCurrentHitCollider == COLLIDER_TYPE::ARM_L || m_eCurrentHitCollider == COLLIDER_TYPE::FOREARM_L || m_eCurrentHitCollider == COLLIDER_TYPE::HAND_L)
	{
		if (true == isFromFront)
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_STUN::_SHOULDER_L_F);
		}

		else
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_STUN::_SHOULDER_L_B);
		}
	}

	if (-1 == iResultAnimationIndex)
		return;

	pBodyModel->Change_Animation(iPlayingIndex, strAnimLayerTag, iResultAnimationIndex);
	pBodyModel->Set_BoneLayer_PlayingInfo(iPlayingIndex, strBoneLayerTag);
}

CDown_Zombie* CDown_Zombie::Create(void* pArg)
{
	CDown_Zombie* pInstance = { new CDown_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CDown_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDown_Zombie::Free()
{
	__super::Free();
}
