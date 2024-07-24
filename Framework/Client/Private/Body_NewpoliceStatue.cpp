#include "stdafx.h"
#include "Body_NewpoliceStatue.h"

#include"NewpoliceStatue.h"
#include"Light.h"

CBody_NewpoliceStatue::CBody_NewpoliceStatue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPart_InteractProps{ pDevice, pContext }
{
}

CBody_NewpoliceStatue::CBody_NewpoliceStatue(const CBody_NewpoliceStatue& rhs)
	: CPart_InteractProps{ rhs }
{

}

HRESULT CBody_NewpoliceStatue::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBody_NewpoliceStatue::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	if (FAILED(Add_Components()))
		return E_FAIL;	

	if (FAILED(Initialize_Model()))
		return E_FAIL;

	m_pModelCom->Set_RootBone("RootNode");
	m_pModelCom->Add_Bone_Layer_All_Bone(TEXT("Default"));
	m_pModelCom->Add_AnimPlayingInfo(false, 0, TEXT("Default"), 1.f);

	m_pModelCom->Active_RootMotion_Rotation(true);

#ifndef NON_COLLISION_PROP

	m_pGameInstance->Create_Px_Collider(m_pModelCom, m_pParentsTransform, &m_iPx_Collider_Id);

#endif

	return S_OK;
}

void CBody_NewpoliceStatue::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CBody_NewpoliceStatue::Late_Tick(_float fTimeDelta)
{
	switch (*m_pState)
	{
	case CNewpoliceStatue::POLICEHALLSTATUE_0:
		m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pState);
		
		break;
	case CNewpoliceStatue::POLICEHALLSTATUE_1:
		/* LN : 키 클리어 : sound_Map_sm41_new_police_statue2_1 */
		m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pState);
		m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pState);		
		Change_Sound(TEXT("sound_Map_sm41_new_police_statue2_1.mp3"), 0);
		if (m_pModelCom->isFinished(0))
			Stop_Sound(0);
		break;
	case CNewpoliceStatue::POLICEHALLSTATUE_2:
		/* LN : 키 클리어 : sound_Map_sm40_conveni_keyhole2_4 */
		m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pState);
		Change_Sound(TEXT("sound_Map_sm41_new_police_statue2_1.mp3"), 1);
		if (m_pModelCom->isFinished(0))
			Stop_Sound(1);
		break;
	case CNewpoliceStatue::POLICEHALLSTATUE_3:
		/* LN : 키 클리어 : sound_Map_sm40_conveni_keyhole2_4 */
		m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pState);
		Change_Sound(TEXT("sound_Map_sm41_new_police_statue2_1.mp3"), 2);
		if (m_pModelCom->isFinished(2))
			Stop_Sound(0);
		break;
	case CNewpoliceStatue::POLICEHALLSTATUE_END:
		/* LN : 키 클리어 : sound_Map_sm40_conveni_keyhole2_4 */
		m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pState);
		break;
	}

	_float4 fTransform4 = m_pParentsTransform->Get_State_Float4(CTransform::STATE_POSITION);
	_float3 fTransform3 = _float3{ fTransform4.x,fTransform4.y,fTransform4.z };
	m_pModelCom->Play_Animation_Light(m_pParentsTransform, fTimeDelta);
	//	m_pModelCom->Play_Animations(m_pParentsTransform, fTimeDelta, &fTransform3);



	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_POINT, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_DIR, this);
#ifdef SPOT_FRUSTRUM_CULLING
	if (m_bRender)
	{
		if (m_bLocalized == false)
		{
			if (m_pGameInstance->isInFrustum_WorldSpace(m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION), 0.2f))
			{
				m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);
			}
		}
	}
#endif
#ifdef ANIM_PROPS_SPOT_SHADOW
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);
#endif
}

HRESULT CBody_NewpoliceStatue::Render()
{
	if (m_bRender == false)
		return S_OK;
	else
		m_bRender = false;


	if (FAILED(__super::Bind_ShaderResources()))
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

		if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXANIMMODEL::PASS_DEFAULT)))
			return E_FAIL;

		m_pModelCom->Render(static_cast<_uint>(i));
	}

	return S_OK;
}

HRESULT CBody_NewpoliceStatue::Render_LightDepth_Dir()
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

HRESULT CBody_NewpoliceStatue::Render_LightDepth_Point()
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

HRESULT CBody_NewpoliceStatue::Render_LightDepth_Spot()
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
			if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXANIMMODEL::PASS_LIGHTDEPTH)))
				return E_FAIL;

			m_pModelCom->Render(static_cast<_uint>(i));
		}
	}

	return S_OK;
}

HRESULT CBody_NewpoliceStatue::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Body_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	
	/* For.Com_Model */
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

HRESULT CBody_NewpoliceStatue::Add_PartObjects()
{

	return S_OK;
}

HRESULT CBody_NewpoliceStatue::Initialize_PartObjects()
{

	return S_OK;
}

HRESULT CBody_NewpoliceStatue::Initialize_Model()
{
	vector<string>			MeshTags = { m_pModelCom->Get_MeshTags() };
	for (auto& strMeshTag : MeshTags)
	{
		if ((strMeshTag.find("Group_53_Sub_1") != string::npos))
			m_strMeshTag = strMeshTag;
	}

	return S_OK;
}

_float4 CBody_NewpoliceStatue::Get_Pos(_int iArg)
{

	_float4 vLocalPos = m_pModelCom->Get_Mesh_Local_Pos(m_strMeshTag) + _float4(0.f, 10.f, -10.f, 0.f);

	_matrix Local_Mesh_Matrix = m_pTransformCom->Get_WorldMatrix();
	Local_Mesh_Matrix.r[3] -= _vector{ vLocalPos.x,-vLocalPos.y,vLocalPos.z }; 
	_matrix TransformationMatrix = m_pParentsTransform->Get_WorldMatrix();

	_float4x4 WorldMatrix = (Local_Mesh_Matrix)*TransformationMatrix;

	_float4 vPos = XMVectorSetW(WorldMatrix.Translation(), 1.f);
#ifdef _DEBUG
#ifdef UI_POS
	m_pColliderCom[Part_INTERACTPROPS_COL_SPHERE]->Tick(WorldMatrix);
	m_pGameInstance->Add_DebugComponents(m_pColliderCom[Part_INTERACTPROPS_COL_SPHERE]);
#endif
#endif
	return vPos;
	return _float4();
}

CBody_NewpoliceStatue* CBody_NewpoliceStatue::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_NewpoliceStatue* pInstance = new CBody_NewpoliceStatue(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBody_NewpoliceStatue"));

		Safe_Release(pInstance);
	}

	return pInstance;

}
CGameObject* CBody_NewpoliceStatue::Clone(void* pArg)
{
	CBody_NewpoliceStatue* pInstance = new CBody_NewpoliceStatue(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CBody_NewpoliceStatue"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_NewpoliceStatue::Free()
{
	__super::Free();

}
