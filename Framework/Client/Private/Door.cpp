#include "stdafx.h"
#include "Door.h"
#include "Player.h"
#include "Bone.h"
#include "PxCollider.h"

CDoor::CDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CInteractProps{ pDevice, pContext }
{
}

CDoor::CDoor(const CDoor& rhs)
	: CInteractProps{ rhs }
{

}

HRESULT CDoor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDoor::Initialize(void* pArg)
{
	/*문자식 파트오브젝트 붙혀야하는데 뼈가 문고리에 없어서 직접 찍어야 하는데
	프로토타입 끝나고 뼈 붙혀보겠나이다*/

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if ((m_tagPropDesc.strGamePrototypeName.find("007") != string::npos) || (m_tagPropDesc.strGamePrototypeName.find("038") != string::npos) || (m_tagPropDesc.strGamePrototypeName.find("113") != string::npos))
		m_eType = DOOR_DOUBLE;
	else
		m_eType = DOOR_ONE;

	if (FAILED(Add_Components()))
		return E_FAIL;


	m_pModelCom->Set_RootBone("RootNode");
	m_pModelCom->Add_Bone_Layer_All_Bone(TEXT("Default"));

	m_pModelCom->Add_AnimPlayingInfo(-1, false, 0, TEXT("Default"), 1.f);
	m_pModelCom->Set_TotalLinearInterpolation(0.2f);

	/*
	if (m_eType == DOOR_DOUBLE)
	{
		m_pModelCom->Add_AnimPlayingInfo(1, false, 0, TEXT("Default"), 0.5f);
		m_pModelCom->Add_AnimPlayingInfo(1, false, 1, TEXT("Default"), 0.5f);
	}
	*/

	m_pModelCom->Active_RootMotion_Rotation(true);
	m_pTransformCom->Set_WorldMatrix(m_tagPropDesc.worldMatrix);

#ifndef NON_COLLISION_PROP

	m_pPx_Collider = m_pGameInstance->Create_Px_Collider(m_pModelCom, m_pTransformCom, &m_iPx_Collider_Id);

#endif

	switch (m_eType)
	{
	case DOOR_DOUBLE:
		m_vecRotationBone[ATC_ROOT] = m_pModelCom->Get_BonePtr("_00");
		m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_L_SIDE_L] = m_pModelCom->Get_BonePtr("_01");
		m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_L_SIDE_R] = m_pModelCom->Get_BonePtr("_03");
		m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_R_SIDE_L] = m_pModelCom->Get_BonePtr("_04");
		m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_R_SIDE_R] = m_pModelCom->Get_BonePtr("_02");
		break;
	case DOOR_ONE:
		m_vecRotationBone[ATC_SINGLE_DOOR_OPEN_L] = m_pModelCom->Get_BonePtr("_01");
		m_vecRotationBone[ATC_SINGLE_DOOR_OPEN_R] = m_pModelCom->Get_BonePtr("_00");
		break;
	}
	
	return S_OK;
}

void CDoor::Tick(_float fTimeDelta)
{
	__super::Check_Player();
	if (!m_bVisible)
	{
		m_pColliderCom[INTERACTPROPS_COL_SPHERE]->Tick(m_pTransformCom->Get_WorldMatrix());
		if (m_eType == DOOR_DOUBLE)
			m_pColDoubledoorCom->Tick(m_pTransformCom->Get_WorldMatrix());
		return;
	}

	if (m_pPlayer == nullptr)
		return;
	if (m_bLock)
		return;
	m_eType == DOOR_ONE ? OneDoor_Tick(fTimeDelta) : DoubleDoor_Tick(fTimeDelta);
}

void CDoor::Late_Tick(_float fTimeDelta)
{
	if (!Visible())
		return;


	if (m_bRender == false)
		return;
	m_eType == DOOR_ONE ? OneDoor_Late_Tick(fTimeDelta) : DoubleDoor_Late_Tick(fTimeDelta);
}

