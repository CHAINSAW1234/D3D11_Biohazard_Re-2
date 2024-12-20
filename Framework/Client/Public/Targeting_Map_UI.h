#pragma once
#include "Map_Manager.h"

BEGIN(Client)

class CTargeting_Map_UI final : public CMap_Manager
{
private: 
	enum class CROSSHAIR_TARGET_TYPE { LEFT_TARGET, RIGHT_TARGET, UP_TARGET, DOWN_TARGET, END_TARGET };

private:
	CTargeting_Map_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTargeting_Map_UI(const CTargeting_Map_UI& rhs);
	virtual ~CTargeting_Map_UI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Change_Tool() override ;

	/* Getter Setter Inline*/
public :
	void										Set_SearchFor_Verification_MapType(_byte _eFloor, _byte _eLocation)
	{ 
		m_eLocationVerification = _eLocation;
		m_eFloorVerification = _eFloor - 2;
		m_isSearchForVerification = true; 
	}


private : /* Find Fuction*/
	void										Find_Item();
	void										Find_NotifyText_RenderState();
	void										Verification_MapType();


private :
	void										Targeting_Render(_float fTimeDelta);
	void										Targeting_Control();
	void										Notify_Font_Position();
	_bool										Item_Hovering(_float4 _mainPos, _float3 _scaled);


private :
	CTargeting_Map_UI*							m_pMain_Target			= { nullptr };
	CTransform*									m_pMainTarget_Transform = { nullptr };
	CGameObject*								pSupervisor = { nullptr };

	_bool										m_isFont_Render			= { false };

private : /* Crosshair Targeting */
	CROSSHAIR_TARGET_TYPE						m_eCrosshair_Type					= { CROSSHAIR_TARGET_TYPE::END_TARGET };
	_float										m_fTarget_Distance					= { 0.f };


private : /* Notify Targeting */
	vector<vector<class CItem_Map_UI*>>			m_ItemStore_Vec;
	_float2										m_fTargetNotify_Distance			= {};
	_float2										m_fTargetNotify_TextBox_Distance	= {};
	wstring										m_wstrItem_Name						= { TEXT("") };

	/* Font Tick이 한 번 밀리기 때문에 다른 다른 Render 변수로 관리할 것이다. */
	_bool										m_isNotifyRender = { false };
	_bool*										m_isNotifyRender_Ptr = { nullptr };

private :
	_bool										m_isSearchForVerification = { false }; /* 아이템으로 인한 맵 상태 확인 탐색용 */
	_byte										m_eLocationVerification = {};
	_byte										m_eFloorVerification = {};

public:	
	static CMap_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
