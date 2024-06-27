#include "stdafx.h"
#include "..\Public\Camera_Free.h"
#include "player.h"

CCamera_Free::CCamera_Free(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCamera{ pDevice, pContext }
{
}

CCamera_Free::CCamera_Free(const CCamera_Free & rhs)
	: CCamera{ rhs }
{
}

HRESULT CCamera_Free::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Free::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;
	
	CAMERA_FREE_DESC*	pCameraFree = (CAMERA_FREE_DESC*)pArg;

	m_fMouseSensor = pCameraFree->fMouseSensor;
	m_fOriginFovy = pCameraFree->fFovy;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pGameInstance->Set_Camera_Transform(m_pTransformCom);

	return S_OK;
}

void CCamera_Free::Tick(_float fTimeDelta)
{
	__super::Bind_PipeLines();
}

_float CCamera_Free::EaseInQuint(_float start, _float end, _float value)
{
	value--;
	end -= start;
	return end * (value * value * value * value * value + 1) + start;
}


void CCamera_Free::Late_Tick(_float fTimeDelta)
{
}

void CCamera_Free::SetPosition(_float4 vPosition)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
}

void CCamera_Free::SetPosition(_vector vPosition)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
}

_float4 CCamera_Free::Get_Position_Float4()
{
	return m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
}

_vector CCamera_Free::Get_Position_Vector()
{
	return m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
}

void CCamera_Free::SetPlayer(CGameObject* pPlayer)
{
	auto Player = dynamic_cast<CPlayer*>(pPlayer);
	if (Player)
		m_pPlayer = Player;
}

void CCamera_Free::LookAt(_float4 vPoint)
{
	if (m_pTransformCom)
	{
		m_pTransformCom->Look_At(XMLoadFloat4(&vPoint));
	}
}

HRESULT CCamera_Free::Render()
{
	return S_OK;
}

CCamera_Free * CCamera_Free::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCamera_Free*		pInstance = new CCamera_Free(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCamera_Free"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject * CCamera_Free::Clone(void * pArg)
{
	CCamera_Free*		pInstance = new CCamera_Free(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CCamera_Free"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Free::Free()
{
	__super::Free();

}
