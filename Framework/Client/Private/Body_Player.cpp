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

	_uint			iNumBones = { static_cast<_uint>(m_pModelCom->Get_BoneNames().size()) };

	m_pModelCom->Add_Bone_Layer_ChildIndices(TEXT("Left_Arm"), "l_arm_clavicle");
	m_pModelCom->Add_Bone_Layer_Range(TEXT("Lower"), 0, 59);
	m_pModelCom->Add_Bone_Layer_Range(TEXT("Upper"), 60, iNumBones -1);


	m_pModelCom->Add_AnimPlayingInfo(0, true, 0, TEXT("Default"), 1.f);
	m_pModelCom->Add_AnimPlayingInfo(0, true, 1, TEXT("Lower"), 1.f);
	m_pModelCom->Add_AnimPlayingInfo(0, true, 2, TEXT("Upper"), 1.f);

	//	m_pModelCom->Add_IK("root", "l_leg_ball", TEXT("IK_L_LEG"), 1);
	//	m_pModelCom->Add_IK("l_leg_femur", "l_leg_ankle", TEXT("IK_L_LEG"), 1, 1.f);
	m_pModelCom->Add_IK("l_leg_femur", "l_leg_ankle", TEXT("IK_L_LEG"), 20, 1.f);
	//	m_pModelCom->Add_IK("hips", "l_leg_ball", TEXT("IK_L_LEG"), 1, 1.f);
	//	m_pModelCom->Add_IK("r_leg_femur", "r_leg_ball", TEXT("IK_R_LEG"), 1, 1.f);
	//	m_pModelCom->Add_IK("r_arm_humerus", "r_arm_wrist", TEXT("IK_R_ARM"), 20, 1.f);
	//	m_pModelCom->Add_IK("r_arm_clavicle", "r_arm_wrist", TEXT("IK_R_ARM"), 1, 1.f);
	//	m_pModelCom->Add_IK("spine_0", "r_arm_wrist", TEXT("IK_R_ARM"), 1, 1.f);

	m_pModelCom->Set_RootBone("root");

	m_pGameInstance->SetBone_Ragdoll(m_pModelCom->GetBoneVector());

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
		fMoveHieght += 1.f * fTimeDelta;

		if (fMoveHieght > 2.5f)
			fMoveHieght = 2.5f;
	}

	if (PRESSING == m_pGameInstance->Get_KeyState('M'))
	{
		fMoveHieght -= 1.f * fTimeDelta;

		if (fMoveHieght < -2.5f)
			fMoveHieght = -2.5f;
	}

	static _float fBlend = { 0.f };

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

	_vector			vMoveDir = { XMVectorSet(0.f, 1.f, 0.f, 0.f) * fMoveHieght };
	_vector			vCurrentPos = { m_pParentsTransform->Get_State_Vector(CTransform::STATE_POSITION) };
	_vector			vCurrentBallPos = { XMVector4Transform(vCurrentPos, XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrix("l_leg_ball"))) };

	_vector			vDirectionToBall = { vCurrentBallPos - vCurrentPos };	

	//	m_pModelCom->Set_Blend_IK(TEXT("IK_L_LEG"), fBlend);
	//	m_pModelCom->Set_NumIteration_IK(TEXT("IK_L_LEG"), 3);

	m_pModelCom->Set_BoneLayer_PlayingInfo(0, TEXT("Default"));
	m_pModelCom->Change_Animation(0, iAnimIndex);

	m_pModelCom->Set_BlendWeight(0, 1.f);

	m_pModelCom->Set_TickPerSec(iAnimIndex, 60.f);


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

	m_pModelCom->Active_RootMotion_XZ(isSetRootXZ);
	m_pModelCom->Active_RootMotion_Y(isSetRootY);
	m_pModelCom->Active_RootMotion_Rotation(isSetRootRotation);

	_uint		iIndex = { 0 };
	{
		vector<_float4>			Translations = { m_pModelCom->Get_ResultTranslation_IK(TEXT("IK_L_LEG")) };
		vector<_float4>			OriginTranslations = { m_pModelCom->Get_OriginTranslation_IK(TEXT("IK_L_LEG")) };
		vector<_float4x4>		CombiendMatrices = { m_pModelCom->Get_JointCombinedMatrices_IK(TEXT("IK_L_LEG")) };

		for (_float4 vTranslation : Translations)
		{
			WorldMatrix = { XMLoadFloat4x4(&m_WorldMatrix) };

			_vector			vPosition = { XMVector3TransformCoord(vTranslation, WorldMatrix) };
			_matrix			TranslationMatrix = { XMMatrixTranslation(vPosition.m128_f32[0], vPosition.m128_f32[1], vPosition.m128_f32[2]) };
			_matrix			ScaleMatrix = { XMMatrixScaling(0.01f * (4 - iIndex), 0.01f * (4 - iIndex), 0.01f * (4 - iIndex)) };

			WorldMatrix = { ScaleMatrix * TranslationMatrix };

			m_PartColliders[iIndex++]->Tick(WorldMatrix);
		}

		for (_float4 vTranslation : OriginTranslations)
		{
			WorldMatrix = { XMLoadFloat4x4(&m_WorldMatrix) };

			_vector			vPosition = { XMVector3TransformCoord(vTranslation, WorldMatrix) };
			_matrix			TranslationMatrix = { XMMatrixTranslation(vPosition.m128_f32[0], vPosition.m128_f32[1], vPosition.m128_f32[2]) };
			_matrix			ScaleMatrix = { XMMatrixScaling(0.005f, 0.005f, 0.005f) };

			WorldMatrix = { ScaleMatrix * TranslationMatrix };

			m_PartColliders[iIndex++]->Tick(WorldMatrix);
		}

		for (_float4x4 CombinedMatrix : CombiendMatrices)
		{
			WorldMatrix = { XMLoadFloat4x4(&CombinedMatrix) * XMLoadFloat4x4(&m_WorldMatrix) };



			_matrix			TranslationMatrix = { XMMatrixTranslation(WorldMatrix.r[3].m128_f32[0], WorldMatrix.r[3].m128_f32[1], WorldMatrix.r[3].m128_f32[2]) };
			_matrix			ScaleMatrix = { XMMatrixScaling(0.01f * (12 - iIndex), 0.01f * (12 - iIndex), 0.01f * (12 - iIndex)) };

			WorldMatrix = { ScaleMatrix * TranslationMatrix };

			m_PartColliders[iIndex++]->Tick(WorldMatrix);
		}
	}
	{
		vector<_float4>			Translations = { m_pModelCom->Get_ResultTranslation_IK(TEXT("IK_R_ARM")) };
		vector<_float4>			OriginTranslations = { m_pModelCom->Get_OriginTranslation_IK(TEXT("IK_R_ARM")) };
		vector<_float4x4>		CombiendMatrices = { m_pModelCom->Get_JointCombinedMatrices_IK(TEXT("IK_R_ARM")) };

		for (_float4 vTranslation : Translations)
		{
			WorldMatrix = { XMLoadFloat4x4(&m_WorldMatrix) };

			_vector			vPosition = { XMVector3TransformCoord(vTranslation, WorldMatrix) };
			_matrix			TranslationMatrix = { XMMatrixTranslation(vPosition.m128_f32[0], vPosition.m128_f32[1], vPosition.m128_f32[2]) };
			_matrix			ScaleMatrix = { XMMatrixScaling(0.01f * (16 - iIndex), 0.01f * (16 - iIndex), 0.01f * (16 - iIndex)) };

			WorldMatrix = { ScaleMatrix * TranslationMatrix };

			m_PartColliders[iIndex++]->Tick(WorldMatrix);
		}

		for (_float4 vTranslation : OriginTranslations)
		{
			WorldMatrix = { XMLoadFloat4x4(&m_WorldMatrix) };

			_vector			vPosition = { XMVector3TransformCoord(vTranslation, WorldMatrix) };
			_matrix			TranslationMatrix = { XMMatrixTranslation(vPosition.m128_f32[0], vPosition.m128_f32[1], vPosition.m128_f32[2]) };
			_matrix			ScaleMatrix = { XMMatrixScaling(0.005f, 0.005f, 0.005f) };

			WorldMatrix = { ScaleMatrix * TranslationMatrix };

			m_PartColliders[iIndex++]->Tick(WorldMatrix);
		}

		for (_float4x4 CombinedMatrix : CombiendMatrices)
		{
			WorldMatrix = { XMLoadFloat4x4(&CombinedMatrix) * XMLoadFloat4x4(&m_WorldMatrix) };



			_matrix			TranslationMatrix = { XMMatrixTranslation(WorldMatrix.r[3].m128_f32[0], WorldMatrix.r[3].m128_f32[1], WorldMatrix.r[3].m128_f32[2]) };
			_matrix			ScaleMatrix = { XMMatrixScaling(0.01f * (24 - iIndex), 0.01f * (24 - iIndex), 0.01f * (24 - iIndex)) };

			WorldMatrix = { ScaleMatrix * TranslationMatrix };

			m_PartColliders[iIndex++]->Tick(WorldMatrix);
		}
	}
}

