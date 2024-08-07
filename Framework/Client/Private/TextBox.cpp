#include "stdafx.h"
#include "TextBox.h"

CTextBox::CTextBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CTextBox::CTextBox(const CTextBox& rhs)
	: CUI{ rhs }
{

}

HRESULT CTextBox::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTextBox::Initialize(void* pArg)
{
	if (pArg != nullptr)
	{
		if (FAILED(__super::Initialize(pArg)))
			return E_FAIL;

		CTextBox::TextBox_DESC*  TextBoxDesc = (TextBox_DESC*)pArg;

		m_wstrText = TextBoxDesc->wstrText;
		m_wstrFontType = TextBoxDesc->wstrFontType;
		m_vFontColor = TextBoxDesc->vFontColor;
		m_iFontSize = TextBoxDesc->iFontSize;
		m_isOuterLine = TextBoxDesc->isOuterLine;
		m_vOutLineColor = TextBoxDesc->vOutLineColor;
		m_isUIRender = TextBoxDesc->isUI_Render;

		m_fX *= static_cast<_float>(g_iWinSizeX) / 1600.f;
		m_fY *= static_cast<_float>(g_iWinSizeY) / 900.f;
		m_fSizeX *= static_cast<_float>(g_iWinSizeX) / 1600.f;
		m_fSizeY *= static_cast<_float>(g_iWinSizeY) / 900.f;

		Set_Position(XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

		m_vTextPos = { m_fX, m_fY, 0.5f };
	}

	return S_OK;
}

void CTextBox::Tick(_float fTimeDelta)
{
	if (true == m_isTransformBase)
	{
		__super::Tick(fTimeDelta);
	}
}

void CTextBox::Late_Tick(_float fTimeDelta)
{
	if(true == m_isUIRender)
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);
	else 
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_FONT, this);
}

HRESULT CTextBox::Render()
{
	wstring strFontType = m_wstrFontType + to_wstring(m_iFontSize);

	//_float4 fPos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

	//if (true == m_isOuterLine)
	//{
	//	if (FAILED(m_pGameInstance->Render_Font(strFontType, m_wstrText, XMVectorSet(m_fX - 2, m_fY, m_fZ, 0.f), m_vOutLineColor, 0.f)))
	//		return E_FAIL;
	//	if (FAILED(m_pGameInstance->Render_Font(strFontType, m_wstrText, XMVectorSet(m_fX + 2, m_fY, m_fZ, 0.f), m_vOutLineColor, 0.f)))
	//		return E_FAIL;
	//	if (FAILED(m_pGameInstance->Render_Font(strFontType, m_wstrText, XMVectorSet(m_fX, m_fY - 2, m_fZ, 0.f), m_vOutLineColor, 0.f)))
	//		return E_FAIL;
	//	if (FAILED(m_pGameInstance->Render_Font(strFontType, m_wstrText, XMVectorSet(m_fX, m_fY + 2, m_fZ, 0.f), m_vOutLineColor, 0.f)))
	//		return E_FAIL;
	//}

	//if (FAILED(m_pGameInstance->Render_Font(strFontType, m_wstrText, XMVectorSet(m_fX, m_fY, m_fZ, 0.f), m_vFontColor, 0.f)))
	//	return E_FAIL;

	if (true == m_isOuterLine)
	{
		if (FAILED(m_pGameInstance->Render_Font(strFontType, m_wstrText, XMVectorSet(m_vTextPos.x - 2, m_vTextPos.y , m_vTextPos.z, 0.f), m_vOutLineColor, 0.f, static_cast<_float>(g_iWinSizeX) / 1600.f)))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Render_Font(strFontType, m_wstrText, XMVectorSet(m_vTextPos.x + 2, m_vTextPos.y , m_vTextPos.z, 0.f), m_vOutLineColor, 0.f, static_cast<_float>(g_iWinSizeX) / 1600.f)))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Render_Font(strFontType, m_wstrText, XMVectorSet(m_vTextPos.x, m_vTextPos.y - 2, m_vTextPos.z, 0.f), m_vOutLineColor, 0.f, static_cast<_float>(g_iWinSizeX) / 1600.f)))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Render_Font(strFontType, m_wstrText, XMVectorSet(m_vTextPos.x, m_vTextPos.y + 2, m_vTextPos.z, 0.f), m_vOutLineColor, 0.f, static_cast<_float>(g_iWinSizeX) / 1600.f)))
			return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Render_Font(strFontType, m_wstrText, XMVectorSet(m_vTextPos.x, m_vTextPos.y, m_vTextPos.z, 0.f), m_vFontColor, 0.f, static_cast<_float>(g_iWinSizeX) / 1600.f)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTextBox::Convert_Resolution()
{
	_float		widthRatio = static_cast<_float>(g_iWinSizeX) / 1600.f;
	_float		heightRatio = static_cast<_float>(g_iWinSizeY) / 900.0f;

	_float4		originalPos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
	_float3		originalScale = m_pTransformCom->Get_Scaled();

	_float		newPosX = originalPos.x * widthRatio;
	_float		newPosY = originalPos.y * heightRatio;

	_float		newWidth = originalScale.x * widthRatio;
	_float		newHeight = originalScale.y * heightRatio;

	_float4		newPos = originalPos;
	newPos.x = newPosX;
	newPos.y = newPosY;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, newPos);
	m_pTransformCom->Set_Scaled(newWidth, newHeight, originalScale.z); // Z축 크기는 기존 값 유지

	m_fX *= static_cast<_float>(g_iWinSizeX) / 1600.f;
	m_fY *= static_cast<_float>(g_iWinSizeY) / 900.f;

	m_vTextPos = { m_fX, m_fY, 0.5f };

	return S_OK;
}

