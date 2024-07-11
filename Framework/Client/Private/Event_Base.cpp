#include "stdafx.h"
#include "Event_Base.h"

#include"InteractProps.h"
#include"CustomCollider.h"
CEvent_Base::CEvent_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEvent{ pDevice, pContext }
{

}

HRESULT CEvent_Base::Initialize(void* pArg)
{
	return S_OK;
}

CEvent::STATE CEvent_Base::Tick(_float fTimeDelta)
{


	return m_eState;
}

HRESULT CEvent_Base::Load_Collider(const wstring& strFile, const wstring& strColLayerTag, _int iLevel, _float4 vColliderColor)
{
	wstring	strFilePath = strFile + L"\\\\" + strColLayerTag + L".dat";

	HANDLE		hFile = CreateFile(strFilePath.c_str(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
		return S_OK;


	DWORD	dwByte(0);

	_uint iObjectNum = { 0 };
	if (!ReadFile(hFile, &iObjectNum, sizeof(_uint), &dwByte, nullptr))
	{
		CloseHandle(hFile);
		return E_FAIL;
	}

	for (_uint i = 0; iObjectNum > i; ++i)
	{
		_float4x4 WorldMatrix = {};
		if (!ReadFile(hFile, &WorldMatrix, sizeof(_float4x4), &dwByte, nullptr))
		{
			CloseHandle(hFile);
			return E_FAIL;
		}
		_int iNum = { 0 };

		if (!ReadFile(hFile, &iNum, sizeof(_int), &dwByte, NULL))
		{
			CloseHandle(hFile);
			return E_FAIL;
		}

		_int iDir = { 0 };

		if (!ReadFile(hFile, &iDir, sizeof(_int), &dwByte, NULL))
		{
			CloseHandle(hFile);
			return E_FAIL;
		}

		_int iRegionNum = { 0 };

		if (!ReadFile(hFile, &iRegionNum, sizeof(_int), &dwByte, NULL))
		{
			CloseHandle(hFile);
			return E_FAIL;
		}

		_int iFloor = { 0 };
		if (!ReadFile(hFile, &iFloor, sizeof(_int), &dwByte, NULL))
		{
			CloseHandle(hFile);
			return E_FAIL;
		}

		CCustomCollider::COLLIDER_DESC collider_desc = {};
		collider_desc.worldMatrix = WorldMatrix;
		collider_desc.iColNum = iNum;
		collider_desc.iDir = iDir;
		collider_desc.iRegionNum = iRegionNum;
		collider_desc.iFloor = iFloor;
		collider_desc.vColor = vColliderColor;
		if (FAILED(m_pGameInstance->Add_Clone(iLevel, strColLayerTag, TEXT("Prototype_GameObject_Collider"), &collider_desc)))
		{
			MSG_BOX(TEXT("Failed to Add_Clone Prototype_GameObject_Monster: CImGUI"));
			return E_FAIL;
		}

	}
	CloseHandle(hFile);
	return S_OK;
}



void CEvent_Base::Free()
{
	__super::Free();
}
