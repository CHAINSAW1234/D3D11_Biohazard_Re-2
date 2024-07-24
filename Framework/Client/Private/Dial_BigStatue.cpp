#include "stdafx.h"
#include "Dial_BigStatue.h"
#include "Player.h"
#include "Light.h"
#include "BigStatue.h"

#define SAFEBOX_KEY_DISTANCE 0.15f

CDial_BigStatue::CDial_BigStatue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPart_InteractProps{ pDevice, pContext }
{
}

CDial_BigStatue::CDial_BigStatue(const CDial_BigStatue& rhs)
	: CPart_InteractProps{ rhs }
{

}

HRESULT CDial_BigStatue::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDial_BigStatue::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	if (pArg != nullptr)
	{
		DIAL_BIGSTATUE_DESC* desc = (DIAL_BIGSTATUE_DESC*)pArg;
		m_pLockState = desc->pLockState;
		m_pPassword = desc->pPassword;
		m_pCameraControl = desc->pCameraControl;
		m_pPressKeyState = desc->pKeyInput;
		m_pAction = desc->pAction;
		m_pParentWorldMatrix = desc->pParentWorldMatrix;
		m_eBigStatueType = desc->eBigStatueType;
	}

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Initialize_Model()))
		return E_FAIL; 
	m_pTransformCom->Rotation(m_pTransformCom->Get_State_Vector(CTransform::STATE_UP), XMConvertToRadians(180.f));

	m_pModelCom->Set_RootBone("RootNode");
	m_pModelCom->Add_Bone_Layer_All_Bone(TEXT("Default"));

	m_pModelCom->Add_AnimPlayingInfo(false, 0, TEXT("Default"), 1.f);

	
	m_pModelCom->Active_RootMotion_Rotation(false);




	return S_OK;
}

void CDial_BigStatue::Tick(_float fTimeDelta)
{

	if (*m_pLockState== CBigStatue::LIVE_LOCK&& DOWN == m_pGameInstance->Get_KeyState(VK_SPACE))
		m_bCheckAnswer = true;

	if (m_bCheckAnswer)
	{
		_bool bOpen = { false };	
		for (_int i = 0; i < BONE_DIAL_END; i++)
		{
 			if (m_pPassword[i] != abs((m_fGoalAngle[i] < 0 ? m_fGoalAngle[i] + 360.f : m_fGoalAngle[i]) / 60.f))
			{
				bOpen = false;
				break;
			}
			bOpen = true;
		}

		if (bOpen)
		{
			*m_pLockState = CBigStatue::CLEAR_LOCK;
		}
		else
			*m_pLockState = CBigStatue::WRONG_LOCK;

		m_bCheckAnswer = false;
	}
}

void CDial_BigStatue::Late_Tick(_float fTimeDelta)
{
	if (m_bDead)
		return;

	switch (*m_pLockState)
	{
	case CBigStatue::STATIC_LOCK:
		m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pLockState);
		break;

	case CBigStatue::LIVE_LOCK:
	{
		switch (*m_pPressKeyState)
		{
		case CBigStatue::KEY_NOTHING:
			break;

		case CBigStatue::KEY_W:
			m_fGoalAngle[m_iCurBoneIndex] -= 60.f;
			*m_pPressKeyState = CBigStatue::KEY_NOTHING;
			InPutKey_Sound(0,0);
			break;

		case CBigStatue::KEY_A:
			--m_iCurBoneIndex;
			if (m_iCurBoneIndex < BONE_DIAL1)
				m_iCurBoneIndex = BONE_DIAL3;
			*m_pPressKeyState = CBigStatue::KEY_NOTHING;
			InPutKey_Sound(0,1);
			break;

		case CBigStatue::KEY_S:
			//뼈 아래로 돌리기
			m_fGoalAngle[m_iCurBoneIndex] += 60.f;
			*m_pPressKeyState = CBigStatue::KEY_NOTHING;
			InPutKey_Sound(0, 2);
			break;

		case CBigStatue::KEY_D:
			++m_iCurBoneIndex;
			if (m_iCurBoneIndex > BONE_DIAL3)
				m_iCurBoneIndex = BONE_DIAL1;
			*m_pPressKeyState = CBigStatue::KEY_NOTHING;
			InPutKey_Sound(0,3);
			break;
		}

	}
	break;

	case CBigStatue::WRONG_LOCK:
		*m_pLockState = CBigStatue::LIVE_LOCK;
		break;

	case CBigStatue::CLEAR_LOCK:
		m_bClear = true;
		break;
	}
	for (_int i = 0; i < BONE_DIAL_END; i++)
	{
		m_fCurAngle[i] = Lerp(m_fCurAngle[i], m_fGoalAngle[i], fTimeDelta * 5.f);


		_float4			vRotate = { m_WorldMatrix.Right() };

		_vector			vRotateAxis = _vector{ vRotate.x,vRotate.y,vRotate.z,vRotate.w };
		vRotateAxis = XMVector3TransformNormal(vRotateAxis, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));

		_vector				vNewQuaternion = { XMQuaternionRotationAxis(vRotateAxis, XMConvertToRadians(m_fCurAngle[i]+60.f)) };

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

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_POINT, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_DIR, this);
#ifdef ANIM_PROPS_SPOT_SHADOW
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);
#endif


	_matrix			WorldMatrix = { m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pSocketMatrix) * m_pParentsTransform->Get_WorldMatrix() };
	
	XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);

	Get_SpecialBone_Rotation(); // for UI
}


