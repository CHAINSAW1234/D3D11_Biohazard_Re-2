#include "stdafx.h"
#include "Mini_BigStatue.h"
#include"Player.h"

#include"BigStatue.h"
#include"Light.h"
#include "Tab_Window.h"

#define PUT_Z -0.9f
#define ROTATION 30.f

CMini_BigStatue::CMini_BigStatue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPart_InteractProps{ pDevice, pContext }
{
}

CMini_BigStatue::CMini_BigStatue(const CMini_BigStatue& rhs)
	: CPart_InteractProps{ rhs }
{

}

HRESULT CMini_BigStatue::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMini_BigStatue::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (pArg != nullptr)
	{
		BODY_MINI_STATUE_DESC* desc = (BODY_MINI_STATUE_DESC*)pArg;

		m_eMiniType = static_cast<_ubyte>(desc->eMiniType);

		m_ePartsType = static_cast<_ubyte>(desc->eParts_Type);

		m_isMedalAnim = desc->isMedalAnim;

		m_isMoveStart = desc->pMove;

		if (PARTS_TYPE::MINI_PARTS == static_cast<PARTS_TYPE>(m_ePartsType))
		{
			m_ParentWorldMatrix = desc->vParts_WorldMatrix;
		}
	}

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pModelCom->Set_RootBone("RootNode");
	m_pModelCom->Add_Bone_Layer_All_Bone(TEXT("Default"));
	m_pModelCom->Add_AnimPlayingInfo(false, 0, TEXT("Default"), 1.f);
	m_pModelCom->Active_RootMotion_Rotation(true);

	m_pModelCom->Change_Animation(0, TEXT("Default"), (_int)m_eAnim); //static상태 유지

	if (PARTS_TYPE::MINI_PARTS == static_cast<PARTS_TYPE>(m_ePartsType))
	{
		m_pTransformCom->Set_WorldMatrix(XMMatrixRotationY(XMConvertToRadians(180.f)));
	}

	Find_Window();


#ifndef NON_COLLISION_PROP

#endif

	return S_OK;
}

void CMini_BigStatue::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (nullptr == m_pTapWindow)
		Find_Window();

	if(DOWN == m_pGameInstance->Get_KeyState('0'))
		MiniMap_Statue(ITEM_NUMBER::unicornmedal01a);

	if (static_cast<_ubyte>(CBigStatue::BIGSTATUE_TYPE::BIGSTATUE_UNICON) == m_eMiniType)
	{
		if (PARTS_TYPE::MINI_PARTS == static_cast<PARTS_TYPE>(m_ePartsType))
		{
			if(true == *m_isMoveStart)
				Unicon_Statue(fTimeDelta);
		}
	}

	else if (static_cast<_ubyte>(CBigStatue::BIGSTATUE_TYPE::BIGSTATUE_WOMAN) == m_eMiniType)
	{
		if (true == *m_isMoveStart)
			Woman_Statue(fTimeDelta);
	}

	else if (static_cast<_ubyte>(CBigStatue::BIGSTATUE_TYPE::BIGSTATUE_LION) == m_eMiniType)
	{
		if (PARTS_TYPE::MINI_PARTS == static_cast<PARTS_TYPE>(m_ePartsType))
		{
			if (true == *m_isMoveStart)
				Lion_Statue(fTimeDelta);
		}
	}
}

void CMini_BigStatue::Late_Tick(_float fTimeDelta)
{
	if (static_cast<_ubyte>(CBigStatue::BIGSTATUE_TYPE::BIGSTATUE_LION) == m_eMiniType)
	{
		if (DOWN == m_pGameInstance->Get_KeyState('R'))
			*m_isMoveStart = true;

		if (PARTS_TYPE::MINI_PARTS == static_cast<PARTS_TYPE>(m_ePartsType))
		{
			if (nullptr != m_isMoveStart && true == *m_isMoveStart)
				Lion_Statue(fTimeDelta);
		} 
	}

	if (PARTS_TYPE::MINI_BODY == static_cast<PARTS_TYPE>(m_ePartsType))
	{
		_matrix WorldMatrix = { m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pSocketMatrix) * (m_pParentsTransform->Get_WorldMatrix()) };
		XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
	}

	else if (PARTS_TYPE::MINI_PARTS == static_cast<PARTS_TYPE>(m_ePartsType))
	{
		m_pModelCom->Play_Animations(m_pParentsTransform, fTimeDelta, 0);

		_matrix			WorldMatrix = { m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_ParentWorldMatrix)};
		XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
	}


	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_POINT, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_DIR, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);

}

