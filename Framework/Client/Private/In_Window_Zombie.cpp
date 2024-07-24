#include "stdafx.h"
#include "GameInstance.h"
#include "In_Window_Zombie.h"
#include "BlackBoard_Zombie.h"

#include "Window.h"
#include "Player.h"
#include "Zombie.h"
#include "Body_Zombie.h"

CIn_Window_Zombie::CIn_Window_Zombie()
	: CTask_Node()
{
}

CIn_Window_Zombie::CIn_Window_Zombie(const CIn_Window_Zombie& rhs)
{
}

HRESULT CIn_Window_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

void CIn_Window_Zombie::Enter()
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel*				pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	m_fAccLinearInterpolateTime = 0.f;

	_int				iRandom = { m_pGameInstance->GetRandom_Int(1, 2) };
	if (1 == iRandom)
		m_iAnimIndex = static_cast<_int>(ANIM_GIMMICK_WINDOW::_IN1);
	else if (2 == iRandom)
		m_iAnimIndex = static_cast<_int>(ANIM_GIMMICK_WINDOW::_IN2);

	CWindow* pWindow = { m_pBlackBoard->Get_Nearest_Window() };
	if (nullptr == pWindow)
		return;

	if (false == m_pBlackBoard->Compute_DeltaMatrix_AnimFirstKeyFrame_From_Target(pWindow->Get_Transform(), static_cast<_uint>(m_eBasePlayingIndex), m_iAnimIndex, m_strAnimLayerTag, &m_DeltaInterpolateMatrix))
		return;

#ifdef _DEBUG

	cout << "Enter In Window" << endl;

#endif 
}

_bool CIn_Window_Zombie::Execute(_float fTimeDelta)
{
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion

	//	필요 조건 => 위치 도달, 창문 깨짐, 바리게이트 안쳐짐
	CWindow*			pWindow = { m_pBlackBoard->Get_Nearest_Window() };
	if (nullptr == pWindow)
		return false;
	_int				iWindowHP = { pWindow->Get_HP_Body() };
	_bool				isWindowBroken = { 0 >= iWindowHP };
	if (false == isWindowBroken)
		return false;

	_bool				isSetBarrigate = { pWindow->Is_Set_Barrigate() };
	if (true == isSetBarrigate)
		return false;

	Play_DropSound();

	if (MONSTER_STATE::MST_IN_WINDOW == m_pBlackBoard->Get_AI()->Get_Current_MonsterState())
	{
		CModel* pBody_Model = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
		if (nullptr == pBody_Model)
			return false;

		if (true == pBody_Model->isFinished(static_cast<_uint>(m_eBasePlayingIndex)))
		{
			m_pBlackBoard->Get_AI()->Set_PoseState(CZombie::POSE_STATE::_CREEP);
			m_pBlackBoard->Get_AI()->Set_FaceState(CZombie::FACE_STATE::_UP);
			m_pBlackBoard->Get_AI()->Set_OutDoor(false);
			m_pBlackBoard->Release_Nearest_Window();
			m_pBlackBoard->Get_AI()->Set_ManualMove(false);
			return false;
		}
	}

	m_pBlackBoard->Organize_PreState(this);

	auto pAI = m_pBlackBoard->Get_AI();
	pAI->Set_State(MONSTER_STATE::MST_IN_WINDOW);

	if (m_fAccLinearInterpolateTime < ZOMBIE_IN_TOTAL_INTERPOLATE_TO_WINDOW_TIME)
	{
		_bool				isComplete = { m_fAccLinearInterpolateTime >= ZOMBIE_IN_TOTAL_INTERPOLATE_TO_WINDOW_TIME };
		if (false == isComplete)
		{
			_float				fTime = fTimeDelta;
			m_fAccLinearInterpolateTime += fTime;

			if (m_fAccLinearInterpolateTime >= ZOMBIE_IN_TOTAL_INTERPOLATE_TO_WINDOW_TIME)
			{
				fTime -= m_fAccLinearInterpolateTime - ZOMBIE_IN_TOTAL_INTERPOLATE_TO_WINDOW_TIME;
			}
			_float				fRatio = { fTime / ZOMBIE_IN_TOTAL_INTERPOLATE_TO_WINDOW_TIME };

			if (false == isComplete)
			{
				if (false == m_pBlackBoard->Apply_Devide_Delta_Matrix(fRatio, XMLoadFloat4x4(&m_DeltaInterpolateMatrix)))
					return false;
			}
		}
	}

	Change_Animation(fTimeDelta);

	return true;
}

void CIn_Window_Zombie::Exit()
{
	if (nullptr == m_pBlackBoard)
		return;
}

void CIn_Window_Zombie::Change_Animation(_float fTimeDelta)
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel* pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	pBodyModel->Change_Animation(static_cast<_uint>(m_eBasePlayingIndex), m_strAnimLayerTag, m_iAnimIndex);
	pBodyModel->Set_Loop(static_cast<_uint>(m_eBasePlayingIndex), false);

#pragma endregion
}

void CIn_Window_Zombie::Play_DropSound()
{
	CModel* pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	_float fTrackPosition = pBodyModel->Get_TrackPosition(static_cast<_uint>(m_eBasePlayingIndex));

	if (abs(fTrackPosition - 280.f) < 1.3f)
	{
		const wchar_t* str = L"Break_Drop_";
		wchar_t result[32];
		_int inum = m_pGameInstance->GetRandom_Int(10, 12);

		std::swprintf(result, sizeof(result) / sizeof(wchar_t), L"%ls%d.mp3", str, inum);

		m_pGameInstance->Change_Sound_3D(m_pBlackBoard->Get_AI()->Get_Transform(), result, (_uint)ZOMBIE_SOUND_CH::_BITE_DROP);

		if (inum == 11)
		{
			m_pGameInstance->Set_Volume_3D(m_pBlackBoard->Get_AI()->Get_Transform(), (_uint)ZOMBIE_SOUND_CH::_BITE_DROP, 0.4f);
		}
	}
}

CIn_Window_Zombie* CIn_Window_Zombie::Create(void* pArg)
{
	CIn_Window_Zombie* pInstance = { new CIn_Window_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CIn_Window_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CIn_Window_Zombie::Free()
{
	__super::Free();
}
