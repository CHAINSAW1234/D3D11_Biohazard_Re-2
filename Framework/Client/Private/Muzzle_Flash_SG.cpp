#include "stdafx.h"
#include "..\Public\Muzzle_Flash_SG.h"

#include "GameInstance.h"
#include "Muzzle_Light_SG.h"
#include "Muzzle_Spark_SG.h"

#define MUZZLE_SPARK_COUNT 6

CMuzzle_Flash_SG::CMuzzle_Flash_SG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect{ pDevice, pContext }
{
}

CMuzzle_Flash_SG::CMuzzle_Flash_SG(const CMuzzle_Flash_SG& rhs)
	: CEffect{ rhs }
{
}

HRESULT CMuzzle_Flash_SG::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMuzzle_Flash_SG::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_ImgSize = m_pTextureCom->GetImgSize();
	m_DivideCount = m_pTextureCom->GetDivideCount();

	m_bRender = false;

	m_FrameDelay = 20;

	m_vFirstFrame_Size = _float2(0.35f, 0.35f);
	m_vSecondFrame_Size = _float2(0.7f, 0.7f);
	m_vThirdFrame_Size = _float2(0.4f, 0.4f);

	m_pMuzzle_Light = CMuzzle_Light_SG::Create(m_pDevice, m_pContext);
	m_pMuzzle_Light->SetSize(3.f, 3.f);

	m_vecMuzzle_Spark_SG.clear();

	for (size_t i = 0; i < MUZZLE_SPARK_COUNT; ++i)
	{
		auto pSpark = CMuzzle_Spark_SG::Create(m_pDevice, m_pContext);
		pSpark->SetType((_uint)i);
		pSpark->Initialize(nullptr);
		pSpark->SetSize(1.f, 1.f);
		m_vecMuzzle_Spark_SG.push_back(pSpark);
	}

	return S_OK;
}

void CMuzzle_Flash_SG::Tick(_float fTimeDelta)
{
	for(size_t i = 0;i<m_vecMuzzle_Spark_SG.size();++i)
	{
		m_vecMuzzle_Spark_SG[i]->SetPosition(m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION));
		m_vecMuzzle_Spark_SG[i]->Tick(fTimeDelta);
	}

	if (m_bRender == false)
	{
		return;
	}

	if (m_pMuzzle_Light)
		m_pMuzzle_Light->Tick(fTimeDelta);

	if (m_FrameDelay + m_FrameTime < GetTickCount64())
	{
		m_FrameTime = GetTickCount64();
		++m_iMainFrame;
	}

	switch (m_iMainFrame)
	{
	case 0:
		m_pTransformCom->Set_Scaled(m_vFirstFrame_Size.x, m_vFirstFrame_Size.y, 1.f);
		m_iFrame = 0;
		break;
	case 1:
		m_pTransformCom->Set_Scaled(m_vSecondFrame_Size.x, m_vSecondFrame_Size.y, 1.f);
		m_iFrame = 1;
		m_pMuzzle_Light->SetPosition(m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION));
		break;
	case 2:
		m_pTransformCom->Set_Scaled(m_vThirdFrame_Size.x, m_vThirdFrame_Size.y, 1.f);
		m_iFrame = 0;
		m_pMuzzle_Light->Set_Render(true);
		m_pMuzzle_Light->Setup_Billboard();
		break;
	}

	if (m_iMainFrame >= 3)
	{
		m_iSparkIndex = m_pGameInstance->GetRandom_Int(0, 5);

		m_pMuzzle_Light->Set_Render(false);
		m_vecMuzzle_Spark_SG[m_iSparkIndex]->Set_Render(true);
		m_bRender = false;
		m_iFrame = 0;
		m_iMainFrame = 0;
	}

	Compute_CurrentUV();

	Setup_Billboard();
}

void CMuzzle_Flash_SG::Late_Tick(_float fTimeDelta)
{
	if (m_vecMuzzle_Spark_SG[m_iSparkIndex])
		m_vecMuzzle_Spark_SG[m_iSparkIndex]->Late_Tick(fTimeDelta);

	if (m_bRender == true)
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_EFFECT_BLOOM, this);

	if (m_pMuzzle_Light)
		m_pMuzzle_Light->Late_Tick(fTimeDelta);
}

HRESULT CMuzzle_Flash_SG::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(2);

	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	return S_OK;
}

void CMuzzle_Flash_SG::Compute_CurrentUV()
{
	IMG_SIZE      ImgSize = m_pTextureCom->GetImgSize();

	_uint   iSizeX = { (_uint)((_float)ImgSize.iSizeX / (_float)m_DivideCount.first) };
	_uint   iSizeY = { (_uint)((_float)ImgSize.iSizeY / (_float)m_DivideCount.second) };

	_uint   iCurrentFrame = m_iFrame;

	_uint   iCurrentX = iCurrentFrame % m_DivideCount.first;
	_uint   iCurrentY = iCurrentFrame / m_DivideCount.first;

	m_fMinUV_X = _float(iCurrentX * iSizeX) / ImgSize.iSizeX;
	m_fMaxUV_X = _float((iCurrentX + 1) * iSizeX) / ImgSize.iSizeX;
	m_fMinUV_Y = _float(iCurrentY * iSizeY) / ImgSize.iSizeY;
	m_fMaxUV_Y = _float((iCurrentY + 1) * iSizeY) / ImgSize.iSizeY;
}

void CMuzzle_Flash_SG::Setup_Billboard()
{
	m_pTransformCom->Look_At(m_pGameInstance->Get_Camera_Pos_Float4());
}

void CMuzzle_Flash_SG::SetSize(_float fSizeX, _float fSizeY)
{
	m_fSizeX = fSizeX;
	m_fSizeY = fSizeY;

	m_pTransformCom->Set_Scaled(fSizeX, fSizeY, 1.f);
}

void CMuzzle_Flash_SG::SetPosition(_float4 Pos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, Pos);
}

HRESULT CMuzzle_Flash_SG::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Muzzle_Flash_SG"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMuzzle_Flash_SG::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fMinUV_X", &m_fMinUV_X, sizeof(m_fMinUV_X))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fMinUV_Y", &m_fMinUV_Y, sizeof(m_fMinUV_Y))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fMaxUV_X", &m_fMaxUV_X, sizeof(m_fMaxUV_X))))
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

CMuzzle_Flash_SG* CMuzzle_Flash_SG::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMuzzle_Flash_SG* pInstance = new CMuzzle_Flash_SG(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMuzzle_Flash_SG"));

		Safe_Release(pInstance);
	}

	pInstance->Initialize(nullptr);

	return pInstance;

}

CGameObject* CMuzzle_Flash_SG::Clone(void* pArg)
{
	CMuzzle_Flash_SG* pInstance = new CMuzzle_Flash_SG(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CMuzzle_Flash_SG"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMuzzle_Flash_SG::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pMuzzle_Light);

	for(size_t i = 0;i<m_vecMuzzle_Spark_SG.size();++i)
	{
		Safe_Release(m_vecMuzzle_Spark_SG[i]);
	}
}