HRESULT CMini_BigStatue::Render()
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

		if (PARTS_TYPE::MINI_PARTS == static_cast<PARTS_TYPE>(m_ePartsType)) {
			if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXANIMMODEL::PASS_DEFAULT)))
				return E_FAIL;
		}
		else {
			if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXMODEL::PASS_DEFAULT)))
				return E_FAIL;
		}


		m_pModelCom->Render(static_cast<_uint>(i));
	}

	return S_OK;
}

HRESULT CMini_BigStatue::Render_LightDepth_Dir()
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

			if (PARTS_TYPE::MINI_PARTS == static_cast<PARTS_TYPE>(m_ePartsType)) {
				if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i))))
					return E_FAIL;

				if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXANIMMODEL::PASS_LIGHTDEPTH)))
					return E_FAIL;
			}
			else {
				if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXMODEL::PASS_LIGHTDEPTH)))
					return E_FAIL;
			}


			m_pModelCom->Render(static_cast<_uint>(i));
		}
	}

	return S_OK;
}

HRESULT CMini_BigStatue::Render_LightDepth_Point()
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
			if (PARTS_TYPE::MINI_PARTS == static_cast<PARTS_TYPE>(m_ePartsType)) {
				if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i))))
					return E_FAIL;

				if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXANIMMODEL::PASS_LIGHTDEPTH_CUBE)))
					return E_FAIL;
			}
			else {
				if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXMODEL::PASS_LIGHTDEPTH_CUBE)))
					return E_FAIL;
			}

			m_pModelCom->Render(static_cast<_uint>(i));
		}

		++iIndex;
	}

	return S_OK;
}

HRESULT CMini_BigStatue::Render_LightDepth_Spot()
{
	if (m_bRender == false)
		return S_OK;

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
			if (PARTS_TYPE::MINI_PARTS == static_cast<PARTS_TYPE>(m_ePartsType)) {
				if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i))))
					return E_FAIL;

				if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXANIMMODEL::PASS_LIGHTDEPTH)))
					return E_FAIL;
			}
			else {
				if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXMODEL::PASS_LIGHTDEPTH)))
					return E_FAIL;
			}

			m_pModelCom->Render(static_cast<_uint>(i));
		}
	}

	return S_OK;
}

