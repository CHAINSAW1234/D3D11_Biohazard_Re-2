#include "stdafx.h"
#include "GameInstance.h"
#include "Break_In_Window_Zombie.h"
#include "BlackBoard_Zombie.h"

#include "Window.h"
#include "Player.h"
#include "Zombie.h"
#include "Body_Zombie.h"

CBreak_In_Window_Zombie::CBreak_In_Window_Zombie()
	: CTask_Node()
{
}

CBreak_In_Window_Zombie::CBreak_In_Window_Zombie(const CBreak_In_Window_Zombie& rhs)
{
}

HRESULT CBreak_In_Window_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

void CBreak_In_Window_Zombie::Enter()
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel*			pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;
	CWindow*			pWindow = { m_pBlackBoard->Get_Nearest_Window() };
	if (nullptr == pWindow)
		return;

	_int				iRandom = { m_pGameInstance->GetRandom_Int(1, 2) };
	if (1 == iRandom)
		m_iAnimIndex = static_cast<_int>(ANIM_GIMMICK_WINDOW::_BREAK_IN1);
	else if (2 == iRandom)
		m_iAnimIndex = static_cast<_int>(ANIM_GIMMICK_WINDOW::_BREAK_IN2);

	if (false == m_pBlackBoard->Compute_DeltaMatrix_AnimFirstKeyFrame_From_Target(pWindow->Get_Transform(), static_cast<_uint>(m_eBasePlayingIndex), m_iAnimIndex, m_strAnimLayerTag, &m_DeltaInterpolateMatrix))
		return;

	pWindow->Attack_Prop();
	Change_Animation();

	m_fAccLinearInterpolateTime = 0.f;

#ifdef _DEBUG

	cout << "Enter Break In Window" << endl;

#endif 
}

_bool CBreak_In_Window_Zombie::Execute(_float fTimeDelta)
{
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion

	MONSTER_STATE		eCurrentMonsterState = { m_pBlackBoard->Get_AI()->Get_Current_MonsterState() };
	CWindow*			pWindow = { m_pBlackBoard->Get_Nearest_Window() };
	if (nullptr == pWindow)
		return false;

	Play_DropSound();

	if (MONSTER_STATE::MST_BREAK_IN_WINDOW == m_pBlackBoard->Get_AI()->Get_Current_MonsterState())
	{
		CModel*			pBody_Model = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
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
	else
	{
		if (MONSTER_STATE::MST_KNOCK_WINDOW != eCurrentMonsterState)
			return false;

		_int				iWindowHP = { pWindow->Get_HP_Body() };
		_bool				isCanBreakWindow = { 1 == iWindowHP };
		if (false == isCanBreakWindow)
			return false;

		if (true == pWindow->Is_Set_Barrigate())
			return false;

		_int				iRandom = { m_pGameInstance->GetRandom_Int(0, 2) };
		_bool				isSuccess = { iRandom == 0 };
		if (false == isSuccess)
			return false;
	}

	m_pBlackBoard->Organize_PreState(this);

	auto pAI = m_pBlackBoard->Get_AI();
	pAI->Set_State(MONSTER_STATE::MST_BREAK_IN_WINDOW);


	if (m_fAccLinearInterpolateTime < ZOMBIE_BREAK_IN_TOTAL_INTERPOLATE_TO_WINDOW_TIME &&
		true == m_isNeedInterpolation)
	{
		_bool				isComplete = { m_fAccLinearInterpolateTime >= ZOMBIE_BREAK_IN_TOTAL_INTERPOLATE_TO_WINDOW_TIME };
		if (false == isComplete)
		{
			_float				fTime = fTimeDelta;
			m_fAccLinearInterpolateTime += fTime;

			if (m_fAccLinearInterpolateTime >= ZOMBIE_BREAK_IN_TOTAL_INTERPOLATE_TO_WINDOW_TIME)
			{
				fTime -= m_fAccLinearInterpolateTime - ZOMBIE_BREAK_IN_TOTAL_INTERPOLATE_TO_WINDOW_TIME;
			}
			_float				fRatio = { fTime / ZOMBIE_BREAK_IN_TOTAL_INTERPOLATE_TO_WINDOW_TIME };

			if (false == isComplete)
			{
				if (false == m_pBlackBoard->Apply_Devide_Delta_Matrix(fRatio, XMLoadFloat4x4(&m_DeltaInterpolateMatrix)))
					return false;
			}
		}
	}

	return true;
}

void CBreak_In_Window_Zombie::Exit()
{
	if (nullptr == m_pBlackBoard)
		return;
}

void CBreak_In_Window_Zombie::Change_Animation()
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

void CBreak_In_Window_Zombie::Play_DropSound()
{
	CModel* pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	_float fTrackPosition = pBodyModel->Get_TrackPosition(static_cast<_uint>(m_eBasePlayingIndex));

	if (abs(fTrackPosition - 150.f) < 1.3f)
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

CBreak_In_Window_Zombie* CBreak_In_Window_Zombie::Create(void* pArg)
{
	CBreak_In_Window_Zombie* pInstance = { new CBreak_In_Window_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CBreak_In_Window_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBreak_In_Window_Zombie::Free()
{
	__super::Free();
}