void CTextBox::Set_Position(_float fPosX, _float fPosY, _float fPosZ)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float4(fPosX, fPosY, fPosZ, 0));

	m_vTextPos.x = fPosX;
	m_vTextPos.y = fPosY;
	m_vTextPos.z = fPosZ;
}

void CTextBox::Set_Position(_vector vPos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_vTextPos = vPos;
}

CTextBox::TextBox_DESC CTextBox::Get_TextBoxDesc() const
{
	TextBox_DESC TextBoxDesc = {};

	TextBoxDesc.wstrText = m_wstrText;

	TextBoxDesc.wstrFontType = m_wstrFontType;

	TextBoxDesc.vFontColor = m_vFontColor;

	TextBoxDesc.iFontSize = m_iFontSize;

	TextBoxDesc.vPos = _float3(m_fX, m_fY, m_fZ);

	TextBoxDesc.vSize = _float2(m_fSizeX, m_fSizeY);

	TextBoxDesc.isOuterLine = m_isOuterLine;

	TextBoxDesc.vOutLineColor = m_vOutLineColor;

	return TextBoxDesc;
}

void CTextBox::Move(_float3 fMove)
{
	// m_fX += fMove.x;
	// m_fY += -fMove.y;
	// m_fZ += fMove.z;

	//m_vTextPos.x += fMove.x * static_cast<_float>(g_iWinSizeX) / 1600.f;
	//m_vTextPos.y += -fMove.y * static_cast<_float>(g_iWinSizeY) / 900.f;
	//m_vTextPos.z += fMove.z;

	m_vTextPos.x += fMove.x;
	m_vTextPos.y += -fMove.y;
	m_vTextPos.z += fMove.z;
}

void CTextBox::Move_WinSize(_float3 fMove)
{
	m_vTextPos.x += fMove.x * static_cast<_float>(g_iWinSizeX) / 1600.f;
	m_vTextPos.y += -fMove.y * static_cast<_float>(g_iWinSizeY) / 900.f;
	m_vTextPos.z += fMove.z;
}

void CTextBox::Set_Position_UI(_float3 fMove)
{
	// m_fX = fMove.x;
	// m_fY = -fMove.y;
	// m_fZ = fMove.z;

	m_vTextPos.x = fMove.x;
	m_vTextPos.y = -fMove.y;
	m_vTextPos.z = fMove.z;
}

CTextBox* CTextBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTextBox* pInstance = new CTextBox(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CTextBox"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTextBox::Clone(void* pArg)
{
	CTextBox* pInstance = new CTextBox(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CTextBox"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTextBox::Free()
{
	__super::Free();
}
