#include "stdafx.h"
#include "GameInstance.h"
#include "Move_To_Target_Lost_Zombie.h"
#include "BlackBoard_Zombie.h"

#include "Zombie.h"
#include "Body_Zombie.h"

#include "Player.h"
#include "Window.h"
#include "Door.h"


CMove_To_Target_Lost_Zombie::CMove_To_Target_Lost_Zombie()
	: CTask_Node()
{
}

CMove_To_Target_Lost_Zombie::CMove_To_Target_Lost_Zombie(const CMove_To_Target_Lost_Zombie& rhs)
{
}

HRESULT CMove_To_Target_Lost_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

void CMove_To_Target_Lost_Zombie::Enter()
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel*				pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

#ifdef _DEBUG
	cout << "Enter Move To Tareget" << endl;
#endif 
}

_bool CMove_To_Target_Lost_Zombie::Execute(_float fTimeDelta)
{
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion

	CZombie::POSE_STATE			ePoseState = { m_pBlackBoard->Get_AI()->Get_PoseState() };
	if (CZombie::POSE_STATE::_UP != ePoseState)
		return false;

	if (false == m_pBlackBoard->Is_LookTarget())
		return false;

	_float3			vDirectionToPlayerLocalFloat3;
	if (false == m_pBlackBoard->Compute_Direction_To_Player_Local(&vDirectionToPlayerLocalFloat3))
		return false;

	_vector			vDirectionToPlayerLocal = { XMLoadFloat3(&vDirectionToPlayerLocalFloat3) };
	_vector			vDirectionToPlayerLocalXZPlane = { XMVector3Normalize(XMVectorSetY(vDirectionToPlayerLocal, 0.f)) };		//	회전량을 xz평면상에서만 고려하기위함
	_vector			vAILookLocal = { XMVectorSet(0.f, 0.f, 1.f, 0.f) };

	_bool			isRight = { XMVectorGetX(vDirectionToPlayerLocalXZPlane) > 0.f };

	_float			fDot = { XMVectorGetX(XMVector3Dot(XMVector3Normalize(vDirectionToPlayerLocal), XMVector3Normalize(vAILookLocal))) };
	_float			fAngleToTarget = { acosf(fDot) };

	_float			fMaxMoveAngle = { m_pBlackBoard->Get_AI()->Get_Status_Ptr()->fMaxMoveAngle };
	if (fMaxMoveAngle < fAngleToTarget)
		return false;

	m_pBlackBoard->Organize_PreState(this);

	auto pAI = m_pBlackBoard->Get_AI();
	pAI->Set_State(MONSTER_STATE::MST_WALK);

	return true;
}

void CMove_To_Target_Lost_Zombie::Exit()
{
	if (nullptr == m_pBlackBoard)
		return;

	m_eMoveLostAnimType = ZOMBIE_MOVE_LOST_TYPE::_END;
}

CMove_To_Target_Lost_Zombie* CMove_To_Target_Lost_Zombie::Create(void* pArg)
{
	CMove_To_Target_Lost_Zombie* pInstance = { new CMove_To_Target_Lost_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CMove_To_Target_Lost_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMove_To_Target_Lost_Zombie::Free()
{
	__super::Free();
}
