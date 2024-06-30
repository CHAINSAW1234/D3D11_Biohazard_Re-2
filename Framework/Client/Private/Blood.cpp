#include "stdafx.h"
#include "..\Public\Blood.h"

#include "GameInstance.h"

CBlood::CBlood(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect{ pDevice, pContext }
{
}

CBlood::CBlood(const CBlood& rhs)
	: CEffect{ rhs }
{
}

HRESULT CBlood::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBlood::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_bRender = false;

	m_FrameDelay = 10;

	return S_OK;
}

void CBlood::Tick(_float fTimeDelta)
{
	if (m_bRender == false)
		return;

	if(m_FrameDelay + m_FrameTime < GetTickCount64())
	{
		++m_iFrame;

		m_FrameTime = GetTickCount64();

		if(m_iType != 0)
		{
			m_fSizeX += 0.07f;
			m_fSizeY += 0.07f;
			m_fSizeZ += 0.07f;

			m_pTransformCom->Set_Scaled(m_fSizeX, m_fSizeY, m_fSizeZ);
		}
	}

	_uint iNumMesh = m_pModelCom->GetNumMesh();

	if (m_iFrame >= iNumMesh)
	{
		m_bRender = false;
		m_iFrame = 0;
		m_fSizeX = m_fSize_X_Default;
		m_fSizeY = m_fSize_Y_Default;
		m_fSizeZ = m_fSize_Z_Default;
		m_pTransformCom->Set_Scaled(m_fSize_X_Default, m_fSize_Y_Default, m_fSize_Z_Default);
	}
}

