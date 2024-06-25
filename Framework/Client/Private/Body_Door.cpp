#include "stdafx.h"
#include "Body_Door.h"
#include "Player.h"
#include "Bone.h"
#include "PxCollider.h"


#include"Door.h"
CBody_Door::CBody_Door(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPart_InteractProps{ pDevice, pContext }
{
}

CBody_Door::CBody_Door(const CBody_Door& rhs)
	: CPart_InteractProps{ rhs }
{

}

HRESULT CBody_Door::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBody_Door::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;
	if (pArg != nullptr)
	{
		BODY_DOOR_DESC* pbody_door_desc = (BODY_DOOR_DESC*)pArg;
		m_pDoubleDoorType = pbody_door_desc->pDoubleDoorType;
		m_pDoubleState = pbody_door_desc->pDoubleDoorState; 
		m_pDoubleState_Prev =pbody_door_desc->pDoubleDoorState_Prev;
		m_pOneState =pbody_door_desc->pOneDoorState;
		m_pOneState_Prev =pbody_door_desc->pOneDoorState_Prev;


	}
	m_pModelCom->Set_RootBone("RootNode");
	m_pModelCom->Add_Bone_Layer_All_Bone(TEXT("Default"));

	m_pModelCom->Add_AnimPlayingInfo(false, 0, TEXT("Default"), 1.f);
	m_pModelCom->Set_TotalLinearInterpolation(0.2f);

	/*
	if (m_eType == DOOR_DOUBLE)
	{
		m_pModelCom->Add_AnimPlayingInfo(1, false, 0, TEXT("Default"), 0.5f);
		m_pModelCom->Add_AnimPlayingInfo(1, false, 1, TEXT("Default"), 0.5f);
	}
	*/

	m_pModelCom->Active_RootMotion_Rotation(true);
	//m_pParentsTransform->Set_WorldMatrix(m_tagPropDesc.worldMatrix);

#ifndef NON_COLLISION_PROP

	if (*m_pState == CDoor::DOOR_DOUBLE)
	{
		if (*m_pDoubleDoorType == DOUBLE_DOOR_MODEL_TYPE::FRONT_DOOR)
		{
			m_pPx_Collider = m_pGameInstance->Create_Px_Collider_Convert_Root_Double_Door(m_pModelCom, m_pParentsTransform, &m_iPx_Collider_Id);

			m_vecRotationBone[ATC_ROOT] = m_pModelCom->Get_BonePtr("_00");
			m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_L_SIDE_L] = m_pModelCom->Get_BonePtr("_01");
			m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_L_SIDE_R] = m_pModelCom->Get_BonePtr("_03");
			m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_R_SIDE_L] = m_pModelCom->Get_BonePtr("_04");
			m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_R_SIDE_R] = m_pModelCom->Get_BonePtr("_02");
		}
		else
		{
			m_pPx_Collider = m_pGameInstance->Create_Px_Collider_Convert_Root(m_pModelCom, m_pParentsTransform, &m_iPx_Collider_Id);

			switch (*m_pState)
			{
			case CDoor::DOOR_DOUBLE:
				m_vecRotationBone[ATC_ROOT] = m_pModelCom->Get_BonePtr("_00");
				m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_L_SIDE_L] = m_pModelCom->Get_BonePtr("_01");
				m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_L_SIDE_R] = m_pModelCom->Get_BonePtr("_03");
				m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_R_SIDE_L] = m_pModelCom->Get_BonePtr("_04");
				m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_R_SIDE_R] = m_pModelCom->Get_BonePtr("_02");
				break;
			case CDoor::DOOR_ONE:
				m_vecRotationBone[ATC_SINGLE_DOOR_OPEN_L] = m_pModelCom->Get_BonePtr("_01");
				m_vecRotationBone[ATC_SINGLE_DOOR_OPEN_R] = m_pModelCom->Get_BonePtr("_00");
				break;
			}
		}
	}
	else
	{
		m_pPx_Collider = m_pGameInstance->Create_Px_Collider(m_pModelCom, m_pParentsTransform, &m_iPx_Collider_Id);

		m_vecRotationBone[ATC_SINGLE_DOOR_OPEN_L] = m_pModelCom->Get_BonePtr("_01");
		m_vecRotationBone[ATC_SINGLE_DOOR_OPEN_R] = m_pModelCom->Get_BonePtr("_00");
	}

