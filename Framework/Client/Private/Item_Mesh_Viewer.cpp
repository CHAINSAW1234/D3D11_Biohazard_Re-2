#include "stdafx.h"

#include "Item_Mesh_Viewer.h"
#include "Camera_Free.h"


constexpr _float	DIST_CAM_FAR_LIMIIT = 10.f;
constexpr _float	DIST_CAM_NEAR_LIMIT = 1.f;

constexpr _float	POPUP_HIDE_TIME_LIMIT = 0.5f;

constexpr _float	POPUP_HIDE_START_RADIAN = 0.f;
constexpr _float	POPUP_HIDE_END_RADIAN = 360.f;

constexpr _float	POPUP_HIDE_START_DIST = 10.f;
constexpr _float	POPUP_HIDE_END_DIST = 0.4f;


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
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(360.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	GAMEOBJECT_DESC* pMapStructureDesc = (GAMEOBJECT_DESC*)pArg;

	m_pTransformCom->Set_WorldMatrix(pMapStructureDesc->worldMatrix);

	Load_ItemModelTags();

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_eViewer_State = POP_UP;

	CGameObject* pCamera = m_pGameInstance->Find_Layer(g_Level, g_strCameraTag)->front();
	
	m_pCameraFree = dynamic_cast<CCamera_Free*>(pCamera);

	m_fDistCam = POPUP_HIDE_START_DIST;

	Safe_AddRef(m_pCameraFree);

	return S_OK;
}

void CItem_Mesh_Viewer::Tick(_float fTimeDelta)
{
	_vector vFrontCamPos = (XMVector4Normalize(m_pCameraFree->GetLookDir_Vector()) * m_fDistCam) + m_pCameraFree->Get_Position_Vector();

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vFrontCamPos);

	switch (m_eViewer_State)
	{
	case Client::POP_UP: {
		PopUp_Operation(fTimeDelta);
		break;
	}
		
	case Client::UI_IDLE: {
		Idle_Operation(fTimeDelta);
		break;
	}

	case Client::UI_SECOND_IDLE: {
		SECOND_IDLE_Operation(fTimeDelta);
		break;
	}

	case Client::HIDE: {
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
	if (ITEM_NUMBER_END == m_eItem_Number)
		return E_FAIL;

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

		if (FAILED(m_vecModelCom[m_eItem_Number]->Bind_ShaderResource_Texture(m_pShaderCom, "g_EmissiveTexture", static_cast<_uint>(i), aiTextureType_EMISSIVE)))
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

		m_vecModelCom[m_eItem_Number]->Render(static_cast<_uint>(i));
	}

	return S_OK;
}

void CItem_Mesh_Viewer::PopUp_Operation(_float fTimeDelta)
{
	m_fPopupHide_CurTime += fTimeDelta;

	if (m_fPopupHide_CurTime > POPUP_HIDE_TIME_LIMIT)
	{
		m_eViewer_State = UI_IDLE;
		m_fPopupHide_CurTime = 0.f;
		m_fDistCam = POPUP_HIDE_END_DIST;
		return;
	}

	if (1.f > m_fPopupHide_CurTime / POPUP_HIDE_TIME_LIMIT)
	{
		m_fDistCam = m_pGameInstance->Get_Ease(Ease_OutQuint, POPUP_HIDE_START_DIST, POPUP_HIDE_END_DIST,
			m_fPopupHide_CurTime / POPUP_HIDE_TIME_LIMIT);

		_float fRadian = m_pGameInstance->Get_Ease(Ease_OutQuint, POPUP_HIDE_START_RADIAN, XMConvertToRadians(POPUP_HIDE_END_RADIAN),
			m_fPopupHide_CurTime / POPUP_HIDE_TIME_LIMIT);

		m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), fRadian);
	}
}

