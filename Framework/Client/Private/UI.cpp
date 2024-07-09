#include "stdafx.h"
#include "UI.h"

CUI::CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CUI::CUI(const CUI& rhs)
	: CGameObject{ rhs }
{

}

HRESULT CUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI::Initialize(void* pArg)
{
	if (pArg != nullptr)
	{
		UI_DESC* UIDesc = (UI_DESC*)pArg;
		UIDesc->fSpeedPerSec = 1.f;
		UIDesc->fRotationPerSec = XMConvertToRadians(100.0f);

		if (FAILED(__super::Initialize(UIDesc)))
			return E_FAIL;


		if (UIDesc->vSize.x == 0 && UIDesc->vSize.y == 0)
		{
			m_pTransformCom->Set_WorldMatrix(UIDesc->worldMatrix);
			XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
			XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));
		}

		else
		{
			m_fSizeX = UIDesc->vSize.x;
			m_fSizeY = UIDesc->vSize.y;
			m_fX = UIDesc->vPos.x;
			m_fY = UIDesc->vPos.y;
			m_fZ = UIDesc->vPos.z;

			m_fAlpha = UIDesc->fAlpha;

			m_pTransformCom->Set_Scaled(m_fSizeX, m_fSizeY, 1.f);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(
				m_fX - g_iWinSizeX * 0.5f,
				-m_fY + g_iWinSizeY * 0.5f,
				m_fZ,
				1.f
			));

			XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
			XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));
		}
	}

	m_vPrePos = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
	_float3 fPreScale = m_pTransformCom->Get_Scaled();
	m_vPreScale = XMVectorSet(fPreScale.x, fPreScale.y, fPreScale.z, 1.f);

	return S_OK;
}

void CUI::Tick(_float fTimeDelta)
{
	_float4 fPos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

	m_fX = fPos.x + g_iWinSizeX * 0.5f;
	m_fY = -fPos.y + g_iWinSizeY * 0.5f;
	m_fZ = fPos.z;

	_float3 fSize = m_pTransformCom->Get_Scaled();

	m_fSizeX = fSize.x;
	m_fSizeY = fSize.y;

}

void CUI::Late_Tick(_float fTimeDelta)
{
	m_vPrePos = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);

	_float3 fPreScale = m_pTransformCom->Get_Scaled();

	m_vPreScale = XMVectorSet(fPreScale.x, fPreScale.y, fPreScale.z, 1.f);
}

void CUI::Set_Size(_float fSizeX, _float fSizeY)
{
	m_pTransformCom->Set_Scaled(fSizeX, fSizeY, 1);
}

void CUI::Set_Position(_float fPosX, _float fPosY, _float fPosZ)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float4(fPosX, fPosY, fPosZ, 0));
}

void CUI::Set_Position(_vector vPos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
}

void CUI::Size_Stretch(_float fConversionValX, _float fConversionValY)
{
	//m_pTransformCom->Set_WorldMatrix(XMMatrixIdentity());

	_float fSizeX = m_fSizeX + fConversionValX;
	_float fSizeY = m_fSizeY + fConversionValY;

	m_pTransformCom->Set_Scaled(fSizeX, fSizeY, 1);
}

void CUI::Move(_float3 fMoveMent)
{
	_vector	vPosition = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
	vPosition += XMLoadFloat3(&fMoveMent);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
}

void CUI::Move_State(_float3 fMoveMent, _int iState)
{
	switch (iState)
	{
	case 0: {
		_vector	vPosition = m_pTransformCom -> Get_State_Vector(CTransform::STATE_POSITION);
		vPosition += XMLoadFloat3(&fMoveMent);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
		break;
	}

	case 1: {
		_float3	fScale = m_pTransformCom->Get_Scaled();
		_vector vScale = XMLoadFloat3(&fScale) + XMLoadFloat3(&fMoveMent);
		fScale = {};
		XMStoreFloat3(&fScale, vScale);
		m_pTransformCom->Set_Scaled(fScale.x, fScale.y, fScale.z);
		break;
	}

	default:
		break;
	}
}

void CUI::State(_float3 fTrans)
{
	m_fX = fTrans.x + g_iWinSizeX * 0.5f;
	m_fY = -fTrans.y + g_iWinSizeY * 0.5f;
	m_fZ = fTrans.z;
}