#endif

	return S_OK;
}

void CBody_Door::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	*m_pState == CDoor::DOOR_ONE ? OneDoor_Tick(fTimeDelta) : DoubleDoor_Tick(fTimeDelta);

}

void CBody_Door::Late_Tick(_float fTimeDelta)
{

	*m_pState == CDoor::DOOR_ONE ? OneDoor_Late_Tick(fTimeDelta) : DoubleDoor_Late_Tick(fTimeDelta);


}

HRESULT CBody_Door::Render()
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

HRESULT CBody_Door::Add_Components()
{

	/* For.Com_Body_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Body_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Body_Model */
	if (FAILED(__super::Add_Component(g_Level, m_strModelComponentName,
		TEXT("Com_Body_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Door::Add_PartObjects()
{
	return S_OK;
}

HRESULT CBody_Door::Initialize_PartObjects()
{
	return S_OK;
}

void CBody_Door::DoubleDoor_Tick(_float fTimeDelta)
{
}

void CBody_Door::DoubleDoor_Late_Tick(_float fTimeDelta)
{
	if (*m_pDoubleDoorType == DOUBLE_DOOR_MODEL_TYPE::FRONT_DOOR)
	{
		switch (*m_pDoubleState)
		{
		case CDoor::LSIDE_DOUBLEDOOR_OPEN_L:
		{
			m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pDoubleState);

			auto Combined = m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_L_SIDE_L]->Get_TrasformationMatrix();
			_float4x4 ResultMat;
			XMStoreFloat4x4(&ResultMat, Combined);
			m_pPx_Collider->Update_Transform_Divided(&ResultMat, DOUBLE_DOOR_MESH_TYPE::DOOR_R);
			break;
		}
		case CDoor::LSIDE_DOUBLEDOOR_OPEN_R:
		{
			m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pDoubleState);

			auto Combined = m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_L_SIDE_R]->Get_TrasformationMatrix();
			_float4x4 ResultMat;
			XMStoreFloat4x4(&ResultMat, Combined);
			m_pPx_Collider->Update_Transform_Divided(&ResultMat, DOUBLE_DOOR_MESH_TYPE::DOOR_R);

			break;
		}
		case CDoor::RSIDE_DOUBLEDOOR_OPEN_L:
		{
			m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pDoubleState);

			auto Combined = m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_R_SIDE_L]->Get_TrasformationMatrix();
			_float4x4 ResultMat;
			XMStoreFloat4x4(&ResultMat, Combined);
			m_pPx_Collider->Update_Transform_Divided(&ResultMat, DOUBLE_DOOR_MESH_TYPE::DOOR_L);
			break;
		}
		case CDoor::RSIDE_DOUBLEDOOR_OPEN_R:
		{
			m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pDoubleState);

			auto Combined = m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_R_SIDE_R]->Get_TrasformationMatrix();
			_float4x4 ResultMat;
			XMStoreFloat4x4(&ResultMat, Combined);
			m_pPx_Collider->Update_Transform_Divided(&ResultMat, DOUBLE_DOOR_MESH_TYPE::DOOR_L);

			break;
		}
		case CDoor::DOUBLEDOOR_STATIC:
		{
			m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pDoubleState);

			switch (*m_pDoubleState_Prev)
			{
			case CDoor::LSIDE_DOUBLEDOOR_OPEN_L:
			{
				auto Combined = m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_L_SIDE_L]->Get_TrasformationMatrix();
				_float4x4 ResultMat;
				XMStoreFloat4x4(&ResultMat, Combined);
				m_pPx_Collider->Update_Transform_Divided(&ResultMat, DOUBLE_DOOR_MESH_TYPE::DOOR_R);
				break;
			}
			case CDoor::LSIDE_DOUBLEDOOR_OPEN_R:
			{
				auto Combined = m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_L_SIDE_R]->Get_TrasformationMatrix();
				_float4x4 ResultMat;
				XMStoreFloat4x4(&ResultMat, Combined);
				m_pPx_Collider->Update_Transform_Divided(&ResultMat, DOUBLE_DOOR_MESH_TYPE::DOOR_R);
				break;
			}
			case CDoor::RSIDE_DOUBLEDOOR_OPEN_L:
			{
				auto Combined = m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_R_SIDE_L]->Get_TrasformationMatrix();
				_float4x4 ResultMat;
				XMStoreFloat4x4(&ResultMat, Combined);
				m_pPx_Collider->Update_Transform_Divided(&ResultMat, DOUBLE_DOOR_MESH_TYPE::DOOR_L);
				break;
			}
			case CDoor::RSIDE_DOUBLEDOOR_OPEN_R:
			{
				auto Combined = m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_R_SIDE_R]->Get_TrasformationMatrix();
				_float4x4 ResultMat;
				XMStoreFloat4x4(&ResultMat, Combined);
				m_pPx_Collider->Update_Transform_Divided(&ResultMat, DOUBLE_DOOR_MESH_TYPE::DOOR_L);

				break;
			}
			}
			break;
		}
		}
	}
	else
	{
		switch (*m_pDoubleState)
		{
		case CDoor::LSIDE_DOUBLEDOOR_OPEN_L:
		{
			m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pDoubleState);

			auto Combined = m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_L_SIDE_L]->Get_TrasformationMatrix();
			_float4x4 ResultMat;
			XMStoreFloat4x4(&ResultMat, Combined);
			m_pPx_Collider->Update_Transform_Divided(&ResultMat, DOUBLE_DOOR_MESH_TYPE::DOOR_L);
			break;
		}
		case CDoor::LSIDE_DOUBLEDOOR_OPEN_R:
		{
			m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pDoubleState);

			auto Combined = m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_L_SIDE_R]->Get_TrasformationMatrix();
			_float4x4 ResultMat;
			XMStoreFloat4x4(&ResultMat, Combined);
			m_pPx_Collider->Update_Transform_Divided(&ResultMat, DOUBLE_DOOR_MESH_TYPE::DOOR_L);

			break;
		}
		case CDoor::RSIDE_DOUBLEDOOR_OPEN_L:
		{
			m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pDoubleState);

			auto Combined = m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_R_SIDE_L]->Get_TrasformationMatrix();
			_float4x4 ResultMat;
			XMStoreFloat4x4(&ResultMat, Combined);
			m_pPx_Collider->Update_Transform_Divided(&ResultMat, DOUBLE_DOOR_MESH_TYPE::DOOR_R);
			break;
		}
		case CDoor::RSIDE_DOUBLEDOOR_OPEN_R:
		{

			m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pDoubleState);

			auto Combined = m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_R_SIDE_R]->Get_TrasformationMatrix();
			_float4x4 ResultMat;
			XMStoreFloat4x4(&ResultMat, Combined);
			m_pPx_Collider->Update_Transform_Divided(&ResultMat, DOUBLE_DOOR_MESH_TYPE::DOOR_R);

			break;
		}
		case CDoor::DOUBLEDOOR_STATIC:
		{
			m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pDoubleState);

			switch (*m_pDoubleState_Prev)
			{
			case CDoor::LSIDE_DOUBLEDOOR_OPEN_L:
			{
				auto Combined = m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_L_SIDE_L]->Get_TrasformationMatrix();
				_float4x4 ResultMat;
				XMStoreFloat4x4(&ResultMat, Combined);
				m_pPx_Collider->Update_Transform_Divided(&ResultMat, DOUBLE_DOOR_MESH_TYPE::DOOR_L);
				break;
			}
			case CDoor::LSIDE_DOUBLEDOOR_OPEN_R:
			{
				auto Combined = m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_L_SIDE_R]->Get_TrasformationMatrix();
				_float4x4 ResultMat;
				XMStoreFloat4x4(&ResultMat, Combined);
				m_pPx_Collider->Update_Transform_Divided(&ResultMat, DOUBLE_DOOR_MESH_TYPE::DOOR_L);
				break;
			}
			case CDoor::RSIDE_DOUBLEDOOR_OPEN_L:
			{
				auto Combined = m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_R_SIDE_L]->Get_TrasformationMatrix();
				_float4x4 ResultMat;
				XMStoreFloat4x4(&ResultMat, Combined);
				m_pPx_Collider->Update_Transform_Divided(&ResultMat, DOUBLE_DOOR_MESH_TYPE::DOOR_R);
				break;
			}
			case CDoor::RSIDE_DOUBLEDOOR_OPEN_R:
			{
				auto Combined = m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_R_SIDE_R]->Get_TrasformationMatrix();
				_float4x4 ResultMat;
				XMStoreFloat4x4(&ResultMat, Combined);
				m_pPx_Collider->Update_Transform_Divided(&ResultMat, DOUBLE_DOOR_MESH_TYPE::DOOR_R);

				break;
			}
			}
			break;
		}
		}
	}

	_float4 fTransform4 = m_pParentsTransform->Get_State_Float4(CTransform::STATE_POSITION);
	_float3 fTransform3 = _float3{ fTransform4.x,fTransform4.y,fTransform4.z };
	m_pModelCom->Play_Animation_Light(m_pParentsTransform, fTimeDelta);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_FIELD_SHADOW_POINT, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_FIELD_SHADOW_DIR, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);

}

