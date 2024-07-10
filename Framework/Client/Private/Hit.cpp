#include "stdafx.h"
#include "..\Public\Hit.h"

#include "GameInstance.h"

CHit::CHit(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect{ pDevice, pContext }
{
}

CHit::CHit(const CHit & rhs)
	: CEffect{ rhs }
{
}

HRESULT CHit::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHit::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_ImgSize = m_pTextureCom->GetImgSize();
	m_DivideCount = m_pTextureCom->GetDivideCount();
	m_iFrame_Total = m_DivideCount.first * m_DivideCount.second;

	m_bRender = false;

	_float fRotation = m_pGameInstance->GetRandom_Real(0, 10.f);

	m_pTransformCom->Turn(m_pTransformCom->Get_State_Vector(CTransform::STATE_LOOK), fRotation);

	return S_OK;
}

void CHit::Tick(_float fTimeDelta)
{
	if (m_bRender == false)
	{
		return;
	}

	if (m_iFrame > m_iFrame_Total-3)
	{
		m_bRender = false;
		m_iFrame = 0;
		return;
	}

	++m_iFrame;

	Compute_CurrentUV();

	Setup_Billboard();
}

void CHit::Late_Tick(_float fTimeDelta)
{
	if(m_bRender == true)
	{
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_EFFECT_BLOOM, this);
	}
}

HRESULT CHit::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(2);

	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	return S_OK;
}

void CHit::Compute_CurrentUV()
{
	IMG_SIZE      ImgSize = m_pTextureCom->GetImgSize();

	_uint   iSizeX = { (_uint)((_float)ImgSize.iSizeX / (_float)m_DivideCount.first) };
	_uint   iSizeY = { (_uint)((_float)ImgSize.iSizeY / (_float)m_DivideCount.second) };

	_uint   iCurrentFrame =	m_iFrame;

	_uint   iCurrentX = iCurrentFrame % m_DivideCount.first;
	_uint   iCurrentY = iCurrentFrame / m_DivideCount.first;

	m_fMinUV_X = _float(iCurrentX * iSizeX) / ImgSize.iSizeX;
	m_fMaxUV_X = _float((iCurrentX + 1) * iSizeX) / ImgSize.iSizeX;
	m_fMinUV_Y = _float(iCurrentY * iSizeY) / ImgSize.iSizeY;
	m_fMaxUV_Y = _float((iCurrentY + 1) * iSizeY) / ImgSize.iSizeY;
}

void CHit::Setup_Billboard()
{
	m_pTransformCom->Look_At(m_pGameInstance->Get_Camera_Pos_Float4());
}

void CHit::SetSize(_float fSizeX, _float fSizeY)
{
	m_fSizeX = fSizeX;
	m_fSizeY = fSizeY;

	m_pTransformCom->Set_Scaled(fSizeX, fSizeY,1.f);
}

void CHit::SetPosition(_float4 Pos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, Pos);
	m_iFrame = 0;
	m_fAlpha_Delta_Sum = 0.f;
}

HRESULT CHit::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Hit"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CHit::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fMinUV_X", &m_fMinUV_X,sizeof(m_fMinUV_X))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fMinUV_Y", &m_fMinUV_Y,sizeof(m_fMinUV_Y))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fMaxUV_X", &m_fMaxUV_X,sizeof(m_fMaxUV_X))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fMaxUV_Y", &m_fMaxUV_Y, sizeof(m_fMaxUV_Y))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha_Delta", &m_fAlpha_Delta_Sum, sizeof(m_fAlpha_Delta_Sum))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShaderCom, TEXT("Target_Depth"), "g_DepthTexture")))
		return E_FAIL;

	return S_OK;
}

CHit * CHit::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CHit*		pInstance = new CHit(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CHit"));

		Safe_Release(pInstance);
	}

	pInstance->Initialize(nullptr);

	return pInstance;

}

CGameObject * CHit::Clone(void * pArg)
{
	CHit*		pInstance = new CHit(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CHit"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHit::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