_float3 CUI::ComputeMovement(_uint iSTATE)
{
	switch (iSTATE)
	{
	case 0: {
		_vector vMove = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION) - m_vPrePos;
		_float3 fMove = {};
		XMStoreFloat3(&fMove, vMove);
		return fMove;
		break;
	}
		
	case 1: {
		_float3 fScaled = m_pTransformCom->Get_Scaled();
		_vector vScaled = XMLoadFloat3(&fScaled) - m_vPreScale;
		fScaled = {};
		XMStoreFloat3(&fScaled, vScaled);
		return fScaled;
		break;
	}
		
	default:
		break;
	}

	return _float3();
}

_bool CUI::IsMouseHover()
{
	_float2 mouse = m_pGameInstance->Get_ViewMousePos();

	_float4 vPosition = { m_pTransformCom->Get_WorldFloat4x4()._41 + g_iWinSizeX * 0.5f, -m_pTransformCom->Get_WorldFloat4x4()._42 + g_iWinSizeY * 0.5f, 0, 0 };

	_float3 vSize = m_pTransformCom->Get_Scaled();

	/* UI를 1080 720 상에서 만든 것이라 정확히 3번 나눈 비율 값인 105와 45를 집어넣어줌*/
	/*
	float fwinsize = g_iWinSizeX - 1080;
	fwinsize = fwinsize / 8 
	*/
	if (vPosition.x - (vSize.x / 2)  <= mouse.x && vPosition.y - (vSize.y / 2)  <= mouse.y
		&& vPosition.x + (vSize.x / 2)  >= mouse.x && vPosition.y + (vSize.y / 2) >= mouse.y)
	{
		return true;
	}

	return false;
}

_bool CUI::IsMouseHover(_float& fPosZ)
{
	_float2 mouse = m_pGameInstance->Get_ViewMousePos();

	_float4 vPosition = { m_pTransformCom->Get_WorldFloat4x4()._41 + g_iWinSizeX * 0.5f, 
							-m_pTransformCom->Get_WorldFloat4x4()._42 + g_iWinSizeY * 0.5f, 
								0, 0 };
	_float3 vSize = m_pTransformCom->Get_Scaled();

	if (vPosition.x - (vSize.x / 2) <= mouse.x && vPosition.y - (vSize.y / 2) <= mouse.y
		&& vPosition.x + (vSize.x / 2) >= mouse.x && vPosition.y + (vSize.y / 2) >= mouse.y)
	{
		fPosZ = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION).z;
		return true;
	}

	return false;
}

_bool CUI::IsPTInRect(_float2 fPoint)
{
	//_float4 vPosition = { m_pTransformCom->Get_WorldFloat4x4()._41/* + g_iWinSizeX * 0.5f*/, 
	//						-m_pTransformCom->Get_WorldFloat4x4()._42 /*+ g_iWinSizeY * 0.5f*/, 
	//							0, 0 };

	//_float3 vSize = m_pTransformCom->Get_Scaled();

	//if (vPosition.x - (vSize.x / 2) <= fPoint.x && vPosition.y - (vSize.y / 2) <= fPoint.y
	//	&& vPosition.x + (vSize.x / 2) >= fPoint.x && vPosition.y + (vSize.y / 2) >= fPoint.y)
	//{
	//	return true;
	//}

	_float4 fPos = GetPosition();

	_float3 fSize = m_pTransformCom->Get_Scaled();

	_float2 fLeftTop = { fPos.x - fSize.x * 0.5f , fPos.y + fSize.y * 0.5f };
	_float2 fRightBottom = { fPos.x + fSize.x * 0.5f , fPos.y - fSize.y * 0.5f };

	if (fLeftTop.x <= fPoint.x && fLeftTop.y >= fPoint.y && fRightBottom.x >= fPoint.x && fRightBottom.y <= fPoint.y)
		return true;
	/*<= ,  >= ,  >= ,  <=   */ 

	return false;
}

void CUI::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pSubMaskTextureCom);
	Safe_Release(m_pMaskTextureCom);
	Safe_Release(m_pVIBufferCom);
}
