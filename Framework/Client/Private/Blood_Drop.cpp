#include "stdafx.h"
#include "..\Public\Blood_Drop.h"

#include "GameInstance.h"
#include "Decal_SSD.h"

#define DIR_AMOUNT 0.3f
#define DECAL_PROB 30.f
#define DECAL_COUNT 5

CBlood_Drop::CBlood_Drop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect{ pDevice, pContext }
{
}

CBlood_Drop::CBlood_Drop(const CBlood_Drop& rhs)
	: CEffect{ rhs }
{
}

HRESULT CBlood_Drop::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBlood_Drop::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_bRender = false;

	m_FrameDelay = 40;

	m_fDropSpeed = m_pGameInstance->GetRandom_Real(4.f, 5.f);

	m_fDissolveSpeed = 0.05f;

	for (size_t i = 0; i < DECAL_COUNT; ++i)
	{
		auto pDecal = new CDecal_SSD(m_pDevice, m_pContext);
		pDecal->SetUsingSound(true);
		pDecal->Initialize(nullptr);
		m_vecDecal.push_back(pDecal);
	}

	//	m_pGameInstance->Add_Object_Sound(m_pTransformCom, 1);

	return S_OK;
}

void CBlood_Drop::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->IsPaused())
	{
		return;
	}

	if (m_iFrame == 1)
	{
		PlaySound();
	}

	if (m_bDecal)
	{
		Tick_SubEffect(fTimeDelta);
	}

	if (m_bRender == false)
		return;

	auto vPos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

	if(m_vDropDir.y > -0.7f )
		m_vDropDir.y -= fTimeDelta*m_fDropSpeed;

	vPos += m_vDropDir*fTimeDelta;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	if (m_bDissolving)
	{
		m_fDissolveAmount += m_fDissolveSpeed;

		if (m_fDissolveAmount >= 1.f)
		{
			m_iFrame = 0;
			m_fSizeX = m_fSize_X_Default;
			m_fSizeY = m_fSize_Y_Default;
			m_fSizeZ = m_fSize_Z_Default;
			m_pTransformCom->Set_Scaled(m_fSize_X_Default, m_fSize_Y_Default, m_fSize_Z_Default);
			m_fDissolveAmount = 0.f;
			m_bRender = false;

			RayCast_Decal();
		}

		return;
	}

	if(m_FrameDelay + m_FrameTime < GetTickCount64())
	{
		++m_iFrame;

		m_FrameTime = GetTickCount64();
	}

	_uint iNumMesh = m_pModelCom->GetNumMesh();

	if (m_iFrame >= iNumMesh)
	{
		m_bDissolving = true;
		/*m_bRender = false;
		m_iFrame = 0;
		m_fSizeX = m_fSize_X_Default;
		m_fSizeY = m_fSize_Y_Default;
		m_fSizeZ = m_fSize_Z_Default;
		m_pTransformCom->Set_Scaled(m_fSize_X_Default, m_fSize_Y_Default, m_fSize_Z_Default);*/
	}
}

void CBlood_Drop::Start()
{
#pragma region 메모리 정리
	m_pModelCom->Release_Dump();
	m_pModelCom->Release_Decal_Dump();

	m_pModelCom_2->Release_Dump();
	m_pModelCom_2->Release_Decal_Dump();

	m_pModelCom_3->Release_Dump();
	m_pModelCom_3->Release_Decal_Dump();

	m_pModelCom_4->Release_Dump();
	m_pModelCom_4->Release_Decal_Dump();

	m_pModelCom_5->Release_Dump();
	m_pModelCom_5->Release_Decal_Dump();

	m_pModelCom_6->Release_Dump();
	m_pModelCom_6->Release_Decal_Dump();

	m_pModelCom_7->Release_Dump();
	m_pModelCom_7->Release_Decal_Dump();

	m_pModelCom_8->Release_Dump();
	m_pModelCom_8->Release_Decal_Dump();

	m_pModelCom_9->Release_Dump();
	m_pModelCom_9->Release_Decal_Dump();

	m_pModelCom_10->Release_Dump();
	m_pModelCom_10->Release_Decal_Dump();

	m_pModelCom_11->Release_Dump();
	m_pModelCom_11->Release_Decal_Dump();

#pragma endregion
}

