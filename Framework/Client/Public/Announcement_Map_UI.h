#pragma once
#include "Map_Manager.h"

BEGIN(Client)

class CAnnouncement_Map_UI final : public CMap_Manager
{
private:
	CAnnouncement_Map_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAnnouncement_Map_UI(const CAnnouncement_Map_UI& rhs);
	virtual ~CAnnouncement_Map_UI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Change_Tool() override;

public :
	void					Set_GetMapItem() { m_isGetMap_Item = true; }

private:
	void					Find_Variable_Condition();

private :
	_bool					m_isGetMap_Item = { false };

public:
	static CMap_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
