#include "stdafx.h"
#include "Lock_Cabinet.h"
#include"Player.h"

#include"Cabinet.h"

#define SAFEBOX_KEY_DISTANCE 0.15f

CLock_Cabinet::CLock_Cabinet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPart_InteractProps{ pDevice, pContext }
{
}

CLock_Cabinet::CLock_Cabinet(const CLock_Cabinet& rhs)
	: CPart_InteractProps{ rhs }
{

}

HRESULT CLock_Cabinet::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLock_Cabinet::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	if (pArg != nullptr)
	{
		BODY_LOCK_CABINET_DESC* desc = (BODY_LOCK_CABINET_DESC*)pArg;
		m_eLockType = (LOCK_TYPE)desc->iLockType;
		m_pLockState = desc->pLockState;
		m_pPassword = desc->pPassword;
		m_pCameraControl = desc->pCameraControl;
		m_pPressKeyState = desc->pKeyInput;
	}

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Initialize_Model()))
		return E_FAIL; 

	if (FAILED(Initialize_SafeBox()))
		return E_FAIL;

	m_pModelCom->Set_RootBone("RootNode");
	m_pModelCom->Add_Bone_Layer_All_Bone(TEXT("Default"));

	m_pModelCom->Add_AnimPlayingInfo(false, 0, TEXT("Default"), 1.f);

	
	m_pModelCom->Active_RootMotion_Rotation(false);




	return S_OK;
}

void CLock_Cabinet::Tick(_float fTimeDelta)
{
	if (*m_pLockState== CCabinet::LIVE_LOCK&& DOWN == m_pGameInstance->Get_KeyState(VK_SPACE))
		m_bCheckAnswer = true;

	if (m_bCheckAnswer)
	{
		switch (m_eLockType)
		{
		case SAFEBOX_DIAL:
			break;
		case OPENLOCKER_DIAL:
			_bool bOpen = { false };
			for (_int i = 0; i < BONE_DIAL_END; i++)
			{
				if (m_iOpenDialAnswer[i] != abs((m_fGoalAngle[i]<0?m_fGoalAngle[i]+360.f :m_fGoalAngle[i]) / 60.f))
				{
					bOpen = false;
					break;
				}
				bOpen = true;
			}

			if (bOpen)
			{
				*m_pLockState = CCabinet::CLEAR_LOCK;
			}
			else
 				*m_pLockState = CCabinet::WRONG_LOCK;
			break;
		}
		m_bCheckAnswer = false;
	}
}

void CLock_Cabinet::Late_Tick(_float fTimeDelta)
{
	if (m_bDead)
		return;

	m_eLockType == SAFEBOX_DIAL ? Safebox_Late_Tick(fTimeDelta) : OpenLocker_Late_Tick(fTimeDelta);

	_matrix			WorldMatrix = { m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pSocketMatrix) * (m_pParentsTransform->Get_WorldMatrix()) };
	XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
}

HRESULT CLock_Cabinet::Render()
{
	if (m_bClear)
		return S_OK;
	if (m_bRender == false)
		return S_OK;
	else
		m_bRender = false;
	
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	list<_uint>			NonHideIndices = { m_pModelCom->Get_NonHideMeshIndices() };

	for (auto& i : NonHideIndices)
	{
		if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(i), aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_NormalTexture", static_cast<_uint>(i), aiTextureType_NORMALS)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i))))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_AlphaTexture", static_cast<_uint>(i), aiTextureType_METALNESS)))
		{
			_bool isAlphaTexture = false;
			if (FAILED(m_pShaderCom->Bind_RawValue("g_isAlphaTexture", &isAlphaTexture, sizeof(_bool))))
				return E_FAIL;
		}
		else
		{
			_bool isAlphaTexture = true;
			if (FAILED(m_pShaderCom->Bind_RawValue("g_isAlphaTexture", &isAlphaTexture, sizeof(_bool))))
				return E_FAIL;
		}

		if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_AOTexture", static_cast<_uint>(i), aiTextureType_SHININESS)))
		{
			_bool isAOTexture = false;
			if (FAILED(m_pShaderCom->Bind_RawValue("g_isAOTexture", &isAOTexture, sizeof(_bool))))
				return E_FAIL;
		}
		else
		{
			_bool isAOTexture = true;
			if (FAILED(m_pShaderCom->Bind_RawValue("g_isAOTexture", &isAOTexture, sizeof(_bool))))
				return E_FAIL;
		}


		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		m_pModelCom->Render(static_cast<_uint>(i));
	}


	return S_OK;
}

