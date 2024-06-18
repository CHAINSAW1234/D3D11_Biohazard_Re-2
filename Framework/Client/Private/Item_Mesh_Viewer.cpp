#include "stdafx.h"

#include "Item_Mesh_Viewer.h"
#include "Camera_Free.h"

CItem_Mesh_Viewer::CItem_Mesh_Viewer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CItem_Mesh_Viewer::CItem_Mesh_Viewer(const CItem_Mesh_Viewer& rhs)
	: CGameObject{ rhs }
{

}
 
HRESULT CItem_Mesh_Viewer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CItem_Mesh_Viewer::Initialize(void* pArg)
{
	GAMEOBJECT_DESC		GameObjectDesc{};

	GameObjectDesc.fSpeedPerSec = 10.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	GAMEOBJECT_DESC* pMapStructureDesc = (GAMEOBJECT_DESC*)pArg;

	m_pTransformCom->Set_WorldMatrix(pMapStructureDesc->worldMatrix);

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_eItem_Number = PISTOL_AMMO;

	m_eViewer_State = IDLE;

	CGameObject* pCamera = m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_ZZZCamera"))->back();
	
	m_pCameraFree = dynamic_cast<CCamera_Free*>(pCamera);

	Safe_AddRef(m_pCameraFree);

	return S_OK;
}

void CItem_Mesh_Viewer::Tick(_float fTimeDelta)
{
	_vector vFrontCamPos = (XMVector4Normalize(m_pCameraFree->GetLookDir_Vector()) * m_fDistCam) + m_pCameraFree->Get_Position_Vector();

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vFrontCamPos);



	switch (m_eViewer_State)
	{
	case Client::CItem_Mesh_Viewer::POP_UP: {
		PopUp_Operation(fTimeDelta);
		break;
	}
		
	case Client::CItem_Mesh_Viewer::IDLE: {
		Idle_Operation(fTimeDelta);
		break;
	}
		
	case Client::CItem_Mesh_Viewer::HIDE: {
		Hide_Operation(fTimeDelta);
		break;
	}
		
	default:
		break;
	}



}

void CItem_Mesh_Viewer::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_EXAMINE, this);
}

HRESULT CItem_Mesh_Viewer::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_vecModelCom[m_eItem_Number]->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_vecModelCom[m_eItem_Number]->Bind_ShaderResource_Texture(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(i), aiTextureType_DIFFUSE)))
			return E_FAIL;
		if (FAILED(m_vecModelCom[m_eItem_Number]->Bind_ShaderResource_Texture(m_pShaderCom, "g_NormalTexture", static_cast<_uint>(i), aiTextureType_NORMALS)))
			return E_FAIL;

		if (FAILED(m_vecModelCom[m_eItem_Number]->Bind_ShaderResource_Texture(m_pShaderCom, "g_AlphaTexture", static_cast<_uint>(i), aiTextureType_METALNESS)))
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

		if (FAILED(m_vecModelCom[m_eItem_Number]->Bind_ShaderResource_Texture(m_pShaderCom, "g_AOTexture", static_cast<_uint>(i), aiTextureType_SHININESS)))
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

		m_vecModelCom[m_eItem_Number]->Render(static_cast<_uint>(i));
	}
}

void CItem_Mesh_Viewer::PopUp_Operation(_float fTimeDelta)
{
	m_fCurPopHide_Time += fTimeDelta;

	if (m_fCurPopHide_Time > m_fPopupHide_TimeLimit)
	{
		m_eViewer_State = IDLE;
		m_fCurPopHide_Time = 0.f;
	}

	
	m_fDistCam -= m_fPopupHide_Speed;
}

void CItem_Mesh_Viewer::Idle_Operation(_float fTimeDelta)
{
	if (true == m_pGameInstance->Check_Wheel_Down())
	{
		m_fDistCam -= 1.f;
	}
	else if (true == m_pGameInstance->Check_Wheel_Up())
	{
		m_fDistCam += 1.f;
	}

	if (PRESSING == m_pGameInstance->Get_KeyState(VK_LBUTTON))
	{
		_long	MouseMove = { 0 };

		if (MouseMove = m_pGameInstance->Get_MouseDeltaPos().x)
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * 0.1f);
		}
		if (MouseMove = m_pGameInstance->Get_MouseDeltaPos().y)
		{
			m_pTransformCom->Turn(m_pTransformCom->Get_State_Vector(CTransform::STATE_RIGHT), fTimeDelta * MouseMove * 0.1f);
		}
	}
}

void CItem_Mesh_Viewer::Hide_Operation(_float fTimeDelta)
{

}


HRESULT CItem_Mesh_Viewer::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;


	//todo : 좀더 좋은 방법을 생각해보자
	CModel* pItemModel = nullptr;
	_uint   iModelCount = 0;
	wstring wstrModelComTag = TEXT("Com_Model");


	wstring ModelTagTrashCan = wstrModelComTag + to_wstring(iModelCount);

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_HandGun"),
		ModelTagTrashCan, (CComponent**)&pItemModel)))
		return E_FAIL;

	m_vecModelCom.push_back(pItemModel);

	pItemModel = nullptr;
	iModelCount++;
	ModelTagTrashCan = wstrModelComTag + to_wstring(iModelCount);


	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_handgun_bullet01a"),
		ModelTagTrashCan, (CComponent**)&pItemModel)))
		return E_FAIL;

	m_vecModelCom.push_back(pItemModel);

	pItemModel = nullptr;
	iModelCount++;
	ModelTagTrashCan = wstrModelComTag + to_wstring(iModelCount);


	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_emergencyspray01a"),
		ModelTagTrashCan, (CComponent**)&pItemModel)))
		return E_FAIL;

	m_vecModelCom.push_back(pItemModel);

	pItemModel = nullptr;
	iModelCount++;
	ModelTagTrashCan = wstrModelComTag + to_wstring(iModelCount);


	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_inkribbon01a"),
		TEXT("Com_Model"), (CComponent**)&pItemModel)))
		return E_FAIL;

	m_vecModelCom.push_back(pItemModel);

	return S_OK;
}

HRESULT CItem_Mesh_Viewer::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CItem_Mesh_Viewer* CItem_Mesh_Viewer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CItem_Mesh_Viewer* pInstance = new CItem_Mesh_Viewer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CItem_Mesh_Viewer"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CItem_Mesh_Viewer::Clone(void* pArg)
{
	CItem_Mesh_Viewer* pInstance = new CItem_Mesh_Viewer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CItem_Mesh_Viewer"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItem_Mesh_Viewer::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);

	for (auto& iter : m_vecModelCom)
	{
		Safe_Release(iter);
	}

	Safe_Release(m_pCameraFree);
}