void CBody_Player::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	static bool Temp = false;
	if (UP == m_pGameInstance->Get_KeyState(VK_SPACE))
	{
		Temp = true;
		m_bRagdoll = true;
	}

	if (!Temp)
	{
		//	m_pModelCom->Play_Animations(fTimeDelta);
		m_pModelCom->Play_Animations(m_pParentsTransform, fTimeDelta, m_pRootTranslation);

		/////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////
	////////////////////////////////////TEST/////////////////////////////////
	/////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////

		//CVIBuffer_Terrain* pTerrainBuffer = { dynamic_cast<CVIBuffer_Terrain*>(const_cast<CComponent*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer"), 0))) };
		//CTransform* pTerrainTransform = { dynamic_cast<CTransform*>(const_cast<CComponent*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Com_Transform"), 0))) };
		//if (nullptr != pTerrainBuffer &&
		//	nullptr != pTerrainTransform)
		//{
		//	_matrix			WorldMatrix = { XMLoadFloat4x4(&m_WorldMatrix) };
		//	_matrix			BallCombinedMatrix = { XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrix("l_leg_ball")) };

		//	_matrix			BallWorldMatrix = { BallCombinedMatrix * WorldMatrix };

		//	_vector		vPosition = { BallWorldMatrix.r[CTransform::STATE_POSITION] };
		//	vPosition.m128_f32[1] += 1.f;
		//	//	vPosition.m128_f32[2] -= 0.3f;

		//	_float4		vPickPosFloat4;
		//	pTerrainBuffer->Compute_Height(pTerrainTransform, vPosition, &vPickPosFloat4);

		//	_vector		vResultPos = { XMLoadFloat4(&vPickPosFloat4) };
		//	vResultPos.m128_f32[1] += 0.1f;

		//	m_pModelCom->Set_TargetPosition_IK(TEXT("IK_L_LEG"), vResultPos);

		//	//	vMoveDir += vResultPos - vPosition;
		//}

		//	m_pModelCom->Play_IK(m_pParentsTransform, fTimeDelta);
		/////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////
	}

