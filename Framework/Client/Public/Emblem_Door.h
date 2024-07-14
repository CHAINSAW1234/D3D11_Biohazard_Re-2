#pragma once

#include "Client_Defines.h"
#include "Part_InteractProps.h"



BEGIN(Client)

class CEmblem_Door final : public CPart_InteractProps
{
public:
	enum class STATUE_ITEM
	{
		KINGSCEPTER_ITEM,
		STATUEHAND_ITEM,
		END_ITEM
	};

	typedef struct tag_EmblemDoor_desc : public CPart_InteractProps::PART_INTERACTPROPS_DESC
	{
	
	}BODY_EMBLEM_DOOR;

private:
	CEmblem_Door(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEmblem_Door(const CEmblem_Door& rhs);
	virtual ~CEmblem_Door() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void				Tick(_float fTimeDelta) override;
	virtual void				Late_Tick(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

private:
	virtual HRESULT				Add_Components();
	virtual HRESULT				Add_PartObjects() override;
	virtual HRESULT				Initialize_PartObjects() override;

public:
	void						Set_Socket(_float4x4* pSocketMatrix) { m_pSocketMatrix = pSocketMatrix; }


private:
	_float4x4*					m_pSocketMatrix = { nullptr };


public:
	static CEmblem_Door* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END