void CDial_BigStatue::InPutKey_Sound(_int iRand, _int iRand1)
{
	switch (iRand1)
	{
	case 0:
		Change_Same_Sound(TEXT("sound_Map_sm42_hieroglyphic_dial_lock2_1.mp3"), 0);
		break;
	case 1:
		Change_Same_Sound(TEXT("sound_Map_sm42_hieroglyphic_dial_lock2_2.mp3"), 1);
		break;
	case 2:
		Change_Same_Sound(TEXT("sound_Map_sm42_hieroglyphic_dial_lock2_3.mp3"), 2);
		break;
	case 3:
		Change_Same_Sound(TEXT("sound_Map_sm42_hieroglyphic_dial_lock2_4.mp3"), 1);
		break;
	case 4:
		Change_Same_Sound(TEXT("sound_Map_sm42_hieroglyphic_dial_lock2_11.mp3"), 1);
		break;
	}


}


HRESULT CDial_BigStatue::Render()
{
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

HRESULT CDial_BigStatue::Render_LightDepth_Dir()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (m_pGameInstance->Get_ShadowLight(CPipeLine::DIRECTION) != nullptr) {

		const CLight* pLight = m_pGameInstance->Get_ShadowLight(CPipeLine::DIRECTION);
		const LIGHT_DESC* pDesc = pLight->Get_LightDesc(0);

		if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &pDesc->ViewMatrix[0])))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &pDesc->ProjMatrix)))
			return E_FAIL;

		list<_uint>			NonHideIndices = { m_pModelCom->Get_NonHideMeshIndices() };
		for (auto& i : NonHideIndices)
		{
			if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXANIMMODEL::PASS_LIGHTDEPTH)))
				return E_FAIL;

			m_pModelCom->Render(static_cast<_uint>(i));
		}
	}

	return S_OK;
}

HRESULT CDial_BigStatue::Render_LightDepth_Point()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	list<LIGHT_DESC*> LightDescList = m_pGameInstance->Get_ShadowPointLightDesc_List();
	_int iIndex = 0;
	for (auto& pLightDesc : LightDescList) {
		const _float4x4* pLightViewMatrices;
		_float4x4 LightProjMatrix;
		pLightViewMatrices = pLightDesc->ViewMatrix;
		LightProjMatrix = pLightDesc->ProjMatrix;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_LightIndex", &iIndex, sizeof(_int))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_Matrices("g_LightViewMatrix", pLightViewMatrices, 6)))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_LightProjMatrix", &LightProjMatrix)))
			return E_FAIL;

		list<_uint>			NonHideIndices = { m_pModelCom->Get_NonHideMeshIndices() };
		for (auto& i : NonHideIndices)
		{
			if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXANIMMODEL::PASS_LIGHTDEPTH_CUBE)))
				return E_FAIL;

			m_pModelCom->Render(static_cast<_uint>(i));
		}

		++iIndex;
	}

	return S_OK;
}

HRESULT CDial_BigStatue::Render_LightDepth_Spot()
{
	if (m_bRender == false)
		return S_OK;

	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (m_pGameInstance->Get_ShadowLight(CPipeLine::SPOT) != nullptr) {

		const CLight* pLight = m_pGameInstance->Get_ShadowLight(CPipeLine::SPOT);
		const LIGHT_DESC* pDesc = pLight->Get_LightDesc(0);

		if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &pDesc->ViewMatrix[0])))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &pDesc->ProjMatrix)))
			return E_FAIL;

		list<_uint>			NonHideIndices = { m_pModelCom->Get_NonHideMeshIndices() };
		for (auto& i : NonHideIndices)
		{
			if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXANIMMODEL::PASS_LIGHTDEPTH)))
				return E_FAIL;

			m_pModelCom->Render(static_cast<_uint>(i));
		}
	}

	return S_OK;
}

HRESULT CDial_BigStatue::Add_Components()
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

HRESULT CDial_BigStatue::Add_PartObjects()
{

	return S_OK;
}

HRESULT CDial_BigStatue::Initialize_PartObjects()
{
	return S_OK;
}

void CDial_BigStatue::Get_SpecialBone_Rotation()
{

}

HRESULT CDial_BigStatue::Initialize_Model()
{
	vector<string>			MeshTags = { m_pModelCom->Get_MeshTags() };
	vector<string>			ResultMeshTags;

	string strFirstTag, strSecondTag, strThirdTag;

	switch (m_eBigStatueType)
	{
	case static_cast<_uint>(CBigStatue::BIGSTATUE_UNICON):
		strFirstTag = "113";
		strSecondTag = "121";
		strThirdTag = "131";
		break;

	case static_cast<_uint>(CBigStatue::BIGSTATUE_WOMAN):
		strFirstTag = "111";
		strSecondTag = "121";
		strThirdTag = "131";
		break;
	
	case static_cast<_uint>(CBigStatue::BIGSTATUE_LION):
		strFirstTag = "113";
		strSecondTag = "121";
		strThirdTag = "131";
		break;
	}

	for (auto& strMeshTag : MeshTags)
		if ((strMeshTag.find(strFirstTag) != string::npos) || (strMeshTag.find(strSecondTag) != string::npos) || (strMeshTag.find(strThirdTag) != string::npos) || (strMeshTag.find("Group_0_") != string::npos) || (strMeshTag.find("Group_1_") != string::npos))
			ResultMeshTags.push_back(strMeshTag);


	for (auto& strMeshTag : MeshTags)
		m_pModelCom->Hide_Mesh(strMeshTag, true);


	for (auto& strMeshTag : ResultMeshTags)
		m_pModelCom->Hide_Mesh(strMeshTag, false);



	return S_OK;
}

CDial_BigStatue* CDial_BigStatue::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDial_BigStatue* pInstance = new CDial_BigStatue(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CDial_BigStatue"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CDial_BigStatue::Clone(void* pArg)
{
	CDial_BigStatue* pInstance = new CDial_BigStatue(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CDial_BigStatue"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDial_BigStatue::Free()
{
	__super::Free();

}
