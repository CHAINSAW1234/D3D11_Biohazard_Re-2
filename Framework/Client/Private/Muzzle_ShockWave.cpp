#include "stdafx.h"
#include "..\Public\Muzzle_ShockWave.h"

#include "GameInstance.h"

CMuzzle_ShockWave::CMuzzle_ShockWave(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect{ pDevice, pContext }
{
}

CMuzzle_ShockWave::CMuzzle_ShockWave(const CMuzzle_ShockWave & rhs)
	: CEffect{ rhs }
{
}

HRESULT CMuzzle_ShockWave::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMuzzle_ShockWave::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scaled(0.5f, 0.5f, 1.f);

	m_ImgSize = m_pTextureCom->GetImgSize();
	m_DivideCount = m_pTextureCom->GetDivideCount();

	m_bRender = false;

	m_FrameDelay = 10;

	m_fAlpha_Delta = 1.f / (_float)(m_DivideCount.first * m_DivideCount.second);

	m_iFrame_Total = m_DivideCount.first * m_DivideCount.second;

	m_fAlpha_Delta_Sum = 1.f;

	m_bAlpha_Direction = false;

	return S_OK;
}

void CMuzzle_ShockWave::Tick(_float fTimeDelta)
{
	if (m_bRender == false)
		return;

	m_fSizeX += 0.05f;
	m_fSizeY += 0.05f;
	m_pTransformCom->Set_Scaled(m_fSizeX, m_fSizeY, 1.f);

	if (m_FrameDelay + m_FrameTime < GetTickCount64())
	{
		++m_iFrame;

		m_FrameTime = GetTickCount64();

		if (m_bAlpha_Direction == false)
			m_fAlpha_Delta_Sum -= m_fAlpha_Delta*6.f;
		else
			m_fAlpha_Delta_Sum += m_fAlpha_Delta;

		if (m_bAlpha_Direction == false)
		{
			if (m_fAlpha_Delta_Sum < 0.f)
			{
				m_bAlpha_Direction = true;
				m_fAlpha_Delta_Sum = 0.f;
			}
		}
		else
		{
			if (m_fAlpha_Delta_Sum >= 1.f)
			{
				m_fAlpha_Delta_Sum = 1.f;
			}
		}

		if (m_iFrame >= m_iFrame_Total)
		{
			if (m_fAlpha_Delta_Sum >= 1.f)
			{
				m_bAlpha_Direction = false;
				m_fAlpha_Delta_Sum = 1.f;
			}

			m_bRender = false;
			m_iFrame = 0;

			m_fSizeX = m_fDefaultSize_X;
			m_fSizeY = m_fDefaultSize_Y;
			m_pTransformCom->Set_Scaled(m_fSizeX, m_fSizeY, 1.f);
		}
	}

	Compute_CurrentUV();

	Setup_Billboard();
}

void CMuzzle_ShockWave::Late_Tick(_float fTimeDelta)
{
	if(m_bRender == true)
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
}

HRESULT CMuzzle_ShockWave::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(2);

	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	return S_OK;
}

void CMuzzle_ShockWave::Compute_CurrentUV()
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

void CMuzzle_ShockWave::Setup_Billboard()
{
	m_pTransformCom->Look_At(m_pGameInstance->Get_CamPosition_Vector());
}

void CMuzzle_ShockWave::SetSize(_float fSizeX, _float fSizeY)
{
	m_fSizeX = fSizeX;
	m_fSizeY = fSizeY;
	m_fDefaultSize_X = fSizeX;
	m_fDefaultSize_Y = fSizeY;

	m_pTransformCom->Set_Scaled(fSizeX, fSizeY,1.f);
}

void CMuzzle_ShockWave::SetPosition(_float4 Pos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, Pos);
	m_iFrame = 0;
	m_fAlpha_Delta_Sum = 1.f;
	m_bAlpha_Direction = false;

	m_fSizeX = m_fDefaultSize_X;
	m_fSizeY = m_fDefaultSize_Y;
	m_pTransformCom->Set_Scaled(m_fSizeX, m_fSizeY, 1.f);
}

HRESULT CMuzzle_ShockWave::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	switch (m_iType)
	{
	case 0:
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Muzzle_ShckWave_SG"),
			TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
		break;
	case 1:
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Muzzle_ShckWave_SG_01"),
			TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
		break;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMuzzle_ShockWave::Bind_ShaderResources()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha_Delta", &m_fAlpha_Delta_Sum, sizeof(m_fAlpha_Delta_Sum))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShaderCom, TEXT("Target_Depth"), "g_DepthTexture")))
		return E_FAIL;

	return S_OK;
}

CMuzzle_ShockWave * CMuzzle_ShockWave::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMuzzle_ShockWave*		pInstance = new CMuzzle_ShockWave(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMuzzle_ShockWave"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject * CMuzzle_ShockWave::Clone(void * pArg)
{
	CMuzzle_ShockWave*		pInstance = new CMuzzle_ShockWave(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CMuzzle_ShockWave"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMuzzle_ShockWave::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
