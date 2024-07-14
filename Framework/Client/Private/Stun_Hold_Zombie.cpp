#include "stdafx.h"
#include "GameInstance.h"
#include "Stun_Hold_Zombie.h"
#include "Player.h"
#include "BlackBoard_Zombie.h"
#include "Zombie.h"
#include "Body_Zombie.h"

CStun_Hold_Zombie::CStun_Hold_Zombie()
	: CTask_Node()
{
}

CStun_Hold_Zombie::CStun_Hold_Zombie(const CStun_Hold_Zombie& rhs)
{
}

HRESULT CStun_Hold_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

void CStun_Hold_Zombie::Enter()
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel* pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	pBodyModel->Set_TotalLinearInterpolation(0.1f);
	pBodyModel->Set_Loop(static_cast<_uint>(PLAYING_INDEX::INDEX_0), false);
	m_eCurrentHitCollider = m_pBlackBoard->Get_AI()->Get_Current_IntersectCollider();

	m_isEntry = true;

#ifdef _DEBUG

	cout << "Enter Stun Hold" << endl;

#endif 
}

_bool CStun_Hold_Zombie::Execute(_float fTimeDelta)
{
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion	

	CModel*			pBody_Model = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
	if (nullptr == pBody_Model)
		return false;

	MONSTER_STATE			eState = { m_pBlackBoard->Get_AI()->Get_Current_MonsterState() };
	if (MONSTER_STATE::MST_DAMAGE_HOLD == eState)
	{
		if (true == pBody_Model->isFinished(static_cast<_uint>(PLAYING_INDEX::INDEX_0)))
		{
			m_pBlackBoard->Get_AI()->Set_PoseState(CZombie::POSE_STATE::_CREEP);
			m_pBlackBoard->Get_AI()->Set_FaceState(CZombie::FACE_STATE::_DOWN);
			return false;
		}
	}

	else if (MONSTER_STATE::MST_HOLD == eState)
	{
		if (CZombie::POSE_STATE::_UP != m_pBlackBoard->Get_AI()->Get_PoseState())
			return false;

		COLLIDER_TYPE			eColliderType = { m_pBlackBoard->Get_AI()->Get_Current_IntersectCollider() };
		if (COLLIDER_TYPE::LEG_L != eColliderType &&
			COLLIDER_TYPE::LEG_R != eColliderType &&
			COLLIDER_TYPE::CALF_L != eColliderType &&
			COLLIDER_TYPE::CALF_R != eColliderType &&
			COLLIDER_TYPE::FOOT_L != eColliderType &&
			COLLIDER_TYPE::FOOT_R != eColliderType)
			return false;
	}

	else
	{
			return false;	
	}	

	m_pBlackBoard->Organize_PreState(this);

	auto pAI = m_pBlackBoard->Get_AI();
	pAI->Set_State(MONSTER_STATE::MST_DAMAGE_HOLD);
	Change_Animation();

	return true;
}

void CStun_Hold_Zombie::Exit()
{
	m_pBlackBoard->Get_AI()->Set_PoseState(CZombie::POSE_STATE::_CREEP);
	m_pBlackBoard->Get_AI()->Set_FaceState(CZombie::FACE_STATE::_DOWN);
}

void CStun_Hold_Zombie::Change_Animation()
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

	CModel* pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	/*_uint			iPlayingIndex = { static_cast<_uint>(PLAYING_INDEX::INDEX_END) };
	for (_uint i = static_cast<_uint>(PLAYING_INDEX::INDEX_10); i < static_cast<_uint>(PLAYING_INDEX::INDEX_20); ++i)
	{
		_float			fBlendWeight = { pBodyModel->Get_BlendWeight(iPlayingIndex) };
		if (0.f == fBlendWeight)
		{
			iPlayingIndex = i;
			pBodyModel->Set_BlendWeight(i, 2.f, 0.2f);
			break;
		}
	}

	if (static_cast<_uint>(PLAYING_INDEX::INDEX_END) == iPlayingIndex)
		return;*/

		//	_uint			iPlayingIndex = { static_cast<_uint>(PLAYING_INDEX::INDEX_10) };
	_uint			iPlayingIndex = { static_cast<_uint>(PLAYING_INDEX::INDEX_0) };
	_int			iResultAnimationIndex = { -1 };
	wstring			strBoneLayerTag = { BONE_LAYER_DEFAULT_TAG };
	wstring			strAnimLayerTag = { TEXT("Ordinary_Hold") };

	if (m_eCurrentHitCollider == COLLIDER_TYPE::LEG_L || m_eCurrentHitCollider == COLLIDER_TYPE::CALF_L || m_eCurrentHitCollider == COLLIDER_TYPE::FOOT_L)
	{
		iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_HOLD::_TRIP_L_FOOT_UP);
	}

	else if (m_eCurrentHitCollider == COLLIDER_TYPE::LEG_R || m_eCurrentHitCollider == COLLIDER_TYPE::CALF_R || m_eCurrentHitCollider == COLLIDER_TYPE::FOOT_R)
	{
		iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_HOLD::_TRIP_R_FOOT_UP);
	}

	if (-1 == iResultAnimationIndex)
		return;

	pBodyModel->Change_Animation(iPlayingIndex, strAnimLayerTag, iResultAnimationIndex);
	pBodyModel->Set_BoneLayer_PlayingInfo(iPlayingIndex, strBoneLayerTag);
}

CStun_Hold_Zombie* CStun_Hold_Zombie::Create(void* pArg)
{
	CStun_Hold_Zombie* pInstance = { new CStun_Hold_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CStun_Hold_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStun_Hold_Zombie::Free()
{
	__super::Free();
}
