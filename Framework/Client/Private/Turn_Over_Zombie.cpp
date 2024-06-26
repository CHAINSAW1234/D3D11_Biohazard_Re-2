#include "stdafx.h"
#include "GameInstance.h"
#include "Turn_Over_Zombie.h"
#include "Player.h"
#include "BlackBoard_Zombie.h"
#include "Zombie.h"
#include "Body_Zombie.h"

CTurn_Over_Zombie::CTurn_Over_Zombie()
	: CTask_Node()
{
}

CTurn_Over_Zombie::CTurn_Over_Zombie(const CTurn_Over_Zombie& rhs)
{
}

HRESULT CTurn_Over_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

void CTurn_Over_Zombie::Enter()
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel* pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	pBodyModel->Set_TotalLinearInterpolation(0.2f);
	pBodyModel->Set_Loop(static_cast<_uint>(PLAYING_INDEX::INDEX_0), false);
}

_bool CTurn_Over_Zombie::Execute(_float fTimeDelta)
{
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion

	m_pBlackBoard->Organize_PreState(this);

	auto pAI = m_pBlackBoard->GetAI();
	pAI->SetState(MONSTER_STATE::MST_DOWN);

	Change_Animation();

	return true;
}

void CTurn_Over_Zombie::Exit()
{
}

void CTurn_Over_Zombie::Change_Animation()
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel*			pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	_uint			iPlayingIndex = { static_cast<_uint>(PLAYING_INDEX::INDEX_0) };
	_int			iResultAnimationIndex = { -1 };
	wstring			strBoneLayerTag = { BONE_LAYER_DEFAULT_TAG };
	wstring			strAnimLayerTag = { TEXT("") };
}

CTurn_Over_Zombie* CTurn_Over_Zombie::Create(void* pArg)
{
	CTurn_Over_Zombie* pInstance = { new CTurn_Over_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CTurn_Over_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTurn_Over_Zombie::Free()
{
	__super::Free();
}
