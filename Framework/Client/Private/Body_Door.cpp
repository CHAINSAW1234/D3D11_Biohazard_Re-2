#include "stdafx.h"
#include "Body_Door.h"
#include "Player.h"
#include "Bone.h"
#include "PxCollider.h"

#include"Door.h"
#include"Light.h"

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

	if (*m_pState == CDoor::DOOR_ONE)
	{
		if (FAILED(Initialize_Model()))
			return E_FAIL;
	}

	if (pArg != nullptr)
	{
		BODY_DOOR_DESC* pbody_door_desc = (BODY_DOOR_DESC*)pArg;

		m_pDoubleDoorType = pbody_door_desc->pDoubleDoorType;

		m_pDoubleState = pbody_door_desc->pDoubleDoorState;

		m_pDoubleState_Prev = pbody_door_desc->pDoubleDoorState_Prev;

		m_pOneState = pbody_door_desc->pOneDoorState;

		m_pOneState_Prev = pbody_door_desc->pOneDoorState_Prev;

		m_isEmblem = pbody_door_desc->isEmblem;
		m_pDoorTexture = pbody_door_desc->pDoorTexture;
	}

	if (*m_pState != CDoor::DOOR_DUMMY)
	{
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
	}

	m_pModelCom->Active_RootMotion_Rotation(false);

	/* Hide Emblem */
	if(true == m_isEmblem)
	{
		if (FAILED(Model_Hide()))
			return E_FAIL;
	}
	
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
		else if (*m_pState == CDoor::DOOR_ONE)
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
	if (*m_pState == CDoor::DOOR_DUMMY)
		return;
	__super::Tick(fTimeDelta);


	*m_pState == CDoor::DOOR_ONE ? OneDoor_Tick(fTimeDelta) : DoubleDoor_Tick(fTimeDelta);

}

void CBody_Door::Late_Tick(_float fTimeDelta)
{
	if (*m_pState == CDoor::DOOR_DUMMY)
		return;

	*m_pState == CDoor::DOOR_ONE ? OneDoor_Late_Tick(fTimeDelta) : DoubleDoor_Late_Tick(fTimeDelta);
	Get_SpecialBone_Rotation();
#ifdef _DEBUG
#ifdef UI_POS
	m_pGameInstance->Add_DebugComponents(m_pColliderCom[Part_INTERACTPROPS_COL_SPHERE]);
#endif
#endif
}

HRESULT CBody_Door::Render()
{

	if (m_bRender == false)
		return S_OK;
	else
		m_bRender = false;


	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	if(true == m_isEmblem)
	{
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


			if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXANIMMODEL::PASS_ALPHABLEND)))
				return E_FAIL;


			m_pModelCom->Render(static_cast<_uint>(i));
		}
	}

	else
	{
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

		if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXANIMMODEL::PASS_ALPHABLEND)))
			return E_FAIL;

			m_pModelCom->Render(static_cast<_uint>(i));
		}

	}


	return S_OK;
}

HRESULT CBody_Door::Render_LightDepth_Dir()
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

			if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXANIMMODEL::PASS_LIGHTDEPTH)))
				return E_FAIL;

			m_pModelCom->Render(static_cast<_uint>(i));
		}
	}

	return S_OK;
}

HRESULT CBody_Door::Render_LightDepth_Point()
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

			if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXANIMMODEL::PASS_LIGHTDEPTH_CUBE)))
				return E_FAIL;

			m_pModelCom->Render(static_cast<_uint>(i));
		}

		++iIndex;
	}

	return S_OK;
}

HRESULT CBody_Door::Render_LightDepth_Spot()
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

			if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXANIMMODEL::PASS_LIGHTDEPTH)))
				return E_FAIL;

			m_pModelCom->Render(static_cast<_uint>(i));
		}
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