void CBlood_Drop::Late_Tick(_float fTimeDelta)
{
	if (m_bDecal)
	{
		Late_Tick_SubEffect(fTimeDelta);
	}

	if (m_bRender == true)
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CBlood_Drop::Render()
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

		if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXMODEL::PASS_BLOOD)))
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

		if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXMODEL::PASS_BLOOD)))
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

		if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXMODEL::PASS_BLOOD)))
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

		if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXMODEL::PASS_BLOOD)))
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

		if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXMODEL::PASS_BLOOD)))
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

		if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXMODEL::PASS_BLOOD)))
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

		if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXMODEL::PASS_BLOOD)))
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

		if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXMODEL::PASS_BLOOD)))
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

		if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXMODEL::PASS_BLOOD)))
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

		if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXMODEL::PASS_BLOOD)))
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

		if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXMODEL::PASS_BLOOD)))
			return E_FAIL;

		m_pModelCom_11->Render(static_cast<_uint>(m_iFrame));
		break;
		break;
	}
	}

	return S_OK;
}

void CBlood_Drop::PlaySound()
{
	const wchar_t* str = L"Blood_";
	wchar_t result[32];
	_int inum = m_pGameInstance->GetRandom_Int(0, 10);

	std::swprintf(result, sizeof(result) / sizeof(wchar_t), L"%ls%d.mp3", str, inum);

	m_pGameInstance->Change_Sound_3D(m_pTransformCom, result, 0);
	m_pGameInstance->Set_Volume_3D(m_pTransformCom, 0, 0.2f);
}

void CBlood_Drop::SetSize(_float fSizeX, _float fSizeY, _float fSizeZ)
{
	m_bDecalSound = false;

	switch (m_iType)
	{
	case 0:
		fSizeX -= 1.5f;
		fSizeY -= 1.5f;
		fSizeZ -= 1.5f;
		m_fSizeX = fSizeX;
		m_fSizeY = fSizeY;
		m_fSizeZ = fSizeZ;
		m_fSize_X_Default = fSizeX;
		m_fSize_Y_Default = fSizeY;
		m_fSize_Z_Default = fSizeZ;
		break;
	case 1:
		fSizeX -= 1.f;
		fSizeY -= 1.f;
		fSizeZ -= 1.f;
		m_fSizeX = fSizeX;
		m_fSizeY = fSizeY;
		m_fSizeZ = fSizeZ;
		m_fSize_X_Default = fSizeX;
		m_fSize_Y_Default = fSizeY;
		m_fSize_Z_Default = fSizeZ;
		break;
	case 2:
		m_fSizeX = fSizeX;
		m_fSizeY = fSizeY;
		m_fSizeZ = fSizeZ;
		m_fSize_X_Default = fSizeX;
		m_fSize_Y_Default = fSizeY;
		m_fSize_Z_Default = fSizeZ;
		break;
	case 3:
		m_fSizeX = fSizeX;
		m_fSizeY = fSizeY;
		m_fSizeZ = fSizeZ;
		m_fSize_X_Default = fSizeX;
		m_fSize_Y_Default = fSizeY;
		m_fSize_Z_Default = fSizeZ;
		break;
	case 4:
		m_fSizeX = fSizeX;
		m_fSizeY = fSizeY;
		m_fSizeZ = fSizeZ;
		m_fSize_X_Default = fSizeX;
		m_fSize_Y_Default = fSizeY;
		m_fSize_Z_Default = fSizeZ;
		break;
	case 5:
		m_fSizeX = fSizeX;
		m_fSizeY = fSizeY;
		m_fSizeZ = fSizeZ;
		m_fSize_X_Default = fSizeX;
		m_fSize_Y_Default = fSizeY;
		m_fSize_Z_Default = fSizeZ;
		break;
	case 6:
		m_fSizeX = fSizeX;
		m_fSizeY = fSizeY;
		m_fSizeZ = fSizeZ;
		m_fSize_X_Default = fSizeX;
		m_fSize_Y_Default = fSizeY;
		m_fSize_Z_Default = fSizeZ;
		break;
	case 7:
		m_fSizeX = fSizeX;
		m_fSizeY = fSizeY;
		m_fSizeZ = fSizeZ;
		m_fSize_X_Default = fSizeX;
		m_fSize_Y_Default = fSizeY;
		m_fSize_Z_Default = fSizeZ;
		break;
	case 8:
		m_fSizeX = fSizeX;
		m_fSizeY = fSizeY;
		m_fSizeZ = fSizeZ;
		m_fSize_X_Default = fSizeX;
		m_fSize_Y_Default = fSizeY;
		m_fSize_Z_Default = fSizeZ;
		break;
	case 9:
		fSizeX -= 2.f;
		fSizeY -= 2.f;
		fSizeZ -= 2.f;
		m_fSizeX = fSizeX;
		m_fSizeY = fSizeY;
		m_fSizeZ = fSizeZ;
		m_fSize_X_Default = fSizeX;
		m_fSize_Y_Default = fSizeY;
		m_fSize_Z_Default = fSizeZ;
		break;
	case 10:
		fSizeX -= 2.f;
		fSizeY -= 2.f;
		fSizeZ -= 2.f;
		m_fSizeX = fSizeX;
		m_fSizeY = fSizeY;
		m_fSizeZ = fSizeZ;
		m_fSize_X_Default = fSizeX;
		m_fSize_Y_Default = fSizeY;
		m_fSize_Z_Default = fSizeZ;
		break;
	}

	m_pTransformCom->Set_Scaled(fSizeX, fSizeY, fSizeZ);
}

