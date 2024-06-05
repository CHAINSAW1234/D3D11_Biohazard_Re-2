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
	}

	return S_OK;
}

void CTextBox::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

}

void CTextBox::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CTextBox::Render()
{
	wstring strFontType = m_wstrFontType + to_wstring(m_iFontSize);

	//_float4 fPos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

	if (FAILED(m_pGameInstance->Render_Font(strFontType, m_wstrText, XMVectorSet(m_fX, m_fY, m_fZ, 0.f), m_vFontColor, 0.f)))
		return E_FAIL;

	return S_OK;
}

CTextBox::TextBox_DESC CTextBox::Get_TextBoxDesc() const
{
	TextBox_DESC TextBoxDesc = {};

	TextBoxDesc.wstrText = m_wstrText;

	TextBoxDesc.wstrFontType = m_wstrFontType;

	TextBoxDesc.vFontColor = m_vFontColor;


	return TextBoxDesc;
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
