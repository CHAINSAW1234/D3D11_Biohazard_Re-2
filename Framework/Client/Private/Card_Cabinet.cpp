#include "stdafx.h"
#include "Card_Cabinet.h"
#include"Player.h"
#include"Light.h"
#include"Cabinet.h"
CCard_Cabinet::CCard_Cabinet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPart_InteractProps{ pDevice, pContext }
{
}

CCard_Cabinet::CCard_Cabinet(const CCard_Cabinet& rhs)
	: CPart_InteractProps{ rhs }
{

}

HRESULT CCard_Cabinet::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCard_Cabinet::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	if (FAILED(Add_Components()))
		return E_FAIL;
	if (pArg != nullptr)
	{
		CARD_CABINET_DESC* pCard = (CARD_CABINET_DESC*)pArg;
		m_pLockState = pCard->pLock_Cabinet_State;
		m_pWorldMatrix = pCard->pLock_WorldMatrix;
		m_bAction = pCard->pAction;
	}
	//m_pModelCom->Set_RootBone("RootNode");
	//m_pModelCom->Add_Bone_Layer_All_Bone(TEXT("Default"));

	//m_pModelCom->Add_AnimPlayingInfo(false, 0, TEXT("Default"), 1.f);


	//m_pModelCom->Active_RootMotion_Rotation(true);

#ifndef NON_COLLISION_PROP

	//m_pGameInstance->Create_Px_Collider(m_pModelCom, m_pParentsTransform, &m_iPx_Collider_Id);

#endif

	return S_OK;
}

void CCard_Cabinet::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	m_pTransformCom->Set_Scaled(150.f, 150.f, 150.f);
}

void CCard_Cabinet::Late_Tick(_float fTimeDelta)
{
	//m_pModelCom->Change_Animation(0, TEXT("Default"), 0);



	//_float4 fTransform4 = m_pParentsTransform->Get_State_Float4(CTransform::STATE_POSITION);
	//_float3 fTransform3 = _float3{ fTransform4.x,fTransform4.y,fTransform4.z };
	//m_pModelCom->Play_Animation_Light(m_pParentsTransform, fTimeDelta);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_FIELD_SHADOW_POINT, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_FIELD_SHADOW_DIR, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);

	m_pTransformCom->Rotation(m_pTransformCom->Get_State_Vector(CTransform::STATE_RIGHT),XMConvertToRadians(90.f));
	_float4			vRotate = { m_pTransformCom->Get_State_Float4(CTransform::STATE_UP) };

	_vector				vNewQuaternion = { XMQuaternionRotationAxis(vRotate, XMConvertToRadians(90.f)) };

	vNewQuaternion = XMQuaternionNormalize(vNewQuaternion);
	_matrix				RotationMatrix = { XMMatrixRotationQuaternion(vNewQuaternion) };

	_matrix			WorldMatrix = { m_pTransformCom->Get_WorldMatrix() * RotationMatrix * XMLoadFloat4x4(m_pSocketMatrix) * XMLoadFloat4x4(m_pWorldMatrix) };

	m_WorldMatrix = WorldMatrix;
}

HRESULT CCard_Cabinet::Render()
{
	if (*m_pLockState != CCabinet::CLEAR_LOCK)
		return S_OK;
		
	if (m_bRender == false)
		return S_OK;
	else
		m_bRender = false;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	list<_uint>			NonHideIndices = { m_pModelCom->Get_NonHideMeshIndices() };

	for (auto& i : NonHideIndices)
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

		if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_EmissiveTexture", static_cast<_uint>(i), aiTextureType_EMISSIVE)))
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



		if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXMODEL::PASS_DEFAULT)))
			return E_FAIL;

		m_pModelCom->Render(static_cast<_uint>(i));
	}

	return S_OK;
}

