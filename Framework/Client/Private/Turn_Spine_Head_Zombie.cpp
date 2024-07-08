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

	if (true == m_isStart)
	{
		_vector			vLook = { m_pBlackBoard->Get_AI()->Get_Transform()->Get_State_Vector(CTransform::STATE_LOOK) };

		XMStoreFloat3(&m_vPreLookDirection, vLook);
		m_isStart = false;
	}


	//	Set_Hand_AdditionalMatrices(fTimeDelta);

	return true;
}

void CTurn_Spine_Head_Zombie::Exit()
{
}

void CTurn_Spine_Head_Zombie::Set_Hand_AdditionalMatrices(_float fTimeDelta)
{
	CTransform*			pZombie_Transform = { m_pBlackBoard->Get_AI()->Get_Transform() };
	CTransform*			pPlayer_Transform = { m_pBlackBoard->Get_Player()->Get_Transform() };
	CModel*				pBody_Model = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };

	if (nullptr == pZombie_Transform || nullptr == pPlayer_Transform || nullptr == pBody_Model)
		return;

	_vector				vZombiePosition = { pZombie_Transform->Get_State_Vector(CTransform::STATE_POSITION) };
	_vector				vPlayerPosition = { pPlayer_Transform->Get_State_Vector(CTransform::STATE_POSITION) };

	//	_vector				vZombieLook = { XMVector3Normalize(pZombie_Transform->Get_State_Vector(CTransform::STATE_LOOK)) };
	_vector				vDirectionToPlayer = { XMVector3Normalize(vPlayerPosition - vZombiePosition) };
	_vector				vPreLookDirection = { XMLoadFloat3(&m_vPreLookDirection) };


	XMStoreFloat3(&m_vPreLookDirection, vDirectionToPlayer);

	//	_vector				vCross = { XMVector3Cross(vZombieLook, vDirectionToPlayer) };
	_vector				vCross = { XMVector3Cross(vPreLookDirection, vDirectionToPlayer) };
	//	_float				fDot = { XMVectorGetX(XMVector3Dot(vZombieLook, vDirectionToPlayer)) };
	_float				fDot = { XMVectorGetX(XMVector3Dot(vPreLookDirection, vDirectionToPlayer)) };
	_float				fAngle = { fminf(acosf(fDot), XMConvertToRadians(50.f) * fTimeDelta) };

	_matrix				ZombieWorldMatrixInv = { pZombie_Transform->Get_WorldMatrix_Inverse() };
	
	_vector				vAxis = { XMVector3TransformNormal(vCross, ZombieWorldMatrixInv) };	
	_vector				vTotalRotateQuaternionLocal = { XMQuaternionRotationAxis(XMVector3Normalize(vAxis), fAngle) };

	list<_uint>			ChildJointIndices;
	pBody_Model->Get_Child_ZointIndices(m_strSpineBoneTag, m_strHeadBoneTag, ChildJointIndices);

	ChildJointIndices.pop_back();

	_uint				iNumChildJoint = { static_cast<_uint>(ChildJointIndices.size()) };
	vector<string>		BoneNames = { pBody_Model->Get_BoneNames() };

	_vector				vDevideQuaternionLocal = { XMQuaternionSlerp(XMQuaternionIdentity(), vTotalRotateQuaternionLocal, 1.f / static_cast<_float>(iNumChildJoint)) };

	_matrix				RotationMatrix = { XMMatrixRotationQuaternion(vDevideQuaternionLocal) };

	for (auto& iJointIndex : ChildJointIndices)
	{
		pBody_Model->Add_Additional_Transformation_World(BoneNames[iJointIndex], RotationMatrix);
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
