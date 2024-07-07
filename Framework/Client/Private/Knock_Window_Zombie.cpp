#include "stdafx.h"
#include "GameInstance.h"
#include "Knock_Window_Zombie.h"
#include "BlackBoard_Zombie.h"

#include "Window.h"
#include "Player.h"
#include "Zombie.h"
#include "Body_Zombie.h"

CKnock_Window_Zombie::CKnock_Window_Zombie()
	: CTask_Node()
{
}

CKnock_Window_Zombie::CKnock_Window_Zombie(const CKnock_Window_Zombie& rhs)
{
}

HRESULT CKnock_Window_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

void CKnock_Window_Zombie::Enter()
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel* pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	m_fAccKnockTime = 0.f;
	m_fAccLinearInterpolateTime = 0.f;

	m_pBlackBoard->Get_AI()->Set_ManualMove(true);

	Set_TargetInterpolate_Matrix();

#ifdef _DEBUG

	cout << "Enter Knock Door" << endl;

#endif 
}

_bool CKnock_Window_Zombie::Execute(_float fTimeDelta)
{
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion

	//	필요 조건 => 위치 도달, 창문 안깨짐, 바리게이트 여부(상관없음)
	CWindow* pWindow = { m_pBlackBoard->Get_Nearest_Window() };
	if (nullptr == pWindow)
		return false;
	_int				iWindowHP = { pWindow->Get_HP_Body() };
	_bool				isWindowBroken = { 0 >= iWindowHP };
	if (true == isWindowBroken)
		return false;

	MONSTER_STATE		eMonsterState = { m_pBlackBoard->Get_AI()->Get_Current_MonsterState() };
	if (MONSTER_STATE::MST_KNOCK_WINDOW != eMonsterState)
	{
		CWindow* pWindow = { m_pBlackBoard->Get_Nearest_Window() };
		if (nullptr == pWindow)
			return false;

		m_eStartDirectionToWindow = DIRECTION::_END;
		_vector			vWindowPosition = { pWindow->Get_Transform()->Get_State_Vector(CTransform::STATE_POSITION) };
		if (false == m_pBlackBoard->Compute_Direction_To_Target_8Direction_Local(vWindowPosition, &m_eStartDirectionToWindow))
			return false;

		if (DIRECTION::_END == m_eStartDirectionToWindow)
			return false;


		if (DIRECTION::_B == m_eStartDirectionToWindow)
		{
#ifdef _DEBUG
			MSG_BOX(TEXT("Called : _bool CKnock_Window_Zombie::Execute(_float fTimeDelta) 창문이 뒤에있다 "));
#endif
			return false;
		}

	}

	m_pBlackBoard->Organize_PreState(this);

	auto pAI = m_pBlackBoard->Get_AI();
	pAI->Set_State(MONSTER_STATE::MST_KNOCK_WINDOW);


	m_fAccKnockTime += fTimeDelta;
	if (m_fAccKnockTime >= ZOMBIE_ATTACK_WINDOW_NEED_TIME)
	{
		m_fAccKnockTime -= ZOMBIE_ATTACK_WINDOW_NEED_TIME;
		pWindow->Attack_Prop();
	}

	if (m_fAccLinearInterpolateTime < ZOMBIE_KNOCK_TOTAL_INTERPOLATE_TO_WINDOW_TIME)
	{
		_bool				isComplete = { m_fAccLinearInterpolateTime >= ZOMBIE_KNOCK_TOTAL_INTERPOLATE_TO_WINDOW_TIME };
		if (false == isComplete)
		{
			_float				fTime = fTimeDelta;
			m_fAccLinearInterpolateTime += fTime;

			if (m_fAccLinearInterpolateTime >= ZOMBIE_KNOCK_TOTAL_INTERPOLATE_TO_WINDOW_TIME)
			{
				fTime -= m_fAccLinearInterpolateTime - ZOMBIE_KNOCK_TOTAL_INTERPOLATE_TO_WINDOW_TIME;
			}
			_float				fRatio = { fTime / ZOMBIE_KNOCK_TOTAL_INTERPOLATE_TO_WINDOW_TIME };

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

void CKnock_Window_Zombie::Exit()
{
	if (nullptr == m_pBlackBoard)
		return;

	//	m_pBlackBoard->Get_AI()->Set_ManualMove(false);
}

void CKnock_Window_Zombie::Change_Animation(_float fTimeDelta)
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel*			pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	_int			iResultAnimationIndex = { -1 };
	_bool			isLoop = { false };

	wstring			strCurrentAnimLayerTag = { pBodyModel->Get_CurrentAnimLayerTag(static_cast<_uint>(m_eBasePlayingIndex)) };
	_int			iCurrentAnimIndex = { pBodyModel->Get_CurrentAnimIndex(static_cast<_uint>(m_eBasePlayingIndex)) };

	_bool			isSameAnimLayer = { strCurrentAnimLayerTag == m_strAnimLayerTag };
	_bool			isKnockAnimIndex = {
		iCurrentAnimIndex == static_cast<_int>(ANIM_GIMMICK_WINDOW::_KNOCK_FROM_F_LOOP) ||
		iCurrentAnimIndex == static_cast<_int>(ANIM_GIMMICK_WINDOW::_KNOCK_FROM_L_LOOP) ||
		iCurrentAnimIndex == static_cast<_int>(ANIM_GIMMICK_WINDOW::_KNOCK_FROM_R_LOOP) ||
		iCurrentAnimIndex == static_cast<_int>(ANIM_GIMMICK_WINDOW::_KNOCK_FROM_F_START) ||
		iCurrentAnimIndex == static_cast<_int>(ANIM_GIMMICK_WINDOW::_KNOCK_FROM_L_START) ||
		iCurrentAnimIndex == static_cast<_int>(ANIM_GIMMICK_WINDOW::_KNOCK_FROM_R_START)
	};

	if (isSameAnimLayer && isKnockAnimIndex)
	{
		//	루프 모션 진행 중
		if (iCurrentAnimIndex == static_cast<_int>(ANIM_GIMMICK_WINDOW::_KNOCK_FROM_F_LOOP) ||
			iCurrentAnimIndex == static_cast<_int>(ANIM_GIMMICK_WINDOW::_KNOCK_FROM_L_LOOP) ||
			iCurrentAnimIndex == static_cast<_int>(ANIM_GIMMICK_WINDOW::_KNOCK_FROM_R_LOOP))
		{
			return;
		}

		//	시작모션이 끝난경우 혹은 진행 중
		if (iCurrentAnimIndex == static_cast<_int>(ANIM_GIMMICK_WINDOW::_KNOCK_FROM_F_START) ||
			iCurrentAnimIndex == static_cast<_int>(ANIM_GIMMICK_WINDOW::_KNOCK_FROM_L_START) ||
			iCurrentAnimIndex == static_cast<_int>(ANIM_GIMMICK_WINDOW::_KNOCK_FROM_R_START))
		{
			if (true == pBodyModel->isFinished(static_cast<_uint>(m_eBasePlayingIndex)))
			{
				if (DIRECTION::_F == m_eStartDirectionToWindow)
				{
					iResultAnimationIndex = static_cast<_int>(ANIM_GIMMICK_WINDOW::_KNOCK_FROM_F_LOOP);
				}

				else if (DIRECTION::_FL == m_eStartDirectionToWindow ||
					DIRECTION::_L == m_eStartDirectionToWindow ||
					DIRECTION::_BL == m_eStartDirectionToWindow)
				{
					iResultAnimationIndex = static_cast<_int>(ANIM_GIMMICK_WINDOW::_KNOCK_FROM_L_LOOP);
				}

				else if (DIRECTION::_FR == m_eStartDirectionToWindow ||
					DIRECTION::_R == m_eStartDirectionToWindow ||
					DIRECTION::_BR == m_eStartDirectionToWindow)
				{
					iResultAnimationIndex = static_cast<_int>(ANIM_GIMMICK_WINDOW::_KNOCK_FROM_R_LOOP);
				}

				isLoop = true;
			}

			//	시작 모션 진행 중 ( 안 끝 남 )
			else
			{
				return;
			}
		}
	}	

	//	아직 관련 모션이아니었던 경우
	else
	{
		if (DIRECTION::_F == m_eStartDirectionToWindow)
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_GIMMICK_WINDOW::_KNOCK_FROM_F_START);
		}

		else if (DIRECTION::_FL == m_eStartDirectionToWindow ||
			DIRECTION::_L == m_eStartDirectionToWindow ||
			DIRECTION::_BL == m_eStartDirectionToWindow)
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_GIMMICK_WINDOW::_KNOCK_FROM_L_START);
		}

		else if (DIRECTION::_FR == m_eStartDirectionToWindow ||
			DIRECTION::_R == m_eStartDirectionToWindow ||
			DIRECTION::_BR == m_eStartDirectionToWindow)
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_GIMMICK_WINDOW::_KNOCK_FROM_R_START);
		}

		isLoop = false;
	}
	
	if (-1 == iResultAnimationIndex)
		return;

	pBodyModel->Change_Animation(static_cast<_uint>(m_eBasePlayingIndex), m_strAnimLayerTag, iResultAnimationIndex);
	pBodyModel->Set_Loop(static_cast<_uint>(m_eBasePlayingIndex), isLoop);

