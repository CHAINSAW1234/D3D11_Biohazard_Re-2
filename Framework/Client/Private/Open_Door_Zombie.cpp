#include "stdafx.h"
#include "GameInstance.h"
#include "Open_Door_Zombie.h"
#include "BlackBoard_Zombie.h"

#include "Door.h"
#include "Player.h"
#include "Zombie.h"
#include "Body_Zombie.h"

#include "Room_Finder.h"

COpen_Door_Zombie::COpen_Door_Zombie()
	: CTask_Node()
{
}

COpen_Door_Zombie::COpen_Door_Zombie(const COpen_Door_Zombie& rhs)
{
}

HRESULT COpen_Door_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

void COpen_Door_Zombie::Enter()
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel*				pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	CDoor*				pDoor = { m_pBlackBoard->Get_Target_Door() };
	if (nullptr == pDoor)
		return;

	_float3				vDirectionFromDoorLocalFloat3 = {};
	if (false == m_pBlackBoard->Compute_Direction_From_Target_Local(pDoor, &vDirectionFromDoorLocalFloat3))
		return;

	_bool				isDoorsFront = { vDirectionFromDoorLocalFloat3.z > 0.f };
	if (false == isDoorsFront)
	{
		m_iAnimIndex = static_cast<_int>(ANIM_GIMMICK_DOOR::_OPEN_FROM_A);
	}

	else
	{
		m_iAnimIndex = static_cast<_int>(ANIM_GIMMICK_DOOR::_OPEN_FROM_B);
	}



	pDoor->Attack_Prop(m_pBlackBoard->Get_AI()->Get_Transform());
	Change_Animation();

	m_pBlackBoard->Get_AI()->Play_Random_Open_Door_Sound();

	m_isDummyDoor = false;
	m_pBlackBoard->Get_AI()->Set_ManualMove(true);
	//	m_fAccLinearInterpolateTime = 0.f;

#ifdef _DEBUG
	cout << "Enter Open Door" << endl;
#endif 
}

_bool COpen_Door_Zombie::Execute(_float fTimeDelta)
{
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion

	CDoor*			pDoor = { m_pBlackBoard->Get_Target_Door() };
	if (nullptr == pDoor)
		return false;

	MONSTER_STATE			ePreMonsterState = { m_pBlackBoard->Get_AI()->Get_Current_MonsterState() };
	if (MONSTER_STATE::MST_OPEN_DOOR == ePreMonsterState)
	{
		CModel*			pBody_Model = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
		if (nullptr == pBody_Model)
			return false;

		_int			iAnimIndex = { pBody_Model->Get_CurrentAnimIndex(static_cast<_uint>(m_eBasePlayingIndex)) };
		wstring			strAnimLayerTag = { pBody_Model->Get_CurrentAnimLayerTag(static_cast<_uint>(m_eBasePlayingIndex)) };

		_bool			isSameAnimLayer = { strAnimLayerTag == m_strAnimLayerTag };
		_bool			isSameAnimIndex = { iAnimIndex == m_iAnimIndex };

		if (isSameAnimIndex && isSameAnimLayer)
		{
			if (true == pBody_Model->isFinished(static_cast<_uint>(m_eBasePlayingIndex)))
				return false;			
		}
	}

	else
	{
		//	필요 조건 => 문 닫힘 ( 체력 1 => 1대치면 열림), 문잠기지않음		
		_int				iDoorHP = { pDoor->Get_HP() };
		_bool				isDoorCanOpen = { 0 == iDoorHP };
		if (false == isDoorCanOpen)
			return false;

		if (true == pDoor->Is_Lock())
			return false;
	}

	m_pBlackBoard->Organize_PreState(this);

	auto pAI = m_pBlackBoard->Get_AI();
	pAI->Set_State(MONSTER_STATE::MST_OPEN_DOOR);


	return true;
}

void COpen_Door_Zombie::Exit()
{
	if (nullptr == m_pBlackBoard)
		return;

	m_pBlackBoard->Get_AI()->Set_ManualMove(false);
	CDoor*		pTarget_Door = { m_pBlackBoard->Get_Target_Door() };

	if (nullptr == pTarget_Door)
		return;

	LOCATION_MAP_VISIT					eCurrentLocation = { m_pBlackBoard->Get_AI()->Get_Location() };
	LOCATION_MAP_VISIT					eNewLocation = { LOCATION_MAP_VISIT::LOCATION_MAP_VISIT_END };
	list<LOCATION_MAP_VISIT>			LinkedLocation = { CRoom_Finder::Get_Instance()->Find_Linked_Loctaion_From_Door(pTarget_Door) };
	
	for (auto& eLocation : LinkedLocation)
	{
		if (eCurrentLocation != eLocation)
		{
			eNewLocation = eLocation;

#ifdef _DEBUG

			cout << "Zombie New Location : " << eNewLocation << endl;

#endif
			break;
		}
	}

	if (eNewLocation != LOCATION_MAP_VISIT::LOCATION_MAP_VISIT_END)
	{
		m_pBlackBoard->Get_AI()->Set_Location(eNewLocation);
	}

#ifdef _DEBUG

	else
	{
		MSG_BOX(TEXT("Called : void COpen_Door_Zombie::Exit() 문과 연결된 방을 못찾음"));
	}

#endif // _DEBUG


	m_pBlackBoard->Release_Target_Door();

}

void COpen_Door_Zombie::Change_Animation()
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel*			pBody_Model = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBody_Model)
		return;

	pBody_Model->Change_Animation(static_cast<_uint>(m_eBasePlayingIndex), m_strAnimLayerTag, m_iAnimIndex);
	pBody_Model->Set_Loop(static_cast<_uint>(m_eBasePlayingIndex), false);

#pragma endregion
}

COpen_Door_Zombie* COpen_Door_Zombie::Create(void* pArg)
{
	COpen_Door_Zombie* pInstance = { new COpen_Door_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : COpen_Door_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void COpen_Door_Zombie::Free()
{
	__super::Free();
}