#ifdef _DEBUG
#ifdef UI_POS
	CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

	ColliderDesc.fRadius = _float(20.f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
	/* For.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Body_Collider"), (CComponent**)&m_pColliderCom[Part_INTERACTPROPS_COL_SPHERE], &ColliderDesc)))
		return E_FAIL;
#endif
#endif
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


HRESULT CBody_Door::Initialize_Model()
{
	vector<string>			MeshTags = { m_pModelCom->Get_MeshTags() };
	for (auto& strMeshTag : MeshTags)
	{
		if ((strMeshTag.find("Group_100_") != string::npos))
			m_strDoorPart[BODY_PART_L_RHANDLE] = strMeshTag;
		if ((strMeshTag.find("Group_101_") != string::npos))
			m_strDoorPart[BODY_PART_L_LHANDLE] = strMeshTag;

	}

	return S_OK;

}

HRESULT CBody_Door::Model_Hide()
{
	/* Set_Hide_Mesh */
	if (true == m_isEmblem)
	{
		vector<string>			MeshTags = { m_pModelCom->Get_MeshTags() };

		vector<string>			ResultMeshTags;
		for (auto& strMeshTag : MeshTags)
		{
			if ((strMeshTag.find("Group_1_") != string::npos))
				ResultMeshTags.push_back(strMeshTag);
		}
		for (auto& strMeshTag : MeshTags)
		{
			m_pModelCom->Hide_Mesh(strMeshTag, true);
		}
		for (auto& strMeshTag : ResultMeshTags)
		{
			m_pModelCom->Hide_Mesh(strMeshTag, false);
		}
	}

	return S_OK;
}

void CBody_Door::Get_SpecialBone_Rotation()
{
	_matrix Combined;
	if(CDoor::DOOR_DOUBLE == *m_pState)
	{
		if (*m_pDoubleState == CDoor::DOUBLEDOOR_STATIC)
			Combined = m_vecRotationBone[0]->Get_TrasformationMatrix();
		else
		{
			if (*m_pDoubleState == CDoor::L_DOUBLEDOOR_OPEN || *m_pDoubleState == CDoor::R_DOUBLEDOOR_OPEN)
				Combined = m_vecRotationBone[0]->Get_TrasformationMatrix();
			else
				Combined = m_vecRotationBone[(_int)(*m_pDoubleState)]->Get_TrasformationMatrix();
		}
	}
	else
		if(*m_pOneState == CDoor::ONEDOOR_STATIC)
			Combined = m_vecRotationBone[0]->Get_TrasformationMatrix();
		else
			Combined = m_vecRotationBone[(_int)(*m_pOneState)]->Get_TrasformationMatrix();

	_vector scale, rotation, translation;
	XMMatrixDecompose(&scale, &rotation, &translation, Combined);
	m_vRotation = rotation;

}

_float4 CBody_Door::Get_Pos(_int iArg)
{
	if (CDoor::DOOR_DOUBLE == *m_pState)
	{
		_matrix Local_Mesh_Matrix = m_pTransformCom->Get_WorldMatrix();
		_float4 vLocalPos;
		_matrix TransformationMatrix = m_pParentsTransform->Get_WorldMatrix();
		_matrix TranslationMatrix = XMMatrixIdentity();
		if (iArg == 0)
		{
			vLocalPos = _float4(94.7047043f, 110.019104f, -7.74451256f, 1.f);
			Local_Mesh_Matrix.r[3] -= _vector{ vLocalPos.x,-vLocalPos.y,vLocalPos.z };
		}
		else if(iArg == 1)
		{
			vLocalPos = _float4(94.7142487f, 110.010277f, 3.20764256f, 1.f);
			Local_Mesh_Matrix.r[3] -= _vector{ vLocalPos.x,-vLocalPos.y,vLocalPos.z };

		}
		else if(iArg == 2)
		{
			vLocalPos = _float4(104.7047043f, 110.019104f, -7.74451256f, 1.f);
			Local_Mesh_Matrix.r[3] += _vector{ vLocalPos.x,vLocalPos.y, -vLocalPos.z };
			TranslationMatrix.r[3] = _vector{ -vLocalPos.x*2.f, 0.f,  7.74451256f*2.f,1.f };
		}
		else if (iArg == 3)
		{
			vLocalPos = _float4(104.7047043f, 110.010277f, 3.20764256f, 1.f);
			Local_Mesh_Matrix.r[3] += _vector{ vLocalPos.x,vLocalPos.y, vLocalPos.z };
			TranslationMatrix.r[3] = _vector{ -vLocalPos.x*2.f, 0.f, -3.20764256f*2.f,1.f };
		}


		XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(m_vRotation);

		_float4x4 WorldMatrix = Local_Mesh_Matrix * rotationMatrix* TranslationMatrix  * TransformationMatrix ;
#ifdef _DEBUG
#ifdef UI_POS
		m_pColliderCom[Part_INTERACTPROPS_COL_SPHERE]->Tick(WorldMatrix);
#endif
#endif
		_float4 vPos = XMVectorSetW( WorldMatrix.Translation(),1.f);
		return vPos;
	}
	else
	{
		_float4 vLocalPos;
		if(iArg == 0)
			vLocalPos = m_pModelCom->Get_Mesh_Local_Pos(m_strDoorPart[BODY_PART_L_LHANDLE]);
		else
			vLocalPos = m_pModelCom->Get_Mesh_Local_Pos(m_strDoorPart[BODY_PART_L_RHANDLE]);

		_matrix Local_Mesh_Matrix = m_pTransformCom->Get_WorldMatrix();
		Local_Mesh_Matrix.r[3] -= _vector{ vLocalPos.x,-vLocalPos.y,vLocalPos.z };
		_matrix TransformationMatrix = m_pParentsTransform->Get_WorldMatrix();

		XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(m_vRotation);

		_float4x4 WorldMatrix = Local_Mesh_Matrix * rotationMatrix * TransformationMatrix;

		_float4 vPos = XMVectorSetW(WorldMatrix.Translation(), 1.f);
#ifdef _DEBUG
#ifdef UI_POS
		m_pColliderCom[Part_INTERACTPROPS_COL_SPHERE]->Tick(WorldMatrix);
#endif
#endif
		return vPos;
	}
	return _float4();
}

