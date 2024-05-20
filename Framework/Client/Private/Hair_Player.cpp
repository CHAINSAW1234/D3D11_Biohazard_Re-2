#include "stdafx.h"
#include "Hair_Player.h"

#include "Player.h"

CHair_Player::CHair_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CHair_Player::CHair_Player(const CHair_Player& rhs)
	: CPartObject{ rhs }
{

}

HRESULT CHair_Player::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHair_Player::Initialize(void* pArg)
{
	HAIR_DESC*		pDesc = { static_cast<HAIR_DESC*>(pArg) };

	pDesc->fSpeedPerSec = 10.f;
	pDesc->fRotationPerSec = XMConvertToRadians(90.0f);

	m_pState = pDesc->pState;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pModelCom->Set_Animation(0, true);

	return S_OK;
}

void CHair_Player::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CHair_Player::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_pColliderCom->Tick(XMLoadFloat4x4(&m_WorldMatrix));
}

void CHair_Player::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pModelCom->Play_Animation(fTimeDelta);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponents(m_pColliderCom);
#endif
}

HRESULT CHair_Player::Render()
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

		if (FAILED(m_pModelCom->Bind_ShaderResource_MaterialDesc(m_pShaderCom, "g_vMaterial", static_cast<_uint>(i))))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i))))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_ATOSTexture", static_cast<_uint>(i), aiTextureType_METALNESS))) {
			if (FAILED(m_pShaderCom->Begin(0)))
				return E_FAIL;
		}
		else {
			if (FAILED(m_pShaderCom->Begin(1)))
				return E_FAIL;
		}


		m_pModelCom->Render(static_cast<_uint>(i));
	}

	return S_OK;
}

HRESULT CHair_Player::Render_LightDepth()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	_float4x4		ViewMatrix, ProjMatrix;

	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(0.f, 10.f, -10.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(120.0f), (_float)g_iWinSizeX / g_iWinSizeY, 0.1f, 2000.f));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;


	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(i), aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i))))
			return E_FAIL;

		/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이ㅏ터를 다 던져야한다. */
		if (FAILED(m_pShaderCom->Begin(1)))
			return E_FAIL;

		m_pModelCom->Render(static_cast<_uint>(i));
	}

	return S_OK;

}

HRESULT CHair_Player::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_LeonHair"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

	/* 로컬상의 정보를 셋팅한다. */
	ColliderDesc.fRadius = 0.5f;
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.fRadius, 0.f);


	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CHair_Player::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	/*if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;	*/

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_float fFar = { 1000.f };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &fFar, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CHair_Player* CHair_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHair_Player* pInstance = new CHair_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CHair_Player"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CHair_Player::Clone(void* pArg)
{
	CHair_Player* pInstance = new CHair_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CHair_Player"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHair_Player::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
