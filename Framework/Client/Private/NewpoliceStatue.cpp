#include "stdafx.h"
#include "NewpoliceStatue.h"
CNewpoliceStatue::CNewpoliceStatue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CInteractProps{ pDevice, pContext }
{
}

CNewpoliceStatue::CNewpoliceStatue(const CNewpoliceStatue& rhs)
	: CInteractProps{ rhs }
{

}

HRESULT CNewpoliceStatue::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNewpoliceStatue::Initialize(void* pArg)
{
	/*문자식 파트오브젝트 붙혀야하는데 뼈가 문고리에 없어서 직접 찍어야 하는데
	프로토타입 끝나고 뼈 붙혀보겠나이다*/

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	if (FAILED(Add_Components()))
		return E_FAIL;

	//if(!m_tagPropDesc.bAnim)
	//	m_pModelCom->Static_Mesh_Cooking(m_pTransformCom);

	//if (!m_tagPropDesc.bAnim&&m_pTransformCom->IsIdentityWorldMatrix())
	//{
	//	m_bLocalized = true;
	//	m_vLocal_To_World_Pos = m_pModelCom->GetCenterPoint();
	//}



	m_pModelCom->Set_RootBone("RootNode");
	m_pModelCom->Add_Bone_Layer_All_Bone(TEXT("Default"));

	m_pModelCom->Add_AnimPlayingInfo(0, false, 0, TEXT("Default"), 1.f);


	m_pModelCom->Active_RootMotion_Rotation(true);
	m_pTransformCom->Set_WorldMatrix(m_tagPropDesc.worldMatrix);

#ifndef NON_COLLISION_PROP

	//m_pGameInstance->Create_Px_Collider(m_pModelCom, m_pTransformCom, &m_iPx_Collider_Id);

#endif

	return S_OK;
}

void CNewpoliceStatue::Tick(_float fTimeDelta)
{

	__super::Check_Player();
	if (!m_bVisible)
		return;

	if (m_pGameInstance->Get_KeyState('U') == DOWN)
		m_eState = POLICEHALLSTATUE_0;
	if (m_pGameInstance->Get_KeyState('I') == DOWN)
		m_eState = POLICEHALLSTATUE_1;

	if (m_pGameInstance->Get_KeyState('O') == DOWN)
		m_eState = POLICEHALLSTATUE_3;
	if (m_pGameInstance->Get_KeyState('P') == DOWN)
		m_eState = POLICEHALLSTATUE_2;

	//__super::Tick(fTimeDelta);
}

void CNewpoliceStatue::Late_Tick(_float fTimeDelta)
{
	if (!Visible())
		return;

	if (m_bRender == false)
		return;



	switch (m_eState)
	{
	case POLICEHALLSTATUE_0:
		m_pModelCom->Change_Animation(0, m_eState);
		break;
	case POLICEHALLSTATUE_1:
		m_pModelCom->Change_Animation(0, m_eState);
		break;
	case POLICEHALLSTATUE_2:
		m_pModelCom->Change_Animation(0, m_eState);

	case POLICEHALLSTATUE_END:
		m_pModelCom->Change_Animation(0, m_eState);
		break;
	}

	_float4 fTransform4 = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
	_float3 fTransform3 = _float3{ fTransform4.x,fTransform4.y,fTransform4.z };
	m_pModelCom->Play_Animations(m_pTransformCom, fTimeDelta, &fTransform3);



	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_FIELD_SHADOW_POINT, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_FIELD_SHADOW_DIR, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);

}

HRESULT CNewpoliceStatue::Render()
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

	/*	if (FAILED(m_pModelCom->Bind_PrevBoneMatrices(m_pShaderCom, "g_PrevBoneMatrices", static_cast<_uint>(i))))
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

HRESULT CNewpoliceStatue::Add_Components()
{
	
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(g_Level, m_tagPropDesc.strModelComponent,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CNewpoliceStatue::Add_PartObjects()
{

	return S_OK;
}

HRESULT CNewpoliceStatue::Initialize_PartObjects()
{

	return S_OK;
}

HRESULT CNewpoliceStatue::Bind_ShaderResources()
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

CNewpoliceStatue* CNewpoliceStatue::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNewpoliceStatue* pInstance = new CNewpoliceStatue(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CNewpoliceStatue"));

		Safe_Release(pInstance);
	}

	return pInstance;

}
CGameObject* CNewpoliceStatue::Clone(void* pArg)
{
	CNewpoliceStatue* pInstance = new CNewpoliceStatue(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CNewpoliceStatue"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNewpoliceStatue::Free()
{
	__super::Free();

}