HRESULT CDoor::Render()
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

		/*if (FAILED(m_pModelCom->Bind_PrevBoneMatrices(m_pShaderCom, "g_PrevBoneMatrices", static_cast<_uint>(i))))
			return E_FAIL;*/

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

HRESULT CDoor::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(g_Level, m_tagPropDesc.strModelComponent,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

	ColliderDesc.fRadius = _float(80.f);
	ColliderDesc.vCenter = _float3(-10.f, 1.f, 0.f);
	/* For.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom[INTERACTPROPS_COL_SPHERE], &ColliderDesc)))
		return E_FAIL;
	if (m_eType == DOOR_DOUBLE)
	{
		CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc1{};

		ColliderDesc1.fRadius = _float(80.f);
		ColliderDesc1.vCenter = _float3(-180.f, 1.f, 0.f);
		/* For.Com_Collider */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			TEXT("Com_Collider_Double"), (CComponent**)&m_pColDoubledoorCom, &ColliderDesc1)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CDoor::Add_PartObjects()
{

	return S_OK;
}

HRESULT CDoor::Initialize_PartObjects()
{

	return S_OK;
}

HRESULT CDoor::Bind_ShaderResources()
{

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	//_bool isMotionBlur = m_pGameInstance->Get_ShaderState(MOTION_BLUR);
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_isMotionBlur", &isMotionBlur, sizeof(_bool))))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevWorldMatrix", &m_pTransformCom->Get_WorldFloat4x4())))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevViewMatrix", &m_pGameInstance->Get_PrevTransform_Float4x4(CPipeLine::D3DTS_VIEW))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevProjMatrix", &m_pGameInstance->Get_PrevTransform_Float4x4(CPipeLine::D3DTS_PROJ))))
	//	return E_FAIL;


	return S_OK;
}

void CDoor::DoubleDoor_Tick(_float fTimeDelta)
{

	if (m_bActive)
		m_fTime += fTimeDelta;

	if (m_fTime > 4.f)
	{
		m_fTime = 0.f;
		m_bActive = false;
		m_eDoubleState = DOUBLEDOOR_STATIC;
	}

	if ((m_bCol||m_bDoubleCol) && !m_bActive)
	{
		//UI띄우고
		if (*m_pPlayerInteract)
			DoubleDoor_Active();
		m_bCol = false;
		m_bDoubleCol = false;
	}

	m_pColliderCom[INTERACTPROPS_COL_SPHERE]->Tick(m_pTransformCom->Get_WorldMatrix());
	m_pColDoubledoorCom->Tick(m_pTransformCom->Get_WorldMatrix());

}