void CBlood::Late_Tick(_float fTimeDelta)
{
	if (m_bRender == true)
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CBlood::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	switch (m_iType)
	{
	case 0:
	{
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

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		m_pModelCom->Render(static_cast<_uint>(m_iFrame));
		break;
	}
	case 1:
	{
		if (FAILED(m_pModelCom_2->Bind_ShaderResource_Texture(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(m_iFrame), aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom_2->Bind_ShaderResource_Texture(m_pShaderCom, "g_AlphaTexture", static_cast<_uint>(m_iFrame), aiTextureType_METALNESS)))
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

		if (FAILED(m_pModelCom_2->Bind_ShaderResource_Texture(m_pShaderCom, "g_AOTexture", static_cast<_uint>(m_iFrame), aiTextureType_SHININESS)))
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

		m_pModelCom_2->Render(static_cast<_uint>(m_iFrame));
		break;
	}
	case 2:
	{
		if (FAILED(m_pModelCom_3->Bind_ShaderResource_Texture(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(m_iFrame), aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom_3->Bind_ShaderResource_Texture(m_pShaderCom, "g_AlphaTexture", static_cast<_uint>(m_iFrame), aiTextureType_METALNESS)))
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

		if (FAILED(m_pModelCom_3->Bind_ShaderResource_Texture(m_pShaderCom, "g_AOTexture", static_cast<_uint>(m_iFrame), aiTextureType_SHININESS)))
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

		m_pModelCom_3->Render(static_cast<_uint>(m_iFrame));
		break;
		break;
	}
	case 3:
	{
		if (FAILED(m_pModelCom_4->Bind_ShaderResource_Texture(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(m_iFrame), aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom_4->Bind_ShaderResource_Texture(m_pShaderCom, "g_AlphaTexture", static_cast<_uint>(m_iFrame), aiTextureType_METALNESS)))
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

		if (FAILED(m_pModelCom_4->Bind_ShaderResource_Texture(m_pShaderCom, "g_AOTexture", static_cast<_uint>(m_iFrame), aiTextureType_SHININESS)))
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

		m_pModelCom_4->Render(static_cast<_uint>(m_iFrame));
		break;
		break;
	}
	case 4:
	{
		if (FAILED(m_pModelCom_5->Bind_ShaderResource_Texture(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(m_iFrame), aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom_5->Bind_ShaderResource_Texture(m_pShaderCom, "g_AlphaTexture", static_cast<_uint>(m_iFrame), aiTextureType_METALNESS)))
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

		if (FAILED(m_pModelCom_5->Bind_ShaderResource_Texture(m_pShaderCom, "g_AOTexture", static_cast<_uint>(m_iFrame), aiTextureType_SHININESS)))
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

		m_pModelCom_5->Render(static_cast<_uint>(m_iFrame));
		break;
		break;
	}
	case 5:
	{
		if (FAILED(m_pModelCom_6->Bind_ShaderResource_Texture(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(m_iFrame), aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom_6->Bind_ShaderResource_Texture(m_pShaderCom, "g_AlphaTexture", static_cast<_uint>(m_iFrame), aiTextureType_METALNESS)))
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

		if (FAILED(m_pModelCom_6->Bind_ShaderResource_Texture(m_pShaderCom, "g_AOTexture", static_cast<_uint>(m_iFrame), aiTextureType_SHININESS)))
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

		m_pModelCom_6->Render(static_cast<_uint>(m_iFrame));
		break;
		break;
	}
	case 6:
	{
		if (FAILED(m_pModelCom_7->Bind_ShaderResource_Texture(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(m_iFrame), aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom_7->Bind_ShaderResource_Texture(m_pShaderCom, "g_AlphaTexture", static_cast<_uint>(m_iFrame), aiTextureType_METALNESS)))
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

		if (FAILED(m_pModelCom_7->Bind_ShaderResource_Texture(m_pShaderCom, "g_AOTexture", static_cast<_uint>(m_iFrame), aiTextureType_SHININESS)))
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

		m_pModelCom_7->Render(static_cast<_uint>(m_iFrame));
		break;
		break;
	}
	case 7:
	{
		if (FAILED(m_pModelCom_8->Bind_ShaderResource_Texture(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(m_iFrame), aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom_8->Bind_ShaderResource_Texture(m_pShaderCom, "g_AlphaTexture", static_cast<_uint>(m_iFrame), aiTextureType_METALNESS)))
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

		if (FAILED(m_pModelCom_8->Bind_ShaderResource_Texture(m_pShaderCom, "g_AOTexture", static_cast<_uint>(m_iFrame), aiTextureType_SHININESS)))
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

		m_pModelCom_8->Render(static_cast<_uint>(m_iFrame));
		break;
		break;
	}
	case 8:
	{
		if (FAILED(m_pModelCom_9->Bind_ShaderResource_Texture(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(m_iFrame), aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom_9->Bind_ShaderResource_Texture(m_pShaderCom, "g_AlphaTexture", static_cast<_uint>(m_iFrame), aiTextureType_METALNESS)))
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

		if (FAILED(m_pModelCom_9->Bind_ShaderResource_Texture(m_pShaderCom, "g_AOTexture", static_cast<_uint>(m_iFrame), aiTextureType_SHININESS)))
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

		m_pModelCom_9->Render(static_cast<_uint>(m_iFrame));
		break;
		break;
	}
	case 9:
	{
		if (FAILED(m_pModelCom_10->Bind_ShaderResource_Texture(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(m_iFrame), aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom_10->Bind_ShaderResource_Texture(m_pShaderCom, "g_AlphaTexture", static_cast<_uint>(m_iFrame), aiTextureType_METALNESS)))
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

		if (FAILED(m_pModelCom_10->Bind_ShaderResource_Texture(m_pShaderCom, "g_AOTexture", static_cast<_uint>(m_iFrame), aiTextureType_SHININESS)))
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

		m_pModelCom_10->Render(static_cast<_uint>(m_iFrame));
		break;
		break;
	}
	case 10:
	{
		if (FAILED(m_pModelCom_11->Bind_ShaderResource_Texture(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(m_iFrame), aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom_11->Bind_ShaderResource_Texture(m_pShaderCom, "g_AlphaTexture", static_cast<_uint>(m_iFrame), aiTextureType_METALNESS)))
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

		if (FAILED(m_pModelCom_11->Bind_ShaderResource_Texture(m_pShaderCom, "g_AOTexture", static_cast<_uint>(m_iFrame), aiTextureType_SHININESS)))
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

		m_pModelCom_11->Render(static_cast<_uint>(m_iFrame));
		break;
		break;
	}
	}

	return S_OK;
}

void CBlood::SetSize(_float fSizeX, _float fSizeY, _float fSizeZ)
{
	m_fSizeX = fSizeX;
	m_fSizeY = fSizeY;
	m_fSizeZ = fSizeZ;

	m_fSize_X_Default = fSizeX;
	m_fSize_Y_Default = fSizeY;
	m_fSize_Z_Default = fSizeZ;

	m_pTransformCom->Set_Scaled(fSizeX, fSizeY, fSizeZ);
}

void CBlood::SetWorldMatrix_With_HitNormal(_vector vUp)
{
	m_pTransformCom->SetWorldMatrix_With_UpVector(vUp);
}

_float4x4 CBlood::GetWorldMatrix()
{
	return m_pTransformCom->Get_WorldMatrix_Pure();
}

HRESULT CBlood::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Blood_01"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Blood_02"),
		TEXT("Com_Model_2"), (CComponent**)&m_pModelCom_2)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Blood_03"),
		TEXT("Com_Model_3"), (CComponent**)&m_pModelCom_3)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Blood_04"),
		TEXT("Com_Model_4"), (CComponent**)&m_pModelCom_4)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Blood_05"),
		TEXT("Com_Model_5"), (CComponent**)&m_pModelCom_5)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Blood_06"),
		TEXT("Com_Model_6"), (CComponent**)&m_pModelCom_6)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Blood_07"),
		TEXT("Com_Model_7"), (CComponent**)&m_pModelCom_7)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Blood_08"),
		TEXT("Com_Model_8"), (CComponent**)&m_pModelCom_8)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Blood_09"),
		TEXT("Com_Model_9"), (CComponent**)&m_pModelCom_9)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Blood_10"),
		TEXT("Com_Model_10"), (CComponent**)&m_pModelCom_10)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Blood_11"),
		TEXT("Com_Model_11"), (CComponent**)&m_pModelCom_11)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBlood::Bind_ShaderResources()
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

CBlood* CBlood::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBlood* pInstance = new CBlood(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBlood"));

		Safe_Release(pInstance);
	}

	pInstance->Initialize(nullptr);

	return pInstance;

}

CGameObject* CBlood::Clone(void* pArg)
{
	CBlood* pInstance = new CBlood(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CBlood"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBlood::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pModelCom_2);
	Safe_Release(m_pModelCom_3);
	Safe_Release(m_pModelCom_4);
	Safe_Release(m_pModelCom_5);
	Safe_Release(m_pModelCom_6);
	Safe_Release(m_pModelCom_7);
	Safe_Release(m_pModelCom_8);
	Safe_Release(m_pModelCom_9);
	Safe_Release(m_pModelCom_10);
	Safe_Release(m_pModelCom_11);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
