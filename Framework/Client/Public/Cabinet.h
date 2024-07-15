#pragma once

#include "Client_Defines.h"
#include "InteractProps.h"



BEGIN(Client)

class CCabinet final : public CInteractProps
{
public:
	enum CABINET_TYPE
	{
		TYPE_NORMAL,
		TYPE_SAFEBOX,
		TYPE_ELECTRIC,
		TYPE_WEAPON,
		TYPE_LEON,
		TYPE_ZOMBIE,
	};
	enum CABINET_STATE
	{
		//캐비넷_바디 애님
		CABINET_CLOSED,
		CABINET_OPEN,
		CABINET_OPENED,
		CABINET_END,
	};
	enum LOCK_STATE
	{
		STATIC_LOCK, // 그냥 정지상태0 0
		LIVE_LOCK, //락을 푸는 행동을 할때 => 뼈를 돌릴때임1
		WRONG_LOCK, // 락이 틀렸을 때2 1
		CLEAR_LOCK // 락이 풀렸을때 3 2
	};
	enum CABINET_PART
	{
		PART_BODY,
		PART_ITEM,
		PART_LOCK,
		PART_LOCK1, //레온꺼
		PART_CARD, //카드키 전용 
		PART_END
	};

	enum LOCK_KEY_INPUT
	{
		KEY_NOTHING,
		KEY_W,
		KEY_A,
		KEY_S,
		KEY_D,
	};
private:
	CCabinet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCabinet(const CCabinet& rhs);
	virtual ~CCabinet() = default;

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
	virtual HRESULT				Bind_ShaderResources() override;

public:
	inline _bool				Is_Opened() { return m_bOpened; }

private:
	void						Camera_Active(CCabinet::CABINET_PART ePart, _float3 vRatio = { 10.f,10.f,10.f });
	void						Reset_Camera();

	void						Safe_Normal_Tick(_float fTimeDelta);
	void						LeonDesk_Tick(_float fTimeDelta);
	void						Electric_Tick(_float fTimeDelta);
	void						Weapon_Tick(_float fTimeDelta);
	void						Zombie_Tick(_float fTimeDelta);

private:
	void						Safe_Normal_Active();
	void						LeonDesk_Active();
	void						Electric_Active();
	void						Weapon_Active();
	void						Zombie_Active();

public:
	virtual _float4		Get_Object_Pos() override;
	void					Set_Lock_False() { m_bLock = false; }
	_int					Get_Cabinet_Type() {return m_eCabinetType; }
	string				Get_Electric_Tag() { return m_strElectTag; }
private:
	_bool					m_bLeonDesk = { false };
	_bool					m_bItemDead = { true };
	_bool					m_bObtain = { false };
	_bool					m_bOpened = { false };
	_bool					m_bLock =	{ false };
	_bool					m_bActivity = { false };
	_bool					m_bCamera = { false };
	_int					m_iPassWord[10] = {1,};
	_ubyte				m_eState = { CABINET_CLOSED };
	_ubyte				m_eLockState = { STATIC_LOCK };
	_int					m_eCabinetType = { TYPE_NORMAL };
	
	_bool					m_isCamera_Reset = { false };
	_ubyte				m_eKeyInput = { KEY_NOTHING }; // 직접 nothing인 타이밍을 알아서 key_nothing으로 세팅 해줘야함

	string				m_strElectTag = { "" };

	_int					m_iPassWordLeon[10] = {1,};
	_bool					m_bLockLeon = { false };
	_ubyte				m_eLockLeonState = { STATIC_LOCK };

public:
	static CCabinet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END