void CDoor::DoubleDoor_Late_Tick(_float fTimeDelta)
{
	switch (m_eDoubleState)
	{
	case LSIDE_DOUBLEDOOR_OPEN_L:
	{
		//m_pModelCom->Set_TotalLinearInterpolation(0.2f); // 잘알아갑니다 꺼억
		m_pModelCom->Change_Animation(0, m_eDoubleState);

		auto Combined = m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_L_SIDE_L]->Get_TrasformationMatrix();
		_float4x4 ResultMat;
		XMStoreFloat4x4(&ResultMat, Combined);
		m_pPx_Collider->Update_Transform_Divided(&ResultMat, DOUBLE_DOOR_MESH_TYPE::DOOR_L);
		break;
	}
	case LSIDE_DOUBLEDOOR_OPEN_R:
	{
		m_pModelCom->Change_Animation(0, m_eDoubleState);

		auto Combined = m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_L_SIDE_R]->Get_TrasformationMatrix();
		_float4x4 ResultMat;
		XMStoreFloat4x4(&ResultMat, Combined);
		m_pPx_Collider->Update_Transform_Divided(&ResultMat, DOUBLE_DOOR_MESH_TYPE::DOOR_L);
	
		break;
	}
	case RSIDE_DOUBLEDOOR_OPEN_L:
	{
		m_pModelCom->Change_Animation(0, m_eDoubleState);

		auto Combined = m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_R_SIDE_L]->Get_TrasformationMatrix();
		_float4x4 ResultMat;
		XMStoreFloat4x4(&ResultMat, Combined);
		m_pPx_Collider->Update_Transform_Divided(&ResultMat, DOUBLE_DOOR_MESH_TYPE::DOOR_R);
		break;
	}
	case RSIDE_DOUBLEDOOR_OPEN_R:
	{
		m_pModelCom->Change_Animation(0, m_eDoubleState);

		auto Combined = m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_R_SIDE_R]->Get_TrasformationMatrix();
		_float4x4 ResultMat;
		XMStoreFloat4x4(&ResultMat, Combined);
		m_pPx_Collider->Update_Transform_Divided(&ResultMat, DOUBLE_DOOR_MESH_TYPE::DOOR_R);

		break;
	}
	case DOUBLEDOOR_STATIC:
	{
		m_pModelCom->Change_Animation(0, m_eDoubleState);
		break;
	}
	}
	_float4 fTransform4 = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
	_float3 fTransform3 = _float3{ fTransform4.x,fTransform4.y,fTransform4.z };
	m_pModelCom->Play_Animation_Light(m_pTransformCom, fTimeDelta);
	//		m_pModelCom->Play_Animations(m_pTransformCom, fTimeDelta, &fTransform3);

	Check_Col_Sphere_Player(); // 여긴 m_bCol 을 true로만 바꿔주기 때문에 반드시 false를 해주는 부분이 있어야함
	
	CCollider* pPlayerCol = static_cast<CCollider*>(m_pPlayer->Get_Component(TEXT("Com_Collider")));
	if (pPlayerCol->Intersect(m_pColDoubledoorCom))
		m_bDoubleCol = true;
	



	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_FIELD_SHADOW_POINT, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_FIELD_SHADOW_DIR, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponents(m_pColliderCom[INTERACTPROPS_COL_SPHERE]);

	m_pGameInstance->Add_DebugComponents(m_pColDoubledoorCom);
	
#endif
}

void CDoor::DoubleDoor_Active()
{

	*m_pPlayerInteract = false;
	m_bActive = true;
	_vector vLook = XMVector4Normalize(m_pTransformCom->Get_State_Vector(CTransform::STATE_LOOK));
	_vector vDir = XMVector4Normalize(m_pPlayerTransform->Get_State_Vector(CTransform::STATE_POSITION) - m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION));
	_float fScala = XMVectorGetX(XMVector4Dot(vLook, vDir));
	if (fScala > 1.f)
		fScala = 1.f;
	else if (fScala < -1.f)
		fScala = -1.f;

	if (XMConvertToDegrees(acosf(fScala)) <= 90.f)
	{
		if (m_bCol && m_bDoubleCol)
			m_eDoubleState = LSIDE_DOUBLEDOOR_OPEN;
		else if (m_bCol)
			m_eDoubleState = LSIDE_DOUBLEDOOR_OPEN_L;
		else
			m_eDoubleState = RSIDE_DOUBLEDOOR_OPEN_R;

	}
	else
	{
		if (m_bCol && m_bDoubleCol)
			m_eDoubleState = RSIDE_DOUBLEDOOR_OPEN;
		else if (m_bCol)
			m_eDoubleState = LSIDE_DOUBLEDOOR_OPEN_R;
		else
			m_eDoubleState = RSIDE_DOUBLEDOOR_OPEN_L;
	}
	

}

void CDoor::OneDoor_Tick(_float fTimeDelta)
{

	if (m_bActive)
		m_fTime += fTimeDelta;

	if (m_fTime > 4.f)
	{
		m_fTime = 0.f;
		m_bActive = false;
		m_eOneState = ONEDOOR_STATIC;
	}

	if (m_bCol && !m_bActive)
	{
		//UI띄우고
		if (*m_pPlayerInteract)
			OneDoor_Active();
		m_bCol = false;
	}
	m_pColliderCom[INTERACTPROPS_COL_SPHERE]->Tick(m_pTransformCom->Get_WorldMatrix());

}

