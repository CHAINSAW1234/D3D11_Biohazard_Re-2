#include "stdafx.h"
#include "..\Public\HG_Cartridge.h"

#include "GameInstance.h"
#include "Rigid_Dynamic.h"

CHG_Cartridge::CHG_Cartridge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect{ pDevice, pContext }
{
}

CHG_Cartridge::CHG_Cartridge(const CHG_Cartridge& rhs)
	: CEffect{ rhs }
{
}

HRESULT CHG_Cartridge::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHG_Cartridge::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_bRender = false;

	m_FrameDelay = 30;

	m_fDissolveSpeed = 0.05f;

	m_pTransformCom->Set_Scaled(0.01f, 0.01f, 0.01f);

	m_pRigid_Dynamic = m_pGameInstance->Create_Rigid_Dynamic(m_pModelCom, m_pTransformCom, &m_iIndex_RigidBody, this);
	m_pRigid_Dynamic->SetKinematic(true);

	m_pModelCom->Release_Decal_Dump();

	return S_OK;
}

void CHG_Cartridge::Tick(_float fTimeDelta)
{
	if (m_bRender == false)
		return;

	if (m_pRigid_Dynamic)
	{
		m_pTransformCom->Set_WorldMatrix(m_pRigid_Dynamic->GetWorldMatrix_Rigid_Dynamic(m_pTransformCom->Get_Scaled()));

		m_pTransformCom->Set_Scaled(0.01f, 0.01f, 0.01f);
	}
}

void CHG_Cartridge::Late_Tick(_float fTimeDelta)
{
	if (m_bRender == true)
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CHG_Cartridge::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(m_iFrame), aiTextureType_DIFFUSE)))
		return E_FAIL;

	if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_AlphaTexture", static_cast<_uint>(m_iFrame), aiTextureType_METALNESS)))
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

	if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_AOTexture", static_cast<_uint>(m_iFrame), aiTextureType_SHININESS)))
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

	if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_EmissiveTexture", static_cast<_uint>(m_iFrame), aiTextureType_EMISSIVE)))
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

	m_pModelCom->Render(static_cast<_uint>(m_iFrame));

	return S_OK;
}

void CHG_Cartridge::SetSize(_float fSizeX, _float fSizeY, _float fSizeZ)
{
	
}

void CHG_Cartridge::SetWorldMatrix_With_HitNormal(_vector vUp)
{
	m_pTransformCom->SetWorldMatrix_With_UpVector(vUp);
}

_float4x4 CHG_Cartridge::GetWorldMatrix()
{
	return m_pTransformCom->Get_WorldMatrix_Pure();
}

_float4 CHG_Cartridge::GetPosition()
{
	return m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
}

void CHG_Cartridge::Start()
{
#pragma region 메모리 정리
	m_pModelCom->Release_Dump();
#pragma endregion
}

void CHG_Cartridge::Initiate(_float4 vPos, _float4 vDir,_float4 vLook)
{
	m_bRender = true;

	m_pRigid_Dynamic->SetPosition(vPos);

	m_vDir = _float4(vDir.x + m_pGameInstance->GetRandom_Real(-0.03f, 0.03f),
		vDir.y + m_pGameInstance->GetRandom_Real(0.05f, 0.1f),
		vDir.z + m_pGameInstance->GetRandom_Real(-0.03f, 0.03f),
		0.f);

	m_vDir = Float4_Normalize(m_vDir);

	m_pRigid_Dynamic->SetKinematic(false);
	m_pRigid_Dynamic->AddForce(m_vDir);
}

HRESULT CHG_Cartridge::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_HG_Cartridge"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CHG_Cartridge::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CHG_Cartridge* CHG_Cartridge::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHG_Cartridge* pInstance = new CHG_Cartridge(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CHG_Cartridge"));

		Safe_Release(pInstance);
	}

	pInstance->Initialize(nullptr);

	return pInstance;

}

CGameObject* CHG_Cartridge::Clone(void* pArg)
{
	CHG_Cartridge* pInstance = new CHG_Cartridge(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CHG_Cartridge"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHG_Cartridge::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