#pragma region Collider 위치 설정 코드
	////Body
	//_float4x4 BoneCombined = m_pModelCom->GetBoneTransform(62);
	//BoneCombined._41 *= 0.045f;
	//BoneCombined._42 *= 0.045f;
	//BoneCombined._43 *= 0.045f;
	//_matrix Mat = XMLoadFloat4x4(&BoneCombined);

	//_matrix Rot = m_pParentsTransform->Get_WorldMatrix_Pure_Mat();

	//Mat = XMMatrixMultiply(Mat, Rot);
	//XMStoreFloat4x4(&BoneCombined, Mat);

	//m_pGameInstance->SetColliderTransform(BoneCombined);

	////Head
	//BoneCombined = m_pModelCom->GetBoneTransform(169);
	//BoneCombined._41 *= 0.045f;
	//BoneCombined._42 *= 0.045f;
	//BoneCombined._43 *= 0.045f;
	//Mat = XMLoadFloat4x4(&BoneCombined);

	//Mat = XMMatrixMultiply(Mat, Rot);
	//XMStoreFloat4x4(&BoneCombined, Mat);
	//m_pGameInstance->SetColliderTransform_Head(BoneCombined);

	////Left Arm
	//BoneCombined = m_pModelCom->GetBoneTransform(84);
	//BoneCombined._41 *= 0.045f;
	//BoneCombined._42 *= 0.045f;
	//BoneCombined._43 *= 0.045f;
	//Mat = XMLoadFloat4x4(&BoneCombined);

	//Mat = XMMatrixMultiply(Mat, Rot);
	//XMStoreFloat4x4(&BoneCombined, Mat);
	//m_pGameInstance->SetColliderTransform_Left_Arm(BoneCombined);

	////Left ForeArm
	//BoneCombined = m_pModelCom->GetBoneTransform(85);
	//BoneCombined._41 *= 0.045f;
	//BoneCombined._42 *= 0.045f;
	//BoneCombined._43 *= 0.045f;
	//Mat = XMLoadFloat4x4(&BoneCombined);

	//Mat = XMMatrixMultiply(Mat, Rot);
	//XMStoreFloat4x4(&BoneCombined, Mat);
	//m_pGameInstance->SetColliderTransform_Left_ForeArm(BoneCombined);

	////Right Arm
	//BoneCombined = m_pModelCom->GetBoneTransform(126);
	//BoneCombined._41 *= 0.045f;
	//BoneCombined._42 *= 0.045f;
	//BoneCombined._43 *= 0.045f;
	//Mat = XMLoadFloat4x4(&BoneCombined);

	//Mat = XMMatrixMultiply(Mat, Rot);
	//XMStoreFloat4x4(&BoneCombined, Mat);
	//m_pGameInstance->SetColliderTransform_Right_Arm(BoneCombined);

	////Right ForeArm
	//BoneCombined = m_pModelCom->GetBoneTransform(127);
	//BoneCombined._41 *= 0.045f;
	//BoneCombined._42 *= 0.045f;
	//BoneCombined._43 *= 0.045f;
	//Mat = XMLoadFloat4x4(&BoneCombined);

	//Mat = XMMatrixMultiply(Mat, Rot);
	//XMStoreFloat4x4(&BoneCombined, Mat);
	//m_pGameInstance->SetColliderTransform_Right_ForeArm(BoneCombined);

	////Pelvis
	//BoneCombined = m_pModelCom->GetBoneTransform(60);
	//BoneCombined._41 *= 0.045f;
	//BoneCombined._42 *= 0.045f;
	//BoneCombined._43 *= 0.045f;
	//Mat = XMLoadFloat4x4(&BoneCombined);

	//Mat = XMMatrixMultiply(Mat, Rot);
	//XMStoreFloat4x4(&BoneCombined, Mat);
	//m_pGameInstance->SetColliderTransform_Pelvis(BoneCombined);

	////Left Leg
	//BoneCombined = m_pModelCom->GetBoneTransform(27);
	//BoneCombined._41 *= 0.045f;
	//BoneCombined._42 *= 0.045f;
	//BoneCombined._43 *= 0.045f;
	//Mat = XMLoadFloat4x4(&BoneCombined);

	//Mat = XMMatrixMultiply(Mat, Rot);
	//XMStoreFloat4x4(&BoneCombined, Mat);
	//m_pGameInstance->SetColliderTransform_Left_Leg(BoneCombined);

	////Left Shin
	//BoneCombined = m_pModelCom->GetBoneTransform(28);
	//BoneCombined._41 *= 0.045f;
	//BoneCombined._42 *= 0.045f;
	//BoneCombined._43 *= 0.045f;
	//Mat = XMLoadFloat4x4(&BoneCombined);

	//Mat = XMMatrixMultiply(Mat, Rot);
	//XMStoreFloat4x4(&BoneCombined, Mat);
	//m_pGameInstance->SetColliderTransform_Left_Shin(BoneCombined);

	////Right Leg
	//BoneCombined = m_pModelCom->GetBoneTransform(41);
	//BoneCombined._41 *= 0.045f;
	//BoneCombined._42 *= 0.045f;
	//BoneCombined._43 *= 0.045f;
	//Mat = XMLoadFloat4x4(&BoneCombined);

	//Mat = XMMatrixMultiply(Mat, Rot);
	//XMStoreFloat4x4(&BoneCombined, Mat);
	//m_pGameInstance->SetColliderTransform_Right_Leg(BoneCombined);

	////Right Shin
	//BoneCombined = m_pModelCom->GetBoneTransform(42);
	//BoneCombined._41 *= 0.045f;
	//BoneCombined._42 *= 0.045f;
	//BoneCombined._43 *= 0.045f;
	//Mat = XMLoadFloat4x4(&BoneCombined);

	//Mat = XMMatrixMultiply(Mat, Rot);
	//XMStoreFloat4x4(&BoneCombined, Mat);

	//m_pGameInstance->SetColliderTransform_Right_Shin(BoneCombined);
