#include "stdafx.h"
#include "..\Public\Effect.h"

#include "GameInstance.h"

CEffect::CEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CBlendObject{ pDevice, pContext }
{
}

CEffect::CEffect(const CEffect & rhs)
	: CBlendObject{ rhs }
{
}

HRESULT CEffect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(static_cast<_float>(rand() % 10), 3.f, static_cast<_float>(rand() % 10), 1.f));

	return S_OK;
}

void CEffect::Tick(_float fTimeDelta)
{
}

void CEffect::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
}

HRESULT CEffect::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(2);

	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	return S_OK;
}

void CEffect::Compute_CurrentUV()
{
	IMG_SIZE      ImgSize = m_pTextureCom->GetImgSize();

	_uint   iSizeX = { (_uint)((_float)ImgSize.iSizeX / (_float)m_iFrameX) };
	_uint   iSizeY = { (_uint)((_float)ImgSize.iSizeY / (_float)m_iFrameY) };

	_uint   iCurrentFrame = { (_uint)m_fFrame };

	_uint   iCurrentX = iCurrentFrame % m_iFrameX;
	_uint   iCurrentY = iCurrentFrame / m_iFrameX;

	m_fMinUV_X = _float(iCurrentX * iSizeX) / ImgSize.iSizeX;
	m_fMaxUV_X = _float((iCurrentX + 1) * iSizeX) / ImgSize.iSizeX;
	m_fMinUV_Y = _float(iCurrentY * iSizeY) / ImgSize.iSizeY;
	m_fMaxUV_Y = _float((iCurrentY + 1) * iSizeY) / ImgSize.iSizeY;
}

HRESULT CEffect::Add_Components()
{
	return S_OK;
}

HRESULT CEffect::Bind_ShaderResources()
{
	return S_OK;
}

CEffect * CEffect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEffect*		pInstance = new CEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CEffect"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject * CEffect::Clone(void * pArg)
{
	CEffect*		pInstance = new CEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CEffect"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
