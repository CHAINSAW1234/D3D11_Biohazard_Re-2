#include "stdafx.h"
#include "Body_ItemProp.h"
#include"Player.h"

#include"ItemProp.h"


CBody_ItemProp::CBody_ItemProp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPart_InteractProps{ pDevice, pContext }
{
}

CBody_ItemProp::CBody_ItemProp(const CBody_ItemProp& rhs)
	: CPart_InteractProps{ rhs }
{

}

HRESULT CBody_ItemProp::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBody_ItemProp::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Initialize_Model()))
		return E_FAIL; 


	BODY_ITEMPROPS_DESC* pDesc = (BODY_ITEMPROPS_DESC*)pArg;
	m_pItemDead = pDesc->pItemDead;
	m_iItemIndex = pDesc->iItemIndex;

	/*m_pModelCom->Set_RootBone("RootNode");
	m_pModelCom->Add_Bone_Layer_All_Bone(TEXT("Default"));

	m_pModelCom->Add_AnimPlayingInfo(false, 0, TEXT("Default"), 1.f);


	m_pModelCom->Active_RootMotion_Rotation(true);*/
	//m_pTransformCom->Set_WorldMatrix(m_tagPropDesc.worldMatrix);




	if (m_strModelComponentName.find(TEXT("_Anim")) == wstring::npos&&(m_strModelComponentName.find(TEXT("ShotGun")) == wstring::npos&&m_strModelComponentName.find(TEXT("emergencyspray01")) == wstring::npos))
		m_pTransformCom->Set_Scaled(100.f, 100.f, 100.f);



#ifndef NON_COLLISION_PROP

	//m_pGameInstance->Create_Px_Collider(m_pModelCom, m_pParentsTransform, &m_iPx_Collider_Id);

#endif



	return S_OK;
}

void CBody_ItemProp::Tick(_float fTimeDelta)
{
	//__super::Tick(fTimeDelta);
}

void CBody_ItemProp::Late_Tick(_float fTimeDelta)
{
	if (*m_pItemDead)
		return;

	/*
	m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pState);

	_float4 fTransform4 = m_pParentsTransform->Get_State_Float4(CTransform::STATE_POSITION);
	_float3 fTransform3 = _float3{ fTransform4.x,fTransform4.y,fTransform4.z };
	m_pModelCom->Play_Animation_Light(m_pParentsTransform, fTimeDelta);*/

	//if (m_strModelComponentName.find(TEXT("ShotGun")) != wstring::npos)
	//	m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);
	if (m_pSocketMatrix != nullptr)
	{
		_matrix			WorldMatrix = { m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pSocketMatrix) * (m_pParentsTransform->Get_WorldMatrix()) };
		XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
	}
	else
	{
		_matrix			WorldMatrix = { (m_pTransformCom->Get_WorldMatrix() *  m_pParentsTransform->Get_WorldMatrix() ) };
		XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
	}


	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_FIELD_SHADOW_POINT, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_FIELD_SHADOW_DIR, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);

	Get_SpecialBone_Rotation();

}

HRESULT CBody_ItemProp::Render()
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

		if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXMODEL::PASS_DEFAULT)))
			return E_FAIL;

		m_pModelCom->Render(static_cast<_uint>(i));
	}
	return S_OK;
}

HRESULT CBody_ItemProp::Add_Components()
{
	/* For.Com_Body_Shader */
	//와 이거 나중에 골때려지겠네 Anim 아이템은 어떻게 가져오냐ㅠ
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
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

HRESULT CBody_ItemProp::Add_PartObjects()
{

	return S_OK;
}

HRESULT CBody_ItemProp::Initialize_PartObjects()
{
	return S_OK;
}

void CBody_ItemProp::Get_SpecialBone_Rotation()
{

}

HRESULT CBody_ItemProp::Initialize_Model()
{
	vector<string>			MeshTags = { m_pModelCom->Get_MeshTags() };
	for (auto& strMeshTag : MeshTags)
	{
		if ((strMeshTag.find("Group_0_") != string::npos)|| (strMeshTag.find("Group_1_") != string::npos))
			m_strMeshTag = strMeshTag;
	}
	if (m_strModelComponentName.find(TEXT("ShotGun")) != wstring::npos)
	{

		vector<string>			ResultMeshTags;
		vector<string>			HidMeshs;

		for (auto& strMeshTag : MeshTags)
		{
			if ((strMeshTag.find("Group_3_Sub_1") == string::npos) && (strMeshTag.find("Group_4_Sub_1") == string::npos))
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

	}
	return S_OK;
}

_float4 CBody_ItemProp::Get_Pos(_int iArg)
{

	if (m_pSocketMatrix == nullptr)
	{
		m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);
		_float4 vLocalPos = m_pModelCom->Get_Mesh_Local_Pos(m_strMeshTag) + _float4(0.f, 10.f, 0.f, 0.f);

		_matrix Local_Mesh_Matrix = m_pTransformCom->Get_WorldMatrix();
		Local_Mesh_Matrix.r[3] += vLocalPos;
		_matrix TransformationMatrix = m_pParentsTransform->Get_WorldMatrix();

		_float4x4 WorldMatrix = (Local_Mesh_Matrix)*TransformationMatrix;

		_float4 vPos = XMVectorSetW(WorldMatrix.Translation(), 1.f);
		if(m_strModelComponentName.find(TEXT("_Anim")) == wstring::npos)
			m_pTransformCom->Set_Scaled(100.f, 100.f, 100.f);
#ifdef _DEBUG
#ifdef UI_POS
		m_pColliderCom[Part_INTERACTPROPS_COL_SPHERE]->Tick(WorldMatrix);
		m_pGameInstance->Add_DebugComponents(m_pColliderCom[Part_INTERACTPROPS_COL_SPHERE]);
#endif
#endif
		return vPos;
	}
	else
	{
		m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);
		_float4 vLocalPos = m_pModelCom->Get_Mesh_Local_Pos(m_strMeshTag) + _float4(0.f, 10.f, 0.f, 0.f);

		_matrix Local_Mesh_Matrix = m_pTransformCom->Get_WorldMatrix();
		Local_Mesh_Matrix.r[3] += vLocalPos;
		_matrix TransformationMatrix = m_pParentsTransform->Get_WorldMatrix();

		_float4x4 WorldMatrix = (Local_Mesh_Matrix)*XMLoadFloat4x4(m_pSocketMatrix) *TransformationMatrix;

		_float4 vPos = XMVectorSetW(WorldMatrix.Translation(), 1.f);
		if (m_strModelComponentName.find(TEXT("_Anim")) == wstring::npos)
			m_pTransformCom->Set_Scaled(100.f, 100.f, 100.f);
#ifdef _DEBUG
#ifdef UI_POS
		m_pColliderCom[Part_INTERACTPROPS_COL_SPHERE]->Tick(WorldMatrix);
		m_pGameInstance->Add_DebugComponents(m_pColliderCom[Part_INTERACTPROPS_COL_SPHERE]);
#endif
#endif
		return vPos;
	}

	return _float4();
}


CBody_ItemProp* CBody_ItemProp::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_ItemProp* pInstance = new CBody_ItemProp(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBody_ItemProp"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CBody_ItemProp::Clone(void* pArg)
{
	CBody_ItemProp* pInstance = new CBody_ItemProp(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CBody_ItemProp"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_ItemProp::Free()
{
	__super::Free();

}
