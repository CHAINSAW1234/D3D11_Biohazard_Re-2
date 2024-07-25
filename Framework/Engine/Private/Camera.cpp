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
	if (true == m_isInterpolate)
	{
		m_fAccInterpoaltionTime += fTimeDelta;
		if (1.f <= m_fAccInterpoaltionTime)
			m_isInterpolate = false;
	}
}

void CCamera::Late_Tick(_float fTimeDelta)
{
}

void CCamera::Set_Interpolation(_float fStartFovY)
{
	m_isInterpolate = true;
	m_fPreFovY = fStartFovY;
	m_fAccInterpoaltionTime = 0.f;
	m_PreCamWorldMatrix = m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_VIEW);
}


HRESULT CCamera::Bind_PipeLines()
{	
	if (!m_isActive)
		return E_FAIL;

	_float			fFovY = { m_fFovy };

	if (true == m_isInterpolate)
	{
		_float				fRatio = { fminf(m_fAccInterpoaltionTime / 0.2f, 1.f) };
		if (fRatio < 1.f)
		{
			_matrix				WorldMatrix = { m_pTransformCom->Get_WorldMatrix() };
			_matrix				LastWorldMatrix = { XMLoadFloat4x4(&m_PreCamWorldMatrix) };
			_matrix				ResultWorldMatrix;

			_vector				vLastScale, vLastQuaternion, vLastTranslation;
			XMMatrixDecompose(&vLastScale, &vLastQuaternion, &vLastTranslation, LastWorldMatrix);

			_vector				vEventScale, vEventQuaternion, vEventTranslation;
			XMMatrixDecompose(&vEventScale, &vEventQuaternion, &vEventTranslation, WorldMatrix);

			_vector				vResultScale = { XMVectorLerp(vLastScale,vEventScale, fRatio) };
			_vector				vResultQuaternion = { XMQuaternionSlerp(XMQuaternionNormalize(vLastQuaternion),XMQuaternionNormalize(vEventQuaternion), fRatio) };
			_vector				vResultTranslation = { XMVectorLerp(vLastTranslation,vEventTranslation, fRatio) };

			WorldMatrix = XMMatrixAffineTransformation(vResultScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vResultQuaternion, vResultTranslation);
			_matrix				WorldMatrixInv = { XMMatrixInverse(nullptr, WorldMatrix) };
			m_pGameInstance->Set_Transform(CPipeLine::D3DTS_VIEW, WorldMatrixInv);

			fFovY = fFovY * fRatio + (1.f - fRatio) * m_fPreFovY;
		}
		else
		{
			m_isInterpolate = false;
			m_pGameInstance->Set_Transform(CPipeLine::D3DTS_VIEW, m_pTransformCom->Get_WorldMatrix_Inverse());
		}
	}
	else
	{
		m_pGameInstance->Set_Transform(CPipeLine::D3DTS_VIEW, m_pTransformCom->Get_WorldMatrix_Inverse());
	}

	m_pGameInstance->Set_Transform(CPipeLine::D3DTS_PROJ, XMMatrixPerspectiveFovLH(fFovY, m_fAspect, m_fNear, m_fFar));
	
	CPipeLine::FRUSTUM_DESC FrustumDesc = {};
	FrustumDesc.fAspect = m_fAspect;
	FrustumDesc.fFar = m_fFar;
	FrustumDesc.fNear = m_fNear;
	FrustumDesc.fFovy = fFovY;
	//m_pGameInstance->Set_Frustum(FrustumDesc);

	return S_OK;
}

void CCamera::Free()
{
	__super::Free();


}
