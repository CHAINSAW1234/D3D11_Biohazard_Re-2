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
	if (PRESSING == m_pGameInstance->Get_KeyState(VK_MBUTTON))
	{
		if (PRESSING == m_pGameInstance->Get_KeyState('A'))
		{
			m_pTransformCom->Go_Left(fTimeDelta / 1.5f);
		}
		if (PRESSING == m_pGameInstance->Get_KeyState('D'))
		{
			m_pTransformCom->Go_Right(fTimeDelta / 1.5f);
		}
		if (PRESSING == m_pGameInstance->Get_KeyState('W'))
		{
			m_pTransformCom->Go_Straight(fTimeDelta / 1.5f);
		}
		if (PRESSING == m_pGameInstance->Get_KeyState('S'))
		{
			m_pTransformCom->Go_Backward(fTimeDelta / 1.5f);
		}

		POINT		ptDeltaPos = { m_pGameInstance->Get_MouseDeltaPos() };
		_vector		vUp = { XMVectorSet(0.f, 1.f, 0.f, 0.f) };

		if ((_long)0 != ptDeltaPos.x)
		{
			m_pTransformCom->Turn(vUp, fTimeDelta * (_float)ptDeltaPos.x * m_fMouseSensor);
		}

		_vector		vRight = { m_pTransformCom->Get_State_Vector(CTransform::STATE_RIGHT) };

		if ((_long)0 != ptDeltaPos.y)
		{
			m_pTransformCom->Turn(vRight, fTimeDelta * (_float)ptDeltaPos.y * m_fMouseSensor);
		}

		POINT ptPos = {};

		GetCursorPos(&ptPos);
		ScreenToClient(g_hWnd, &ptPos);

		RECT rc = {};
		GetClientRect(g_hWnd, &rc);

		ptPos.x = _long(_float(rc.right - rc.left) * 0.5f);
		ptPos.y = _long(_float(rc.bottom - rc.top) * 0.5f);

		m_pGameInstance->Set_MouseCurPos(ptPos);

		ClientToScreen(g_hWnd, &ptPos);
		SetCursorPos(ptPos.x, ptPos.y);
	}

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