#pragma endregion

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_DIR, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_POINT, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);

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

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i))))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_PrevBoneMatrices(m_pShaderCom, "g_PrevBoneMatrices", static_cast<_uint>(i))))
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

	//vector<string>					BoneTags = { m_pModelCom->Get_BoneNames() };
	//map<string, _float4x4>			CombinedMatrices;
	//for (auto& strBoneTag : BoneTags)
	//{
	//	_float4x4					CombinedMatrix = { *m_pModelCom->Get_CombinedMatrix(strBoneTag) };
	//	CombinedMatrices[strBoneTag] = CombinedMatrix;
	//}

	//_matrix							WorldMatrix = { XMLoadFloat4x4(&m_WorldMatrix) };
	//_matrix							ViewMatrix = { m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW) };
	//_matrix							ProjMatrix = { m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ) };
	//_matrix							WVPMatrix = { WorldMatrix * ViewMatrix * ProjMatrix };

	//for (auto& Pair : CombinedMatrices)
	//{
	//	string			strBoneTag = { Pair.first };
	//	_matrix			CombinedMatrix = { XMLoadFloat4x4(&Pair.second) };
	//	_matrix			ProjSpaceCombinedMatrix = { CombinedMatrix * WVPMatrix };

	//	//	-WinSizeX * 0.5f ~ WinSizeX * 0.5f;

	//	_vector			vProjSpacePosition = {ProjSpaceCombinedMatrix.r[CTransform::STATE_POSITION]};
	//	vProjSpacePosition = { XMVectorSet(
	//		XMVectorGetX(vProjSpacePosition) / XMVectorGetW(vProjSpacePosition),
	//		XMVectorGetY(vProjSpacePosition) / XMVectorGetW(vProjSpacePosition),
	//		XMVectorGetZ(vProjSpacePosition) / XMVectorGetW(vProjSpacePosition),
	//		XMVectorGetW(vProjSpacePosition) / XMVectorGetW(vProjSpacePosition)) };

	//	_vector			vScreenSpacePosition = {
	//		XMVectorGetX(vProjSpacePosition) * static_cast<_float>(g_iWinSizeX) * 0.5f + static_cast<_float>(g_iWinSizeX * 0.5f),
	//		(XMVectorGetY(vProjSpacePosition) * static_cast<_float>(g_iWinSizeY) * 0.5f - static_cast<_float>(g_iWinSizeY * 0.5f)) * -1.f,
	//		0.f, 0.f
	//	};

	//	_float2			vScreenSpacePositionFloat2 = {};
	//	XMStoreFloat2(&vScreenSpacePositionFloat2, vScreenSpacePosition);

	//	_tchar			szTemp[MAX_PATH] = { L"" };
	//	MultiByteToWideChar(CP_ACP, 0, strBoneTag.c_str(), (_uint)strlen(strBoneTag.c_str()), szTemp, MAX_PATH);
	//	wstring			wstrBoneTag = { szTemp };
	//	m_pGameInstance->Render_Font(TEXT("Font_Default"), wstrBoneTag, vScreenSpacePositionFloat2, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f);
	//}

	m_PartColliders[4]->Active_Color(true);
	m_PartColliders[5]->Active_Color(true);
	m_PartColliders[6]->Active_Color(true);
	m_PartColliders[7]->Active_Color(true);

	m_PartColliders[4]->Set_Color(_float4(0.f, 0.f, 1.f, 1.f));
	m_PartColliders[5]->Set_Color(_float4(0.f, 0.f, 1.f, 1.f));
	m_PartColliders[6]->Set_Color(_float4(0.f, 0.f, 1.f, 1.f));
	m_PartColliders[7]->Set_Color(_float4(0.f, 0.f, 1.f, 1.f));

	m_PartColliders[8]->Active_Color(true);
	m_PartColliders[9]->Active_Color(true);
	m_PartColliders[10]->Active_Color(true);
	m_PartColliders[11]->Active_Color(true);

	m_PartColliders[8]->Set_Color(_float4(1.f, 0.f, 0.f, 1.f));
	m_PartColliders[9]->Set_Color(_float4(1.f, 0.f, 0.f, 1.f));
	m_PartColliders[10]->Set_Color(_float4(1.f, 0.f, 0.f, 1.f));
	m_PartColliders[11]->Set_Color(_float4(1.f, 0.f, 0.f, 1.f));


	m_PartColliders[4 + 12]->Active_Color(true);
	m_PartColliders[5 + 12]->Active_Color(true);
	m_PartColliders[6 + 12]->Active_Color(true);
	m_PartColliders[7 + 12]->Active_Color(true);

	m_PartColliders[4 + 12]->Set_Color(_float4(0.f, 0.f, 1.f, 1.f));
	m_PartColliders[5 + 12]->Set_Color(_float4(0.f, 0.f, 1.f, 1.f));
	m_PartColliders[6 + 12]->Set_Color(_float4(0.f, 0.f, 1.f, 1.f));
	m_PartColliders[7 + 12]->Set_Color(_float4(0.f, 0.f, 1.f, 1.f));

	m_PartColliders[8 + 12]->Active_Color(true);
	m_PartColliders[9 + 12]->Active_Color(true);
	m_PartColliders[10 + 12]->Active_Color(true);
	m_PartColliders[11 + 12]->Active_Color(true);

	m_PartColliders[8 + 12]->Set_Color(_float4(1.f, 0.f, 0.f, 1.f));
	m_PartColliders[9 + 12]->Set_Color(_float4(1.f, 0.f, 0.f, 1.f));
	m_PartColliders[10 + 12]->Set_Color(_float4(1.f, 0.f, 0.f, 1.f));
	m_PartColliders[11 + 12]->Set_Color(_float4(1.f, 0.f, 0.f, 1.f));

	for (_uint i = 0; i < 24; ++i)
		m_PartColliders[i]->Render();

	/*for (auto& pPartCollider : m_PartColliders)
	{
		pPartCollider->Render();
	}*/