void CBody_Door::DoubleDoor_Tick(_float fTimeDelta)
{
}

void CBody_Door::DoubleDoor_Late_Tick(_float fTimeDelta)
{
	_int iRand = m_pGameInstance->GetRandom_Int(0, 1);
	_int iRandPlus = m_pGameInstance->GetRandom_Int(0, 3);

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
			if (*m_pDoorTexture == CDoor::WOOD)
				WoodSound_Double(iRand, iRandPlus);


			break;
		}
		case CDoor::LSIDE_DOUBLEDOOR_OPEN_R:
		{
			m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pDoubleState);

			auto Combined = m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_L_SIDE_R]->Get_TrasformationMatrix();
			_float4x4 ResultMat;
			XMStoreFloat4x4(&ResultMat, Combined);
			m_pPx_Collider->Update_Transform_Divided(&ResultMat, DOUBLE_DOOR_MESH_TYPE::DOOR_R);
			if (*m_pDoorTexture == CDoor::WOOD)
				WoodSound_Double(iRand, iRandPlus);
			break;
		}
		case CDoor::RSIDE_DOUBLEDOOR_OPEN_L:
		{
			m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pDoubleState);

			auto Combined = m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_R_SIDE_L]->Get_TrasformationMatrix();
			_float4x4 ResultMat;
			XMStoreFloat4x4(&ResultMat, Combined);
			m_pPx_Collider->Update_Transform_Divided(&ResultMat, DOUBLE_DOOR_MESH_TYPE::DOOR_L);
			if (*m_pDoorTexture == CDoor::WOOD)
				WoodSound_Double(iRand, iRandPlus);
			break;
		}
		case CDoor::RSIDE_DOUBLEDOOR_OPEN_R:
		{
			m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pDoubleState);

			auto Combined = m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_R_SIDE_R]->Get_TrasformationMatrix();
			_float4x4 ResultMat;
			XMStoreFloat4x4(&ResultMat, Combined);
			m_pPx_Collider->Update_Transform_Divided(&ResultMat, DOUBLE_DOOR_MESH_TYPE::DOOR_L);
			if (*m_pDoorTexture == CDoor::WOOD)
				WoodSound_Double(iRand, iRandPlus);

			break;
		}
		case CDoor::DOUBLEDOOR_STATIC:
		{
			m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pDoubleState);
			if (!m_pModelCom->isFinished(0))
				Change_Sound(TEXT("sound_Map_sm40_doubledoor_m_wood_normal2_7.mp3"), 0);
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
			case CDoor::L_DOUBLEDOOR_OPEN:
			{
				auto Combined = m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_L_SIDE_L]->Get_TrasformationMatrix();
				_float4x4 ResultMat;
				XMStoreFloat4x4(&ResultMat, Combined);
				m_pPx_Collider->Update_Transform_Divided(&ResultMat, DOUBLE_DOOR_MESH_TYPE::DOOR_R);
				m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pDoubleState);

				Combined = m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_R_SIDE_R]->Get_TrasformationMatrix();
				XMStoreFloat4x4(&ResultMat, Combined);
				m_pPx_Collider->Update_Transform_Divided(&ResultMat, DOUBLE_DOOR_MESH_TYPE::DOOR_L);

				break;
			}
			case CDoor::R_DOUBLEDOOR_OPEN:
			{
				auto Combined = m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_L_SIDE_R]->Get_TrasformationMatrix();
				_float4x4 ResultMat;
				XMStoreFloat4x4(&ResultMat, Combined);
				m_pPx_Collider->Update_Transform_Divided(&ResultMat, DOUBLE_DOOR_MESH_TYPE::DOOR_R);
				m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pDoubleState);

				Combined = m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_R_SIDE_L]->Get_TrasformationMatrix();
				XMStoreFloat4x4(&ResultMat, Combined);
				m_pPx_Collider->Update_Transform_Divided(&ResultMat, DOUBLE_DOOR_MESH_TYPE::DOOR_L);

				break;
			}
			}
			break;
		}
		case CDoor::L_DOUBLEDOOR_OPEN:
		{
			m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pDoubleState);

			auto Combined = m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_L_SIDE_L]->Get_TrasformationMatrix();
			_float4x4 ResultMat;
			XMStoreFloat4x4(&ResultMat, Combined);
			m_pPx_Collider->Update_Transform_Divided(&ResultMat, DOUBLE_DOOR_MESH_TYPE::DOOR_R);
			m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pDoubleState);
			Combined = m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_R_SIDE_R]->Get_TrasformationMatrix();
			XMStoreFloat4x4(&ResultMat, Combined);
			m_pPx_Collider->Update_Transform_Divided(&ResultMat, DOUBLE_DOOR_MESH_TYPE::DOOR_L);
			if (*m_pDoorTexture == CDoor::WOOD)
				WoodSound_Double(iRand, iRandPlus);
			break;
		}
		case CDoor::R_DOUBLEDOOR_OPEN:
		{
			m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pDoubleState);

			auto Combined = m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_L_SIDE_R]->Get_TrasformationMatrix();
			_float4x4 ResultMat;
			XMStoreFloat4x4(&ResultMat, Combined);
			m_pPx_Collider->Update_Transform_Divided(&ResultMat, DOUBLE_DOOR_MESH_TYPE::DOOR_R);
			m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pDoubleState);

			Combined = m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_R_SIDE_L]->Get_TrasformationMatrix();
			XMStoreFloat4x4(&ResultMat, Combined);
			m_pPx_Collider->Update_Transform_Divided(&ResultMat, DOUBLE_DOOR_MESH_TYPE::DOOR_L);
			if (*m_pDoorTexture == CDoor::WOOD)
				WoodSound_Double(iRand, iRandPlus);
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
			if (*m_pDoorTexture == CDoor::WOOD)
				WoodSound_Double(iRand, iRandPlus);

			break;
		}
		case CDoor::LSIDE_DOUBLEDOOR_OPEN_R:
		{
			m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pDoubleState);

			auto Combined = m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_L_SIDE_R]->Get_TrasformationMatrix();
			_float4x4 ResultMat;
			XMStoreFloat4x4(&ResultMat, Combined);
			m_pPx_Collider->Update_Transform_Divided(&ResultMat, DOUBLE_DOOR_MESH_TYPE::DOOR_L);
			if (*m_pDoorTexture == CDoor::WOOD)
				WoodSound_Double(iRand, iRandPlus);

			break;
		}
		case CDoor::RSIDE_DOUBLEDOOR_OPEN_L:
		{
			m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pDoubleState);

			auto Combined = m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_R_SIDE_L]->Get_TrasformationMatrix();
			_float4x4 ResultMat;
			XMStoreFloat4x4(&ResultMat, Combined);
			m_pPx_Collider->Update_Transform_Divided(&ResultMat, DOUBLE_DOOR_MESH_TYPE::DOOR_R);
			if (*m_pDoorTexture == CDoor::WOOD)
				WoodSound_Double(iRand, iRandPlus);
			break;
		}
		case CDoor::RSIDE_DOUBLEDOOR_OPEN_R:
		{

			m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pDoubleState);

			auto Combined = m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_R_SIDE_R]->Get_TrasformationMatrix();
			_float4x4 ResultMat;
			XMStoreFloat4x4(&ResultMat, Combined);
			m_pPx_Collider->Update_Transform_Divided(&ResultMat, DOUBLE_DOOR_MESH_TYPE::DOOR_R);
			if (*m_pDoorTexture == CDoor::WOOD)
				WoodSound_Double(iRand, iRandPlus);
			break;
		}
		case CDoor::DOUBLEDOOR_STATIC:
		{
			m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pDoubleState);
			if (!m_pModelCom->isFinished(0))
				Change_Sound(TEXT("sound_Map_sm40_doubledoor_m_wood_normal2_6.mp3"), 0);

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
			case CDoor::L_DOUBLEDOOR_OPEN:
			{
				auto Combined = m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_L_SIDE_L]->Get_TrasformationMatrix();
				_float4x4 ResultMat;
				XMStoreFloat4x4(&ResultMat, Combined);
				m_pPx_Collider->Update_Transform_Divided(&ResultMat, DOUBLE_DOOR_MESH_TYPE::DOOR_L);
				m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pDoubleState);

				Combined = m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_R_SIDE_R]->Get_TrasformationMatrix();
				XMStoreFloat4x4(&ResultMat, Combined);
				m_pPx_Collider->Update_Transform_Divided(&ResultMat, DOUBLE_DOOR_MESH_TYPE::DOOR_R);

				break;
			}
			case CDoor::R_DOUBLEDOOR_OPEN:
			{
				auto Combined = m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_L_SIDE_R]->Get_TrasformationMatrix();
				_float4x4 ResultMat;
				XMStoreFloat4x4(&ResultMat, Combined);
				m_pPx_Collider->Update_Transform_Divided(&ResultMat, DOUBLE_DOOR_MESH_TYPE::DOOR_L);
				m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pDoubleState);

				Combined = m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_R_SIDE_L]->Get_TrasformationMatrix();
				XMStoreFloat4x4(&ResultMat, Combined);
				m_pPx_Collider->Update_Transform_Divided(&ResultMat, DOUBLE_DOOR_MESH_TYPE::DOOR_R);

				break;
			}

			}
			break;
		}
		case CDoor::L_DOUBLEDOOR_OPEN:
		{
			m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pDoubleState);

			auto Combined = m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_L_SIDE_L]->Get_TrasformationMatrix();
			_float4x4 ResultMat;
			XMStoreFloat4x4(&ResultMat, Combined);
			m_pPx_Collider->Update_Transform_Divided(&ResultMat, DOUBLE_DOOR_MESH_TYPE::DOOR_L);
			m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pDoubleState);
			Combined = m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_R_SIDE_R]->Get_TrasformationMatrix();
			XMStoreFloat4x4(&ResultMat, Combined);
			m_pPx_Collider->Update_Transform_Divided(&ResultMat, DOUBLE_DOOR_MESH_TYPE::DOOR_R);
			if (*m_pDoorTexture == CDoor::WOOD)
				WoodSound_Double(iRand, iRandPlus);
			break;
		}
		case CDoor::R_DOUBLEDOOR_OPEN:
		{
			m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pDoubleState);

			auto Combined = m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_L_SIDE_R]->Get_TrasformationMatrix();
			_float4x4 ResultMat;
			XMStoreFloat4x4(&ResultMat, Combined);
			m_pPx_Collider->Update_Transform_Divided(&ResultMat, DOUBLE_DOOR_MESH_TYPE::DOOR_L);
			m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pDoubleState);

			Combined = m_vecRotationBone[ATC_DOUBLE_DOOR_OPEN_R_SIDE_L]->Get_TrasformationMatrix();
			XMStoreFloat4x4(&ResultMat, Combined);
			m_pPx_Collider->Update_Transform_Divided(&ResultMat, DOUBLE_DOOR_MESH_TYPE::DOOR_R);
			if (*m_pDoorTexture == CDoor::WOOD)
				WoodSound_Double(iRand, iRandPlus);
			break;
		}

		}
	}

	_float4 fTransform4 = m_pParentsTransform->Get_State_Float4(CTransform::STATE_POSITION);
	_float3 fTransform3 = _float3{ fTransform4.x,fTransform4.y,fTransform4.z };
	m_pModelCom->Play_Animation_Light(m_pParentsTransform, fTimeDelta);

	Get_SpecialBone_Rotation();

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_BLEND, this);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_POINT, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_DIR, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);

}

