#include "stdafx.h"
#include "..\Public\Body_Player.h"

#include "Player.h"
#include "Light.h"

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
	{d
		iBoneIndices.emplace_back(i);
	}
	AnimDesc.TargetBoneIndices = iBoneIndices;
	m_pModelCom->Set_Animation_Blend(AnimDesc, 0);*/

	m_pModelCom->Set_RootBone("root");
	m_pModelCom->Add_Bone_Layer_All_Bone(TEXT("Default"));

	list<_uint>			LeftArmBoneIndices;
	m_pModelCom->Get_Child_BoneIndices("l_arm_clavicle", LeftArmBoneIndices);
	m_pModelCom->Add_Bone_Layer(TEXT("Left_Arm"), LeftArmBoneIndices);


	list<_uint>			LowerBoneIndices;
	for (_uint iBoneIndex = 0; iBoneIndex < 60; ++iBoneIndex)
		LowerBoneIndices.emplace_back(iBoneIndex);
	m_pModelCom->Add_Bone_Layer(TEXT("Lower"), LowerBoneIndices);

	list<_uint>			UpperBoneIndices;
	for (_uint iBoneIndex = 60; iBoneIndex < static_cast<_uint>(m_pModelCom->Get_BoneNames().size()); ++iBoneIndex)
	{
		list<_uint>::iterator		iter = { find(LeftArmBoneIndices.begin(), LeftArmBoneIndices.end(), iBoneIndex) };
		if (LeftArmBoneIndices.end() != iter)
			continue;

		UpperBoneIndices.emplace_back(iBoneIndex);
	}

	m_pModelCom->Add_Bone_Layer(TEXT("Upper"), UpperBoneIndices);

	//	m_pModelCom->Add_IK("root", "l_leg_ball", TEXT("IK_L_LEG"), 1);
	m_pModelCom->Add_IK("l_leg_femur", "l_leg_ball", TEXT("IK_L_LEG"), 1, 1.f);
	//	m_pModelCom->Add_IK("r_leg_femur", "r_leg_ball", TEXT("IK_R_LEG"), 1, 1.f);
	m_pModelCom->Add_IK("r_arm_clavicle", "r_arm_wrist", TEXT("IK_R_ARM"), 1, 1.f);

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
	if (UP == m_pGameInstance->Get_KeyState(VK_UP))
	{
		++iAnimIndex;
		if (50 <= iAnimIndex)
			iAnimIndex = 50;
	}

	if (DOWN == m_pGameInstance->Get_KeyState(VK_DOWN))
	{
		--iAnimIndex;
		if (1000000 < iAnimIndex)
			iAnimIndex = 0;
	}	

	m_pModelCom->Set_TickPerSec(iAnimIndex, 40.f);

	static _float fWeight = { 1.f };
	static _float fMoveHieght = { 0.f };

	if (PRESSING == m_pGameInstance->Get_KeyState('N'))
	{
		/*fWeight -= 0.02f;
		if (fWeight < 0.f)
			fWeight = 0.f;*/

		fMoveHieght += 1.f * fTimeDelta;

		if (fMoveHieght > 0.5f)
			fMoveHieght = 0.5f;
	}

	if (PRESSING == m_pGameInstance->Get_KeyState('M'))
	{
		fMoveHieght -= 1.f * fTimeDelta;

		if (fMoveHieght < -0.5f)
			fMoveHieght = -0.5f;
	}

	static _float fBlend = { 1.f };

	if (PRESSING == m_pGameInstance->Get_KeyState('Z'))
	{
		fBlend -= 0.02f;
		if (fBlend < 0.f)
			fBlend = 0.f;
	}

	if (PRESSING == m_pGameInstance->Get_KeyState('X'))
	{
		fBlend += 0.02f;
		if (fBlend > 1.f)
			fBlend = 1.f;
	}

	//	fMoveHieght = 0.f;

	_vector			vMoveDir = { XMVectorSet(0.f, 1.f, 1.f, 0.f) * fMoveHieght };
	_vector			vCurrentPos = { m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION) };
	_vector			vCurrentBallPos = { XMVector4Transform(vCurrentPos, XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrix("l_leg_ball"))) };

	_vector			vDirectionToBall = { vCurrentBallPos - vCurrentPos };

	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////TEST/////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////

	CVIBuffer_Terrain* pTerrainBuffer = { dynamic_cast<CVIBuffer_Terrain*>(const_cast<CComponent*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_LandBackGround"), TEXT("Com_VIBuffer"), 0))) };
	CTransform* pTerrainTransform = { dynamic_cast<CTransform*>(const_cast<CComponent*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_LandBackGround"), TEXT("Com_Transform"), 0))) };
	if (nullptr != pTerrainBuffer &&
		nullptr != pTerrainTransform)
	{
		_matrix			WorldMatrix = { XMLoadFloat4x4(&m_WorldMatrix) };
		_matrix			BallCombinedMatrix = { XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrix("l_leg_ball")) };

		_matrix			BallWorldMatrix = { BallCombinedMatrix * WorldMatrix };

		_vector		vPosition = { BallWorldMatrix.r[CTransform::STATE_POSITION] };
		_float4		vPickPosFloat4;
		pTerrainBuffer->Compute_Height(pTerrainTransform, vPosition, &vPickPosFloat4);

		_vector		vResultPos = { XMLoadFloat4(&vPickPosFloat4) };
		vMoveDir = vResultPos - vPosition;


		if (0.f >= XMVectorGetY(vMoveDir))
			vMoveDir = XMVectorZero();
	}

	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////

	//	_vector			vMoveDir = { XMVectorSetY(vCurrentBallPos, XMVectorGetY(vCurrentPos) + 0.05f) - vCurrentBallPos };
	//	_vector			vMoveDir = { XMVectorSetY(vCurrentBallPos, XMVectorGetY(vCurrentPos) + 0.05f) - vCurrentBallPos };
	m_pModelCom->Set_Direction_IK(TEXT("IK_L_LEG"), vMoveDir);
	//	m_pModelCom->Set_Direction_IK(TEXT("IK_R_LEG"), vMoveDir);
	//	m_pModelCom->Set_Direction_IK(TEXT("IK_R_ARM"), vMoveDir);

	m_pModelCom->Set_Blend_IK(TEXT("IK_L_LEG"), fBlend);
	//	m_pModelCom->Set_Blend_IK(TEXT("IK_R_LEG"), fBlend);
	m_pModelCom->Set_Blend_IK(TEXT("IK_R_ARM"), fBlend);

	/*_matrix			WorldMatrix = { m_pTransformCom->Get_WorldMatrix() };
	_matrix			TranslationMatrix = { XMMatrixTranslation(XMVectorGetX(vMoveDir), )}*/

	m_pModelCom->Set_NumIteration_IK(TEXT("IK_L_LEG"), 10);
	m_pModelCom->Set_NumIteration_IK(TEXT("IK_R_LEG"), 10);
	m_pModelCom->Set_NumIteration_IK(TEXT("IK_R_ARM"), 10);

	CModel::ANIM_PLAYING_DESC		AnimDesc;
	//	AnimDesc.iAnimIndex = 30;
	AnimDesc.iAnimIndex = iAnimIndex;
	AnimDesc.isLoop = true;
	//	AnimDesc.fWeight = fWeight;
	AnimDesc.fWeight = 1.f;
	AnimDesc.strBoneLayerTag = TEXT("Default");
	m_pModelCom->Set_Animation_Blend(AnimDesc, 0);

	//	AnimDesc.iAnimIndex = 34;
	//	AnimDesc.iAnimIndex = 24;
	AnimDesc.iAnimIndex = 14;
	AnimDesc.isLoop = true;
	//	AnimDesc.fWeight = 1.f - fWeight;
	AnimDesc.fWeight = 0.1f;
	AnimDesc.strBoneLayerTag = TEXT("Upper");
	m_pModelCom->Set_Animation_Blend(AnimDesc, 1);


	AnimDesc.iAnimIndex = iAnimIndex + 1;
	AnimDesc.isLoop = true;
	//	AnimDesc.fWeight = 1.f - fWeight;
	AnimDesc.fWeight = 1.f;
	AnimDesc.strBoneLayerTag = TEXT("Left_Arm");
	m_pModelCom->Set_Animation_Blend(AnimDesc, 2);


	m_pModelCom->Set_Weight(0, 1.f);
	m_pModelCom->Set_Weight(1, 0.f);
	m_pModelCom->Set_Weight(2, 1.f);


	m_pModelCom->Set_TickPerSec(iAnimIndex, 60.f);

	m_pModelCom->Set_RootBone("root");

	static _bool		isSetRootXZ = false;
	static _bool		isSetRootRotation = false;
	static _bool		isSetRootY = false;
	if (UP == m_pGameInstance->Get_KeyState('I'))
	{
		isSetRootY = !isSetRootY;
	}

	if (UP == m_pGameInstance->Get_KeyState('O'))
	{
		isSetRootXZ = !isSetRootXZ;
	}

	if (UP == m_pGameInstance->Get_KeyState('P'))
	{
		isSetRootRotation = !isSetRootRotation;
	}

	CModel::ANIM_PLAYING_DESC		AnimDesc;
	//AnimDesc.iAnimIndex = 22;
	AnimDesc.iAnimIndex = iAnimIndex;
	AnimDesc.isLoop = true;
	AnimDesc.fWeight = fWeight;
	_uint		iNumBones = { static_cast<_uint>(m_pModelCom->Get_BoneNames().size()) };
	list<_uint>		iBoneIndices;
	for (_uint i = 0; i < iNumBones; ++i)
	m_pModelCom->Active_RootMotion_Rotation(isSetRootRotation);
	m_pModelCom->Active_RootMotion_XZ(isSetRootXZ);
	m_pModelCom->Active_RootMotion_Y(isSetRootY);

	//	m_pModelCom->Add_IK("l_leg_femur", "l_leg_ball", TEXT("IK_L_LEG"), 1);
	//	m_pModelCom->Add_IK("r_arm_clavicle", "r_arm_wrist", TEXT("IK_R_ARM"), 1);

	{
		_matrix		WorldMatrix = { XMLoadFloat4x4(&m_WorldMatrix) };
		_matrix		CombinedMatrix = { XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrix("l_leg_ball")) };
		_matrix		ScaleMatrix = { XMMatrixScaling(10.f, 10.f, 100.f) };

		_matrix		ResultMatrix = { ScaleMatrix * CombinedMatrix * WorldMatrix  };

		m_PartColliders[0]->Tick(ResultMatrix);
	}

	{
		_matrix		WorldMatrix = { XMLoadFloat4x4(&m_WorldMatrix) };
		_matrix		CombinedMatrix = { XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrix("l_leg_femur")) };
		_matrix		ScaleMatrix = { XMMatrixScaling(10.f, 10.f, 100.f) };

		_matrix		ResultMatrix = { ScaleMatrix * CombinedMatrix * WorldMatrix };

		m_PartColliders[1]->Tick(ResultMatrix);
	}

	{
		_matrix		WorldMatrix = { XMLoadFloat4x4(&m_WorldMatrix) };
		_matrix		CombinedMatrix = { XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrix("l_leg_ankle")) };
		_matrix		ScaleMatrix = { XMMatrixScaling(10.f, 10.f, 100.f) };

		_matrix		ResultMatrix = { ScaleMatrix * CombinedMatrix * WorldMatrix };

		m_PartColliders[2]->Tick(ResultMatrix);
	}

	{
		_matrix		WorldMatrix = { XMLoadFloat4x4(&m_WorldMatrix) };
		_matrix		CombinedMatrix = { XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrix("l_leg_tibia")) };
		_matrix		ScaleMatrix = { XMMatrixScaling(10.f, 10.f, 100.f) };

		_matrix		ResultMatrix = { ScaleMatrix * CombinedMatrix * WorldMatrix };

		m_PartColliders[3]->Tick(ResultMatrix);
	}
}