HRESULT CLock_Cabinet::Add_Components()
{
	/* For.Com_Body_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Body_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	
	/* For.Com_Body_Model */
	if (FAILED(__super::Add_Component(g_Level, m_strModelComponentName,
		TEXT("Com_Body_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;
#ifdef _DEBUG
#ifdef UI_POS
	CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

	ColliderDesc.fRadius = _float(20.f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
	/* For.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Body_Collider"), (CComponent**)&m_pColliderCom[Part_INTERACTPROPS_COL_SPHERE], &ColliderDesc)))
		return E_FAIL;
#endif
#endif

	return S_OK;
}

HRESULT CLock_Cabinet::Add_PartObjects()
{

	return S_OK;
}

HRESULT CLock_Cabinet::Initialize_PartObjects()
{
	return S_OK;
}

void CLock_Cabinet::Get_SpecialBone_Rotation()
{

}

HRESULT CLock_Cabinet::Initialize_Model()
{
	vector<string>			MeshTags = { m_pModelCom->Get_MeshTags() };
	for (auto& strMeshTag : MeshTags)
	{
		if ((strMeshTag.find("Group_0_") != string::npos)|| (strMeshTag.find("Group_1_") != string::npos))
			m_strMeshTag = strMeshTag;
	}

	if (m_eLockType == OPENLOCKER_DIAL)
	{
		vector<string>			MeshTags = { m_pModelCom->Get_MeshTags() };

		vector<string>			ResultMeshTags;			
		_int iFirst = m_pPassword[0] / 5;
		_int iFirstmod = m_pPassword[0] % 5;
		iFirstmod == 0? ( iFirst % 10 == 0 ? iFirst : iFirst -= 1): iFirst;
		string strFirstTag = "11" + to_string(iFirst +1);
		m_iOpenDialAnswer[BONE_DIAL1] = m_pPassword[0] + (iFirst+1) * -5 + 5;


		_int iSecond = m_pPassword[1] / 5;
		_int iSecondmod = m_pPassword[1] % 5;	
		iSecondmod == 0 ?(iSecond % 10 == 0? iSecond: iSecond-=1): iSecond;
		string strSecondtTag = "12" + to_string(iSecond + 1);
		m_iOpenDialAnswer[BONE_DIAL2] = m_pPassword[1] + (iSecond + 1) * -5 + 5;

		_int iThird = m_pPassword[2] / 5;
		_int iThirdmod = m_pPassword[2] % 5;
		iThirdmod == 0 ? (iThird % 10 == 0 ? iThird : iThird -= 1) : iThird;
		string strThirdTag = "13" + to_string(iThird + 1);
		m_iOpenDialAnswer[BONE_DIAL3] = m_pPassword[2] + (iThird + 1) * -5 + 5;

		for (auto& strMeshTag : MeshTags)
			if ((strMeshTag.find(strFirstTag) != string::npos) ||(strMeshTag.find(strSecondtTag) != string::npos) || (strMeshTag.find(strThirdTag) != string::npos) || (strMeshTag.find("Group_0_") != string::npos) || (strMeshTag.find("Group_1_") != string::npos))
				ResultMeshTags.push_back(strMeshTag);
			

		for (auto& strMeshTag : MeshTags)
			m_pModelCom->Hide_Mesh(strMeshTag, true);
		

		for (auto& strMeshTag : ResultMeshTags)
			m_pModelCom->Hide_Mesh(strMeshTag, false);
		


	}

	return S_OK;
}

HRESULT CLock_Cabinet::Initialize_SafeBox()
{
	if (SAFEBOX_DIAL == m_eLockType)
	{
		/* 실제 번호 :  왼 6 . 오 2 . 왼11 */
		SAFEBOX_PASSWORD password;

		password.iCount = m_pPassword[0];
		password.eAllow = LOCK_ALLOW_KEY::LEFT_LOCK_KEY;
		m_eClearKey.push_back(password);

		password.iCount = m_pPassword[1];
		password.eAllow = LOCK_ALLOW_KEY::RIGHT_LOCK_KEY;
		m_eClearKey.push_back(password);

		password.iCount = m_pPassword[2];
		password.eAllow = LOCK_ALLOW_KEY::LEFT_LOCK_KEY;
		m_eClearKey.push_back(password);
	}

	return S_OK;
}
 
void CLock_Cabinet::Safebox_Late_Tick(_float fTimeDelta)
{
	switch (*m_pLockState)
	{
	case CCabinet::STATIC_LOCK:
	{
		m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pLockState);

		Safebox_Return();
	}
	break;

	case CCabinet::LIVE_LOCK:
	{
		/* 1. 조작 키*/
		if (DOWN == m_pGameInstance->Get_KeyState('Q')) /* 왼쪽 */
		{
			m_eMoveingKey = LOCK_ALLOW_KEY::LEFT_LOCK_KEY;
			m_iRotationCnt--;
			
		}
		
		else if (DOWN == m_pGameInstance->Get_KeyState('E')) /* 오른쪽 */
		{
			m_eMoveingKey = LOCK_ALLOW_KEY::RIGHT_LOCK_KEY;

			m_iRotationCnt++;
		}

		/* 2. 행동 패턴 */
		if(LOCK_ALLOW_KEY::END_LOCK_KEY != m_eMoveingKey)
		{
			Safebox_RotationLock(m_eMoveingKey, fTimeDelta);
		}

		/* 3. Clear 조건 */
		if (DOWN == m_pGameInstance->Get_KeyState(VK_SPACE))
		{
			Safebox_Clear_Condition();
		}
	}
	break;

	case CCabinet::WRONG_LOCK:
		m_eMoveingKey = LOCK_ALLOW_KEY::RIGHT_LOCK_KEY;
		Safebox_RotationLock(m_eMoveingKey, fTimeDelta);
		break;

	case CCabinet::CLEAR_LOCK:
		break;
	}

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_DIR, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_POINT, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);

	_float3	vDirection = { };
	_float4 fTransform4 = m_pParentsTransform->Get_State_Float4(CTransform::STATE_POSITION);
	_float3 fTransform3 = _float3{ fTransform4.x,fTransform4.y,fTransform4.z };

	if (*m_pLockState == CCabinet::STATIC_LOCK || LOCK_ALLOW_KEY::END_LOCK_KEY != m_eMoveingKey || m_eMoveingKey == LOCK_ALLOW_KEY::LEFT_LOCK_KEY)
		m_pModelCom->Play_Animations(m_pParentsTransform, fTimeDelta, &vDirection);
	
	Get_SpecialBone_Rotation(); // for UI
}

void CLock_Cabinet::Safebox_RotationLock(LOCK_ALLOW_KEY _eKeyType, _float fTimeDelta)
{
	if(*m_pLockState == CCabinet::LIVE_LOCK)
	{
		if (false == m_isPosition_Register)
		{
			m_isPosition_Register = true;

			m_fPrevRotationAngle = m_fRotationAngle;
		}

		/* 1. 회전 방향 */
		if (LOCK_ALLOW_KEY::LEFT_LOCK_KEY == _eKeyType)
		{
			m_fRotationAngle -= fTimeDelta * 30.f;
				
			/* 4. 목표 회전을 채웠다면 초기화 */
			if (7.5f * m_iRotationCnt >= m_fRotationAngle)
			{
				m_fRotationAngle = 7.5f * m_iRotationCnt;

				m_eMoveingKey = LOCK_ALLOW_KEY::END_LOCK_KEY;

				m_isPosition_Register = false;


				/* 회전을 바꿨다면 */
				if (m_eCurrentKey.empty() || LOCK_ALLOW_KEY::LEFT_LOCK_KEY != m_eCurrentKey.back().eAllow)
				{
					SAFEBOX_PASSWORD eResult;
					eResult.iCount = 1;
					eResult.eAllow = LOCK_ALLOW_KEY::LEFT_LOCK_KEY;

					m_eCurrentKey.push_back(eResult);
				}
				else 
					m_eCurrentKey[m_eCurrentKey.size() - 1].iCount++;

				return;
			}
		}

		else if (LOCK_ALLOW_KEY::RIGHT_LOCK_KEY == _eKeyType)
		{
			m_fRotationAngle += fTimeDelta * 30.f; 

			/* 4. 목표 회전을 채웠다면 초기화 */
			if (7.5f * m_iRotationCnt <= m_fRotationAngle)
			{
				m_fRotationAngle = 7.5f * m_iRotationCnt;

				m_eMoveingKey = LOCK_ALLOW_KEY::END_LOCK_KEY;

				m_isPosition_Register = false;


				/* 회전을 바꿨다면 */
				if (m_eCurrentKey.empty() || LOCK_ALLOW_KEY::RIGHT_LOCK_KEY != m_eCurrentKey.back().eAllow)
				{
					SAFEBOX_PASSWORD eResult;
					eResult.iCount = 1;
					eResult.eAllow = LOCK_ALLOW_KEY::RIGHT_LOCK_KEY;

					m_eCurrentKey.push_back(eResult);
				}
				else 
					m_eCurrentKey[m_eCurrentKey.size() - 1].iCount++;

				return;
			}
		}
	}

	else if (*m_pLockState == CCabinet::WRONG_LOCK)
	{
		m_fWrongTimer += fTimeDelta;

		if (LOCK_ALLOW_KEY::LEFT_LOCK_KEY == _eKeyType)
			m_fRotationAngle -= 7.f;

		else if (LOCK_ALLOW_KEY::RIGHT_LOCK_KEY == _eKeyType)
			m_fRotationAngle += 7.f;

		if (m_fWrongTimer >= 0.8f)
		{
			m_fWrongTimer = 0.f;

			*m_pLockState = (_ubyte)CCabinet::LOCK_STATE::STATIC_LOCK;
			
			*m_pCameraControl = true;
		}
	}

	/* 2. 회전 행렬 생성 */
	_vector vRotateAxis = { m_pTransformCom->Get_State_Vector(CTransform::STATE_LOOK) };
	_vector vNewQuaternion = { XMQuaternionRotationAxis(vRotateAxis, XMConvertToRadians(m_fRotationAngle)) };
	_matrix RotationMatrix = { XMMatrixRotationQuaternion(vNewQuaternion) };

	/* 3. 뼈 회전 적용 */
	vector<string> BoneNames = { m_pModelCom->Get_BoneNames() };
	list<_uint> ChildJointIndices;
	m_pModelCom->Get_Child_ZointIndices("RootNode", "_01_end_end", ChildJointIndices);

	for (auto& iJointIndex : ChildJointIndices)
	{
		m_pModelCom->Add_Additional_Transformation_World(BoneNames[iJointIndex], RotationMatrix);
	}
}

void CLock_Cabinet::Safebox_Clear_Condition()
{
	/*왼 6.오 2.왼11*/
	if (m_eCurrentKey.size() != m_eClearKey.size())
	{
		*m_pLockState = (_ubyte)CCabinet::LOCK_STATE::WRONG_LOCK;
		return;
	}
	
	if(m_eCurrentKey[0].iCount!= m_eClearKey[0].iCount || m_eCurrentKey[0].eAllow != m_eClearKey[0].eAllow ||
		m_eCurrentKey[1].iCount != m_eClearKey[1].iCount || m_eCurrentKey[1].eAllow != m_eClearKey[1].eAllow || 
		m_eCurrentKey[2].iCount != m_eClearKey[2].iCount || m_eCurrentKey[2].eAllow != m_eClearKey[2].eAllow)
	{
		*m_pLockState = (_ubyte)CCabinet::LOCK_STATE::WRONG_LOCK;
		return;
	}

	*m_pLockState = (_ubyte)CCabinet::LOCK_STATE::CLEAR_LOCK;
}

void CLock_Cabinet::Safebox_Return()
{
	m_eCurrentKey.clear();

	m_eMoveingKey = { LOCK_ALLOW_KEY::END_LOCK_KEY };

	m_iCurrentKey_Cnt = 0;

	m_iRotationCnt = { 0 };

	m_fPrevRotationAngle = { 0.f };

	m_fRotationAngle = { 0.f };

	m_isPosition_Register = { false };
}

void CLock_Cabinet::OpenLocker_Late_Tick(_float fTimeDelta)
{

	switch (*m_pLockState)
	{
	case CCabinet::STATIC_LOCK:
		m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pLockState);
		break;

	case CCabinet::LIVE_LOCK:
	{
		m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pLockState);

		switch (*m_pPressKeyState)
		{
		case CCabinet::KEY_NOTHING:
			
			break;

		case CCabinet::KEY_W:
			//뼈 위로 돌리기
			m_fGoalAngle[m_iCurBoneIndex] += 60.f;
			/*if (360.f <= m_fGoalAngle[m_iCurBoneIndex] )
				m_fGoalAngle[m_iCurBoneIndex] = 360.f;*/

			*m_pPressKeyState = CCabinet::KEY_NOTHING;
			break;

		case CCabinet::KEY_A:
			//뼈 인덱스 바꾸기 이미 뼈 인덱스 0이면 max로
			--m_iCurBoneIndex;
			if (m_iCurBoneIndex < BONE_DIAL1)
				m_iCurBoneIndex = BONE_DIAL3;
			*m_pPressKeyState = CCabinet::KEY_NOTHING;
			break;

		case CCabinet::KEY_S:
			//뼈 아래로 돌리기
			m_fGoalAngle[m_iCurBoneIndex] -= 60.f;
			
			/*if (-360.f >= m_fGoalAngle[m_iCurBoneIndex])
				m_fGoalAngle[m_iCurBoneIndex] = -360.f;*/
			*m_pPressKeyState = CCabinet::KEY_NOTHING;

			break;

		case CCabinet::KEY_D:
			//뼈 인덱스 바꾸기 이미 뼈 인덱스 max이면 0

			++m_iCurBoneIndex;
			if (m_iCurBoneIndex > BONE_DIAL3)
				m_iCurBoneIndex = BONE_DIAL1;
			*m_pPressKeyState = CCabinet::KEY_NOTHING;
			break;
		}

	}
	break;

	case CCabinet::WRONG_LOCK:
		m_pModelCom->Change_Animation(0, TEXT("Default"), (_int)(*m_pLockState) );
		if (m_pModelCom->isFinished(0))
			*m_pLockState = CCabinet::LIVE_LOCK;
		
		break;

	case CCabinet::CLEAR_LOCK:

		m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pLockState );
		if (m_pModelCom->isFinished(0))
			m_bClear = true;
		break;
	}
	for (_int i = 0; i < BONE_DIAL_END; i++)
	{
		m_fCurAngle[i] = Lerp(m_fCurAngle[i], m_fGoalAngle[i], fTimeDelta*5.f);


		_float4			vRotate = { m_WorldMatrix.Right()};
		
		_vector			vRotateAxis = _vector{ vRotate.x,vRotate.y,vRotate.z,vRotate.w };
		vRotateAxis = XMVector3TransformNormal(vRotateAxis, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));

		_vector				vNewQuaternion = { XMQuaternionRotationAxis(vRotateAxis, XMConvertToRadians(m_fCurAngle[i])) };

		vNewQuaternion = XMQuaternionNormalize(vNewQuaternion);
		_matrix				RotationMatrix = { XMMatrixRotationQuaternion(vNewQuaternion) };
		_float4x4          test = RotationMatrix;
		m_pModelCom->Add_Additional_Transformation_World(m_strOpenDial[i], RotationMatrix);

		if (m_fCurAngle[i] >= 359.5f && m_fGoalAngle[i] >= 360.f)
		{
			m_fCurAngle[i] = 0.f;
			m_fGoalAngle[i] -= 360.f;
		}
		else if (m_fCurAngle[i] < -359.5f && m_fGoalAngle[i] <= -360.f)
		{
			m_fCurAngle[i] = 0.f;
			m_fGoalAngle[i] += 360.f;
		}
		
	}
	_float4 fTransform4 = m_pParentsTransform->Get_State_Float4(CTransform::STATE_POSITION);
	_float3 fTransform3 = _float3{ fTransform4.x,fTransform4.y,fTransform4.z };
	_float3				vDirection = { };

	m_pModelCom->Play_Animations(m_pParentsTransform, fTimeDelta, &vDirection);
	if (*m_pLockState != CCabinet::CLEAR_LOCK)
	{
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_POINT, this);
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_DIR, this);
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);
	}
	else
		if (!m_pModelCom->isFinished(0))
		{
			m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

			m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_POINT, this);
			m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_DIR, this);
			m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);
		}
	


	Get_SpecialBone_Rotation(); // for UI

}

CLock_Cabinet* CLock_Cabinet::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLock_Cabinet* pInstance = new CLock_Cabinet(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CLock_Cabinet"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CLock_Cabinet::Clone(void* pArg)
{
	CLock_Cabinet* pInstance = new CLock_Cabinet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CLock_Cabinet"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLock_Cabinet::Free()
{
	__super::Free();

}
