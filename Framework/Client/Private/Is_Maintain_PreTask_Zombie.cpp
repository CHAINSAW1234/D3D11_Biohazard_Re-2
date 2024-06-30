#include "stdafx.h"
#include "GameInstance.h"
#include "Is_Maintain_PreTask_Zombie.h"
#include "BlackBoard_Zombie.h"
#include "Player.h"
#include "Zombie.h"

CIs_Maintain_PreTask_Zombie::CIs_Maintain_PreTask_Zombie()
	: CDecorator_Node()
{

}

CIs_Maintain_PreTask_Zombie::CIs_Maintain_PreTask_Zombie(const CIs_Maintain_PreTask_Zombie& rhs)
{
}

HRESULT CIs_Maintain_PreTask_Zombie::Initialize(void* pArg)
{
	m_MaintainStates.emplace(MONSTER_STATE::MST_BITE);
	m_MaintainStates.emplace(MONSTER_STATE::MST_DAMAGE);
	m_MaintainStates.emplace(MONSTER_STATE::MST_TURN);
	m_MaintainStates.emplace(MONSTER_STATE::MST_HOLD);
	m_MaintainStates.emplace(MONSTER_STATE::MST_SLEEP);

	return S_OK;
}

_bool CIs_Maintain_PreTask_Zombie::Condition_Check()
{
	_bool				isMaintain = { false };

	if (nullptr == m_pBlackBoard)
		return isMaintain;

	MONSTER_STATE			eCurrentState = { m_pBlackBoard->GetAI()->Get_Current_MonsterState() };
	//if (true == Is_Maintain_State(eCurrentState))
	//{
	//	if (false == Is_CanFinish(eCurrentState))
	//	{
	//		isMaintain = true;
	//	}
	//}

	//return isMaintain;

	return Is_Maintain_State(eCurrentState);
}

_bool CIs_Maintain_PreTask_Zombie::Is_Maintain_State(MONSTER_STATE eState)
{
	_bool			isMaintain = { false };

	unordered_set<MONSTER_STATE>::iterator		iter = { m_MaintainStates.find(eState) };
	if (iter != m_MaintainStates.end())
		isMaintain = true;

	return isMaintain;
}

