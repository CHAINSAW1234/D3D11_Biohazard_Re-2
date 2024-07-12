#include "stdafx.h"
#include "Camera_Gimmick.h"
#include "player.h"

CCamera_Gimmick::CCamera_Gimmick(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCamera{ pDevice, pContext }
{
}

CCamera_Gimmick::CCamera_Gimmick(const CCamera_Gimmick & rhs)
	: CCamera{ rhs }
{
}

HRESULT CCamera_Gimmick::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Gimmick::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;
	
	CAMERA_GIMMICK_DESC*	pCameraFree = (CAMERA_GIMMICK_DESC*)pArg;

	m_fMouseSensor = pCameraFree->fMouseSensor;
	m_fOriginFovy = pCameraFree->fFovy;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pGameInstance->Set_Camera_Transform(m_pTransformCom);

	return S_OK;
}

void CCamera_Gimmick::Tick(_float fTimeDelta)
{
	__super::Bind_PipeLines();
}

void CCamera_Gimmick::Late_Tick(_float fTimeDelta)
{
}

void CCamera_Gimmick::SetPosition(_float4 vPosition)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
}

void CCamera_Gimmick::SetPosition(_vector vPosition)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
}

_float4 CCamera_Gimmick::Get_Position_Float4()
{
	return m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
}

_vector CCamera_Gimmick::Get_Position_Vector()
{
	return m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
}

void CCamera_Gimmick::SetPlayer(CGameObject* pPlayer)
{
	auto Player = dynamic_cast<CPlayer*>(pPlayer);
	if (Player)
		m_pPlayer = Player;
}

void CCamera_Gimmick::LookAt(_float4 vPoint)
{
	if (m_pTransformCom)
	{
		m_pTransformCom->Look_At(XMLoadFloat4(&vPoint));
	}
}

HRESULT CCamera_Gimmick::Render()
{
	return S_OK;
}

void CCamera_Gimmick::Bind_PipeLine()
{
	__super::Bind_PipeLines();
}

void CCamera_Gimmick::Active_Camera(_bool isActive)
{
	m_isActive = isActive;
	if(m_pPlayer)
		m_pPlayer->ResetCamera();
	if (m_isActive)
		Bind_PipeLines();
}

CCamera_Gimmick * CCamera_Gimmick::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCamera_Gimmick*		pInstance = new CCamera_Gimmick(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCamera_Gimmick"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject * CCamera_Gimmick::Clone(void * pArg)
{
	CCamera_Gimmick*		pInstance = new CCamera_Gimmick(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CCamera_Gimmick"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Gimmick::Free()
{
	__super::Free();

}