#endif

	return S_OK;
}

HRESULT CBody_Player::Render_LightDepth_Dir()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (m_bRagdoll)
	{
		auto WorldMat = m_pParentsTransform->Get_WorldFloat4x4();
		WorldMat._41 = 0.f;
		WorldMat._42 = 0.f;
		WorldMat._43 = 0.f;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &WorldMat)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
			return E_FAIL;
	}

	if (m_pGameInstance->Get_ShadowLight(CPipeLine::DIRECTION) != nullptr) {

		const CLight* pLight = m_pGameInstance->Get_ShadowLight(CPipeLine::DIRECTION);
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

HRESULT CBody_Player::Render_LightDepth_Spot()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (m_bRagdoll)
	{
		auto WorldMat = m_pParentsTransform->Get_WorldFloat4x4();
		WorldMat._41 = 0.f;
		WorldMat._42 = 0.f;
		WorldMat._43 = 0.f;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &WorldMat)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
			return E_FAIL;
	}

	if (m_pGameInstance->Get_ShadowLight(CPipeLine::SPOT) != nullptr) {

		const CLight* pLight = m_pGameInstance->Get_ShadowLight(CPipeLine::SPOT);
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

			if (FAILED(m_pShaderCom->Begin(3)))
				return E_FAIL;

			m_pModelCom->Render(static_cast<_uint>(i));
		}
	}

	Update_WorldMatrix();

	return S_OK;
}