_bool CIs_Maintain_PreTask_Zombie::Is_CanFinish(MONSTER_STATE eState)
{
	_bool			isCanFinish = { false };
	if (MONSTER_STATE::MST_TURN == eState)
	{
		CModel*				pBodyModel = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
		if (nullptr != pBodyModel)
		{
			_int			iAnimIndex = { pBodyModel->Get_AnimIndex_PlayingInfo(static_cast<_uint>(PLAYING_INDEX::INDEX_0)) };
			wstring			strAnimLayerTag = { pBodyModel->Get_CurrentAnimLayerTag(static_cast<_uint>(PLAYING_INDEX::INDEX_0)) };

			if (-1 != iAnimIndex && TEXT("") != strAnimLayerTag)
			{
				if (strAnimLayerTag == TEXT("Ordinary_PivotTurn"))
				{
					_bool				isFinsihed = { pBodyModel->isFinished(static_cast<_uint>(PLAYING_INDEX::INDEX_0)) };
					isCanFinish = isFinsihed;
				}

				else if (strAnimLayerTag == TEXT("Lost_Turn"))
				{

				}
			}
		}

#ifdef _DEBUG 
		else
		{
			MSG_BOX(TEXT("Called : _bool CIs_Maintain_PreTask_Zombie::Is_CanFinish(MONSTER_STATE eState)"));
		}

#endif
	}

	else if (MONSTER_STATE::MST_HOLD == eState)
	{
		CModel*				pBodyModel = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
		if (nullptr != pBodyModel)
		{
			_int			iAnimIndex = { pBodyModel->Get_AnimIndex_PlayingInfo(static_cast<_uint>(PLAYING_INDEX::INDEX_0)) };
			wstring			strAnimLayerTag = { pBodyModel->Get_CurrentAnimLayerTag(static_cast<_uint>(PLAYING_INDEX::INDEX_0)) };

			if (-1 != iAnimIndex && TEXT("") != strAnimLayerTag)
			{
				if (strAnimLayerTag == TEXT("Ordinary_Hold"))
				{
					_bool				isFinsihed = { pBodyModel->isFinished(static_cast<_uint>(PLAYING_INDEX::INDEX_0)) };
					isCanFinish = isFinsihed;

					if (COLLIDER_TYPE::CALF_L == m_pBlackBoard->GetAI()->Get_Current_IntersectCollider() ||
						COLLIDER_TYPE::CALF_R == m_pBlackBoard->GetAI()->Get_Current_IntersectCollider() ||
						COLLIDER_TYPE::LEG_L == m_pBlackBoard->GetAI()->Get_Current_IntersectCollider() ||
						COLLIDER_TYPE::LEG_R == m_pBlackBoard->GetAI()->Get_Current_IntersectCollider() ||
						COLLIDER_TYPE::FOOT_L == m_pBlackBoard->GetAI()->Get_Current_IntersectCollider() ||
						COLLIDER_TYPE::FOOT_R == m_pBlackBoard->GetAI()->Get_Current_IntersectCollider())
					{
						isCanFinish = true;
					}
				}

				else if (strAnimLayerTag == TEXT("Lost_Hold"))
				{

				}
			}
		}

#ifdef _DEBUG 
		else
		{
			MSG_BOX(TEXT("Called : _bool CIs_Maintain_PreTask_Zombie::Is_CanFinish(MONSTER_STATE eState)"));
		}

#endif
	}

	else if (MONSTER_STATE::MST_BITE == eState)
	{
		CModel*				pBodyModel = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
		if (nullptr != pBodyModel)
		{
			_int			iAnimIndex = { pBodyModel->Get_AnimIndex_PlayingInfo(static_cast<_uint>(PLAYING_INDEX::INDEX_0)) };
			wstring			strAnimLayerTag = { pBodyModel->Get_CurrentAnimLayerTag(static_cast<_uint>(PLAYING_INDEX::INDEX_0)) };

			if (-1 != iAnimIndex && TEXT("") != strAnimLayerTag)
			{

			}
		}

#ifdef _DEBUG 
		else
		{
			MSG_BOX(TEXT("Called : _bool CIs_Maintain_PreTask_Zombie::Is_CanFinish(MONSTER_STATE eState)"));
		}

#endif
	}

	else if (MONSTER_STATE::MST_DAMAGE == eState)
	{
		CModel*				pBodyModel = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
		if (nullptr != pBodyModel)
		{
			_int			iAnimIndex = { pBodyModel->Get_AnimIndex_PlayingInfo(static_cast<_uint>(PLAYING_INDEX::INDEX_0)) };
			wstring			strAnimLayerTag = { pBodyModel->Get_CurrentAnimLayerTag(static_cast<_uint>(PLAYING_INDEX::INDEX_0)) };

			if (-1 != iAnimIndex && TEXT("") != strAnimLayerTag)
			{
				if (strAnimLayerTag == TEXT("Damage_Stun") ||
					strAnimLayerTag == TEXT("Damage_Knockback"))
				{
					_bool				isFinsihed = { pBodyModel->isFinished(static_cast<_uint>(PLAYING_INDEX::INDEX_0)) };
					isCanFinish = isFinsihed;
				}

				else if (strAnimLayerTag == TEXT("Ordinary_Hold"))
				{
					_bool				isDamageAnim = { false };
					_int				iAnimIndex = { pBodyModel->Get_CurrentAnimIndex(static_cast<_uint>(PLAYING_INDEX::INDEX_0)) };
					isDamageAnim = (iAnimIndex == static_cast<_int>(ANIM_ORDINARY_HOLD::_TRIP_L_FOOT_UP)) || (iAnimIndex == static_cast<_int>(ANIM_ORDINARY_HOLD::_TRIP_R_FOOT_UP));
					
					_bool				isFinished = { pBodyModel->isFinished(static_cast<_uint>(PLAYING_INDEX::INDEX_0)) };
					
					isCanFinish = isFinished && isDamageAnim;
				}

				else if (strAnimLayerTag == TEXT("Damage_Burst"))
				{

				}

				else if (strAnimLayerTag == TEXT("Damage_Down"))
				{

				}

				else if (strAnimLayerTag == TEXT("Damage_Default"))
				{

				}

				else if (strAnimLayerTag == TEXT("Damage_Stairs_Down"))
				{

				}

				else if (strAnimLayerTag == TEXT("Damage_Stairs_Up"))
				{

				}

				else if (strAnimLayerTag == TEXT("Damage_Electric_Shock"))
				{

				}
			}
		}

#ifdef _DEBUG 
		else
		{
			MSG_BOX(TEXT("Called : _bool CIs_Maintain_PreTask_Zombie::Is_CanFinish(MONSTER_STATE eState)"));
		}

#endif
	}

#ifdef _DEBUG

	else
	{
		MSG_BOX(TEXT("Called : _bool CIs_Maintain_PreTask_Zombie::Is_CanFinish(MONSTER_STATE eState)"));
	}

#endif


	return isCanFinish;
}

CIs_Maintain_PreTask_Zombie* CIs_Maintain_PreTask_Zombie::Create(void* pArg)
{
	CIs_Maintain_PreTask_Zombie* pInstance = { new CIs_Maintain_PreTask_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CIs_Maintain_PreTask_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CIs_Maintain_PreTask_Zombie::Free()
{
	__super::Free();
}
