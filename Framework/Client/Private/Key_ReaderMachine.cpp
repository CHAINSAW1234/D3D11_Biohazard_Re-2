#include "stdafx.h"
#include "Key_ReaderMachine.h"
#include"Player.h"

#include"ReaderMachine.h"

CKey_ReaderMachine::CKey_ReaderMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPart_InteractProps{ pDevice, pContext }
{
}

CKey_ReaderMachine::CKey_ReaderMachine(const CKey_ReaderMachine& rhs)
	: CPart_InteractProps{ rhs }
{

}

HRESULT CKey_ReaderMachine::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CKey_ReaderMachine::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	if (pArg != nullptr)
	{
		KEY_READER_DESC* pKey_Desc = (KEY_READER_DESC*)pArg;
		m_pPressKeyState = pKey_Desc->pKeyInput;
		m_pKeyState = pKey_Desc->pKeyState;
	}

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Initialize_Model()))
		return E_FAIL; 


	m_pModelCom->Set_RootBone("RootNode");
	m_pModelCom->Add_Bone_Layer_All_Bone(TEXT("Default"));

	m_pModelCom->Add_AnimPlayingInfo(false, 0, TEXT("Default"), 1.f);

	
	m_pModelCom->Active_RootMotion_Rotation(false);




	return S_OK;
}

void CKey_ReaderMachine::Tick(_float fTimeDelta)
{
	if (m_bCheckAnswer)
	{
	
		m_bCheckAnswer = false;
	}
}

void CKey_ReaderMachine::Late_Tick(_float fTimeDelta)
{
	if (m_bDead)
		return;



	switch (*m_pKeyState)
	{
	case CReaderMachine::READERMACHINE_KEY_STATIC:
		m_pModelCom->Change_Animation(0, TEXT("Default"), 0);
		break;

	case CReaderMachine::READERMACHINE_KEY_LIVE:
	{
		m_pModelCom->Change_Animation(0, TEXT("Default"), 0);

		switch (*m_pPressKeyState)
		{
		case CReaderMachine::KEY_NOTHING:

			break;

		case CReaderMachine::KEY_W:
			*m_pPressKeyState = CReaderMachine::KEY_NOTHING;
			break;

		case CReaderMachine::KEY_A:
			*m_pPressKeyState = CReaderMachine::KEY_NOTHING;

			break;

		case CReaderMachine::KEY_S:
			*m_pPressKeyState = CReaderMachine::KEY_NOTHING;

			break;

		case CReaderMachine::KEY_D:
			*m_pPressKeyState = CReaderMachine::KEY_NOTHING;
			break;

		case CReaderMachine::KEY_SPACE:
			*m_pPressKeyState = CReaderMachine::KEY_NOTHING;

			break;
		}

	}
	break;

	case CReaderMachine::READERMACHINE_KEY_WRONG:
		m_pModelCom->Change_Animation(0, TEXT("Default"),0);
		if (m_pModelCom->isFinished(0))
			*m_pKeyState = CReaderMachine::READERMACHINE_KEY_LIVE;

		break;

	case CReaderMachine::READERMACHINE_KEY_CORRECT:

		m_pModelCom->Change_Animation(0, TEXT("Default"),0);
		if (m_pModelCom->isFinished(0))
			m_bClear = true;
		break;
	}
	
	_float4 fTransform4 = m_pParentsTransform->Get_State_Float4(CTransform::STATE_POSITION);
	_float3 fTransform3 = _float3{ fTransform4.x,fTransform4.y,fTransform4.z };
	_float3				vDirection = { };

	m_pModelCom->Play_Animations(m_pParentsTransform, fTimeDelta, &vDirection);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_POINT, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_DIR, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);



	_matrix			WorldMatrix = { m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pSocketMatrix) * (m_pParentsTransform->Get_WorldMatrix()) };
	XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
	Get_SpecialBone_Rotation(); // for UI
}

HRESULT CKey_ReaderMachine::Render()
{
	if (m_bClear)
		return S_OK;
	if (m_bRender == false)
		return S_OK;
	else
		m_bRender = false;
	
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

HRESULT CKey_ReaderMachine::Add_Components()
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

HRESULT CKey_ReaderMachine::Add_PartObjects()
{

	return S_OK;
}

HRESULT CKey_ReaderMachine::Initialize_PartObjects()
{
	return S_OK;
}

void CKey_ReaderMachine::Get_SpecialBone_Rotation()
{

}

HRESULT CKey_ReaderMachine::Initialize_Model()
{
	vector<string>			MeshTags = { m_pModelCom->Get_MeshTags() };

	vector<string>			ResultMeshTags;
	m_HidMesh.emplace_back("Group_102");
	m_HidMesh.emplace_back("Group_125");
	for (auto& strMeshTag : MeshTags)
	{
		if ((strMeshTag.find("Group_102") == string::npos) && (strMeshTag.find("Group_125") == string::npos))
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


	return S_OK;
}


CKey_ReaderMachine* CKey_ReaderMachine::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CKey_ReaderMachine* pInstance = new CKey_ReaderMachine(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CKey_ReaderMachine"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CKey_ReaderMachine::Clone(void* pArg)
{
	CKey_ReaderMachine* pInstance = new CKey_ReaderMachine(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CKey_ReaderMachine"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CKey_ReaderMachine::Free()
{
	__super::Free();

}
