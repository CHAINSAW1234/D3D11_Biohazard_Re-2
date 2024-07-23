#include "stdafx.h"
#include "Body_MovingShelf.h"
#include"Player.h"
#include "PxCollider.h"
#include "Bone.h"

#include"MovingShelf.h"
#include"Light.h"

CBody_MovingShlef::CBody_MovingShlef(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPart_InteractProps{ pDevice, pContext }
{
}

CBody_MovingShlef::CBody_MovingShlef(const CBody_MovingShlef& rhs)
	: CPart_InteractProps{ rhs }
{

}

HRESULT CBody_MovingShlef::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBody_MovingShlef::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	if (FAILED(Add_Components()))
		return E_FAIL;

	
	if (FAILED(Initialize_Model()))
		return E_FAIL;


	m_pModelCom->Set_RootBone("RootNode");
	m_pModelCom->Add_Bone_Layer_All_Bone(TEXT("Default"));

	m_pModelCom->Add_AnimPlayingInfo(false, 0, TEXT("Default"), 1.f);
	
	m_pModelCom->Active_RootMotion_Rotation(false);

	//m_pModelCom->Active_RootMotion_Rotation(true);
	
#ifndef NON_COLLISION_PROP

	//m_pPx_Collider = m_pGameInstance->Create_Px_Collider_Cabinet(m_pModelCom, m_pParentsTransform, &m_iPx_Collider_Id);

	//m_vecRotationBone[ANIM_BONE_TYPE_COLLIDER_CABINET::ATC_CABINET_DOOR] = m_pModelCom->Get_BonePtr("_01");

#endif
	return S_OK;
}

void CBody_MovingShlef::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	if (*m_pState == CMovingShelf::SHELF_FINISH && m_pModelCom->isFinished(0))
		return;
}

void CBody_MovingShlef::Late_Tick(_float fTimeDelta)
{
	switch (*m_pState)
	{
	case CMovingShelf::SHELF_FINISH:
		m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pState);
		Change_Sound(TEXT("sound_Map_sm42_move_shelf2_3.mp3"), 0);

		break;
	case CMovingShelf::SHELF_MOVE:
	{
		m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pState);
		Change_Sound(TEXT("sound_Map_sm42_move_shelf2_6.mp3"), 0);

		break;
	}
	case CMovingShelf::SHELF_START:
	{
		m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pState);
		Change_Sound(TEXT("sound_Map_sm42_move_shelf2_4.mp3"), 0);

		break;
	}
	case CMovingShelf::SHELF_STOP:
	{
		m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pState);
		Change_Sound(TEXT("sound_Map_sm42_move_shelf2_1.mp3"), 0);

		break;
	}
	case CMovingShelf::SHELF_STATIC:
	{
		m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pState);

		break;
	}
	}

	_float4 fTransform4 = m_pParentsTransform->Get_State_Float4(CTransform::STATE_POSITION);
	_float3 fTransform3 = _float3{ fTransform4.x,fTransform4.y,fTransform4.z };
	_float3 fTemp = {};
	m_pModelCom->Play_Animations(m_pParentsTransform, fTimeDelta, &fTemp);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_POINT, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_DIR, this);
#ifdef SPOT_FRUSTRUM_CULLING
	if (m_bRender)
	{
		if (m_bLocalized == false)
		{
			if (m_pGameInstance->isInFrustum_WorldSpace(m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION), 0.2f))
			{
				m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);
			}
		}
	}
#endif

#ifndef SPOT_FRUSTRUM_CULLING
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);
#endif
	//Get_SpecialBone_Rotation(); // for UI

#ifdef _DEBUG
#ifdef UI_POS
	m_pGameInstance->Add_DebugComponents(m_pColliderCom[Part_INTERACTPROPS_COL_SPHERE]);
#endif
#endif
}

HRESULT CBody_MovingShlef::Render()
{
	if (m_bRender == false)
		return S_OK;
	else
		m_bRender = false;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
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

		if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_EmissiveTexture", static_cast<_uint>(i), aiTextureType_EMISSIVE)))
		{
			_bool isEmissive = false;
			if (FAILED(m_pShaderCom->Bind_RawValue("g_isEmissiveTexture", &isEmissive, sizeof(_bool))))
				return E_FAIL;
		}
		else
		{
			_bool isEmissive = true;
			if (FAILED(m_pShaderCom->Bind_RawValue("g_isEmissiveTexture", &isEmissive, sizeof(_bool))))
				return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXANIMMODEL::PASS_DEFAULT)))
			return E_FAIL;

		m_pModelCom->Render(static_cast<_uint>(i));

	}

	return S_OK;
}