void CBody_Door::OneDoor_Tick(_float fTimeDelta)
{

}

void CBody_Door::OneDoor_Late_Tick(_float fTimeDelta)
{

	switch (*m_pOneState)
	{
	case CDoor::ONEDOOR_OPEN_L:
	{
		m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pOneState);

		//auto Combined = XMLoadFloat4x4(m_pRotationBone->Get_CombinedTransformationMatrix());
		auto Combined = m_vecRotationBone[ATC_SINGLE_DOOR_OPEN_L]->Get_TrasformationMatrix();
		//Combined = Combined * m_pParentsTransform->Get_WorldMatrix();
		_float4x4 ResultMat;
		XMStoreFloat4x4(&ResultMat, Combined);
		m_pPx_Collider->Update_Transform(&ResultMat);
		break;
	}
	case CDoor::ONEDOOR_OPEN_R:
	{
		m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pOneState);
	}
	case CDoor::ONEDOOR_STATIC:
		m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pOneState);

		switch (*m_pOneState_Prev)
		{
		case CDoor::ONEDOOR_OPEN_L:
		{
			auto Combined = m_vecRotationBone[ATC_SINGLE_DOOR_OPEN_L]->Get_TrasformationMatrix();
			_float4x4 ResultMat;
			XMStoreFloat4x4(&ResultMat, Combined);
			m_pPx_Collider->Update_Transform(&ResultMat);
			break;
		}
		case CDoor::ONEDOOR_OPEN_R:
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

	_float4 fTransform4 = m_pParentsTransform->Get_State_Float4(CTransform::STATE_POSITION);
	_float3 fTransform3 = _float3{ fTransform4.x,fTransform4.y,fTransform4.z };
	m_pModelCom->Play_Animation_Light(m_pParentsTransform, fTimeDelta);



	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_FIELD_SHADOW_POINT, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_FIELD_SHADOW_DIR, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);

}

CBody_Door* CBody_Door::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Door* pInstance = new CBody_Door(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBody_Door"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CBody_Door::Clone(void* pArg)
{
	CBody_Door* pInstance = new CBody_Door(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CBody_Door"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Door::Free()
{
	__super::Free();
}
