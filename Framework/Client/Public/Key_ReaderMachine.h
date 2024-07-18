#pragma once

#include "Client_Defines.h"
#include "Part_InteractProps.h"


BEGIN(Client)

class CKey_ReaderMachine final : public CPart_InteractProps
{
public:
	enum READERMACHINE_BONE
	{
		BONE_KEY0,
		BONE_KEY1,
		BONE_KEY2,
		BONE_KEY3,
		BONE_KEY4,
		BONE_KEY5,
		BONE_KEY6,
		BONE_KEY7,
		BONE_KEY8,
		BONE_KEY9,
		BONE_KEYB,
		BONE_KEYENTER,
		READERMACHINE_BONE_END
	};

	typedef struct tagKeyReader_desc : public CPart_InteractProps::PART_INTERACTPROPS_DESC
	{
		_ubyte*		pKeyInput = {nullptr};
		_ubyte*		pKeyState = {nullptr};
		_bool			pCanPush[5][3];
		_bool*		pDoOpen= {nullptr};
		_int*			pPush = {nullptr};
		_int*			pSelectRow = {nullptr};
		_int*			pSelectCol = {nullptr};
		_int			iKeyPad[5][3];
		_int*			pKeyNum = {nullptr};
		_bool*			pHideKey = { nullptr };
	}KEY_READER_DESC;

private:
	CKey_ReaderMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CKey_ReaderMachine(const CKey_ReaderMachine& rhs);
	virtual ~CKey_ReaderMachine() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void					Tick(_float fTimeDelta) override;
	virtual void					Late_Tick(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

private:
	virtual HRESULT				Add_Components();
	virtual HRESULT				Add_PartObjects() override;
	virtual HRESULT				Initialize_PartObjects() override;
	virtual void					Get_SpecialBone_Rotation() override;
	HRESULT						Initialize_Model();
	_bool								Check_PadFull();
	_bool								Check_CanPush();

public:
	_bool								Get_Clear() { return m_bClear; }
	
private:
	_int*								m_pPassword				= { nullptr };
	_bool*							m_pCameraControl		= { nullptr };
	_bool*							m_pDoOpen = { nullptr };
	_bool								m_bCheckAnswer = { false };
	_bool								m_bClear = { false };
	_ubyte*							m_pPressKeyState = { nullptr };
	_ubyte*							m_pKeyState = { nullptr };
	_bool								m_pCanPush[5][3] = {false,};
	_int*								m_pPush = { nullptr };
	_int*								m_pSelectRow = { nullptr };
	_int*								m_pSelectCol = { nullptr };
	_int								m_iKeyPad[5][3];
	_int*								m_pKeyNum = { nullptr };
	_bool*							m_pHideKey = { nullptr };
	_float3							m_fCurLength[READERMACHINE_BONE_END] = { {0.f,0.f,0.f} ,{0.f,0.f,0.f} ,{0.f,0.f,0.f} ,{0.f,0.f,0.f} ,{0.f,0.f,0.f} ,{0.f,0.f,0.f},{0.f,0.f,0.f},{0.f,0.f,0.f}, { 0.f,0.f,0.f },{0.f,0.f,0.f},{0.f,0.f,0.f},{0.f,0.f,0.f} };
	_float3							m_fGoalLength[READERMACHINE_BONE_END] = { {0.f,0.f,0.f} ,{0.f,0.f,0.f} ,{0.f,0.f,0.f} ,{0.f,0.f,0.f} ,{0.f,0.f,0.f} ,{0.f,0.f,0.f},{0.f,0.f,0.f},{0.f,0.f,0.f}, { 0.f,0.f,0.f },{0.f,0.f,0.f},{0.f,0.f,0.f},{0.f,0.f,0.f} };
	string							m_strBoneTag[READERMACHINE_BONE_END] = { "Key0","Key1", "Key2", "Key3", "Key4", "Key5", "Key6", "Key7", "Key8", "Key9", "KeyBack", "KeyEnter"};
	_int								m_iCurBoneIndex = { BONE_KEY0 };
	vector<string>				m_HidMesh = {};



public:
	static CKey_ReaderMachine* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END