#pragma once

#include "Client_Defines.h"
#include "Part_InteractProps.h"



BEGIN(Client)

class CMark_Door final : public CPart_InteractProps
{
public:

	enum class EMBLEM_ANIM
	{
		STATIC_ANIM,
		OPEN_ANIM,
		START_ANIM,
		END_ANIM
	};

	typedef struct tag_EmblemDoor_desc : public CPart_InteractProps::PART_INTERACTPROPS_DESC
	{
		_ubyte eEmblemType;
		_ubyte* EmblemAnim;
		_ubyte* pDoorState = { nullptr };

	}EMBLEMMARK_DOOR_DESC;

private:
	CMark_Door(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMark_Door(const CMark_Door& rhs);
	virtual ~CMark_Door() = default;

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

public :
	_float4x4*					Get_WorldMatrix() { return &m_WorldMatrix; }

public:
	void						Set_Socket01(_float4x4* pSocketMatrix) { m_pSocketMatrix_01 = pSocketMatrix; }


private:
	_bool							m_bSide = { false };
	_float4x4*					m_pSocketMatrix_01 = { nullptr };

	_ubyte						m_eEmblemType = {};
	_ubyte*						m_pEmblem_Anim = {nullptr};
	_ubyte*						m_pDoorState = {nullptr};

public:
	static CMark_Door* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END