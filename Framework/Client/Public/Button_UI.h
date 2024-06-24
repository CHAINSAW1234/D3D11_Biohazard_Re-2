#pragma once
#include "Customize_UI.h"

BEGIN(Client)

class CButton_UI final : public CCustomize_UI
{
protected:
	CButton_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CButton_UI(const CButton_UI& rhs);
	virtual ~CButton_UI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void FirstTick_Seting();

public:
	void TurnOn_ColorBlend()
	{
		m_isColorChange = true;
		m_isBlending = true;
	}

	void TurnOn_ColorBlend(_uint iChildToControl)
	{
		m_isColorChange = true;
		m_isBlending = true;

		static_cast<CButton_UI*>(m_vecChildUI[iChildToControl])->TurnOn_ColorBlend();
	}

	void Set_ColorBlend_Value(_float4 vColor, _float fBlendStrength)
	{
		m_vCurrentColor = vColor;
		m_fBlending = fBlendStrength;
	}

	void Set_ColorBlend_Value(_float4 vColor, _float fBlendStrength, _uint iChildToControl)
	{
		m_vCurrentColor = vColor;
		m_fBlending = fBlendStrength;

		static_cast<CButton_UI*>(m_vecChildUI[iChildToControl])->Set_ColorBlend_Value(vColor, fBlendStrength);
	}

	void Set_Clickable(_bool bClickable) { m_bClickable = bClickable; }

	virtual _bool IsMouseHover() override;

	virtual _bool IsMouseHover(_float& fPosZ) override;

	virtual void Frame_Change_ValueColor(_uint iChange_FrameNum) override {

		__super::Frame_Change_ValueColor(iChange_FrameNum);

		if (0 == iChange_FrameNum)
			m_bClickable = false;
		else if (1 == iChange_FrameNum)
			m_bClickable = true;
	}

public:
	_bool m_bClickable = { true };

	


public:
	static CButton_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END