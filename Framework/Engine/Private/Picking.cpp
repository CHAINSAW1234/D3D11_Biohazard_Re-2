#include "..\Public\Picking.h"
#include "Transform.h"
#include "GameInstance.h"

CPicking::CPicking(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:m_pDevice(pDevice),
	m_pContext(pContext)
{

}

HRESULT CPicking::Initialize(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
{
	m_hWnd = hWnd;

	m_iWinSizeX = iWinSizeX;
	m_iWinSizeY = iWinSizeY;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	return S_OK;
}

void CPicking::Update()
{
	POINT			ptMouse;
	GetCursorPos(&ptMouse);

	ScreenToClient(m_hWnd, &ptMouse);

	/*뷰변환*/
	m_fViewMousePos.x = (_float)ptMouse.x;
	m_fViewMousePos.y = (_float)ptMouse.y;

	_float3		vMousePos;
	_float3		vMouseEnd;

	vMousePos.x = ptMouse.x / (m_iWinSizeX * 0.5f) - 1.f;
	vMousePos.y = ptMouse.y / -(m_iWinSizeY * 0.5f) + 1.f;
	vMousePos.z = 0.f;

	/*투영 변환*/
	m_fProjMousePos.x = ptMouse.x / (m_iWinSizeX * 0.5f) - 1.f;
	m_fProjMousePos.y = ptMouse.y / -(m_iWinSizeY * 0.5f) + 1.f;

	vMouseEnd.x = ptMouse.x / (m_iWinSizeX * 0.5f) - 1.f;
	vMouseEnd.y = ptMouse.y / -(m_iWinSizeY * 0.5f) + 1.f;
	vMouseEnd.z = 1.f;

	_matrix	ProjMatrixInv;
	ProjMatrixInv = CGameInstance::Get_Instance()->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_PROJ);

	_vector vecMousePos = XMLoadFloat3(&vMousePos);
	_vector vecMouseEnd = XMLoadFloat3(&vMouseEnd);

	vecMousePos = XMVector3TransformCoord(vecMousePos, ProjMatrixInv);
	vecMouseEnd = XMVector3TransformCoord(vecMouseEnd, ProjMatrixInv);

	_vector vecRayDir = vecMouseEnd - vecMousePos;

	_matrix 	ViewMatrixInv;
	ViewMatrixInv = CGameInstance::Get_Instance()->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_VIEW);

	m_vRayPos = Convert_Vector_To_Float3(XMVector3TransformCoord(vecMousePos, ViewMatrixInv));
	m_vRayDir = Convert_Vector_To_Float3(XMVector3Normalize(XMVector3TransformNormal(vecRayDir, ViewMatrixInv)));
}

void CPicking::Transform_PickingToLocalSpace(CTransform* pTransform, _Out_ _float3* pRayDir, _Out_ _float3* pRayPos)
{
	//XMFLOAT4 VecDir = Convert_Float3_To_Float4_Vec(m_vRayDir);

	//_vector vRayDir = XMLoadFloat4(&VecDir);

	//XMFLOAT4 VecPos = Convert_Float3_To_Float4_Coord(m_vRayPos);

	//_vector vRayPos = XMLoadFloat4(&VecPos);

	////_matrix Pure = pTransform->Get_WorldMatrix_Pure_Mat();
	//_matrix Inverse_WorldMat = XMMatrixInverse(nullptr, Pure);

	//vRayDir = XMVector4Transform(vRayDir, Inverse_WorldMat);
	//vRayPos = XMVector4Transform(vRayPos, Inverse_WorldMat);

	//*pRayDir = Convert_Vector_To_Float3(vRayDir);
	//*pRayPos = Convert_Vector_To_Float3(vRayPos);
}

void CPicking::Transform_PickingToWorldSpace(_float4* pRayDir, _float4* pRayPos)
{
	XMFLOAT4 VecDir = Convert_Float3_To_Float4_Dir(m_vRayDir);

	_vector vRayDir = XMLoadFloat4(&VecDir);

	XMFLOAT4 VecPos = Convert_Float3_To_Float4_Coord(m_vRayPos);

	_vector vRayPos = XMLoadFloat4(&VecPos);

	XMStoreFloat4(pRayDir, vRayDir);
	XMStoreFloat4(pRayPos, vRayPos);
}

void CPicking::Get_PickingWordSpace(_float3* pRayDir, _float3* pRayPos)
{
	XMStoreFloat3(pRayDir, XMLoadFloat3(&m_vRayDir));
	XMStoreFloat3(pRayPos, XMLoadFloat3(&m_vRayPos));
}

void CPicking::Get_PickingWordSpace(_vector& pRayDir, _vector& pRayPos)
{
	pRayDir = XMLoadFloat3(&m_vRayDir);
	pRayPos = XMLoadFloat3(&m_vRayPos);
}

void CPicking::ClipCursor(HWND hWnd)
{
	RECT rect;
	GetClientRect(hWnd, &rect);

	_float x = rect.right * 0.5f;
	_float y = rect.bottom * 0.5f;

	POINT pt = { (_long)x, (_long)y };
	ClientToScreen(hWnd, &pt);

	SetCursorPos(pt.x, pt.y);
}

CPicking* CPicking::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
{
	CPicking* pInstance = new CPicking(pDevice, pContext);

	if (FAILED(pInstance->Initialize(hWnd, iWinSizeX, iWinSizeY)))
	{
		MSG_BOX(TEXT("Failed To Created : CPicking"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPicking::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

