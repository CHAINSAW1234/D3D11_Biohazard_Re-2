#pragma once
#include "Map_Manager.h"

BEGIN(Client)

class CMain_Map_UI final : public CMap_Manager
{
private:
	enum class MAP_CHILD_TYPE { PARENT_MAP, BACKGROUND_MAP, LINE_MAP, END_MAP };

private:
	CMain_Map_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMain_Map_UI(const CMain_Map_UI& rhs);
	virtual ~CMain_Map_UI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Change_Tool() override;

public :
	_bool*				Distance_End_Ptr()				{ return &m_isMainEnd;  }
	_vector*			Player_Between_Distance_Ptr()	{ return &m_vMapOpen_Player_Distance; }
	_bool*				Map_Clear_Ptr()					{ return &m_isMapSearch_Clear; }


private :
	void				Find_DoorObj();

private :	
	void				Change_Search_Type(MAP_STATE_TYPE _searType, _bool isDoor = false);
	void				Region_State();
	void				Search_Map_Type(MAP_STATE_TYPE _searType, LOCATION_MAP_VISIT _mapType);

	void				Rendering();
	void				Player_BetweenDistance();

	void				Door_State();
	void				Search_Door_Type(MAP_STATE_TYPE _searType, DOOR_TYPE _searchDoor);

private :
	list<class CDoor*>	m_DoorList;
	MAP_FLOOR_TYPE		m_ePrevViewFloor				= { MAP_FLOOR_TYPE::FLOOR_1 };

	_bool				m_isMapSearch_Clear				= { false };
	_bool				m_isEnd_OnesRole				= { false };

private :
	_int				m_iWhichChild					= { 0 };		/* Init���� ��ü�� ������ ���� */

private : 
	_float4				m_vOriginPos					= {};			/* ���� �÷��̾ ���� �� Position */
	_vector				m_vMapOpen_Player_Distance		= {};			/* Player�� ���� Distance */
	_bool				m_isMainEnd						= { false };	/* Player���� Distance�� ���� ����ߴ°�? */


public:
	static CMap_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
