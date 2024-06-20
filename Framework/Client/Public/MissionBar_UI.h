#pragma once
#include "Customize_UI.h"

BEGIN(Client)

class CMissionBar_UI final : public CCustomize_UI
{
private:
	enum class MISSION_UI_TYPE { BAR_MISSION, ARROW_MISSION, FONT_MISSION, END_MISSION };

private:
	CMissionBar_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMissionBar_UI(const CMissionBar_UI& rhs);
	virtual ~CMissionBar_UI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void					Write_Mission_Text(wstring _write);

private:
	void					Operater_MissionBar(_float fTimeDelta);
	void					Operater_MissionArrow(_float fTimeDelta);
	void					Operater_MissionFont(_float fTimeDelta);

	void					Mission_Start();
	void					Mission_Light(_float fTimeDelta);


private:
	MISSION_UI_TYPE			m_eMissionUI_Type = { MISSION_UI_TYPE::END_MISSION };
	CMissionBar_UI*			m_pMissionBar = { nullptr };


private: /* Mission Bar */
	_bool					m_isMission = {};
	_bool					m_isMission_NonLighting = { false };
	_float					m_fLifeTimer = {};


private: /* Font */
	wstring					m_strMissionText = { TEXT("") };
	_float					m_fMissionFont_Timer = {};
	_float					m_fMissionArrow_Timer = {};
public:
	static CCustomize_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};


END