void CBody_Player::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	
	static bool Temp = false;
	if (UP == m_pGameInstance->Get_KeyState(VK_SPACE))
	{
		Temp = true;
	}

	//if(!Temp)
	{
		m_pModelCom->Play_Animations_RootMotion(m_pParentsTransform, fTimeDelta, m_pRootTranslation);
	}
	Temp = true;

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
	BoneCombined = m_pModelCom->GetBoneTransform(60);
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

#ifdef _DEBUG
	m_pGameInstance->SetColliderTransform_Right_Shin(BoneCombined);

	
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_POINT, this);

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

#ifdef _DEBUG

	m_PartColliders[0]->Render();
	m_PartColliders[1]->Render();
	m_PartColliders[2]->Render();
	m_PartColliders[3]->Render();

	/*for (auto& pPartCollider : m_PartColliders)
	{
		pPartCollider->Render();
	}*/

#endif

	return S_OK;
}

HRESULT CBody_Player::Render_LightDepth()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (m_pGameInstance->Get_ShadowSpotLight() != nullptr) {

		const CLight* pLight = m_pGameInstance->Get_ShadowSpotLight();
		const LIGHT_DESC* pDesc = pLight->Get_LightDesc(0);

		if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &pDesc->ViewMatrix[0])))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &pDesc->ProjMatrix)))
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
	}

	return S_OK;
}

HRESULT CBody_Player::Render_LightDepth_Cube()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	list<LIGHT_DESC*> LightDescList = m_pGameInstance->Get_ShadowLightDesc_List();
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

			if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i))))
				return E_FAIL;

			/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이ㅏ터를 다 던져야한다. */
			if (FAILED(m_pShaderCom->Begin(5)))
				return E_FAIL;

			m_pModelCom->Render(static_cast<_uint>(i));
		}

		++iIndex;
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

	
	for (_uint i = 0; i < 10; ++i)
	{
		CBounding_OBB::BOUNDING_OBB_DESC		ColliderOBBDesc{};
		ColliderOBBDesc.vCenter = { 0.f, 0.f, 0.f };
		ColliderOBBDesc.vRotation = { 0.f, 0.f, 0.f };
		ColliderOBBDesc.vSize = { 1.f, 1.f, 1.f };

		CComponent*			pCollider = { m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"), &ColliderOBBDesc) };
		if (nullptr == pCollider)
			return E_FAIL;

		m_PartColliders.push_back(dynamic_cast<CCollider*>(pCollider));
	}


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

	for (auto& pCollider : m_PartColliders)
		Safe_Release(pCollider);
	m_PartColliders.clear();
}
