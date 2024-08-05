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
	m_isStart = true;
	m_fRotatePerSec = XMConvertToRadians(90.f);

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

	MONSTER_STATE			eMonsterState = { m_pBlackBoard->Get_AI()->Get_Current_MonsterState() };
	CZombie::POSE_STATE		ePoseState = { m_pBlackBoard->Get_AI()->Get_PoseState() };
	_bool					isLookTarget = { m_pBlackBoard->Is_LookTarget() };
	_float					fDistanceToPlayer = {};
	if (false == m_pBlackBoard->Compute_Distance_To_Player_World(&fDistanceToPlayer))
		return false;

	if (m_pBlackBoard->Get_AI()->Get_Status_Ptr()->fRecognitionRange >= fDistanceToPlayer &&
		(MONSTER_STATE::MST_IDLE == eMonsterState ||
		MONSTER_STATE::MST_WALK == eMonsterState ||
		MONSTER_STATE::MST_TURN == eMonsterState) &&
		CZombie::POSE_STATE::_UP == ePoseState)
	{
		m_fMaxAngle += XMConvertToRadians(90.f) * fTimeDelta;
		if (m_fMaxAngle > XMConvertToRadians(100.f))
			m_fMaxAngle = XMConvertToRadians(100.f);
	}

	else
	{
		m_fMaxAngle -= XMConvertToRadians(90.f) * fTimeDelta;
		if (m_fMaxAngle < 0.f)
		{
			m_fMaxAngle = XMConvertToRadians(0.f);
			m_fPreAngle = XMConvertToRadians(0.f);
		}
	}

	if (true == m_isStart)
	{
		_vector			vLook = { m_pBlackBoard->Get_AI()->Get_Transform()->Get_State_Vector(CTransform::STATE_LOOK) };

		XMStoreFloat3(&m_vPreLookDirection, vLook);
		m_isStart = false;
	}


	if(m_fMaxAngle > 0.f)
		Set_Spine_To_Head_AdditionalMatrices(fTimeDelta);

	return true;
}

void CTurn_Spine_Head_Zombie::Exit()
{
}

void CTurn_Spine_Head_Zombie::Set_Spine_To_Head_AdditionalMatrices(_float fTimeDelta)
{
	CTransform*			pZombie_Transform = { m_pBlackBoard->Get_AI()->Get_Transform() };
	CTransform*			pPlayer_Transform = { m_pBlackBoard->Get_Player()->Get_Transform() };
	CModel*				pBody_Model = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };

	if (nullptr == pZombie_Transform || nullptr == pPlayer_Transform || nullptr == pBody_Model)
		return;

	_vector				vZombieLook = { pZombie_Transform->Get_State_Vector(CTransform::STATE_LOOK) };
	_vector				vZombiePosition = { pZombie_Transform->Get_State_Vector(CTransform::STATE_POSITION) };
	_vector				vPlayerPosition = { pPlayer_Transform->Get_State_Vector(CTransform::STATE_POSITION) };

	_matrix				ZombieWorldMatrix = { pZombie_Transform->Get_WorldMatrix() };
	_matrix				ZombieWorldMatrixInv = { pZombie_Transform->Get_WorldMatrix_Inverse() };

	_vector				vDirectionToPlayer = { XMVector3Normalize(vPlayerPosition - vZombiePosition) };
	_vector				vDirectionToPlayerLocal = { XMVector3TransformNormal(vDirectionToPlayer, ZombieWorldMatrixInv) };
	vDirectionToPlayerLocal = XMVectorSetY(vDirectionToPlayerLocal, 0.f);

	_vector				vCross = { XMVector3Cross(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMVector3Normalize(vDirectionToPlayerLocal)) };
	_float				fDot = { XMVectorGetX(XMVector3Dot(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMVector3Normalize(vDirectionToPlayerLocal)))};

	if (fDot >= 0.999f)
		return;

	//	_float				fTargetAngle = { fminf(acosf(fDot), XMConvertToRadians(50.f) * fTimeDelta) };
	_float				fTargetAngle = { fminf(acosf(fDot), m_fMaxAngle) };

	if (XMVectorGetY(vCross) < 0.f)
	{
		vCross = XMVectorSetY(vCross, XMVectorGetY(vCross) * -1.f);
		fTargetAngle = fTargetAngle * -1.f;
	}
	_float				fResultAngle = { 0.f };

	if (m_fPreAngle > fTargetAngle)
	{
		fResultAngle = m_fPreAngle - (m_fRotatePerSec * fTimeDelta);
		if (fResultAngle < fTargetAngle)
			fResultAngle = fTargetAngle;
	}

	else
	{
		fResultAngle = m_fPreAngle + (m_fRotatePerSec * fTimeDelta);
		if (fResultAngle > fTargetAngle)
			fResultAngle = fTargetAngle;
	}

	m_fPreAngle = fResultAngle;

	_vector				vTotalRotateQuaternionLocal = { XMQuaternionRotationAxis(XMVector3Normalize(vCross), fResultAngle) };

	list<_uint>			ChildJointIndices;
	pBody_Model->Get_Child_ZointIndices(m_strSpineBoneTag, m_strHeadBoneTag, ChildJointIndices);

	ChildJointIndices.pop_back();

	_uint				iNumChildJoint = { static_cast<_uint>(ChildJointIndices.size()) };
	vector<string>		BoneNames = { pBody_Model->Get_BoneNames() };

	_vector				vDevideQuaternionLocal = { XMQuaternionSlerp(XMQuaternionIdentity(), vTotalRotateQuaternionLocal, 1.f / static_cast<_float>(iNumChildJoint)) };

	_matrix				DevideRotationMatrix = { XMMatrixRotationQuaternion(vDevideQuaternionLocal) };
	/*_matrix				TotalRotationMatrix = { XMMatrixRotationQuaternion(vTotalRotateQuaternionLocal) };

	_vector				vResultZombieLook = { XMVector3TransformNormal(vDirectionToPlayer, TotalRotationMatrix * ZombieWorldMatrix) };
	XMStoreFloat3(&m_vPreLookDirection, vResultZombieLook);*/

	for (auto& iJointIndex : ChildJointIndices)
	{
		pBody_Model->Add_Additional_Transformation_World(BoneNames[iJointIndex], DevideRotationMatrix);
	}
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