#pragma endregion
}

void CKnock_Window_Zombie::Set_TargetInterpolate_Matrix()
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

	CModel*			pBody_Model = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };

	vector<CAnimation*>		Animations = { pBody_Model->Get_Animations(m_strAnimLayerTag) };
	_int					iStartAnimIndex = { -1 };
	if (DIRECTION::_F == m_eStartDirectionToWindow)
	{
		iStartAnimIndex = static_cast<_uint>(ANIM_GIMMICK_WINDOW::_KNOCK_FROM_F_START);
	}

	else if (DIRECTION::_FL == m_eStartDirectionToWindow ||
		DIRECTION::_L == m_eStartDirectionToWindow ||
		DIRECTION::_BL == m_eStartDirectionToWindow)
	{
		iStartAnimIndex = static_cast<_uint>(ANIM_GIMMICK_WINDOW::_KNOCK_FROM_L_START);
	}

	else if (DIRECTION::_FR == m_eStartDirectionToWindow ||
		DIRECTION::_R == m_eStartDirectionToWindow ||
		DIRECTION::_BR == m_eStartDirectionToWindow)
	{
		iStartAnimIndex = static_cast<_uint>(ANIM_GIMMICK_WINDOW::_KNOCK_FROM_R_START);
	}

	_matrix			RootFirstKeyFrameMatrix = { pBody_Model->Get_FirstKeyFrame_Root_TransformationMatrix(m_strAnimLayerTag, iStartAnimIndex) };
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

CKnock_Window_Zombie* CKnock_Window_Zombie::Create(void* pArg)
{
	CKnock_Window_Zombie* pInstance = { new CKnock_Window_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CKnock_Window_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CKnock_Window_Zombie::Free()
{
	__super::Free();
}