HRESULT CBody_Player::Render_LightDepth_Point()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (m_bRagdoll)
	{
		auto WorldMat = m_pParentsTransform->Get_WorldFloat4x4();
		WorldMat._41 = 0.f;
		WorldMat._42 = 0.f;
		WorldMat._43 = 0.f;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &WorldMat)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
			return E_FAIL;
	}

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

			if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i))))
				return E_FAIL;

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

	ColliderDesc.fRadius = 0.5f;
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.fRadius, 0.f);


	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	
	for (_uint i = 0; i < 100; ++i)
	{
		CBounding_OBB::BOUNDING_OBB_DESC		ColliderOBBDesc{};
		ColliderOBBDesc.vCenter = { 0.f, 0.f, 0.f };
		ColliderOBBDesc.vRotation = { 0.f, 0.f, 0.f };
		ColliderOBBDesc.vSize = { 10.f, 10.f, 10.f };

		CComponent*			pCollider = { m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"), &ColliderOBBDesc) };
		if (nullptr == pCollider)
			return E_FAIL;

		m_PartColliders.push_back(dynamic_cast<CCollider*>(pCollider));
	}


	return S_OK;
}

HRESULT CBody_Player::Bind_ShaderResources()
{
	if (m_bRagdoll)
	{
		auto WorldMat = m_pParentsTransform->Get_WorldFloat4x4();
		WorldMat._41 = 0.f;
		WorldMat._42 = 0.f;
		WorldMat._43 = 0.f;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &WorldMat)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
			return E_FAIL;
	}

	_bool isMotionBlur = m_pGameInstance->Get_ShaderState(MOTION_BLUR);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_isMotionBlur", &isMotionBlur, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevWorldMatrix", &m_PrevWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevViewMatrix", &m_pGameInstance->Get_PrevTransform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;									
	if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevProjMatrix", &m_pGameInstance->Get_PrevTransform_Float4x4(CPipeLine::D3DTS_PROJ))))
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
