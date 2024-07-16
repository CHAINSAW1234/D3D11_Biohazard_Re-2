#include "stdafx.h"
#include "..\Public\Grenade_Explosion.h"

#include "GameInstance.h"

CGrenade_Explosion::CGrenade_Explosion(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect{ pDevice, pContext }
{
}

CGrenade_Explosion::CGrenade_Explosion(const CGrenade_Explosion & rhs)
	: CEffect{ rhs }
{
}

HRESULT CGrenade_Explosion::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGrenade_Explosion::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_ImgSize = m_pTextureCom->GetImgSize();
	m_DivideCount = m_pTextureCom->GetDivideCount();
	m_iFrame_Total = m_DivideCount.first * m_DivideCount.second;

	m_bRender = false;

	m_FrameDelay = 5;

	m_fAlpha_Delta_Sum = 0.f;
	m_fAlpha_Delta = 0.f;
	m_fAlpha = 1.f;

	return S_OK;
}

void CGrenade_Explosion::Tick(_float fTimeDelta)
{
	if (m_bRender == false)
		return;

	if (m_FrameDelay + m_FrameTime < GetTickCount64())
	{
		++m_iFrame;
		m_FrameTime = GetTickCount64();
	}

	//m_fSizeX += 0.05f;
	//m_fSizeY += 0.05f;
	//m_pTransformCom->Set_Scaled(m_fSizeX, m_fSizeY, 1.f);
	/*auto vPos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
	vPos.y += 0.05f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);*/

	if (m_iFrame >= m_iFrame_Total)
	{
		m_fSizeX = m_fDefaultSize_X;
		m_fSizeY = m_fDefaultSize_Y;
		m_pTransformCom->Set_Scaled(m_fSizeX, m_fSizeY, 1.f);

		m_bRender = false;
		m_iFrame = 0;
	}

	Compute_CurrentUV();

	Setup_Billboard();
}

void CGrenade_Explosion::Late_Tick(_float fTimeDelta)
{
	if(m_bRender == true)
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_EFFECT_BLOOM, this);
}

HRESULT CGrenade_Explosion::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(2);

	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	return S_OK;
}

void CGrenade_Explosion::Compute_CurrentUV()
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

void CGrenade_Explosion::Setup_Billboard()
{
	m_pTransformCom->Look_At(m_pGameInstance->Get_CamPosition_Vector());
}

void CGrenade_Explosion::SetSize(_float fSizeX, _float fSizeY)
{
	m_fSizeX = fSizeX;
	m_fSizeY = fSizeY;

	m_fDefaultSize_X = fSizeX;
	m_fDefaultSize_Y = fSizeY;

	m_pTransformCom->Set_Scaled(fSizeX, fSizeY,1.f);
}

void CGrenade_Explosion::SetPosition(_float4 Pos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, Pos);
	m_fAlpha_Delta_Sum = 0.f;
}

HRESULT CGrenade_Explosion::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Grenade_Explosion"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGrenade_Explosion::Bind_ShaderResources()
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

CGrenade_Explosion * CGrenade_Explosion::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CGrenade_Explosion*		pInstance = new CGrenade_Explosion(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CGrenade_Explosion"));

		Safe_Release(pInstance);
	}

	pInstance->Initialize(nullptr);

	return pInstance;

}

CGameObject * CGrenade_Explosion::Clone(void * pArg)
{
	CGrenade_Explosion*		pInstance = new CGrenade_Explosion(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CGrenade_Explosion"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGrenade_Explosion::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
