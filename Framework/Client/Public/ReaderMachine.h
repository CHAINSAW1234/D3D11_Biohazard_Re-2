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
	enum READERMACHINE_PADKEY
	{
		PAD_0,
		PAD_1,
		PAD_2,
		PAD_3,
		PAD_4,
		PAD_5,
		PAD_6,
		PAD_7,
		PAD_8,
		PAD_9,
		PAD_DELETE,
		PAD_ENTER,
		PAD_END
	};
	enum READERMACHINE_PADKEY_ROW
	{
		ROW_0,
		ROW_1,
		ROW_2,
		ROW_3,
		ROW_4,
		ROW_END
	};
	
	enum READERMACHINE_PADKEY_COL
	{
		COL_0,
		COL_1,
		COL_2,
		COL_END
	};		
	
	enum READERMACHINE_PADKEY_NUM
	{
		NUM_0,
		NUM_1,
		NUM_2,
		NUM_END,
	};	




private:
	CReaderMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CReaderMachine(const CReaderMachine& rhs);
	virtual ~CReaderMachine() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void				Start() override;
	virtual void				Tick(_float fTimeDelta) override;
	virtual void				Late_Tick(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

private:
	virtual HRESULT				Add_Components();
	virtual HRESULT				Add_PartObjects() override;
	virtual HRESULT				Initialize_PartObjects() override;
	virtual HRESULT				Bind_ShaderResources() override;
	virtual void					Do_Interact_Props() override;

private:
	void						Active();
	void						Reset_Camera();
	void						Camera_Active(CReaderMachine::READERMACHINE_PART ePart, _float3 vRatio);
	_bool						Open_Cabinet();
public:
	virtual _float4				Get_Object_Pos() override;

private:
	_bool				m_bCamera = { false };
	_bool				m_bActivity = { false };
	_bool				m_bDoOpen = { false };
	_bool				m_bCameraReset = { false };
	_float				m_fTime = { 0.f };
	_ubyte				m_eState = { READERMACHINE_STATIC };
	_ubyte				m_eMachine_Key_State = { READERMACHINE_KEY_STATIC };
	_ubyte				m_eKeyInput = { KEY_NOTHING };
	_int				m_iSelectRow = { ROW_0 };
	_int				m_iSelectCol = { COL_0 };
	_bool				m_bCanPush[ROW_END][COL_END] = {false,};
	_int				m_iKeyPad[ROW_END][COL_END] = { {PAD_7,PAD_8,PAD_9},{PAD_4,PAD_5,PAD_6},{PAD_1,PAD_2,PAD_3},{PAD_0,PAD_0,PAD_DELETE},{PAD_ENTER,PAD_ENTER,PAD_ENTER} };
	_int				m_iPush[NUM_END] = { -1,-1,-1 };
	_int				m_iSelectPushNum = { NUM_0 };
	_bool				m_bKey[2] = { false };

	map<string, class CCabinet*> m_Cabinets;  //elect캐비넷 tag와 그 주소

public:
	static CReaderMachine* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END