HRESULT CBody_MovingShlef::Render_LightDepth_Dir()
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

HRESULT CBody_MovingShlef::Render_LightDepth_Point()
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

HRESULT CBody_MovingShlef::Render_LightDepth_Spot()
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

HRESULT CBody_MovingShlef::Add_Components()
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

HRESULT CBody_MovingShlef::Add_PartObjects()
{

	return S_OK;
}

HRESULT CBody_MovingShlef::Initialize_PartObjects()
{

	return S_OK;
}

HRESULT CBody_MovingShlef::Initialize_Model_i44()
{
	/* Set_Hide_Mesh */
	vector<string>			MeshTags = { m_pModelCom->Get_MeshTags() };

	vector<string>			ResultMeshTags;
	for (auto& strMeshTag : MeshTags)
	{
		string strFindTag = "10" + to_string(m_iPropType);
		if (m_iPropType >= 10)
			strFindTag = "20" + to_string(m_iPropType - 10);
		if ((strMeshTag.find(strFindTag) != string::npos) || (strMeshTag.find("Group_0_") != string::npos) || (strMeshTag.find("Group_1_") != string::npos))
			ResultMeshTags.push_back(strMeshTag);
	}

	for (auto& strMeshTag : MeshTags)
	{
		m_pModelCom->Hide_Mesh(strMeshTag, true);
	}

	for (auto& strMeshTag : ResultMeshTags)
	{
		m_pModelCom->Hide_Mesh(strMeshTag, false);
	}

	m_NonHideIndices = { m_pModelCom->Get_NonHideMeshIndices() };

	return S_OK;
}

HRESULT CBody_MovingShlef::Initialize_Model()
{

	vector<string>			MeshTags = { m_pModelCom->Get_MeshTags() };
	for (auto& strMeshTag : MeshTags)
	{
		if ((strMeshTag.find("Group_1_Sub_2") != string::npos)||(strMeshTag.find("Group_3_Sub_1") != string::npos)||(strMeshTag.find("Group_2_Sub_1") != string::npos))
			m_strMeshTag = strMeshTag;
	}
	
	return S_OK;

}

_float4 CBody_MovingShlef::Get_Pos(_int iArg)
{
	_float4 vLocalPos =m_pModelCom->Get_Mesh_Local_Pos(m_strMeshTag);
	_matrix Local_Mesh_Matrix = m_pTransformCom->Get_WorldMatrix();
	Local_Mesh_Matrix.r[3] -= _vector{ vLocalPos.x,-vLocalPos.y,vLocalPos.z };
	_matrix TransformationMatrix = m_pParentsTransform->Get_WorldMatrix();

	XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(m_vRotation);

	_float4x4 WorldMatrix = Local_Mesh_Matrix* rotationMatrix * TransformationMatrix;
#ifdef _DEBUG
#ifdef UI_POS
	m_pColliderCom[Part_INTERACTPROPS_COL_SPHERE]->Tick(WorldMatrix);
#endif
#endif
	_float4 vPos = XMVectorSetW(WorldMatrix.Translation(), 1.f);
	return vPos;
}

void CBody_MovingShlef::Get_SpecialBone_Rotation()
{
	_matrix Combined = m_vecRotationBone[FIRE_WALL_ROTATE_BONE_TYPE::DOOR]->Get_TrasformationMatrix();

	_vector scale, rotation, translation;
	XMMatrixDecompose(&scale, &rotation, &translation, Combined);
	m_vRotation = rotation;
}


CBody_MovingShlef* CBody_MovingShlef::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_MovingShlef* pInstance = new CBody_MovingShlef(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBody_MovingShlef"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CBody_MovingShlef::Clone(void* pArg)
{
	CBody_MovingShlef* pInstance = new CBody_MovingShlef(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CBody_MovingShlef"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_MovingShlef::Free()
{
	__super::Free();

}
