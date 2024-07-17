#pragma once

#include "Client_Defines.h"
#include "Part_InteractProps.h"



BEGIN(Client)

class CEmblem_Door final : public CPart_InteractProps
{
public:
	enum class EMBLEM_TYPE
	{
		HEART_EMBLEM,
		SPADE_EMBLEM,
		CLUB_EMBLEM,
		DIA_EMBLEM,
		END_EMBLEM
	};

	enum class EMBLEM_ANIM
	{
		STATIC_ANIM,
		START_ANIM,
		OPEN_ANIM,
		END_ANIM
	};

	typedef struct tag_EmblemDoor_desc : public CPart_InteractProps::PART_INTERACTPROPS_DESC
	{
		_ubyte eEmblemType;
		_ubyte* EmblemAnim;

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
	virtual HRESULT				Render_LightDepth_Dir()override;
	virtual HRESULT				Render_LightDepth_Point() override;
	virtual HRESULT				Render_LightDepth_Spot()override;

private:
	virtual HRESULT				Add_Components();
	virtual HRESULT				Add_PartObjects() override;
	virtual HRESULT				Initialize_PartObjects() override;

public :
	_float4x4*					Get_WorldMatrix() { return &m_WorldMatrix; }

public:
	void						Set_Socket(_float4x4* pSocketMatrix) { m_pSocketMatrix = pSocketMatrix; }


private:
	_float4x4*					m_pSocketMatrix = { nullptr };

	_ubyte						m_eEmblemType = {};
	_ubyte*						m_pEmblem_Anim;

public:
	static CEmblem_Door* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END