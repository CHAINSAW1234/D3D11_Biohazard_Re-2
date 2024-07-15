#include "stdafx.h"
#include "Key_Door.h"
#include"Player.h"
#include "Emblem_Door.h"
CKey_Door::CKey_Door(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPart_InteractProps{ pDevice, pContext }
{
}

CKey_Door::CKey_Door(const CKey_Door& rhs)
	: CPart_InteractProps{ rhs }
{

}

HRESULT CKey_Door::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CKey_Door::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (pArg != nullptr)
	{
		KEY_DOOR* desc = (KEY_DOOR*)pArg;

		m_pEmblemAnim = desc->EmblemAnim;

		m_pParentsTransform = desc->pParentsTransform;

		m_pParentWorldMatrix = desc->pParentWorldMatrix;
	}

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Initialize_Model()))
		return E_FAIL;
	
	m_pTransformCom->Set_Scaled(100.f, 100.f, 100.f);
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
	
	return S_OK;
}

void CKey_Door::Tick(_float fTimeDelta)
{
	/* Static 상태 */
	if (DOWN == m_pGameInstance->Get_KeyState('1'))
		*m_pEmblemAnim = 0;

	/* 아이템을 사용할 수 있어서 열었다면, */
	if (DOWN == m_pGameInstance->Get_KeyState('2'))
		m_isKeyUsing = true;

	/* 처음 문에 접속했을 때 덜컹덜컹 */
	if (DOWN == m_pGameInstance->Get_KeyState('3'))
		*m_pEmblemAnim = 2;

	if (true == m_isKeyUsing)
	{
		m_isKeyRender = true;

		*m_pEmblemAnim = 1;
	}
}

void CKey_Door::Late_Tick(_float fTimeDelta)
{
	if (m_bDead)
		return;

	_matrix			WorldMatrix = { m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pSocketMatrix) * XMLoadFloat4x4(m_pParentWorldMatrix) };

	XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_POINT, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_DIR, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);

	Get_SpecialBone_Rotation(); // for UI	
}

HRESULT CKey_Door::Render()
{
	if (false == m_isKeyRender)
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

HRESULT CKey_Door::Add_Components()
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

HRESULT CKey_Door::Add_PartObjects()
{

	return S_OK;
}

HRESULT CKey_Door::Initialize_PartObjects()
{
	return S_OK;
}


HRESULT CKey_Door::Initialize_Model()
{
	vector<string>			MeshTags = { m_pModelCom->Get_MeshTags() };

	for (auto& strMeshTag : MeshTags)
	{
		if ((strMeshTag.find("Group_0_") != string::npos) || (strMeshTag.find("Group_1_") != string::npos))
			m_strMeshTag = strMeshTag;
	}
	
	return S_OK;
}


CKey_Door* CKey_Door::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CKey_Door* pInstance = new CKey_Door(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CKey_Door"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CKey_Door::Clone(void* pArg)
{
	CKey_Door* pInstance = new CKey_Door(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CKey_Door"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CKey_Door::Free()
{
	__super::Free();

}
