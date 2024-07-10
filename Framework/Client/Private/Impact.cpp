#include "stdafx.h"
#include "..\Public\Impact.h"

#include "GameInstance.h"

CImpact::CImpact(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect{ pDevice, pContext }
{
}

CImpact::CImpact(const CImpact & rhs)
	: CEffect{ rhs }
{
}

HRESULT CImpact::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CImpact::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_ImgSize = m_pTextureCom->GetImgSize();
	m_DivideCount = m_pTextureCom->GetDivideCount();

	m_bRender = false;

	m_FrameDelay = 25;

	m_fAlpha_Delta = 0.5f;

	return S_OK;
}

void CImpact::Tick(_float fTimeDelta)
{
	if (m_bRender == false)
	{
		return;
	}

	m_fSizeX += 0.09f;
	m_fSizeY += 0.09f;
	m_pTransformCom->Set_Scaled(m_fSizeX, m_fSizeY, 1.f);

	if (m_FrameDelay + m_FrameTime < GetTickCount64())
	{
		m_FrameTime = GetTickCount64();
		++m_iFrame;
	}

	if (m_iFrame >= 3)
	{
		m_bRender = false;
		m_iFrame = 0;
		return;
	}

	Setup_Billboard();
}

void CImpact::Late_Tick(_float fTimeDelta)
{
	if(m_bRender == true)
	{
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_EFFECT_BLOOM, this);
	}
}

HRESULT CImpact::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(7);

	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	return S_OK;
}

void CImpact::Setup_Billboard()
{
	m_pTransformCom->Look_At(m_pGameInstance->Get_Camera_Pos_Float4());
}

void CImpact::SetSize(_float fSizeX, _float fSizeY)
{
	m_fSizeX = fSizeX;
	m_fSizeY = fSizeY;
	m_fDefaultSize_X = fSizeX;
	m_fDefaultSize_Y = fSizeY;
	m_pTransformCom->Set_Scaled(fSizeX, fSizeY,1.f);
}

void CImpact::SetPosition(_float4 Pos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, Pos);
	m_iFrame = 0;
	m_fAlpha_Delta_Sum = 0.f;
	m_FrameTime = GetTickCount64();

	m_fSizeX = m_fDefaultSize_X;
	m_fSizeY = m_fDefaultSize_Y;
	m_pTransformCom->Set_Scaled(m_fSizeX, m_fSizeY, 1.f);
}

HRESULT CImpact::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Impact"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CImpact::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha_Delta", &m_fAlpha_Delta,sizeof(m_fAlpha_Delta))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture",m_iFrame)))
		return E_FAIL;

	return S_OK;
}

CImpact * CImpact::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CImpact*		pInstance = new CImpact(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CImpact"));

		Safe_Release(pInstance);
	}

	pInstance->Initialize(nullptr);

	return pInstance;

}

CGameObject * CImpact::Clone(void * pArg)
{
	CImpact*		pInstance = new CImpact(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CImpact"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CImpact::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
