#pragma once
#include "Map_Manager.h"

BEGIN(Client)

class CStatic_Map_UI final : public CMap_Manager
{
public  :
	enum class STATIC_MAP_TYPE { MASK_MAP_TYPE, BACKGROUND_MAP_TYPE, SEARCH_MAP_TYPE , LINE_MAP_TYPE , END_MAP_TYPE  };

private:
	CStatic_Map_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStatic_Map_UI(const CStatic_Map_UI& rhs);
	virtual ~CStatic_Map_UI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Change_Tool() override;

private :
	void				Rendering();

public :
	STATIC_MAP_TYPE*	Get_Static_Type() { return &m_eStatic_Type; }


private :
	STATIC_MAP_TYPE		m_eStatic_Type = { STATIC_MAP_TYPE::END_MAP_TYPE };
	
public:
	static CMap_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