void CBlood_Drop::SetWorldMatrix_With_HitNormal(_vector vUp)
{
	m_pTransformCom->SetWorldMatrix_With_UpVector(vUp);
}

_float4x4 CBlood_Drop::GetWorldMatrix()
{
	return m_pTransformCom->Get_WorldMatrix_Pure();
}

_float4 CBlood_Drop::GetPosition()
{
	return m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
}

void CBlood_Drop::SetPosition(_float4 Pos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, Pos);
	m_iFrame = 0;
	m_fAlpha_Delta_Sum = 0.f;

	m_vDropDir = _float4(m_pGameInstance->GetRandom_Real(-DIR_AMOUNT, DIR_AMOUNT),
		m_pGameInstance->GetRandom_Real(0.1f, 1.f),
		m_pGameInstance->GetRandom_Real(-DIR_AMOUNT, DIR_AMOUNT), 0.f);

	//m_vDropDir = Float4_Normalize(m_vDropDir);

	m_bDissolving = false;
	m_fDissolveAmount = 0.f;
}

void CBlood_Drop::RayCast_Decal()
{
	if(m_pGameInstance->GetRandom_Real(0.f,100.f) <= DECAL_PROB)
	{
		_float4 vBlockPoint;
		_float4 vBlockNormal;

		if (m_pGameInstance->RayCast_Decal(m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION), _float4(0.f, -1.f, 0.f, 0.f), &vBlockPoint, &vBlockNormal))
		{
			m_vecDecal[m_iDecalCount]->Set_Render(true);
			m_vecDecal[m_iDecalCount]->SetPosition(vBlockPoint);
			m_vecDecal[m_iDecalCount]->LookAt(vBlockNormal);
			m_bDecal = true;
			++m_iDecalCount;

			if (m_iDecalCount >= DECAL_COUNT)
			{
				m_iDecalCount = 0;
			}

			m_bDecalSound = true;
		}
	}
}

HRESULT CBlood_Drop::Add_Components()
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

	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Blood_04"),
		TEXT("Com_Model_3"), (CComponent**)&m_pModelCom_3)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Blood_05"),
		TEXT("Com_Model_4"), (CComponent**)&m_pModelCom_4)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Blood_06"),
		TEXT("Com_Model_5"), (CComponent**)&m_pModelCom_5)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Blood_07"),
		TEXT("Com_Model_6"), (CComponent**)&m_pModelCom_6)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Blood_09"),
		TEXT("Com_Model_7"), (CComponent**)&m_pModelCom_7)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Blood_10"),
		TEXT("Com_Model_8"), (CComponent**)&m_pModelCom_8)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Blood_11"),
		TEXT("Com_Model_9"), (CComponent**)&m_pModelCom_9)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Blood_08"),
		TEXT("Com_Model_10"), (CComponent**)&m_pModelCom_10)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Blood_03"),
		TEXT("Com_Model_11"), (CComponent**)&m_pModelCom_11)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBlood_Drop::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bDissolve", &m_bDissolving, sizeof(_bool))))
		return E_FAIL;

	if (m_bDissolving)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveAmount", &m_fDissolveAmount, sizeof(_float))))
			return E_FAIL;
	}

	return S_OK;
}

void CBlood_Drop::Tick_SubEffect(_float fTimeDelta)
{
	for (size_t i = 0; i < m_vecDecal.size(); ++i)
	{
		m_vecDecal[i]->Tick(fTimeDelta);
	}
}

void CBlood_Drop::Late_Tick_SubEffect(_float fTimeDelta)
{
	for (size_t i = 0; i < m_vecDecal.size(); ++i)
	{
		m_vecDecal[i]->Late_Tick(fTimeDelta);
	}
}

CBlood_Drop* CBlood_Drop::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBlood_Drop* pInstance = new CBlood_Drop(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBlood_Drop"));

		Safe_Release(pInstance);
	}

	pInstance->Initialize(nullptr);

	return pInstance;

}

CGameObject* CBlood_Drop::Clone(void* pArg)
{
	CBlood_Drop* pInstance = new CBlood_Drop(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CBlood_Drop"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBlood_Drop::Free()
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

	for(size_t i = 0;i<m_vecDecal.size();++i)
	{
		Safe_Release(m_vecDecal[i]);
	}
}
