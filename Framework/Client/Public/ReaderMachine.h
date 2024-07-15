#pragma once

#include "Client_Defines.h"
#include "InteractProps.h"



BEGIN(Client)

class CReaderMachine final : public CInteractProps
{
public:
	//카드키는 LOCK_CABINET으로 이전
	enum READERMACHINE_STATE
	{
		READERMACHINE_STATIC,
	};
	enum READERMACHINE_KEY_STATE
	{
		READERMACHINE_KEY_STATIC,
		READERMACHINE_KEY_LIVE,
		READERMACHINE_KEY_WRONG, 
		READERMACHINE_KEY_CORRECT,
	};

	enum READERMACHINE_BODY_STATE
	{
		READERMACHINE_BODY_STATIC,
		READERMACHINE_BODY_CLEAR,

	};

	enum READERMACHINE_PART
	{
		PART_READER,
		PART_BODY,
		PART_END,
	};
	enum READERMACHINE_INPUT
	{
		KEY_NOTHING,
		KEY_W,
		KEY_A,
		KEY_S,
		KEY_D,
		KEY_SPACE,
	};
private:
	CReaderMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CReaderMachine(const CReaderMachine& rhs);
	virtual ~CReaderMachine() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void					Start() override;
	virtual void					Tick(_float fTimeDelta) override;
	virtual void					Late_Tick(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

private:
	virtual HRESULT				Add_Components();
	virtual HRESULT				Add_PartObjects() override;
	virtual HRESULT				Initialize_PartObjects() override;
	virtual HRESULT				Bind_ShaderResources() override;

private:
	void Active();
	void Reset_Camera();
	void Camera_Active(CReaderMachine::READERMACHINE_PART ePart, _float3 vRatio);
	
public:
	virtual _float4 Get_Object_Pos() override;

private:
	_bool				m_bCamera = { false };
	_bool				m_bActivity = { false };
	_float			m_fTime = { 0.f };
	_ubyte			m_eState = { READERMACHINE_STATIC };
	_ubyte			m_eMachine_Key_State = { READERMACHINE_KEY_STATIC };
	_ubyte			m_eKeyInput = { KEY_NOTHING };

	

	
	map<string, class CCabinet*> m_Cabinets;  //elect캐비넷 tag와 그 주소

public:
	static CReaderMachine* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END