void CBody_Door::OneDoor_Tick(_float fTimeDelta)
{

}

void CBody_Door::OneDoor_Late_Tick(_float fTimeDelta)
{
	_int iRand = m_pGameInstance->GetRandom_Int(0, 1);
	_int iRandPlus = m_pGameInstance->GetRandom_Int(0, 4);

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

		if (*m_pDoorTexture == CDoor::WOOD)
			WoodSound(iRand, iRandPlus);
		else
			IronSound(iRand, iRandPlus);
		

		break;
	}
	case CDoor::ONEDOOR_OPEN_R:
	{
		m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pOneState);

		auto Combined = m_vecRotationBone[ATC_SINGLE_DOOR_OPEN_R]->Get_TrasformationMatrix();
		_float4x4 ResultMat;
		XMStoreFloat4x4(&ResultMat, Combined);
		m_pPx_Collider->Update_Transform(&ResultMat);
		if (*m_pDoorTexture == CDoor::WOOD)
			WoodSound(iRand, iRandPlus);
		else
			IronSound(iRand, iRandPlus);
		break;

	}
	case CDoor::ONEDOOR_STATIC:
		m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pOneState);
		if (!m_pModelCom->isFinished(0))
			Change_Sound(TEXT("sound_Map_sm40_door_m_wood_normal2_1.mp3"), 0);

			
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

	if (true == m_isHit)
		Update_Hit_Reaction(fTimeDelta);

	_float4 fTransform4 = m_pParentsTransform->Get_State_Float4(CTransform::STATE_POSITION);
	_float3 fTransform3 = _float3{ fTransform4.x,fTransform4.y,fTransform4.z };
	if(false == m_isHit)
		m_pModelCom->Play_Animation_Light(m_pParentsTransform, fTimeDelta);



	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_BLEND, this);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_POINT, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_DIR, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);

}

