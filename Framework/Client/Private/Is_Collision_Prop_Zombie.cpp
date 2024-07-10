#include "stdafx.h"
#include "GameInstance.h"
#include "Is_Collision_Prop_Zombie.h"
#include "BlackBoard_Zombie.h"
#include "Player.h"
#include "Zombie.h"
#include "CustomCollider.h"

CIs_Collision_Prop_Zombie::CIs_Collision_Prop_Zombie()
	: CDecorator_Node()
{

}

CIs_Collision_Prop_Zombie::CIs_Collision_Prop_Zombie(const CIs_Collision_Prop_Zombie& rhs)
{
}

HRESULT CIs_Collision_Prop_Zombie::Initialize(COLL_PROP_TYPE ePropType, RETURN_TYPE eReturnType)
{
	m_eTargetCollPropType = ePropType;
	if (COLL_PROP_TYPE::_END == m_eTargetCollPropType)
		return E_FAIL;

	m_eReturnType = eReturnType;
	if (RETURN_TYPE::_END == m_eReturnType)
		return E_FAIL;

	return S_OK;
}

_bool CIs_Collision_Prop_Zombie::Condition_Check()
{
	if (nullptr == m_pBlackBoard)
		return false;

	CZombie*		pZombie = { m_pBlackBoard->Get_AI() };
	if (nullptr == pZombie)
		return false;
	CCustomCollider*				pCustomCollider = { nullptr };

	if (COLL_PROP_TYPE::_WINDOW == m_eTargetCollPropType)
	{
		pCustomCollider = { m_pBlackBoard->Get_Nearest_Window_CustomCollider() };
	}

	else if (COLL_PROP_TYPE::_DOOR == m_eTargetCollPropType)
	{
		//	m_pBlackBoard->Research_NearestDoor();
		pCustomCollider = { m_pBlackBoard->Get_Target_Door_CustomCollider() };		
	}

	if (nullptr == pCustomCollider)
		return false;

	CCollider* pMyCollider = { static_cast<CCollider*>(m_pBlackBoard->Get_AI()->Get_Component(TEXT("Com_Collider_Bounding"))) };
	CCollider* pPropsCollider = { static_cast<CCollider*>(pCustomCollider->Get_Component(TEXT("Com_Collider"))) };

	if (nullptr == pMyCollider || nullptr == pPropsCollider)
		return false;

	_bool						isCollision = { pMyCollider->Intersect(pPropsCollider) };

	if (RETURN_TYPE::_STARARIGHT == m_eReturnType)
		return isCollision;

	else if (RETURN_TYPE::_REVERSE == m_eReturnType)
		return !isCollision;

	return false;
}

CIs_Collision_Prop_Zombie* CIs_Collision_Prop_Zombie::Create(COLL_PROP_TYPE ePropType, RETURN_TYPE eReturnType)
{
	CIs_Collision_Prop_Zombie* pInstance = { new CIs_Collision_Prop_Zombie() };

	if (FAILED(pInstance->Initialize(ePropType, eReturnType)))
	{
		MSG_BOX(TEXT("Failed To Created : CIs_Collision_Prop_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CIs_Collision_Prop_Zombie::Free()
{
	__super::Free();
}
