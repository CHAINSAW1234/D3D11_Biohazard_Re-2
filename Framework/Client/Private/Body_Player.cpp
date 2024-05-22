#include "stdafx.h"
#include "..\Public\Body_Player.h"

#include "Player.h"

CBody_Player::CBody_Player(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CPartObject{ pDevice, pContext }
{
}

CBody_Player::CBody_Player(const CBody_Player & rhs)
	: CPartObject{ rhs }
{

}

HRESULT CBody_Player::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBody_Player::Initialize(void * pArg)
{
	BODY_DESC*		pDesc = { static_cast<BODY_DESC*>(pArg) };

	pDesc->fSpeedPerSec = 10.f;
	pDesc->fRotationPerSec = XMConvertToRadians(90.0f);
	m_pRootTranslation = pDesc->pRootTranslation;

	if (nullptr == m_pRootTranslation)
		return E_FAIL;

	m_pState = pDesc->pState;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;	

	/*CModel::ANIM_PLAYING_DESC		AnimDesc;
	AnimDesc.iAnimIndex = 0;
	AnimDesc.isLoop = true;
	_uint		iNumBones = { static_cast<_uint>(m_pModelCom->Get_BoneNames().size()) };
	list<_uint>		iBoneIndices;
	for (_uint i = 0; i < iNumBones; ++i)
	{
		iBoneIndices.emplace_back(i);
	}
	AnimDesc.TargetBoneIndices = iBoneIndices;
	m_pModelCom->Set_Animation_Blend(AnimDesc, 0);*/

	m_pModelCom->Set_RootBone("root");

	m_pModelCom->Set_SpineBone("spine_0");

	m_pModelCom->Init_Separate_Bones();

	return S_OK;
}

void CBody_Player::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CBody_Player::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	//Upper 13
	//Lower 33
	
	_matrix         WorldMatrix = { XMLoadFloat4x4(&m_WorldMatrix) };

	_vector         vLook = { WorldMatrix.r[CTransform::STATE_LOOK] };
	_vector         vPosition = { WorldMatrix.r[CTransform::STATE_POSITION] };

	vPosition += XMVector3Normalize(vLook) * 3.f;

	WorldMatrix.r[CTransform::STATE_POSITION] = vPosition;

	m_pColliderCom->Tick(WorldMatrix);
	
	static _uint iAnimIndex = { 0 };
	if (DOWN == m_pGameInstance->Get_KeyState(VK_UP))
	{
		++iAnimIndex;
		if (50 <= iAnimIndex)
			iAnimIndex = 50;
	}

	if (DOWN == m_pGameInstance->Get_KeyState(VK_DOWN))
	{
		--iAnimIndex;
		if (0 > iAnimIndex)
			iAnimIndex = 0;
	}

	static bool Temp = false;
	static bool Temp2 = false;
	if (UP == m_pGameInstance->Get_KeyState('P'))
	{
		Temp = !Temp;
	}

	if (UP == m_pGameInstance->Get_KeyState('O'))
	{
		Temp2 = !Temp2;
	}

	if (Temp)
	{
		m_pModelCom->Active_RootMotion_Rotation(true);
	}
	else
	{
		m_pModelCom->Active_RootMotion_Rotation(false);
	}

	if (Temp2)
	{
		m_pModelCom->Active_RootMotion_XZ(true);
	}
	else
	{
		m_pModelCom->Active_RootMotion_XZ(false);
	}

	m_pModelCom->Set_TickPerSec(iAnimIndex, 40.f);

	static _float fWeight = { 1.f };

	if (PRESSING == m_pGameInstance->Get_KeyState('N'))
	{
		fWeight -= 0.02f;
		if (fWeight < 0.f)
			fWeight = 0.f;
	}

	if (PRESSING == m_pGameInstance->Get_KeyState('M'))
	{
		fWeight += 0.02f;
		if (fWeight > 1.f)
			fWeight = 1.f;
	}

	CModel::ANIM_PLAYING_DESC		AnimDesc;
	AnimDesc.iAnimIndex = 22;
	AnimDesc.isLoop = true;
	AnimDesc.fWeight = fWeight;
	_uint		iNumBones = { static_cast<_uint>(m_pModelCom->Get_BoneNames().size()) };
	list<_uint>		iBoneIndices;
	for (_uint i = 0; i < iNumBones; ++i)
	{
		iBoneIndices.emplace_back(i);
	}
	AnimDesc.TargetBoneIndices = iBoneIndices;
	m_pModelCom->Set_Animation_Blend(AnimDesc, 0);

	AnimDesc.iAnimIndex = 33;
	AnimDesc.isLoop = true;
	AnimDesc.fWeight = 1.f - fWeight;
	iBoneIndices.clear();
	for (_uint i = 0; i < iNumBones; ++i)
	{
		iBoneIndices.emplace_back(i);
	}
	AnimDesc.TargetBoneIndices = iBoneIndices;
	m_pModelCom->Set_Animation_Blend(AnimDesc, 1);


	m_pModelCom->Set_Weight(0, fWeight);
	m_pModelCom->Set_Weight(1, 1.f - fWeight);


	//	m_pModelCom->Set_TickPerSec(iAnimIndex, 60.f);

	m_pModelCom->Set_RootBone("root");
	//m_pModelCom->Active_RootMotion_XZ(iAnimIndex, true);
	m_pModelCom->Active_RootMotion_Y(false);
	//m_pModelCom->Active_RootMotion_Rotation(iAnimIndex, true);
}

