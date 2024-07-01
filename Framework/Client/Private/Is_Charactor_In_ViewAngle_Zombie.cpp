#include "stdafx.h"
#include "GameInstance.h"
#include "Is_Character_In_Range_Zombie.h"
#include "BlackBoard_Zombie.h"
#include "Player.h"
#include "Zombie.h"
#include "Is_Charactor_In_ViewAngle_Zombie.h"

CIs_Charactor_In_ViewAngle_Zombie::CIs_Charactor_In_ViewAngle_Zombie()
	: CDecorator_Node{}
{
}
CIs_Charactor_In_ViewAngle_Zombie::CIs_Charactor_In_ViewAngle_Zombie(const CIs_Charactor_In_ViewAngle_Zombie& rhs)
	:CDecorator_Node{ rhs }
{
}
HRESULT CIs_Charactor_In_ViewAngle_Zombie::Initialize_Prototype()
{
	return S_OK;
}
HRESULT CIs_Charactor_In_ViewAngle_Zombie::Initialize(void* pArg)
{
	return S_OK;
}
_bool CIs_Charactor_In_ViewAngle_Zombie::Condition_Check()
{
	if (nullptr == m_pBlackBoard)
		return false;

	_float			fViewAngle = { m_pBlackBoard->Get_AI()->Get_Status_Ptr()->fViewAngle };

	_float3			vLookFloat3;
	_bool			isCompute = { m_pBlackBoard->Compute_Direction_To_Player_Local(&vLookFloat3) };
	if (true == isCompute)
	{
		_vector		vAbsoluteLook = { XMVectorSet(0.f, 0.f, 1.f, 0.f) };
		_vector		vLookTarget = { XMLoadFloat3(&vLookFloat3) };

		_vector		vLookTargetXZPlange = { XMVectorSetY(vLookTarget, 0.f) };

		_float		fDot = { XMVectorGetX(XMVector3Dot(XMVector3Normalize(vAbsoluteLook), XMVector3Normalize(vLookTargetXZPlange))) };
		_float		fAngle = { acosf(fDot) };

		if (fAngle <= fViewAngle)
		{
			return true;
		}
	}

	return false;
}
CIs_Charactor_In_ViewAngle_Zombie* CIs_Charactor_In_ViewAngle_Zombie::Create()
{
	CIs_Charactor_In_ViewAngle_Zombie* pInstance = { new CIs_Charactor_In_ViewAngle_Zombie() };

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CIs_Charactor_In_ViewAngle_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}
void CIs_Charactor_In_ViewAngle_Zombie::Free()
{
	__super::Free();
}