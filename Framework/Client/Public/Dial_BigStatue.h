#pragma once

#include "Client_Defines.h"
#include "Part_InteractProps.h"

BEGIN(Client)

class CDial_BigStatue final : public CPart_InteractProps
{
private :
	

public:
	enum OPEN_LOCK_BONE
	{
		BONE_DIAL1,
		BONE_DIAL2,
		BONE_DIAL3,
		BONE_DIAL_END,
	};

	typedef struct tagDialBigStatue_desc : public CPart_InteractProps::PART_INTERACTPROPS_DESC
	{
		_ubyte*		pLockState = { nullptr };
		_int*			pPassword = {nullptr};
		_ubyte*		pKeyInput = {nullptr};
		_bool*		pCameraControl	= { nullptr };
		_bool*		pAction = { nullptr };
		_float4x4* pParentWorldMatrix = { nullptr };
		_int			eBigStatueType = { 0 };

	}DIAL_BIGSTATUE_DESC;

private:
	CDial_BigStatue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDial_BigStatue(const CDial_BigStatue& rhs);
	virtual ~CDial_BigStatue() = default;

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

public:
	_bool								Get_Clear() { return m_bClear; }
	
private:
	_int								m_eBigStatueType = {0};
	_ubyte*							m_pLockState			= { nullptr };
	_int*								m_pPassword				= { nullptr };
	_bool*							m_pCameraControl		= { nullptr };
	_bool*							m_pAction = { nullptr };
	_bool								m_bCheckAnswer = { false };
	_bool								m_bClear = { false };
	_ubyte*							m_pPressKeyState = { nullptr };

	string							m_strOpenDial[BONE_DIAL_END] = { "Dial1","Dial2","Dial3" };
	_int								m_iCurBoneIndex = { BONE_DIAL1 };
	_float							m_fCurAngle[BONE_DIAL_END] = { 0.f,0.f,0.f };
	_float							m_fGoalAngle[BONE_DIAL_END] = { 0.f,0.f,0.f };
	_int								m_iOpenDialAnswer[BONE_DIAL_END] = {0,0,0};

	_float4x4*						m_pParentWorldMatrix = { nullptr };


public:
	static CDial_BigStatue* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END