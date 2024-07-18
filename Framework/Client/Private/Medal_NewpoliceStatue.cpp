#include "stdafx.h"
#include "Medal_NewpoliceStatue.h"
#include"Player.h"

CMedal_NewpoliceStatue::CMedal_NewpoliceStatue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPart_InteractProps{ pDevice, pContext }
{
}

CMedal_NewpoliceStatue::CMedal_NewpoliceStatue(const CMedal_NewpoliceStatue& rhs)
	: CPart_InteractProps{ rhs }
{

}

HRESULT CMedal_NewpoliceStatue::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMedal_NewpoliceStatue::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (pArg != nullptr)
	{
		BODY_MEDAL_POLICESTATUS* desc = (BODY_MEDAL_POLICESTATUS*)pArg;
		
		m_eMedelType = desc->eMedelType;
		m_eRender_MedalType = desc->eMedalRenderType;
	}

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Initialize_Model()))
		return E_FAIL;
	
	m_pTransformCom->Set_Scaled(100, 100, 100);

	
	return S_OK;
}

void CMedal_NewpoliceStatue::Tick(_float fTimeDelta)
{
}

void CMedal_NewpoliceStatue::Late_Tick(_float fTimeDelta)
{
	if (m_bDead)
		return;

	m_pTransformCom->Set_Scaled(130.f, 130.f, 130.f);

	_float4x4 vTrans = m_pTransformCom->Get_WorldMatrix();
	vTrans._43 += 54.3f;
	vTrans._42 -= 58.f;

	/* 왼쪽 메달 : 유니콘*/
	if (MEDAL_TYPE::MEDAL_UNICORN == m_eMedelType)
	{
		vTrans._41 -= 2.3f;
		vTrans._42 -= 1.f;
		vTrans._43 -= 0.3f;
	}
	/* 오른쪽 메달 : 사자 */
	else if (MEDAL_TYPE::MEDAL_LION == m_eMedelType)
	{
		vTrans._41 += 4.f;
		vTrans._42 -= 3.f;
		vTrans._43 -= 2.5f;

	}
	/* 가운데 메달 : 여자*/
	else if (MEDAL_TYPE::MEDAL_WOMAN == m_eMedelType)
	{
		vTrans._41 -= 1.2f;
		vTrans._42 -= 0.7f;
	}

	_matrix			WorldMatrix = { vTrans * XMLoadFloat4x4(m_pSocketMatrix) * (m_pParentsTransform->Get_WorldMatrix()) };
	XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_POINT, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_DIR, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);

	Get_SpecialBone_Rotation(); // for UI
}

HRESULT CMedal_NewpoliceStatue::Render()
{
	if (m_bRender == false)
		return S_OK;
	else
		m_bRender = false;

	if (false == m_eRender_MedalType[(_int)m_eMedelType])
		return E_FAIL;

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


		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		m_pModelCom->Render(static_cast<_uint>(i));
	}


	return S_OK;
}

HRESULT CMedal_NewpoliceStatue::Add_Components()
{
	/* For.Com_Body_Shader */
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

HRESULT CMedal_NewpoliceStatue::Add_PartObjects()
{

	return S_OK;
}

HRESULT CMedal_NewpoliceStatue::Initialize_PartObjects()
{
	return S_OK;
}

void CMedal_NewpoliceStatue::Get_SpecialBone_Rotation()
{

}

HRESULT CMedal_NewpoliceStatue::Initialize_Model()
{
	vector<string>			MeshTags = { m_pModelCom->Get_MeshTags() };

	for (auto& strMeshTag : MeshTags)
	{
		if ((strMeshTag.find("Group_0_") != string::npos) || (strMeshTag.find("Group_1_") != string::npos))
			m_strMeshTag = strMeshTag;
	}
	
	return S_OK;
}


CMedal_NewpoliceStatue* CMedal_NewpoliceStatue::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMedal_NewpoliceStatue* pInstance = new CMedal_NewpoliceStatue(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMedal_NewpoliceStatue"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CMedal_NewpoliceStatue::Clone(void* pArg)
{
	CMedal_NewpoliceStatue* pInstance = new CMedal_NewpoliceStatue(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CMedal_NewpoliceStatue"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMedal_NewpoliceStatue::Free()
{
	__super::Free();

}
