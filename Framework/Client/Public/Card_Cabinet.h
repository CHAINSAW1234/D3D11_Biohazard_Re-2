#pragma once

#include "Client_Defines.h"
#include "Part_InteractProps.h"

BEGIN(Client)

class CCard_Cabinet final : public CPart_InteractProps
{
public:
	enum Pannel_Model
	{
		
		
	};

	typedef struct CARD_CABINET_Desc : public CPart_InteractProps::PART_INTERACTPROPS_DESC
	{
		const _float4x4* pLock_WorldMatrix = { nullptr };
		_ubyte* pLock_Cabinet_State = { nullptr };
		_bool* pAction = { nullptr };

	}CARD_CABINET_DESC;



private:
	CCard_Cabinet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCard_Cabinet(const CCard_Cabinet& rhs);
	virtual ~CCard_Cabinet() = default;

public:
	virtual HRESULT					Initialize_Prototype() override;
	virtual HRESULT					Initialize(void* pArg) override;
	virtual void						Tick(_float fTimeDelta) override;
	virtual void						Late_Tick(_float fTimeDelta) override;
	virtual HRESULT					Render() override;
	virtual HRESULT					Render_LightDepth_Dir()override;
	virtual HRESULT					Render_LightDepth_Point() override;
	virtual HRESULT					Render_LightDepth_Spot()override;

private:
	virtual HRESULT					Add_Components();
	virtual HRESULT					Add_PartObjects() override;
	virtual HRESULT					Initialize_PartObjects() override;

private:
	_ubyte*								m_pLockState = { nullptr };
	const _float4x4*					m_pWorldMatrix = { nullptr };
	_bool*								m_bAction = { nullptr };
public:
	static CCard_Cabinet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END