void CBody_Door::Update_Hit_Reaction(_float fTimeDelta)
{
	if (false == m_isHit)
		return;

	if (false == m_isInverseRotate_HitRecation)
	{
		m_fAccHitTime += fTimeDelta;

		if (DOOR_HIT_REACTION_TIME < m_fAccHitTime)
		{
			m_isInverseRotate_HitRecation = true;
			m_fAccHitTime = DOOR_HIT_REACTION_TIME;
		}
	}

	else
	{
		m_fAccHitTime -= fTimeDelta;

		if (0.f > m_fAccHitTime)
		{
			m_isInverseRotate_HitRecation = false;
			m_isHit = false;
			m_fAccHitTime = 0.f;
		}
	}

	_float			fDuration;
	if (true == m_isHitFromFront)
	{
		m_pModelCom->Change_Animation(0, TEXT("Default"), CDoor::ONEDOOR_STATE::ONEDOOR_OPEN_L);
		fDuration = { m_pModelCom->Get_Duration_From_Anim(TEXT("Default"), CDoor::ONEDOOR_STATE::ONEDOOR_OPEN_L) };
	}

	else
	{
		m_pModelCom->Change_Animation(0, TEXT("Default"), CDoor::ONEDOOR_STATE::ONEDOOR_OPEN_R);
		fDuration = { m_pModelCom->Get_Duration_From_Anim(TEXT("Default"), CDoor::ONEDOOR_STATE::ONEDOOR_OPEN_R) };
	}

	_float			fMaxTrackPositionRatio = { 0.05f };
	_float			fMaxTrackPosition = { fDuration * fMaxTrackPositionRatio };

	_float			fRatio = { m_fAccHitTime / DOOR_HIT_REACTION_TIME };

	_float			fCurrentTrackPosition = { fMaxTrackPosition * fRatio };

	m_pModelCom->Set_TrackPosition(0, fCurrentTrackPosition, true);

	m_pModelCom->Play_Animation_Light(m_pParentsTransform, 0.0001f);
}

