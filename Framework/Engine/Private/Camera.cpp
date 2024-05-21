#include "..\Public\Camera.h"
#include "GameInstance.h"

CCamera::CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{


}

CCamera::CCamera(const CCamera & rhs)
	: CGameObject(rhs)
{
}

HRESULT CCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera::Initialize(void * pArg)
{
	CAMERA_DESC*		pCameraDesc = (CAMERA_DESC*)pArg;

	m_fFovy = pCameraDesc->fFovy;
	m_fAspect = pCameraDesc->fAspect;
	m_fNear = pCameraDesc->fNear;
	m_fFar = pCameraDesc->fFar;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pCameraDesc->vEye));
	m_pTransformCom->Look_At(XMLoadFloat4(&pCameraDesc->vAt));

	return S_OK;
}

void CCamera::Tick(_float fTimeDelta)
{
}

void CCamera::Late_Tick(_float fTimeDelta)
{
}


HRESULT CCamera::Bind_PipeLines()
{	
	_vector			vUp = { m_pTransformCom->Get_State_Vector(CTransform::STATE_UP) };
	_vector			vLook = { m_pTransformCom->Get_State_Vector(CTransform::STATE_LOOK) };
	_vector			vPos = { m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION) };

	_matrix			ViewMatrix = { XMMatrixLookAtLH(vPos, vLook + vPos, XMVectorSet(0.f, 1.f, 0.f, 0.f))};
	_matrix			ProjMatrix = { XMMatrixPerspectiveFovLH(m_fFovy, m_fAspect, m_fNear, m_fFar) };

	XMStoreFloat4x4(&m_ViewMatrix, ViewMatrix);
	XMStoreFloat4x4(&m_ProjMatrix, ProjMatrix);

	m_pGameInstance->Set_Transform(CPipeLine::D3DTS_PROJ, XMLoadFloat4x4(&m_ProjMatrix));
	m_pGameInstance->Set_Transform(CPipeLine::D3DTS_VIEW, XMLoadFloat4x4(&m_ViewMatrix));

	return S_OK;
}

void CCamera::Free()
{
	__super::Free();


}
