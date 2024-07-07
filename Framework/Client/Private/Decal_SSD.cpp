#include "stdafx.h"
#include "Decal_SSD.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"

CDecal_SSD::CDecal_SSD(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CDecal_SSD::CDecal_SSD(const CDecal_SSD& rhs)
	: CGameObject{ rhs }
{

}

HRESULT CDecal_SSD::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDecal_SSD::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_vExtent = _float3(0.25f, 0.25f, 0.25f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float4(2.f, 0.f, 3.f, 1.f));

	m_bRender = false;

	return S_OK;
}

void CDecal_SSD::Tick(_float fTimeDelta)
{

}

void CDecal_SSD::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_DECAL, this);
}

HRESULT CDecal_SSD::Render()
{
	if (m_bRender == false)
		return S_OK;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	return m_pGameInstance->Render_Decal_Deferred();
}

void CDecal_SSD::SetWorldMatrix(_float4x4 WorldMatrix)
{
	m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&WorldMatrix));
}

void CDecal_SSD::SetWorldMatrix_With_HitNormal(_vector vUp)
{
	m_pTransformCom->SetWorldMatrix_With_UpVector(vUp);
}

void CDecal_SSD::SetPosition(_float4 vPos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_iFrame = m_pGameInstance->GetRandom_Int(0, 10);
}

void CDecal_SSD::LookAt(_float4 vDir)
{
	_float4 vPos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
	
	vPos.x += vDir.x;
	vPos.y += vDir.y;
	vPos.z += vDir.z;
	vPos.z += 0.001f;

	m_vNormal = vDir;
	m_pTransformCom->Look_At(XMLoadFloat4(&vPos));
}

HRESULT CDecal_SSD::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Decal_Blood_SSD"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CDecal_SSD::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	auto pDeferredShader = m_pGameInstance->GetDeferredShader();

	if (FAILED(pDeferredShader->Bind_Matrix("g_Decal_WorldMatrix_Inv", &m_pTransformCom->Get_WorldFloat4x4_Inverse())))
		return E_FAIL;
	if (FAILED(pDeferredShader->Bind_RawValue("g_vExtent", &m_vExtent,sizeof(_float3))))
		return E_FAIL;
	if (FAILED(pDeferredShader->Bind_RawValue("g_vDecalNormal", &m_vNormal, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(pDeferredShader, "g_DecalTexture",m_iFrame)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RTShaderResource(pDeferredShader, TEXT("Target_Field_Depth"), "g_DepthTexture_Decal")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RTShaderResource(pDeferredShader, TEXT("Target_Normal"), "g_NormalTexture")))
		return E_FAIL;

	return S_OK;
}

CDecal_SSD* CDecal_SSD::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDecal_SSD* pInstance = new CDecal_SSD(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CDecal_SSD"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CDecal_SSD::Clone(void* pArg)
{
	CDecal_SSD* pInstance = new CDecal_SSD(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CDecal_SSD"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDecal_SSD::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
}