HRESULT CCard_Cabinet::Render_LightDepth_Dir()
{
	if (m_bRender)
		return S_OK;
		//nonAnim

	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	list<LIGHT_DESC*> LightDescList = m_pGameInstance->Get_ShadowPointLightDesc_List();
	_int iIndex = 0;
	for (auto& pLightDesc : LightDescList) {
		const _float4x4* pLightViewMatrices;
		_float4x4 LightProjMatrix;
		pLightViewMatrices = pLightDesc->ViewMatrix;
		LightProjMatrix = pLightDesc->ProjMatrix;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_LightIndex", &iIndex, sizeof(_int))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_Matrices("g_LightViewMatrix", pLightViewMatrices, 6)))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_LightProjMatrix", &LightProjMatrix)))
			return E_FAIL;

		_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
		for (size_t i = 0; i < iNumMeshes; i++)
		{
			if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(i), aiTextureType_DIFFUSE)))
				return E_FAIL;

			/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이ㅏ터를 다 던져야한다. */
			if (FAILED(m_pShaderCom->Begin(1)))
				return E_FAIL;

			m_pModelCom->Render(static_cast<_uint>(i));
		}

		++iIndex;
	}



	return S_OK;
}

HRESULT CCard_Cabinet::Render_LightDepth_Point()
{
	if (m_bRender)
		return S_OK;

	//nonAnim
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	list<LIGHT_DESC*> LightDescList = m_pGameInstance->Get_ShadowPointLightDesc_List();
	_int iIndex = 0;
	for (auto& pLightDesc : LightDescList) {
		const _float4x4* pLightViewMatrices;
		_float4x4 LightProjMatrix;
		pLightViewMatrices = pLightDesc->ViewMatrix;
		LightProjMatrix = pLightDesc->ProjMatrix;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_LightIndex", &iIndex, sizeof(_int))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_Matrices("g_LightViewMatrix", pLightViewMatrices, 6)))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_LightProjMatrix", &LightProjMatrix)))
			return E_FAIL;

		_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
		for (size_t i = 0; i < iNumMeshes; i++)
		{
			if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(i), aiTextureType_DIFFUSE)))
				return E_FAIL;

			/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이ㅏ터를 다 던져야한다. */
			if (FAILED(m_pShaderCom->Begin(2)))
				return E_FAIL;

			m_pModelCom->Render(static_cast<_uint>(i));
		}

		++iIndex;
	}

	return S_OK;
}

HRESULT CCard_Cabinet::Render_LightDepth_Spot()
{
	if (m_bRender == false)
		return S_OK;

	if (m_bRender)
		return S_OK;

	//nonAnim
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	list<LIGHT_DESC*> LightDescList = m_pGameInstance->Get_ShadowPointLightDesc_List();
	_int iIndex = 0;
	for (auto& pLightDesc : LightDescList) {
		const _float4x4* pLightViewMatrices;
		_float4x4 LightProjMatrix;
		pLightViewMatrices = pLightDesc->ViewMatrix;
		LightProjMatrix = pLightDesc->ProjMatrix;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_LightIndex", &iIndex, sizeof(_int))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_Matrices("g_LightViewMatrix", pLightViewMatrices, 6)))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_LightProjMatrix", &LightProjMatrix)))
			return E_FAIL;

		_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
		for (size_t i = 0; i < iNumMeshes; i++)
		{
			if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(i), aiTextureType_DIFFUSE)))
				return E_FAIL;

			/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이ㅏ터를 다 던져야한다. */
			if (FAILED(m_pShaderCom->Begin(1)))
				return E_FAIL;

			m_pModelCom->Render(static_cast<_uint>(i));
		}

		++iIndex;
	}
	
	return S_OK;
}

HRESULT CCard_Cabinet::Add_Components()
{
	/* For.Com_Pannel_Shader */

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Pannel_Anim_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Pannel_Model */
	if (FAILED(__super::Add_Component(g_Level, m_strModelComponentName,
		TEXT("Com_Pannel_Model_Anim"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCard_Cabinet::Add_PartObjects()
{

	return S_OK;
}

HRESULT CCard_Cabinet::Initialize_PartObjects()
{
	return S_OK;
}


CCard_Cabinet* CCard_Cabinet::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCard_Cabinet* pInstance = new CCard_Cabinet(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCard_Cabinet"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CCard_Cabinet::Clone(void* pArg)
{
	CCard_Cabinet* pInstance = new CCard_Cabinet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CCard_Cabinet"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCard_Cabinet::Free()
{
	__super::Free();
}
