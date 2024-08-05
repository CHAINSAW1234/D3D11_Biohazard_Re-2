#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CTextBox final : public CUI
{
public:
	typedef struct TextBox_DESC : public CUI::UI_DESC
	{
		wstring		wstrText = { TEXT("") };
		wstring		wstrFontType = { TEXT("") };
		_vector		vFontColor = { 1.f, 1.f, 1.f, 1.f };
		_uint		iFontSize = { 10 };
		_bool		isOuterLine = { false };
		_vector		vOutLineColor = { 0.f, 0.f, 0.f, 1.f };
		_bool		isUI_Render = { false };


	}TextBox_DESC;

private:
	CTextBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTextBox(const CTextBox& rhs);
	virtual ~CTextBox() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	HRESULT Convert_Resolution();

public:
	void Set_Text(wstring strText){ m_wstrText = strText; }
	void Set_FontColor(_vector vFontColor) { m_vFontColor = vFontColor; }
	void Set_FontSize(_uint iFontSize) { m_iFontSize = iFontSize; }
	void Set_FontType(wstring wstrFontType) { m_wstrFontType = wstrFontType; }
	void Set_isOutLine(_bool isOuterLine) { m_isOuterLine = isOuterLine; }
	void Set_OutLineColor(_vector vOutLineColor) { m_vOutLineColor = vOutLineColor; }
	void Set_isTransformBase(_bool istransbase) { m_isTransformBase = istransbase; }
	void Set_isUIRender(_bool isUIRender) { m_isUIRender = isUIRender; }

public:
	wstring Get_Text() { return m_wstrText; }
	_vector Get_FontColor() const { return m_vFontColor; }
	_uint Get_FontSize() const { return m_iFontSize; }
	wstring Get_FontType() { return m_wstrFontType; }
	_bool Get_isOutLine() { return m_isOuterLine; }
	_vector Get_OutLineColor() { return m_vOutLineColor; }


public:
	TextBox_DESC Get_TextBoxDesc() const;
	virtual void Move(_float3 fMove) override;
	void Set_Position_UI(_float3 fMove);
	_float3 Get_Position_UI() { return _float3(m_vTextPos.x, m_vTextPos.y, m_vTextPos.z); }

private:
	wstring m_wstrText = { TEXT("") };
	wstring m_wstrFontType = { TEXT("") };
	_uint	m_iFontSize = { 10 };
	_vector m_vFontColor = { 1.f, 1.f, 1.f, 1.f };

	_bool	m_isOuterLine = { false };
	_vector m_vOutLineColor = { 0.f, 0.f, 0.f, 1.f };

	_float3	m_vTextPos = {};

private :
	_bool	m_isUIRender = { false };
	_bool	m_isTransformBase = { true };

public:
	static CTextBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END