void CBody_Door::Hit(CTransform* pTransform)
{
	if (nullptr == pTransform)
		return;

	m_isHit = true;
	m_fAccHitTime = 0.f;
	m_isInverseRotate_HitRecation = false;

	_vector				vTargetPosition = { pTransform->Get_State_Vector(CTransform::STATE_POSITION) };
	_vector				vMyPosition = { m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION) };

	_vector				vDirectionToTarget = { vTargetPosition - vMyPosition };
	_matrix				WorldMatrixInv = { m_pTransformCom->Get_WorldMatrix_Inverse() };

	_vector				vDirectionToTargetLocal = { XMVector3TransformNormal(vDirectionToTarget, WorldMatrixInv) };
	_vector				vDirectionToTargetLocalXZPlane = { XMVector3Normalize(XMVectorSetY(vDirectionToTargetLocal, 0.f)) };

	//	SingleDoor = { Front -> Open -> R }
	_bool				isFront = { XMVectorGetZ(vDirectionToTargetLocalXZPlane) > 0.f };
	m_isHitFromFront = true;
}

void CBody_Door::WoodSound_Double(_int iRandom, _int iRandom1)
{

	if (m_pModelCom->Get_TrackPosition(0) >= 12.f)
		;
	else if (m_pModelCom->Get_TrackPosition(0) >= 10.f)
	{
		switch (iRandom1)
		{
		case 0:
			Change_Same_Sound(TEXT("sound_Map_sm40_doubledoor_m_wood_normal2_17.mp3"), iRandom);
			break;
		case 1:
			Change_Same_Sound(TEXT("sound_Map_sm40_doubledoor_m_wood_normal2_14.mp3"), iRandom);
			break;
		case 2:
			Change_Same_Sound(TEXT("sound_Map_sm40_doubledoor_m_wood_normal2_12.mp3"), iRandom);
			break;
		case 3:
			Change_Same_Sound(TEXT("sound_Map_sm40_door_m_wood_normal2_15.mp3"), iRandom);
			break;
		}
	}
	else
		Change_Same_Sound(TEXT("sound_Map_sm40_door_m_wood_normal2_2.mp3"), !iRandom);
}

