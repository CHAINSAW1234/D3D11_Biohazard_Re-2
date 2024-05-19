#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
class CPartObject;
class CNavigation;
END

BEGIN(Client)

class CPlayer final : public CGameObject
{
public:
	enum STATE {
		STATE_IDLE = 0x01,
		STATE_RUN = 0x02,
		STATE_ATT = 0x04,
	};

	enum PART {
		PART_BODY,
		PART_HEAD,
		PART_HAIR,
		PART_END
	};

private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	void Priority_Tick_PartObjects(_float fTimeDelta);
	void Tick_PartObjects(_float fTimeDelta);
	void Late_Tick_PartObjects(_float fTimeDelta);

private:
	vector<CPartObject*>						m_PartObjects;
	_ubyte										m_eState = {};
	CCollider*									m_pColliderCom = { nullptr };
	CNavigation*								m_pNavigationCom = { nullptr };


private:
	HRESULT Add_Components();
	HRESULT Add_PartObjects();
	HRESULT Initialize_PartModels();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END