#include "stdafx.h"
#include "GameInstance.h"
#include "Region_Update_Zombie.h"
#include "Player.h"
#include "BlackBoard_Zombie.h"
#include "Zombie.h"
#include "Body_Zombie.h"
#include "Stun_Zombie.h"
#include "CustomCollider.h"

CRegion_Update_Zombie::CRegion_Update_Zombie()
	: CTask_Node()
{
}

CRegion_Update_Zombie::CRegion_Update_Zombie(const CRegion_Update_Zombie& rhs)
{
}

HRESULT CRegion_Update_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

void CRegion_Update_Zombie::Enter()
{
}

_bool CRegion_Update_Zombie::Execute(_float fTimeDelta)
{
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;

	list<CGameObject*>*				pRegionColliders = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Collider"));
	if (nullptr == pRegionColliders)
		return false;

	CCollider*						pMyCollider = { static_cast<CCollider*>(m_pBlackBoard->Get_AI()->Get_Component(TEXT("Com_Collider_Bounding"))) };
	if (nullptr == pMyCollider)
		return false;

	list<CCustomCollider*>			IntersectColliders;
	
	for (auto& iterCollider : *pRegionColliders)
	{
		if (nullptr == iterCollider)
			continue;

		CCustomCollider*		pCustomeCollider = { static_cast<CCustomCollider*>(iterCollider) };
		CCollider*				pDstCollider = { static_cast<CCollider*>(pCustomeCollider->Get_Component(TEXT("Com_Collider"))) };
		if (nullptr == pDstCollider)
			return false;

		if (true == pMyCollider->Intersect(pDstCollider))
		{
			IntersectColliders.emplace_back(pCustomeCollider);
		}
	}

	_vector						vZombiePosition = { m_pBlackBoard->Get_AI()->Get_Transform()->Get_State_Vector(CTransform::STATE_POSITION) };
	_float						fMinDistance = { 10000.f };
	CCustomCollider*			pNearestCollider = { nullptr };

	for (auto& pCollider : IntersectColliders)
	{
		CTransform*				pTransform = { pCollider->Get_Transform() };
		_vector					vColliderPosition = { pTransform->Get_State_Vector(CTransform::STATE_POSITION) };
		
		_float					fDistanceFromZombie = { XMVectorGetX(XMVector3Length(vColliderPosition - vZombiePosition)) };

		if (fMinDistance > fDistanceFromZombie)
		{
			fMinDistance = fDistanceFromZombie;

			pNearestCollider = pCollider;
		}
	}

	if (nullptr != pNearestCollider)
	{
		LOCATION_MAP_VISIT				eLocation = { static_cast<LOCATION_MAP_VISIT>(pNearestCollider->Get_Region()) };
		m_pBlackBoard->Get_AI()->Set_Location(eLocation);
	}

#ifdef _DEBUG
	else
	{
		MSG_BOX(TEXT("Called : _bool CRegion_Update_Zombie::Execute(_float fTimeDelta) 접촉한 방정보 없음"));
		return false;
	}
#endif

	return true;
#pragma endregion
}

void CRegion_Update_Zombie::Exit()
{
}

CRegion_Update_Zombie* CRegion_Update_Zombie::Create(void* pArg)
{
	CRegion_Update_Zombie* pInstance = { new CRegion_Update_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CRegion_Update_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRegion_Update_Zombie::Free()
{
	__super::Free();
}

