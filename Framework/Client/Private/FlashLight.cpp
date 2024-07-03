#include "stdafx.h"
#include "FlashLight.h"
#include "Light.h"
#include "Bone.h"

CFlashLight::CFlashLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CFlashLight::CFlashLight(const CFlashLight& rhs)
	: CPartObject{ rhs }
{
}

HRESULT CFlashLight::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CFlashLight::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	LIGHT_DESC eDesc;
	eDesc.eType = LIGHT_DESC::TYPE_SPOT;
	eDesc.bShadow = true;
	eDesc.bRender = false;

	eDesc.fRange = 15.f;
	eDesc.fCutOff = XMConvertToRadians(30.f);
	eDesc.fOutCutOff = XMConvertToRadians(35.f);

	eDesc.vPosition = _float4(0.f, 0.f, 0.f, 1.f);
	eDesc.vDirection = _float4(0.f, 0.f, 1.f, 1.f);

	eDesc.vDiffuse = _float4(.1f, .1f, .1f, 1.f);
	eDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
	eDesc.vSpecular = _float4(0.4f, 0.4f, 0.4f, 1.f);

	m_pGameInstance->Add_Light(m_strLightTag, eDesc);
	
	m_bRender = false;

	m_pModelCom->Set_RootBone("root");
	m_pModelCom->Add_Bone_Layer_All_Bone(TEXT("Default"));
	m_pModelCom->Add_AnimPlayingInfo(false, 0, TEXT("Default"), 1.f);
	//m_pModelCom->Change_Animation(0, TEXT("Default"), 0);
	//m_pTransformCom->Set_Scaled(0.1f, 0.1f, 0.1f);
	//m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.0f));
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float4(0.8f, 0.f, 0.f, 1.f));

	return S_OK;
}

void CFlashLight::Tick(_float fTimeDelta)
{
	//__super::Tick(fTimeDelta);
}

void CFlashLight::Late_Tick(_float fTimeDelta)
{
	if (m_bRender) {
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_DIR, this);
		//m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_POINT, this);
		//m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);
	}

	_float3				vDirection = { };
	m_pModelCom->Play_Animations(m_pParentsTransform, fTimeDelta, &vDirection);

	_matrix			WorldMatrix = { m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pSocketMatrix) * m_pParentsTransform->Get_WorldMatrix() };
	XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
	
	const LIGHT_DESC* eDesc = m_pGameInstance->Get_LightDesc(m_strLightTag, 0);

	LIGHT_DESC eNewDesc = *eDesc;


	if (m_bRender) {

		const _float4x4 pMatrix = m_pModelCom->Get_BonePtr(0)->Get_CombinedTransformationMatrix_Var();

		_vector vScale;
		_vector vRoation;
		_vector vTranspose;
		XMMatrixDecompose(&vScale, &vRoation, &vTranspose, XMLoadFloat4x4(&m_WorldMatrix));

		_matrix vMatrix = XMLoadFloat4x4(&pMatrix) * XMLoadFloat4x4(&m_WorldMatrix);
		_float4x4 fMatrix;
		XMStoreFloat4x4(&fMatrix, vMatrix);
		_float4 vPos = _float4(fMatrix._41, fMatrix._42, fMatrix._43, 1.f);


		eNewDesc.vDirection = XMVectorSetW(XMVector3TransformNormal(XMVectorSet(0.f, 0.f, -1.f, 0.f), fMatrix), 0.f);
		eNewDesc.vPosition = vPos + eNewDesc.vDirection * 10;
		//eNewDesc.vDirection = XMVectorSetW(-m_WorldMatrix.Forward(), 0.f);


		eNewDesc.bRender = true;
		eNewDesc.bShadow = true;
		m_pGameInstance->Add_ShadowLight(CPipeLine::SPOT, m_strLightTag);
	}
	else {
		eNewDesc.bRender = false;
		eNewDesc.bShadow = false;
	}

	m_pGameInstance->Update_Light(m_strLightTag, eNewDesc, 0, 1.f);



#ifdef _DEBUG
	//m_pGameInstance->Add_DebugComponents(m_pColliderCom);
#endif
}

HRESULT CFlashLight::Render()
{
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

HRESULT CFlashLight::Render_LightDepth_Dir()
{
    return S_OK;
}

HRESULT CFlashLight::Render_LightDepth_Point()
{
    return S_OK;
}

HRESULT CFlashLight::Render_LightDepth_Spot()
{
	return S_OK;
}

HRESULT CFlashLight::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_FlashLight"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

    return S_OK;
}

HRESULT CFlashLight::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_DecalRender", &m_bDecalRender, sizeof(_bool))))
		return E_FAIL;

	return S_OK;
}

CFlashLight* CFlashLight::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFlashLight* pInstance = new CFlashLight(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CFlashLight"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFlashLight::Clone(void* pArg)
{
	CFlashLight* pInstance = new CFlashLight(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CFlashLight"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFlashLight::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