void CItem_Mesh_Viewer::Idle_Operation(_float fTimeDelta)
{
	if (true == m_pGameInstance->Check_Wheel_Down())
	{
		m_fDistCam -= 0.01f;
	}
	else if (true == m_pGameInstance->Check_Wheel_Up())
	{
		m_fDistCam += 0.01f;
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
			//m_pTransformCom->Turn(m_pTransformCom->Get_State_Vector(CTransform::STATE_RIGHT), fTimeDelta * MouseMove * 0.1f);
			m_pTransformCom->Turn(XMVectorSet(1.f, 0.f, 0.f, 0.f), fTimeDelta * MouseMove * 0.1f);
		}
	}
}

void CItem_Mesh_Viewer::SECOND_IDLE_Operation(_float fTimeDelta)
{
	m_fPopupHide_CurTime += fTimeDelta;

	if (m_fPopupHide_CurTime > POPUP_HIDE_TIME_LIMIT)
	{
		m_eViewer_State = UI_IDLE;
		m_fPopupHide_CurTime = 0.f;
		m_fDistCam = POPUP_HIDE_END_DIST;
		return;
	}

	if (1.f > m_fPopupHide_CurTime / POPUP_HIDE_TIME_LIMIT)
	{
		m_fDistCam = m_pGameInstance->Get_Ease(Ease_OutQuint, POPUP_HIDE_START_DIST, POPUP_HIDE_END_DIST,
			m_fPopupHide_CurTime / POPUP_HIDE_TIME_LIMIT);
	}
}

void CItem_Mesh_Viewer::Hide_Operation(_float fTimeDelta)
{
	m_fPopupHide_CurTime += fTimeDelta;

	if (m_fPopupHide_CurTime > POPUP_HIDE_TIME_LIMIT)
	{
		m_eViewer_State = POP_UP;
		m_fPopupHide_CurTime = 0.f;
		m_fDistCam = POPUP_HIDE_START_DIST;
		m_bDead = true;
		m_eItem_Number = ITEM_NUMBER_END;
		return;
	}

	if (1.f > m_fPopupHide_CurTime / POPUP_HIDE_TIME_LIMIT)
	{
		m_fDistCam = m_pGameInstance->Get_Ease(Ease_OutQuint, POPUP_HIDE_END_DIST, POPUP_HIDE_START_DIST,
			m_fPopupHide_CurTime / POPUP_HIDE_TIME_LIMIT);
	}
}

void CItem_Mesh_Viewer::Set_Operation(UI_OPERRATION eOperation, ITEM_NUMBER eCallItemType)
{
	switch (eOperation)
	{
	case Client::POP_UP: {
		m_bDead = false;
		m_eItem_Number = eCallItemType;
		m_eViewer_State = eOperation;
		break;
	}
		
	case Client::UI_IDLE: {
		m_bDead = false;
		m_eItem_Number = eCallItemType;
		m_eViewer_State = eOperation;
		break;
	}

	case Client::UI_SECOND_IDLE: {
		m_bDead = false;
		m_eItem_Number = eCallItemType;
		m_eViewer_State = eOperation;
		break;
	}

		
	case Client::HIDE: {
		m_bDead = false;
		//m_eItem_Number = eCallItemType;
		m_eViewer_State = eOperation;
		break;
	}
		
	default:
		break;
	}
}

HRESULT CItem_Mesh_Viewer::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(Load_ItemsModel()))
		return E_FAIL;

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


HRESULT CItem_Mesh_Viewer::Load_ItemsModel()
{
	wstring wstrModelComTag = TEXT("Com_Model");
	
	for (_uint i = 0; i < m_vecModelTag.size(); i++)
	{
		CModel* pItemModel = nullptr;

		wstring ModelTagTrashCan = wstrModelComTag + to_wstring(i);

		/* For.Com_Model */
		if (FAILED(__super::Add_Component(g_Level, m_vecModelTag[i],
			ModelTagTrashCan, (CComponent**)&pItemModel)))
			return E_FAIL;

		m_vecModelCom.push_back(pItemModel);
	}

	return S_OK;
}