void CBody_Door::IronSound(_int iRandom, _int iRandom1)
{
	if (m_pModelCom->Get_TrackPosition(0) >= 12.f)
		;
	else if (m_pModelCom->Get_TrackPosition(0) >= 10.f)
	{
		switch (iRandom1)
		{
		case 0:
			Change_Same_Sound(TEXT("sound_Map_sm40_door_m_iron_normal2_1.mp3"), iRandom);
			break;
		case 1:
			Change_Same_Sound(TEXT("sound_Map_sm40_door_m_iron_normal2_15.mp3"), iRandom);
			break;
		case 2:
			Change_Same_Sound(TEXT("sound_Map_sm40_door_m_iron_fence2_7.mp3"), iRandom);
			break;
		case 3:
			Change_Same_Sound(TEXT("sound_Map_sm40_door_m_wood_normal2_15.mp3"), iRandom);
			break;
		}
	}
	else
		Change_Sound(TEXT("sound_Map_sm40_door_m_iron_normal2_9.mp3"), !iRandom);
}

void CBody_Door::WoodSound(_int iRandom, _int iRandom1)
{
	if (m_pModelCom->Get_TrackPosition(0) >= 12.f)
		;
	else if (m_pModelCom->Get_TrackPosition(0) >= 10.f)
	{
		switch (iRandom1)
		{
		case 0:
			Change_Same_Sound(TEXT("sound_Map_sm40_door_m_wood_normal2_4.mp3"), iRandom);
			break;
		case 1:
			Change_Same_Sound(TEXT("sound_Map_sm40_door_m_wood_normal2_9.mp3"), iRandom);
			break;
		case 2:
			Change_Same_Sound(TEXT("sound_Map_sm40_doubledoor_m_wood_normal2_12.mp3"), iRandom);
			break;
		case 3:
			Change_Same_Sound(TEXT("sound_Map_sm40_door_m_wood_normal2_15.mp3"), iRandom);
			break;
		}
	}
	else
		Change_Same_Sound(TEXT("sound_Map_sm40_door_m_wood_normal2_2.mp3"), !iRandom);
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
