#pragma once
#include "Customize_UI.h"

BEGIN(Client)

class CMissionBar_UI final : public CCustomize_UI
{
public :
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
	virtual HRESULT Change_Tool() override;

private :
	void							Find_MissionVariable();

public:
	void							Write_Mission_Text(wstring _write);
	void							MissionCollider_Access();
	void							Mission_Text(wstring _write);
	
	_ubyte							Get_MissionType() { return static_cast<_ubyte>(m_eMissionUI_Type); }
	CMissionBar_UI*					Get_MissionBar() { return m_pMissionBar; }
	_bool*							Get_FontStart_Ptr() { return &m_isFontStart;  }

private:
	void							TabWindow_Mission();
	void							InGame_Mission(_float fTimeDelta);

	void							Operater_MissionBar(_float fTimeDelta);
	void							Operater_MissionArrow(_float fTimeDelta);
	void							Operater_MissionFont(_float fTimeDelta);
	void							Mission_Start();

	void							Mission_Variable_Return();

private:
	MISSION_UI_TYPE					m_eMissionUI_Type	= { MISSION_UI_TYPE::END_MISSION };
	CMissionBar_UI*					m_pMissionBar		= { nullptr };
	class CTab_Window*				m_pTabWindow		= { nullptr };
	_bool							m_isTabDead			= { true };

private: /* Mission Bar */
	_bool							m_isMission					= {};
	_bool							m_isMission_NonLighting		= { false };
	_float							m_fLifeTimer				= {};
	_float							m_fLightPositionY			= {};

private: /* Font */
	wstring							m_strMissionText			= { TEXT("") };
	_float							m_fMissionFont_Timer		= {};
	_float							m_fMissionArrow_Timer		= {};
	_bool							m_isFontStart				= { false };

private :
	_bool*							m_isMissionClear			= { nullptr };
	_ubyte*							m_eMissionType				= { nullptr };

private : 
	/* Font 包府 */
	queue<wstring>					m_pEventsQueue;

	/* Collider 包府 */
	queue<class CCustomCollider*>	m_pMission_ColliderQueue;
	vector<CCustomCollider*>		m_pMissionCollVec;

public:
	static CCustomize_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};


END