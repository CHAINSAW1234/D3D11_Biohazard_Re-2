#pragma once
#include "Map_Manager.h"

BEGIN(Client)

class CItem_Map_UI final : public CMap_Manager
{
private:
	CItem_Map_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItem_Map_UI(const CItem_Map_UI& rhs);
	virtual ~CItem_Map_UI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	
public :
	wstring*		Item_Name() { return &m_wstrItemNumber;  }
	_bool*			Distance_ItemEnd() { return &m_isItemEnd;  }
	void			Destory_Item(MAP_FLOOR_TYPE _floorType, LOCATION_MAP_VISIT _locationType, ITEM_NUMBER _ItemType);
	_vector*		Player_Between_Item_Distance() { return &m_vMapOpen_Player_Distance; }


private :
	CGameObject*	Search_Item(MAP_FLOOR_TYPE _floorType, LOCATION_MAP_VISIT _locationType, ITEM_NUMBER _ItemType);
	void			Item_Name_Selection(); /* 아이템 Type에 따른 이름 선정 */

	void			Rendering();
	void			Player_BetweenDistance();


private :
	ITEM_NUMBER						m_eItem_Type					= { ITEM_NUMBER::ITEM_NUMBER_END }; /* 아이템 종류 */
	wstring							m_wstrItemNumber				= { TEXT("") };
	LOCATION_MAP_VISIT				m_ePrevRegion					= { LOCATION_MAP_VISIT::LOCATION_MAP_VISIT_END };

	_bool							m_isItemRender					= { false };
	_bool							m_isItemEnd						= { false };
	_vector							m_vMapOpen_Player_Distance		= {};

	_float4							m_vOriginPos					= {};

private:
	MAP_FLOOR_TYPE					m_ePrevViewFloor				= { MAP_FLOOR_TYPE::FLOOR_1 };

public:
	static CMap_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
