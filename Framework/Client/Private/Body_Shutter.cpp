#include "stdafx.h"
#include "Body_Shutter.h"
#include"Player.h"

#include"Shutter.h"
CBody_Shutter::CBody_Shutter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPart_InteractProps{ pDevice, pContext }
{
}

CBody_Shutter::CBody_Shutter(const CBody_Shutter& rhs)
	: CPart_InteractProps{ rhs }
{

}

HRESULT CBody_Shutter::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBody_Shutter::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	if (FAILED(Add_Components()))
		return E_FAIL;

	BODY_SHUTTER_DESC* pDesc = (BODY_SHUTTER_DESC*)pArg;
	m_eType = pDesc->eShutter_Type;
	m_eNormalState = pDesc->eNormalState;
	m_e033State = pDesc->e033State;
	m_e034State = pDesc->e034State;

	m_pModelCom->Set_RootBone("RootNode");
	m_pModelCom->Add_Bone_Layer_All_Bone(TEXT("Default"));

	m_pModelCom->Add_AnimPlayingInfo(false, 0, TEXT("Default"), 1.f);


	m_pModelCom->Active_RootMotion_Rotation(true);
	//m_pTransformCom->Set_WorldMatrix(m_tagPropDesc.worldMatrix);

#ifndef NON_COLLISION_PROP

	m_pGameInstance->Create_Px_Collider(m_pModelCom, m_pTransformCom, &m_iPx_Collider_Id);

#endif
	return S_OK;
}

void CBody_Shutter::Tick(_float fTimeDelta)
{
}

void CBody_Shutter::Late_Tick(_float fTimeDelta)
{

	switch (*m_eType)
	{
	case CShutter::SHUTTER_NORMAL:
		Shutter_Normal_Late_Tick(fTimeDelta);
		break;
	case CShutter::SHUTTER_033:
		Shutter_033_Late_Tick(fTimeDelta);
		break;
	case CShutter::SHUTTER_034:
		Shutter_034_Late_Tick(fTimeDelta);
		break;
	}
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_FIELD_SHADOW_POINT, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_FIELD_SHADOW_DIR, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);

}

HRESULT CBody_Shutter::Render()
{
	if (m_bRender == false)
		return S_OK;
	else
		m_bRender = false;


	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(i), aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_NormalTexture", static_cast<_uint>(i), aiTextureType_NORMALS)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i))))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_AlphaTexture", static_cast<_uint>(i), aiTextureType_METALNESS)))
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

		if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_AOTexture", static_cast<_uint>(i), aiTextureType_SHININESS)))
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

		m_pModelCom->Render(static_cast<_uint>(i));
	}

	return S_OK;
}

HRESULT CBody_Shutter::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Body_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(g_Level, m_strModelComponentName,
		TEXT("Com_Body_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Shutter::Add_PartObjects()
{

	return S_OK;
}

HRESULT CBody_Shutter::Initialize_PartObjects()
{
	return S_OK;
}

void CBody_Shutter::Shutter_Normal_Late_Tick(_float fTimeDelta)
{

	switch (*m_eNormalState)
	{
	case CShutter::SHUTTER_OPEN:
		//m_pModelCom->Set_TotalLinearInterpolation(0.2f); // Àß¾Ë¾Æ°©´Ï´Ù ²¨¾ï
		m_pModelCom->Change_Animation(0, TEXT("Default"), *m_eNormalState);
		break;
	case CShutter::SHUTTER_OPEN_STATIC:
		m_pModelCom->Change_Animation(0, TEXT("Default"), *m_eNormalState);
		break;
	case CShutter::SHUTTER_STATIC:
		m_pModelCom->Change_Animation(0, TEXT("Default"), *m_eNormalState);
		break;
	}
	_float4 fTransform4 = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
	_float3 fTransform3 = _float3{ fTransform4.x,fTransform4.y,fTransform4.z };
	m_pModelCom->Play_Animation_Light(m_pTransformCom, fTimeDelta);




}

void CBody_Shutter::Shutter_033_Late_Tick(_float fTimeDelta)
{
	switch (*m_e033State)
	{
	case CShutter::SHTTER_033_CLOSED:
		//m_pModelCom->Set_TotalLinearInterpolation(0.2f); // Àß¾Ë¾Æ°©´Ï´Ù ²¨¾ï
		m_pModelCom->Change_Animation(0, TEXT("Default"), *m_e033State);
		break;
	case CShutter::SHTTER_033_FULL_CLOSED:
		m_pModelCom->Change_Animation(0, TEXT("Default"), *m_e033State);
		break;
	case CShutter::SHTTER_033_FULL_OPEN:
		m_pModelCom->Change_Animation(0, TEXT("Default"), *m_e033State);
		break;
	case CShutter::SHTTER_033_FULL_OPENED:
		m_pModelCom->Change_Animation(0, TEXT("Default"), *m_e033State);
		break;
	case CShutter::SHTTER_033_HALF_CLOSED:
		m_pModelCom->Change_Animation(0, TEXT("Default"), *m_e033State);
		break;
	case CShutter::SHTTER_033_HALF_OPEN:
		m_pModelCom->Change_Animation(0, TEXT("Default"), *m_e033State);
		break;
	case CShutter::SHTTER_033_HALF_OPENED:
		m_pModelCom->Change_Animation(0, TEXT("Default"), *m_e033State);
		break;
	}
	_float4 fTransform4 = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
	_float3 fTransform3 = _float3{ fTransform4.x,fTransform4.y,fTransform4.z };
	m_pModelCom->Play_Animation_Light(m_pTransformCom, fTimeDelta);
		
}

void CBody_Shutter::Shutter_034_Late_Tick(_float fTimeDelta)
{
	switch (*m_e034State)
	{
	case CShutter::SHTTER_034_CLOSED:
		//m_pModelCom->Set_TotalLinearInterpolation(0.2f); // Àß¾Ë¾Æ°©´Ï´Ù ²¨¾ï
		m_pModelCom->Change_Animation(0, TEXT("Default"), *m_e034State);
		break;
	case CShutter::SHTTER_034_END:
		m_pModelCom->Change_Animation(0, TEXT("Default"), *m_e034State);
		break;
	case CShutter::SHTTER_034_OPEN:
		m_pModelCom->Change_Animation(0, TEXT("Default"), *m_e034State);
		break;
	case CShutter::SHTTER_034_OPENED:
		m_pModelCom->Change_Animation(0, TEXT("Default"), *m_e034State);
		break;
	case CShutter::SHTTER_034_START:
		m_pModelCom->Change_Animation(0, TEXT("Default"), *m_e034State);
		break;
	}
	_float4 fTransform4 = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
	_float3 fTransform3 = _float3{ fTransform4.x,fTransform4.y,fTransform4.z };
	m_pModelCom->Play_Animation_Light(m_pTransformCom, fTimeDelta);


}


CBody_Shutter* CBody_Shutter::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Shutter* pInstance = new CBody_Shutter(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBody_Shutter"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CBody_Shutter::Clone(void* pArg)
{
	CBody_Shutter* pInstance = new CBody_Shutter(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CBody_Shutter"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Shutter::Free()
{
	__super::Free();

}
