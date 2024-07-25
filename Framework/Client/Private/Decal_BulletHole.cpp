#include "stdafx.h"
#include "Decal_BulletHole.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"

CDecal_BulletHole::CDecal_BulletHole(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CDecal_BulletHole::CDecal_BulletHole(const CDecal_BulletHole& rhs)
	: CGameObject{ rhs }
{

}

HRESULT CDecal_BulletHole::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDecal_BulletHole::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_vExtent = _float3(0.07f, 0.07f, 0.07f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float4(2.f, 0.f, 3.f, 1.f));

	m_bRender = false;

	m_pGameInstance->Add_Object_Sound(m_pTransformCom, 1);

	return S_OK;
}

void CDecal_BulletHole::Tick(_float fTimeDelta)
{
	m_iFrame = 0;
}

void CDecal_BulletHole::Late_Tick(_float fTimeDelta)
{
	if (m_bRender)
	{
		if (IsPlayerNearBy())
		{
			if (m_pGameInstance->isInFrustum_WorldSpace(m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION), m_vExtent.x))
			{
				m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_DECAL, this);
			}
		}
	}
}

HRESULT CDecal_BulletHole::Render()
{
	if (m_bRender == false)
		return S_OK;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	return m_pGameInstance->Render_Decal_Deferred();
}

void CDecal_BulletHole::SetWorldMatrix(_float4x4 WorldMatrix)
{
	m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&WorldMatrix));
}

void CDecal_BulletHole::SetWorldMatrix_With_HitNormal(_vector vUp)
{
	m_pTransformCom->SetWorldMatrix_With_UpVector(vUp);
}

void CDecal_BulletHole::SetPosition(_float4 vPos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_iFrame = m_pGameInstance->GetRandom_Int(0, 10);
}

void CDecal_BulletHole::LookAt(_float4 vDir)
{
	_float4 vPos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
	
	vPos.x += vDir.x;
	vPos.y += vDir.y;
	vPos.z += vDir.z;
	vPos.z += 0.001f;

	m_vNormal = vDir;
	m_pTransformCom->Look_At(XMLoadFloat4(&vPos));

	PlaySound();
}

void CDecal_BulletHole::PlaySound()
{
	const wchar_t* str = L"Wall_Shot_";
	wchar_t result[32];
	_int inum = m_pGameInstance->GetRandom_Int(0, 3);

	std::swprintf(result, sizeof(result) / sizeof(wchar_t), L"%ls%d.mp3", str, inum);

	m_pGameInstance->Change_Sound_3D(m_pTransformCom, result, 0);
	m_pGameInstance->Set_Volume_3D(m_pTransformCom, 0, 0.5f);
}

_bool CDecal_BulletHole::IsPlayerNearBy()
{
	auto pPlayer = m_pGameInstance->GetPlayer();
	auto vPlayerPos = pPlayer->GetPositionVector();
	auto vPos = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);

	_vector vDelta = vPlayerPos - vPos;
	if (XMVectorGetX(XMVector3Length(vDelta)) < CULLING_DISTANCE)
		return true;
	else
		return false;
}

HRESULT CDecal_BulletHole::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Decal_BulletHole"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CDecal_BulletHole::Bind_ShaderResources()
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
	if (FAILED(pDeferredShader->Bind_RawValue("g_bBlood", &m_bBlood, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(pDeferredShader, "g_DecalTexture",m_iFrame)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RTShaderResource(pDeferredShader, TEXT("Target_Field_Depth"), "g_DepthTexture_Decal")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RTShaderResource(pDeferredShader, TEXT("Target_Normal"), "g_NormalTexture")))
		return E_FAIL;

	return S_OK;
}

CDecal_BulletHole* CDecal_BulletHole::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDecal_BulletHole* pInstance = new CDecal_BulletHole(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CDecal_BulletHole"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CDecal_BulletHole::Clone(void* pArg)
{
	CDecal_BulletHole* pInstance = new CDecal_BulletHole(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CDecal_BulletHole"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDecal_BulletHole::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
}