void CBody_Player::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pModelCom->Play_Animations_RootMotion(m_pParentsTransform, fTimeDelta, m_pRootTranslation);

	//Body
	_float4x4 BoneCombined = m_pModelCom->GetBoneTransform(62);
	BoneCombined._41 *= 0.045f;
	BoneCombined._42 *= 0.045f;
	BoneCombined._43 *= 0.045f;
	_matrix Mat = XMLoadFloat4x4(&BoneCombined);

	_matrix Rot = m_pParentsTransform->Get_WorldMatrix_Pure_Mat();

	Mat = XMMatrixMultiply(Mat, Rot);
	XMStoreFloat4x4(&BoneCombined, Mat);

	m_pGameInstance->SetColliderTransform(BoneCombined);

	//Head
	BoneCombined = m_pModelCom->GetBoneTransform(169);
	BoneCombined._41 *= 0.045f;
	BoneCombined._42 *= 0.045f;
	BoneCombined._43 *= 0.045f;
	Mat = XMLoadFloat4x4(&BoneCombined);

	Mat = XMMatrixMultiply(Mat, Rot);
	XMStoreFloat4x4(&BoneCombined, Mat);
	m_pGameInstance->SetColliderTransform_Head(BoneCombined);

	//Left Arm
	BoneCombined = m_pModelCom->GetBoneTransform(84);
	BoneCombined._41 *= 0.045f;
	BoneCombined._42 *= 0.045f;
	BoneCombined._43 *= 0.045f;
	Mat = XMLoadFloat4x4(&BoneCombined);

	Mat = XMMatrixMultiply(Mat, Rot);
	XMStoreFloat4x4(&BoneCombined, Mat);
	m_pGameInstance->SetColliderTransform_Left_Arm(BoneCombined);

	//Left ForeArm
	BoneCombined = m_pModelCom->GetBoneTransform(85);
	BoneCombined._41 *= 0.045f;
	BoneCombined._42 *= 0.045f;
	BoneCombined._43 *= 0.045f;
	Mat = XMLoadFloat4x4(&BoneCombined);

	Mat = XMMatrixMultiply(Mat, Rot);
	XMStoreFloat4x4(&BoneCombined, Mat);
	m_pGameInstance->SetColliderTransform_Left_ForeArm(BoneCombined);

	//Right Arm
	BoneCombined = m_pModelCom->GetBoneTransform(126);
	BoneCombined._41 *= 0.045f;
	BoneCombined._42 *= 0.045f;
	BoneCombined._43 *= 0.045f;
	Mat = XMLoadFloat4x4(&BoneCombined);

	Mat = XMMatrixMultiply(Mat, Rot);
	XMStoreFloat4x4(&BoneCombined, Mat);
	m_pGameInstance->SetColliderTransform_Right_Arm(BoneCombined);

	//Right ForeArm
	BoneCombined = m_pModelCom->GetBoneTransform(127);
	BoneCombined._41 *= 0.045f;
	BoneCombined._42 *= 0.045f;
	BoneCombined._43 *= 0.045f;
	Mat = XMLoadFloat4x4(&BoneCombined);

	Mat = XMMatrixMultiply(Mat, Rot);
	XMStoreFloat4x4(&BoneCombined, Mat);
	m_pGameInstance->SetColliderTransform_Right_ForeArm(BoneCombined);

	//Pelvis
	BoneCombined = m_pModelCom->GetBoneTransform(22);
	BoneCombined._41 *= 0.045f;
	BoneCombined._42 *= 0.045f;
	BoneCombined._43 *= 0.045f;
	Mat = XMLoadFloat4x4(&BoneCombined);

	Mat = XMMatrixMultiply(Mat, Rot);
	XMStoreFloat4x4(&BoneCombined, Mat);
	m_pGameInstance->SetColliderTransform_Pelvis(BoneCombined);

	//Left Leg
	BoneCombined = m_pModelCom->GetBoneTransform(27);
	BoneCombined._41 *= 0.045f;
	BoneCombined._42 *= 0.045f;
	BoneCombined._43 *= 0.045f;
	Mat = XMLoadFloat4x4(&BoneCombined);

	Mat = XMMatrixMultiply(Mat, Rot);
	XMStoreFloat4x4(&BoneCombined, Mat);
	m_pGameInstance->SetColliderTransform_Left_Leg(BoneCombined);

	//Left Shin
	BoneCombined = m_pModelCom->GetBoneTransform(28);
	BoneCombined._41 *= 0.045f;
	BoneCombined._42 *= 0.045f;
	BoneCombined._43 *= 0.045f;
	Mat = XMLoadFloat4x4(&BoneCombined);

	Mat = XMMatrixMultiply(Mat, Rot);
	XMStoreFloat4x4(&BoneCombined, Mat);
	m_pGameInstance->SetColliderTransform_Left_Shin(BoneCombined);


	//Right Leg
	BoneCombined = m_pModelCom->GetBoneTransform(41);
	BoneCombined._41 *= 0.045f;
	BoneCombined._42 *= 0.045f;
	BoneCombined._43 *= 0.045f;
	Mat = XMLoadFloat4x4(&BoneCombined);

	Mat = XMMatrixMultiply(Mat, Rot);
	XMStoreFloat4x4(&BoneCombined, Mat);
	m_pGameInstance->SetColliderTransform_Right_Leg(BoneCombined);

	//Right Shin
	BoneCombined = m_pModelCom->GetBoneTransform(42);
	BoneCombined._41 *= 0.045f;
	BoneCombined._42 *= 0.045f;
	BoneCombined._43 *= 0.045f;
	Mat = XMLoadFloat4x4(&BoneCombined);

	Mat = XMMatrixMultiply(Mat, Rot);
	XMStoreFloat4x4(&BoneCombined, Mat);
	m_pGameInstance->SetColliderTransform_Right_Shin(BoneCombined);

	
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponents(m_pColliderCom);
#endif
}

HRESULT CBody_Player::Render()
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

HRESULT CBody_Player::Render_LightDepth()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	_float4x4		ViewMatrix, ProjMatrix;
	
	ViewMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW, CPipeLine::SHADOW);
	ProjMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ, CPipeLine::SHADOW);
	
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
		if (FAILED(m_pShaderCom->Begin(3)))
			return E_FAIL;

		m_pModelCom->Render(static_cast<_uint>(i));
	}

	return S_OK;

}

HRESULT CBody_Player::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"), 
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_LeonBody"),
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

HRESULT CBody_Player::Bind_ShaderResources()
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

CBody_Player * CBody_Player::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBody_Player*		pInstance = new CBody_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBody_Player"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject * CBody_Player::Clone(void * pArg)
{
	CBody_Player*		pInstance = new CBody_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CBody_Player"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Player::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);	
	Safe_Release(m_pModelCom);
}
