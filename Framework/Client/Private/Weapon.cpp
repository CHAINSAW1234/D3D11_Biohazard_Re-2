#include "stdafx.h"
#include "..\Public\Weapon.h"

#include "Bone.h"
#include "Light.h"

CWeapon::CWeapon(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CPartObject{ pDevice, pContext }
{
}

CWeapon::CWeapon(const CWeapon & rhs)
	: CPartObject{ rhs }
{

}

HRESULT CWeapon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeapon::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	WEAPON_DESC* pDesc = (WEAPON_DESC*)pArg;
	m_eEquip = pDesc->eEquip;
	m_eSetPropsLocation = pDesc->eSetprops_Location;

	for (size_t i = 0; i < NONE; i++)
	{
		m_pSocketMatrix[i] = pDesc->pSocket[i];
		m_fTransformationMatrices[i] = pDesc->fTransformationMatrices[i];
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;	

	m_bRender = false;

	switch(m_eEquip) {
	case CPlayer::HG:
		m_iMaxBullet = 12;

		m_pModelCom->Hide_Mesh("wp0000vp70_1_Group_1_Sub_1__wp0100_VP70Custom_Mat_mesh0002", true);
		m_pModelCom->Hide_Mesh("wp0000vp70_1_Group_2_Sub_1__wp0000_PowerUp_Mat_mesh0003", true);
		m_pModelCom->Hide_Mesh("wp0000vp70_1_Group_6_Sub_1__wp0000_PowerUp_Mat_mesh0004", true);
		break;
	case CPlayer::STG:
		m_iMaxBullet = 7;

		m_pModelCom->Hide_Mesh("wp1000shotgun_1_Group_3_Sub_1__wp1000_mt_mesh0004", true);
		m_pModelCom->Hide_Mesh("wp1000shotgun_1_Group_4_Sub_1__wp1100_mt_mesh0005", true);
		break;
	}

	m_pModelCom->Set_RootBone("root");
	m_pModelCom->Add_Bone_Layer_All_Bone(TEXT("Default"));
	m_pModelCom->Add_AnimPlayingInfo(false, 0, TEXT("Default"), 1.f);
	m_pModelCom->Change_Animation(0, TEXT("Default"), 0);

	//m_pTransformCom->Set_Scaled(0.1f, 0.1f, 0.1f);
	//m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.0f));
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float4(0.8f, 0.f, 0.f, 1.f));

	return S_OK;
}

void CWeapon::Tick(_float fTimeDelta)
{
	//__super::Tick(fTimeDelta);
	//m_pColliderCom->Tick(XMLoadFloat4x4(&m_WorldMatrix));
}

void CWeapon::Late_Tick(_float fTimeDelta)
{
	if (m_eRenderLocation != NONE) {
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_DIR, this);
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_POINT, this);
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);

		_float3				vDirection = { };
		m_pModelCom->Play_Animations(m_pParentsTransform, fTimeDelta, &vDirection);

		_matrix			WorldMatrix = { m_fTransformationMatrices[m_eRenderLocation] * m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pSocketMatrix[m_eRenderLocation]) * m_pParentsTransform->Get_WorldMatrix() };

		XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);

	}
		
#ifdef _DEBUG
	//m_pGameInstance->Add_DebugComponents(m_pColliderCom);
#endif
}

HRESULT CWeapon::Render()
{
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

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		m_pModelCom->Render(static_cast<_uint>(i));
	}

	return S_OK;
}

HRESULT CWeapon::Render_LightDepth_Dir()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (m_pGameInstance->Get_ShadowLight(CPipeLine::DIRECTION) != nullptr) {

		const CLight* pLight = m_pGameInstance->Get_ShadowLight(CPipeLine::DIRECTION);
		const LIGHT_DESC* pDesc = pLight->Get_LightDesc(0);

		if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &pDesc->ViewMatrix[0])))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &pDesc->ProjMatrix)))
			return E_FAIL;

		list<_uint>			NonHideIndices = { m_pModelCom->Get_NonHideMeshIndices() };
		for (auto& i : NonHideIndices)
		{
			if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(2)))
				return E_FAIL;

			m_pModelCom->Render(static_cast<_uint>(i));
		}
	}

	return S_OK;
}

HRESULT CWeapon::Render_LightDepth_Point()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
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

		list<_uint>			NonHideIndices = { m_pModelCom->Get_NonHideMeshIndices() };
		for (auto& i : NonHideIndices)
		{
			if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(4)))
				return E_FAIL;

			m_pModelCom->Render(static_cast<_uint>(i));
		}

		++iIndex;
	}

	return S_OK;
}

HRESULT CWeapon::Render_LightDepth_Spot()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (m_pGameInstance->Get_ShadowLight(CPipeLine::SPOT) != nullptr) {

		const CLight* pLight = m_pGameInstance->Get_ShadowLight(CPipeLine::SPOT);
		const LIGHT_DESC* pDesc = pLight->Get_LightDesc(0);

		if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &pDesc->ViewMatrix[0])))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &pDesc->ProjMatrix)))
			return E_FAIL;

		list<_uint>			NonHideIndices = { m_pModelCom->Get_NonHideMeshIndices() };
		for (auto& i : NonHideIndices)
		{
			if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i))))
				return E_FAIL;

			m_pModelCom->Render(static_cast<_uint>(i));
		}
	}

	return S_OK;
}