HRESULT CItem_Mesh_Viewer::Load_ItemModelTags()
{
	wstring filePath = TEXT("../Bin/DataFiles/Scene_TabWindow/Inventory/Item_Prototype.dat");

	HANDLE		hFile = CreateFile(filePath.c_str(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	DWORD	dwByte(0);

	_uint iObjectNum = { 0 };
	if (!ReadFile(hFile, &iObjectNum, sizeof(_uint), &dwByte, nullptr))
		return E_FAIL;

	for (_uint i = 0; iObjectNum > i; ++i)
	{

		PROTOTYPE_INFORM* Inform = new PROTOTYPE_INFORM;

		_uint dwLen = { 0 };

		_bool bAnim = { false };
		if (!ReadFile(hFile, &bAnim, sizeof(_uint), &dwByte, nullptr))
		{
			Safe_Delete(Inform);
			CloseHandle(hFile);
			return E_FAIL;
		}
		Inform->bAnim = bAnim;


		if (!ReadFile(hFile, &dwLen, sizeof(_uint), &dwByte, nullptr))
		{
			Safe_Delete(Inform);
			CloseHandle(hFile);
			return E_FAIL;
		}
		char* strModelPath = new char[dwLen / sizeof(char) + 1];
		if (!ReadFile(hFile, strModelPath, dwLen, &dwByte, nullptr))
		{
			delete[] strModelPath;
			Safe_Delete(Inform);
			CloseHandle(hFile);

			return E_FAIL;
		}
		strModelPath[dwLen / sizeof(char)] = '\0';
		Inform->strModelPath = strModelPath;
		delete[] strModelPath;


		if (!ReadFile(hFile, &dwLen, sizeof(_uint), &dwByte, nullptr))
		{
			Safe_Delete(Inform);
			CloseHandle(hFile);
			return E_FAIL;
		}
		wchar_t* wstrModelPrototypeName = new wchar_t[dwLen / sizeof(wchar_t) + 1];
		if (!ReadFile(hFile, wstrModelPrototypeName, dwLen, &dwByte, nullptr))
		{
			Safe_Delete(Inform);
			delete[] wstrModelPrototypeName;
			CloseHandle(hFile);
			return E_FAIL;
		}
		wstrModelPrototypeName[dwLen / sizeof(wchar_t)] = L'\0';
		Inform->wstrModelPrototypeName = wstrModelPrototypeName;
		delete[] wstrModelPrototypeName;

		if (!ReadFile(hFile, &dwLen, sizeof(_uint), &dwByte, nullptr))
		{
			CloseHandle(hFile);
			Safe_Delete(Inform);
			return E_FAIL;
		}
		wchar_t* wstrGameObjectPrototypeName = new wchar_t[dwLen / sizeof(wchar_t) + 1];
		if (!ReadFile(hFile, wstrGameObjectPrototypeName, dwLen, &dwByte, nullptr))
		{
			Safe_Delete(Inform);
			delete[] wstrGameObjectPrototypeName;
			CloseHandle(hFile);
			return E_FAIL;
		}
		wstrGameObjectPrototypeName[dwLen / sizeof(wchar_t)] = L'\0';
		Inform->wstrGameObjectPrototypeName = wstrGameObjectPrototypeName;
		delete[] wstrGameObjectPrototypeName;

		if (!ReadFile(hFile, &dwLen, sizeof(_uint), &dwByte, nullptr))
		{
			CloseHandle(hFile);
			Safe_Delete(Inform);
			return E_FAIL;
		}
		char* strGameObjectPrototypeName = new char[dwLen / sizeof(char) + 1];
		if (!ReadFile(hFile, strGameObjectPrototypeName, dwLen, &dwByte, nullptr))
		{
			Safe_Delete(Inform);
			delete[] strGameObjectPrototypeName;

			CloseHandle(hFile);
			return E_FAIL;
		}
		strGameObjectPrototypeName[dwLen / sizeof(char)] = '\0';
		Inform->strGameObjectPrototypeName = strGameObjectPrototypeName;
		delete[] strGameObjectPrototypeName;

		if (Inform->wstrGameObjectPrototypeName.find(TEXT("sm7")) != wstring::npos)
		{
			m_vecModelTag.push_back(Inform->wstrModelPrototypeName);//УЂБе
		}

		Safe_Delete(Inform);

	}

	CloseHandle(hFile);
	return S_OK;



}