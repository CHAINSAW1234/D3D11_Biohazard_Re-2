#pragma once
#include "Customize_UI.h"

class CMap_UI final : public CCustomize_UI
{

private :
	enum class MAP_CHILD_TYPE { PARENT_MAP, BACKGROUND_MAP, LINE_MAP, END_MAP };

public:
	enum class MAP_STATE_TYPE { NONE_STATE, SEARCH_STATE, SEARCH_CLEAR_STATE };

private:
	CMap_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMap_UI(const CMap_UI& rhs);
	virtual ~CMap_UI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;


public :
	void			Search_Map_Type(MAP_STATE_TYPE _searType, LOCATION_MAP_VISIT _mapType);
	void			Change_Search_Type(MAP_STATE_TYPE _searType, CMap_UI* pMap);

	void			Mouse_Pos(_float fTimeDelta);
	void			Transform_Adjustment();
	void			EX_ColorChange();
	void			Render_Condition();

	void			Search_TabWindow();

private :
	_uint			m_iWhichChild = { 0 };
	_float			fMouseSensor = { 20.f };
	_uint			m_iCnt = { 0 };
	_bool			m_isGara = { false };

private :
	MAP_STATE_TYPE		m_eMapState = { MAP_STATE_TYPE::NONE_STATE };
	CGameObject*		m_pTab_Window = { nullptr };

public:
	static CCustomize_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};