_float4 CWeapon::Get_MuzzlePosition()
{
	if (1) {
		return Get_BonePosition("vfx_muzzle1");			//기본 상태
	}
	else if (1) {
		return Get_BonePosition("vfx_muzzle3");			// 총신 부착시
	}

	return _float4(0.f, 0.f, 0.f, 0.f);						// 쓰레기
}

_float4 CWeapon::Get_CartridgePosition()
{
	return Get_BonePosition("vfx_muzzle2");
}

_float4 CWeapon::Get_CartridgeDir()
{
	_matrix WorldMat = XMLoadFloat4x4(&m_WorldMatrix);
	_float4 vRight;
	XMStoreFloat4(&vRight, WorldMat.r[2]);
	return Float4_Normalize(vRight);
}

_float4 CWeapon::Get_MuzzlePosition_Forward()
{
	if (1) {
		return Get_BonePosition_Forward("vfx_muzzle1");			//기본 상태
	}
	else if (1) {
		return Get_BonePosition_Forward("vfx_muzzle3");			// 총신 부착시
	}

	return _float4(0.f, 0.f, 0.f, 0.f);						// 쓰레기
}

_float4 CWeapon::Get_BonePosition(const char* pBoneName)
{
	_float4x4 pMatrix = m_pModelCom->Get_BonePtr(pBoneName)->Get_CombinedTransformationMatrix_Var();

	_vector vScale;
	_vector vRoation;
	_vector vTranspose;

	XMMatrixDecompose(&vScale, &vRoation, &vTranspose, XMLoadFloat4x4(&m_WorldMatrix));
	/*
		_float4x4			m_WorldMatrix;					// 자신의 월드 행렬
	const _float4x4*	m_pParentMatrix = { nullptr };	// 이 파츠를 보유하고 있는 GameObject == Parent의 월드 행렬을 포인터로 보유
	*/

	//_matrix vMatrix = XMMatrixRotationQuaternion(vRoation)*XMLoadFloat4x4(&pMatrix);
	_matrix vMatrix = XMLoadFloat4x4(&pMatrix) * XMLoadFloat4x4(&m_WorldMatrix);
	_float4x4 fMatrix;
	XMStoreFloat4x4(&fMatrix, vMatrix);
	//_float4 vPos = { fMatrix.Forward().x + vTranspose.m128_f32[0] ,	fMatrix.Forward().y + vTranspose.m128_f32[1],	fMatrix.Forward().z + vTranspose.m128_f32[2],	1.f };
	//_float4 vPos = { fMatrix._41 + vTranspose.m128_f32[0] ,	fMatrix._42 + vTranspose.m128_f32[1],	fMatrix._43 + vTranspose.m128_f32[2],	1.f };
	_float4 vPos = _float4(fMatrix._41, fMatrix._42, fMatrix._43, 1.f);


	return vPos;
}

_float4 CWeapon::Get_BonePosition_Forward(const char* pBoneName)
{
	_float4x4 pMatrix = m_pModelCom->Get_BonePtr(pBoneName)->Get_CombinedTransformationMatrix_Var();

	_vector vScale;
	_vector vRoation;
	_vector vTranspose;

	XMMatrixDecompose(&vScale, &vRoation, &vTranspose, XMLoadFloat4x4(&m_WorldMatrix));

	_matrix vMatrix = XMLoadFloat4x4(&pMatrix) * XMLoadFloat4x4(&m_WorldMatrix);
	_float4x4 fMatrix;
	XMStoreFloat4x4(&fMatrix, vMatrix);
	
	_float4 vLook = Float4_Normalize(_float4(fMatrix._21, fMatrix._22, fMatrix._23, 0.f));
	XMStoreFloat4(&vLook, XMVectorScale(XMLoadFloat4(&vLook), 0.3f));
	vLook.y *= 0.1f;
	_float4 vPos = _float4(fMatrix._41 + vLook.x, fMatrix._42 + vLook.y, fMatrix._43 + vLook.z, 1.f);


	return vPos;
}

HRESULT CWeapon::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	wstring strModelTag;
	switch (m_eEquip) {
	case CPlayer::HG:
		strModelTag = TEXT("Prototype_Component_Model_HandGun");
		break;
	case CPlayer::STG:
		strModelTag = TEXT("Prototype_Component_Model_ShotGun");
		break;
	case CPlayer::GRENADE:
		strModelTag = TEXT("Prototype_Component_Model_Grenade");
		break;
	case CPlayer::FLASHBANG:
		strModelTag = TEXT("Prototype_Component_Model_FlashBang");
		break;
	}

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(g_Level, strModelTag,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	///* Com_Collider */
	//CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

	//ColliderDesc.vSize = _float3(2.0f, 2.0f, 3.1f);
	//ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);


	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
	//	TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CWeapon::Bind_ShaderResources()
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

CWeapon * CWeapon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CWeapon*		pInstance = new CWeapon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CWeapon"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CWeapon::Clone(void * pArg)
{
	CWeapon*		pInstance = new CWeapon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CWeapon"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);	
	Safe_Release(m_pModelCom);
}