HRESULT CMini_BigStatue::Add_Components()
{
	/* For.Com_Body_Shader */
	if (PARTS_TYPE::MINI_PARTS == static_cast<PARTS_TYPE>(m_ePartsType)) {
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
			TEXT("Com_Body_Shader"), (CComponent**)&m_pShaderCom)))
			return E_FAIL;
	}
	else {
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
			TEXT("Com_Body_Shader"), (CComponent**)&m_pShaderCom)))
			return E_FAIL;
	}



	/* For.Com_Body_Model */
	if (FAILED(__super::Add_Component(g_Level, m_strModelComponentName,
		TEXT("Com_Body_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMini_BigStatue::Add_PartObjects()
{

	return S_OK;
}

HRESULT CMini_BigStatue::Initialize_PartObjects()
{
	return S_OK;
}

void CMini_BigStatue::Find_Window()
{
	list<class CGameObject*>* pUIList = m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_TabWindow"));

	m_pTapWindow = static_cast<CTab_Window*>(pUIList->back());
}

void CMini_BigStatue::Unicon_Statue(_float fTimeDelta)
{
	vector<string> BoneNames = { m_pModelCom->Get_BoneNames() };

	if(false == m_isMoveEnd)
	{
		/* 1. 처음에 Z 축으로 들어감*/
		if (m_fAdditionalZ <= PUT_Z)
		{
			m_fZTimer += fTimeDelta;
		}

		else
		{
			if(m_fRotationAngle >= ROTATION)
			{
				if (m_fAdditionalZ >= PUT_Z)
					m_fAdditionalZ -= 3.5f * fTimeDelta;
			}

			else
			{
				m_fRotationAngle += fTimeDelta * 60.f;
			}
		}

		if (m_fZTimer >= 0.8f)
		{
			m_fAdditionalHeight_D -= 5.f * fTimeDelta;
			m_fAdditionalHeight += 5.f * fTimeDelta;
		}

		/* Rotation */
		_vector vRotateAxis = { m_pTransformCom->Get_State_Vector(CTransform::STATE_LOOK) };
		_vector vNewQuaternion = { XMQuaternionRotationAxis(vRotateAxis, XMConvertToRadians(m_fRotationAngle)) };
		_matrix RotationMatrix = { XMMatrixRotationQuaternion(vNewQuaternion) };

		list<_uint> ChildJointIndices_Root;
		m_pModelCom->Get_Child_ZointIndices("RootNode", "_00", ChildJointIndices_Root);

		for (auto& iJointIndex : ChildJointIndices_Root)
		{
			m_pModelCom->Add_Additional_Transformation_World(BoneNames[iJointIndex], RotationMatrix);
		}

		/* 1. 아래로 내리기 */
		_matrix			TranslationMatrix1 = { XMMatrixTranslation(-m_fAdditionalHeight_D, m_fAdditionalHeight_D, m_fAdditionalZ) };
		list<_uint>		ChildJointIndices_1;
		m_pModelCom->Get_Child_ZointIndices("_00", "_01_end_end_end", ChildJointIndices_1);

		for (auto& iJointIndex : ChildJointIndices_1)
		{
			m_pModelCom->Add_Additional_Transformation_World(BoneNames[iJointIndex], TranslationMatrix1);
		}

		/* 2. 위로 올려야 함 */
		_matrix			TranslationMatrix2 = { XMMatrixTranslation(-m_fAdditionalHeight, m_fAdditionalHeight, m_fAdditionalZ) };

		list<_uint> ChildJointIndices_2;
		m_pModelCom->Get_Child_ZointIndices("_00", "_02_end_end_end", ChildJointIndices_2);

		for (auto& iJointIndex : ChildJointIndices_2)
		{
			m_pModelCom->Add_Additional_Transformation_World(BoneNames[iJointIndex], TranslationMatrix2);
		}

		if (m_fAdditionalHeight >= 9.f)
		{
			m_isMoveEnd = true;
			*m_isMedalAnim = true;
		}
	}

	else if (true == m_isMoveEnd)
	{
		MiniMap_Statue(ITEM_NUMBER::unicornmedal01a);

		/* Rotation */
		_vector vRotateAxis = { m_pTransformCom->Get_State_Vector(CTransform::STATE_LOOK) };
		_vector vNewQuaternion = { XMQuaternionRotationAxis(vRotateAxis, XMConvertToRadians(m_fRotationAngle)) };
		_matrix RotationMatrix = { XMMatrixRotationQuaternion(vNewQuaternion) };

		list<_uint> ChildJointIndices_Root;
		m_pModelCom->Get_Child_ZointIndices("RootNode", "_00", ChildJointIndices_Root);

		for (auto& iJointIndex : ChildJointIndices_Root)
		{
			m_pModelCom->Add_Additional_Transformation_World(BoneNames[iJointIndex], RotationMatrix);
		}


		_matrix			TranslationMatrix = { XMMatrixTranslation(-m_fAdditionalHeight_D, m_fAdditionalHeight_D, m_fAdditionalZ) };

		list<_uint> ChildJointIndices_1;
		// root / RootNode 위치도 함께 흘러 내림
		m_pModelCom->Get_Child_ZointIndices("_00", "_01_end_end_end", ChildJointIndices_1);
		for (auto& iJointIndex : ChildJointIndices_1)
		{
			m_pModelCom->Add_Additional_Transformation_World(BoneNames[iJointIndex], TranslationMatrix);
		}

		_matrix			TranslationMatrix2 = { XMMatrixTranslation(-m_fAdditionalHeight, m_fAdditionalHeight, m_fAdditionalZ) };

		list<_uint> ChildJointIndices_2;
		m_pModelCom->Get_Child_ZointIndices("_00", "_02_end_end_end", ChildJointIndices_2);

		for (auto& iJointIndex : ChildJointIndices_2)
		{
			m_pModelCom->Add_Additional_Transformation_World(BoneNames[iJointIndex], TranslationMatrix2);
		}

		
	}

	if (DOWN == m_pGameInstance->Get_KeyState('K'))
	{
		m_fAdditionalHeight = 0.f;
		m_fAdditionalHeight_D = 0.f;
		m_fAdditionalZ = 0.f;
		m_fZTimer = 0.f;
		m_fRotationAngle = 0.f;
		m_isMoveStart = false;
		m_isMoveEnd = false;
	}
}

void CMini_BigStatue::Woman_Statue(_float fTimeDelta)
{
	m_fAdditionalHeight_D -= 5.f * fTimeDelta;
	m_fAdditionalHeight += 5.f * fTimeDelta;

	/* Rotation */
	_matrix			TranslationMatrix1 = { XMMatrixTranslation(0.f, m_fAdditionalHeight_D, m_fAdditionalZ) };
	_matrix			TranslationMatrix2 = { XMMatrixTranslation(0.f, m_fAdditionalHeight_D, m_fAdditionalZ) };
	_matrix			TranslationMatrix3 = { XMMatrixTranslation(0.f, m_fAdditionalHeight_D, m_fAdditionalZ) };
	_matrix			TranslationMatrix4 = { XMMatrixTranslation(0.f, m_fAdditionalHeight_D, m_fAdditionalZ) };
	_matrix			TranslationMatrix5 = { XMMatrixTranslation(0.f, m_fAdditionalHeight_D, m_fAdditionalZ) };
	_matrix			TranslationMatrix6 = { XMMatrixTranslation(0.f, m_fAdditionalHeight_D, m_fAdditionalZ) };

	vector<string> BoneNames = { m_pModelCom->Get_BoneNames() };

	/* 1. 가운데 상단*/
	list<_uint> ChildJointIndices_Root_CenterTop;
	m_pModelCom->Get_Child_ZointIndices("_01", "_01_end_end_end_end", ChildJointIndices_Root_CenterTop);

	for (auto& iJointIndex : ChildJointIndices_Root_CenterTop)
	{
		m_pModelCom->Add_Additional_Transformation_World(BoneNames[iJointIndex], TranslationMatrix1);
	}

	/* 2. 우측 상단*/
	list<_uint> ChildJointIndices_Root_Right_Top;
	m_pModelCom->Get_Child_ZointIndices("_02", "_02_end_end_end_end", ChildJointIndices_Root_Right_Top);

	for (auto& iJointIndex : ChildJointIndices_Root_Right_Top)
	{
		m_pModelCom->Add_Additional_Transformation_World(BoneNames[iJointIndex], TranslationMatrix2);
	}

	/* 3. 우측 하단*/
	list<_uint> ChildJointIndices_Root_Right_Bottom;
	m_pModelCom->Get_Child_ZointIndices("_03", "_03_end_end_end_end", ChildJointIndices_Root_Right_Bottom);

	for (auto& iJointIndex : ChildJointIndices_Root_Right_Bottom)
	{
		m_pModelCom->Add_Additional_Transformation_World(BoneNames[iJointIndex], TranslationMatrix3);
	}

	/* 4. 가운데 하단*/
	list<_uint> ChildJointIndices_Root_CenterBottom;
	m_pModelCom->Get_Child_ZointIndices("_04", "_04_end_end_end_end", ChildJointIndices_Root_CenterBottom);

	for (auto& iJointIndex : ChildJointIndices_Root_CenterBottom)
	{
		m_pModelCom->Add_Additional_Transformation_World(BoneNames[iJointIndex], TranslationMatrix4);
	}

	/* 5. 좌측 하단*/
	list<_uint> ChildJointIndices_Root_LeftBottom;
	m_pModelCom->Get_Child_ZointIndices("_05", "_05_end_end_end_end", ChildJointIndices_Root_LeftBottom);

	for (auto& iJointIndex : ChildJointIndices_Root_LeftBottom)
	{
		m_pModelCom->Add_Additional_Transformation_World(BoneNames[iJointIndex], TranslationMatrix5);
	}

	/* 6. 좌측 상단 */
	list<_uint> ChildJointIndices_Root_LeftTop;
	m_pModelCom->Get_Child_ZointIndices("_06", "_06_end_end_end_end", ChildJointIndices_Root_LeftTop);

	for (auto& iJointIndex : ChildJointIndices_Root_LeftTop)
	{
		m_pModelCom->Add_Additional_Transformation_World(BoneNames[iJointIndex], TranslationMatrix6);
	}

}

void CMini_BigStatue::Lion_Statue(_float fTimeDelta)
{
	vector<string> BoneNames = { m_pModelCom->Get_BoneNames() };

	if (false == m_isMoveEnd)
	{
		/* 1. 처음에 Z 축으로 들어감*/
		if (m_fAdditionalZ <= PUT_Z)
		{
			m_fZTimer += fTimeDelta;
		}

		else
		{
			if (m_fRotationAngle >= ROTATION)
			{
				if (m_fAdditionalZ >= PUT_Z)
					m_fAdditionalZ -= 3.5f * fTimeDelta;
			}

			else
			{
				m_fRotationAngle += fTimeDelta * 60.f;
			}
		}

		if (m_fZTimer >= 0.8f)
		{
			m_fAdditionalHeight_D -= 5.f * fTimeDelta;
			m_fAdditionalHeight += 5.f * fTimeDelta;
		}

		/* Rotation */
		_vector vRotateAxis = { m_pTransformCom->Get_State_Vector(CTransform::STATE_LOOK) };
		_vector vNewQuaternion = { XMQuaternionRotationAxis(vRotateAxis, XMConvertToRadians(m_fRotationAngle)) };
		_matrix RotationMatrix = { XMMatrixRotationQuaternion(vNewQuaternion) };

		list<_uint> ChildJointIndices_Root;
		m_pModelCom->Get_Child_ZointIndices("RootNode", "_00", ChildJointIndices_Root);

		for (auto& iJointIndex : ChildJointIndices_Root)
		{
			m_pModelCom->Add_Additional_Transformation_World(BoneNames[iJointIndex], RotationMatrix);
		}

		/* 1. 아래로 내리기 */
		_matrix			TranslationMatrix1 = { XMMatrixTranslation(-m_fAdditionalHeight_D, m_fAdditionalHeight_D, m_fAdditionalZ)};
		list<_uint>		ChildJointIndices_1;
		m_pModelCom->Get_Child_ZointIndices("_00", "_01_end_end_end", ChildJointIndices_1);

		for (auto& iJointIndex : ChildJointIndices_1)
		{
			m_pModelCom->Add_Additional_Transformation_World(BoneNames[iJointIndex], TranslationMatrix1);
		}

		/* 2. 위로 올려야 함 */
		_matrix			TranslationMatrix2 = { XMMatrixTranslation(-m_fAdditionalHeight , m_fAdditionalHeight, m_fAdditionalZ) };

		list<_uint> ChildJointIndices_2;
		m_pModelCom->Get_Child_ZointIndices("_00", "_02_end_end_end", ChildJointIndices_2);

		for (auto& iJointIndex : ChildJointIndices_2)
		{
			m_pModelCom->Add_Additional_Transformation_World(BoneNames[iJointIndex], TranslationMatrix2);
		}

		if (m_fAdditionalHeight >= 9.f)
		{
			m_isMoveEnd = true;
			*m_isMedalAnim = true;
		}
	}

	else if (true == m_isMoveEnd)
	{
		MiniMap_Statue(ITEM_NUMBER::virginmedal02a);
		
		/* Rotation */
		_vector vRotateAxis = { m_pTransformCom->Get_State_Vector(CTransform::STATE_LOOK) };
		_vector vNewQuaternion = { XMQuaternionRotationAxis(vRotateAxis, XMConvertToRadians(m_fRotationAngle)) };
		_matrix RotationMatrix = { XMMatrixRotationQuaternion(vNewQuaternion) };

		list<_uint> ChildJointIndices_Root;
		m_pModelCom->Get_Child_ZointIndices("RootNode", "_00", ChildJointIndices_Root);

		for (auto& iJointIndex : ChildJointIndices_Root)
		{
			m_pModelCom->Add_Additional_Transformation_World(BoneNames[iJointIndex], RotationMatrix);
		}


		_matrix			TranslationMatrix = { XMMatrixTranslation(-m_fAdditionalHeight_D, m_fAdditionalHeight_D, m_fAdditionalZ) };

		list<_uint> ChildJointIndices_1;
		// root / RootNode 위치도 함께 흘러 내림
		m_pModelCom->Get_Child_ZointIndices("_00", "_01_end_end_end", ChildJointIndices_1);
		for (auto& iJointIndex : ChildJointIndices_1)
		{
			m_pModelCom->Add_Additional_Transformation_World(BoneNames[iJointIndex], TranslationMatrix);
		}

		_matrix			TranslationMatrix2 = { XMMatrixTranslation(-m_fAdditionalHeight, m_fAdditionalHeight, m_fAdditionalZ) };

		list<_uint> ChildJointIndices_2;
		m_pModelCom->Get_Child_ZointIndices("_00", "_02_end_end_end", ChildJointIndices_2);

		for (auto& iJointIndex : ChildJointIndices_2)
		{
			m_pModelCom->Add_Additional_Transformation_World(BoneNames[iJointIndex], TranslationMatrix2);
		}
	}

	if (DOWN == m_pGameInstance->Get_KeyState('K'))
	{
		m_fAdditionalHeight = 0.f;
		m_fAdditionalHeight_D = 0.f;
		m_fAdditionalZ = 0.f;
		m_fZTimer = 0.f;
		m_fRotationAngle = 0.f;
		m_isMoveStart = false;
		m_isMoveEnd = false;
	}
}

void CMini_BigStatue::MiniMap_Statue(ITEM_NUMBER _item)
{
	if (false == m_isTab)
	{
		m_isTab = true;

		m_pTapWindow->Destroy_Statue_Item(_item);
	}
}


CMini_BigStatue* CMini_BigStatue::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMini_BigStatue* pInstance = new CMini_BigStatue(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMini_BigStatue"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CMini_BigStatue::Clone(void* pArg)
{
	CMini_BigStatue* pInstance = new CMini_BigStatue(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CMini_BigStatue"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMini_BigStatue::Free()
{
	__super::Free();

}
