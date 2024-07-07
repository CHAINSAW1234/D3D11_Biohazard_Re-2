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

	MONSTER_STATE		ePreMonsterState = { m_pBlackBoard->Get_AI()->Get_Current_MonsterState() };
	if (MONSTER_STATE::MST_BREAK_WINDOW != ePreMonsterState)
	{
		m_isNeedInterpolation = true;
		Set_TargetInterpolate_Matrix();
	}
	else
	{
		m_isNeedInterpolation = false;
	}

	_int				iRandom = { m_pGameInstance->GetRandom_Int(1, 2) };
	if (1 == iRandom)
		m_iAnimIndex = static_cast<_int>(ANIM_GIMMICK_WINDOW::_IN1);
	else if (2 == iRandom)
		m_iAnimIndex = static_cast<_int>(ANIM_GIMMICK_WINDOW::_IN2);

	Set_TargetInterpolate_Matrix();
	m_pBlackBoard->Get_AI()->Set_ManualMove(true);

#ifdef _DEBUG

	cout << "Enter Knock Door" << endl;

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

	m_pBlackBoard->Organize_PreState(this);

	auto pAI = m_pBlackBoard->Get_AI();
	pAI->Set_State(MONSTER_STATE::MST_IN_WINDOW);

	if (m_fAccLinearInterpolateTime < ZOMBIE_IN_TOTAL_INTERPOLATE_TO_WINDOW_TIME &&
		true == m_isNeedInterpolation)
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
				_matrix				InterpolationMatrix = { XMLoadFloat4x4(&m_DeltaInterpolateMatrix) };

				_vector				vScale, vQuaternion, vTranslation;
				XMMatrixDecompose(&vScale, &vQuaternion, &vTranslation, InterpolationMatrix);

				_vector				vDevideQuaternion = { XMQuaternionSlerp(XMQuaternionIdentity(), XMQuaternionNormalize(vQuaternion), fRatio) };
				_vector				vDevideTranslation = { XMVectorSetW(vTranslation * fRatio, 0.f) };


				_matrix				WorldMatrix = { m_pBlackBoard->Get_AI()->Get_Transform()->Get_WorldMatrix() };
				_vector				vWorldScale, vWorldQuaternion, vWorldTranslation;
				XMMatrixDecompose(&vWorldScale, &vWorldQuaternion, &vWorldTranslation, WorldMatrix);

				_vector				vResultQuaternion = { XMQuaternionMultiply(XMQuaternionNormalize(vWorldQuaternion), XMQuaternionNormalize(vDevideQuaternion)) };
				_vector				vResultTranslation = { XMVectorSetW(vWorldTranslation + vDevideTranslation, 1.f) };

				_matrix				AplliedMatrix = { XMMatrixAffineTransformation(vWorldScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vResultQuaternion, vResultTranslation) };
				m_pBlackBoard->Get_AI()->Get_Transform()->Set_WorldMatrix(AplliedMatrix);
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

	m_pBlackBoard->Get_AI()->Set_ManualMove(false);
	m_pBlackBoard->Release_Nearest_Window();
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

void CIn_Window_Zombie::Set_TargetInterpolate_Matrix()
{
	CWindow* pWindow = { m_pBlackBoard->Get_Nearest_Window() };
	if (nullptr == pWindow)
		return;

	_matrix			WindowWorldMatrix = { pWindow->Get_Transform()->Get_WorldMatrix() };
	_matrix			Zombie_WorldMatrix = { m_pBlackBoard->Get_AI()->Get_Transform()->Get_WorldMatrix() };

	_vector			vZombieScale, vZombieQuaternion, vZombieTranslation;
	_vector			vWindowScale, vWindowQuaternion, vWindowTranslation;

	XMMatrixDecompose(&vZombieScale, &vZombieQuaternion, &vZombieTranslation, Zombie_WorldMatrix);
	XMMatrixDecompose(&vWindowScale, &vWindowQuaternion, &vWindowTranslation, WindowWorldMatrix);

	_matrix			TargetWorldMatrix = { XMMatrixAffineTransformation(vZombieScale, XMVectorSet(0.f, 0.f, 0.f ,1.f), vWindowQuaternion, vWindowTranslation) };

	CModel* pBody_Model = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };

	vector<CAnimation*>		Animations = { pBody_Model->Get_Animations(m_strAnimLayerTag) };

	_matrix			RootFirstKeyFrameMatrix = { pBody_Model->Get_FirstKeyFrame_Root_TransformationMatrix(m_strAnimLayerTag, m_iAnimIndex) };
	_matrix			ModelTransformMatrix = { XMLoadFloat4x4(&pBody_Model->Get_TransformationMatrix()) };

	_vector					vRootScale, vRootQuaternion, vRootTranslation;
	XMMatrixDecompose(&vRootScale, &vRootQuaternion, &vRootTranslation, RootFirstKeyFrameMatrix);

	vRootTranslation = XMVector3TransformNormal(vRootTranslation, ModelTransformMatrix);

	_vector			vRootRotateAxis = { XMVectorSetW(vRootQuaternion, 0.f) };
	vRootRotateAxis = XMVector3TransformNormal(vRootRotateAxis, ModelTransformMatrix);
	vRootQuaternion = XMVectorSetW(vRootRotateAxis, XMVectorGetW(vRootQuaternion));
	RootFirstKeyFrameMatrix = XMMatrixAffineTransformation(vRootScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRootQuaternion, vRootTranslation);

	_matrix			InterpolateTargetMatrix = { RootFirstKeyFrameMatrix * TargetWorldMatrix };

	_vector			vInterpolateScale, vInterpolateQuaternion, vInterpolateTranslation;
	XMMatrixDecompose(&vInterpolateScale, &vInterpolateQuaternion, &vInterpolateTranslation, InterpolateTargetMatrix);

	_vector			vDeltaQuaternion = { XMQuaternionMultiply(XMQuaternionNormalize(XMQuaternionInverse(vZombieQuaternion)), XMQuaternionNormalize(vInterpolateQuaternion)) };
	_vector			vDeltaTranslation = { vInterpolateTranslation - vZombieTranslation };

	_matrix			DeltaMatrix = { XMMatrixAffineTransformation(vZombieScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vDeltaQuaternion, vDeltaTranslation) };

	XMStoreFloat4x4(&m_DeltaInterpolateMatrix, DeltaMatrix);
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
