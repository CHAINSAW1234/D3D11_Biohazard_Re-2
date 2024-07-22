#include "stdafx.h"
#include "Medal_BigStatue.h"
#include"Player.h"

CMedal_BigStatue::CMedal_BigStatue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPart_InteractProps{ pDevice, pContext }
{
}

CMedal_BigStatue::CMedal_BigStatue(const CMedal_BigStatue& rhs)
	: CPart_InteractProps{ rhs }
{

}

HRESULT CMedal_BigStatue::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMedal_BigStatue::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (pArg != nullptr)
	{
		MEDAL_BIGSTATUE_DESC* desc = (MEDAL_BIGSTATUE_DESC*)pArg;

		m_eMedelType = desc->eMedelType;

		m_pParentWorldMatrix = desc->pParentWorldMatrix;

		m_eMedelType = desc->eMedelType;

		m_isMedalAnim = desc->isMedalAnim;
	}

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Initialize_Model()))
		return E_FAIL;
	
	m_pTransformCom->Set_WorldMatrix(XMMatrixRotationY(XMConvertToRadians(180.f)));

	m_pTransformCom->Set_Scaled(100, 100, 100);

	return S_OK;
}

void CMedal_BigStatue::Tick(_float fTimeDelta)
{
	//if (m_eMedelType == MEDAL_TYPE::MEDAL_UNICORN)
	//	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));

	//else if (m_eMedelType == MEDAL_TYPE::MEDAL_LION)
	//	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(0.f));

	//else
	//	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
}

void CMedal_BigStatue::Late_Tick(_float fTimeDelta)
{
	if (m_bDead)
		return;
	
	
	_float4x4 vMedalpos = m_pTransformCom->Get_WorldMatrix();
	vMedalpos._43 -= 1.f;

	if (nullptr != m_isMedalAnim)
	{
		if (true == *m_isMedalAnim)
		{
			/* LN : 키 클리어 : sound_Map_sm41_wisdom_statue2_1 */
			if (m_fStore_ZPos - 2.5f < m_pTransformCom->Get_WorldFloat4x4()._43)
			{
				vMedalpos._43 -= fTimeDelta * 1.5f;
				m_pTransformCom->Set_WorldMatrix(vMedalpos);
			}
		}
		else
			m_fStore_ZPos = vMedalpos._43;
	}

	_matrix			WorldMatrix = { vMedalpos * XMLoadFloat4x4(m_pParentWorldMatrix) };
	XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_POINT, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_DIR, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);

	Get_SpecialBone_Rotation(); // for UI
}

HRESULT CMedal_BigStatue::Render()
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


		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		m_pModelCom->Render(static_cast<_uint>(i));
	}


	return S_OK;
}

HRESULT CMedal_BigStatue::Add_Components()
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

HRESULT CMedal_BigStatue::Add_PartObjects()
{

	return S_OK;
}

HRESULT CMedal_BigStatue::Initialize_PartObjects()
{
	return S_OK;
}

void CMedal_BigStatue::Get_SpecialBone_Rotation()
{

}

HRESULT CMedal_BigStatue::Initialize_Model()
{
	vector<string>			MeshTags = { m_pModelCom->Get_MeshTags() };

	for (auto& strMeshTag : MeshTags)
	{
		if ((strMeshTag.find("Group_0_") != string::npos) || (strMeshTag.find("Group_1_") != string::npos))
			m_strMeshTag = strMeshTag;
	}

	return S_OK;
}


CMedal_BigStatue* CMedal_BigStatue::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMedal_BigStatue* pInstance = new CMedal_BigStatue(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMedal_BigStatue"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CMedal_BigStatue::Clone(void* pArg)
{
	CMedal_BigStatue* pInstance = new CMedal_BigStatue(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CMedal_BigStatue"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMedal_BigStatue::Free()
{
	__super::Free();

}