void CDoor::OneDoor_Late_Tick(_float fTimeDelta)
{
	Check_Col_Sphere_Player(); // 여긴 m_bCol 을 true로만 바꿔주기 때문에 반드시 false를 해주는 부분이 있어야함

	switch (m_eOneState)
	{
	case ONEDOOR_OPEN_L:
	{
		m_eOneState_Prev = m_eOneState;
		//m_pModelCom->Set_TotalLinearInterpolation(0.2f); // 잘알아갑니다 꺼억
		m_pModelCom->Change_Animation(0, m_eOneState);

		//auto Combined = XMLoadFloat4x4(m_pRotationBone->Get_CombinedTransformationMatrix());
		auto Combined = m_vecRotationBone[ATC_SINGLE_DOOR_OPEN_L]->Get_TrasformationMatrix();
		//Combined = Combined * m_pTransformCom->Get_WorldMatrix();
		_float4x4 ResultMat;
		XMStoreFloat4x4(&ResultMat, Combined);
		m_pPx_Collider->Update_Transform(&ResultMat);
		break;
	}
	case ONEDOOR_OPEN_R:
	{
		m_eOneState_Prev = m_eOneState;
		m_pModelCom->Change_Animation(0, m_eOneState);
	}
	case ONEDOOR_STATIC:
		m_pModelCom->Change_Animation(0, m_eOneState);

		switch (m_eOneState_Prev)
		{
		case ONEDOOR_OPEN_L:
		{
			auto Combined = m_vecRotationBone[ATC_SINGLE_DOOR_OPEN_L]->Get_TrasformationMatrix();
			_float4x4 ResultMat;
			XMStoreFloat4x4(&ResultMat, Combined);
			m_pPx_Collider->Update_Transform(&ResultMat);
			break;
		}
		case ONEDOOR_OPEN_R:
		{
			auto Combined = m_vecRotationBone[ATC_SINGLE_DOOR_OPEN_R]->Get_TrasformationMatrix();
			_float4x4 ResultMat;
			XMStoreFloat4x4(&ResultMat, Combined);
			m_pPx_Collider->Update_Transform(&ResultMat);
			break;
		}
		}
		
		break;
	}

	_float4 fTransform4 = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
	_float3 fTransform3 = _float3{ fTransform4.x,fTransform4.y,fTransform4.z };
	m_pModelCom->Play_Animation_Light(m_pTransformCom, fTimeDelta);



	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_FIELD_SHADOW_POINT, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_FIELD_SHADOW_DIR, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponents(m_pColliderCom[INTERACTPROPS_COL_SPHERE]);
#endif
}

void CDoor::OneDoor_Active()
{
	*m_pPlayerInteract = false;
	m_bActive = true;
	_vector vLook = XMVector4Normalize(m_pTransformCom->Get_State_Vector(CTransform::STATE_LOOK));
	_vector vDir = XMVector4Normalize(m_pPlayerTransform->Get_State_Vector(CTransform::STATE_POSITION) - m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION));
	_float fScala = XMVectorGetX(XMVector4Dot(vLook, vDir));
	if (fScala > 1.f)
		fScala = 1.f;
	else if (fScala < -1.f)
		fScala = -1.f;

	if (XMConvertToDegrees(acosf(fScala)) <= 90.f)
		m_eOneState = ONEDOOR_OPEN_L;
	else
		m_eOneState = ONEDOOR_OPEN_R;




}


CDoor* CDoor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDoor* pInstance = new CDoor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CDoor"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CDoor::Clone(void* pArg)
{
	CDoor* pInstance = new CDoor(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CDoor"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDoor::Free()
{
	__super::Free();
	if(m_eType ==  DOOR_DOUBLE)
		Safe_Release(m_pColDoubledoorCom);
}
