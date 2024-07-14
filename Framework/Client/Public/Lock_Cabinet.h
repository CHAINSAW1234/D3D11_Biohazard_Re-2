#pragma once

#include "Client_Defines.h"
#include "Part_InteractProps.h"



BEGIN(Client)

class CLock_Cabinet final : public CPart_InteractProps
{
private :
	enum class LOCK_ALLOW_KEY { LEFT_LOCK_KEY, RIGHT_LOCK_KEY, END_LOCK_KEY };

	struct SAFEBOX_PASSWORD
	{
		_uint iCount;
		LOCK_ALLOW_KEY eAllow;
	};

public:
	// safebox => 뼈를 돌려야함(좌로 움직일것인지, 우로 움직일 것인지)
	// OpenLocker_Dial => 틀리면 흔들리고, 맞으면 열리는 동작 , 뼈를 돌려야함
	enum LOCK_TYPE
	{
		SAFEBOX_DIAL,
		OPENLOCKER_DIAL
	};

	typedef struct tagLockCabinet_desc : public CPart_InteractProps::PART_INTERACTPROPS_DESC
	{
		_int		iLockType		= { SAFEBOX_DIAL };
		_ubyte*		pLockState		= { nullptr };
		_int*		pPassword		= {nullptr};
		_bool*		pCameraControl	= { nullptr };

	}BODY_LOCK_CABINET_DESC;

private:
	CLock_Cabinet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLock_Cabinet(const CLock_Cabinet& rhs);
	virtual ~CLock_Cabinet() = default;

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
	virtual void				Get_SpecialBone_Rotation() override;
	HRESULT						Initialize_Model();

private: /* Safe Box */
	HRESULT						Initialize_SafeBox();
	void						Safebox_Late_Tick(_float fTimeDelta);
	void						Safebox_RotationLock(LOCK_ALLOW_KEY _eKeyType, _float fTimeDelta);
	void						Safebox_Clear_Condition();
	void						Safebox_Return();


private : /* Open_Locker */
	void						OpenLocker_Late_Tick(_float fTimeDelta);


public:
	void						Set_Socket(_float4x4* pSocketMatrix) { m_pSocketMatrix = pSocketMatrix; }

	
private:
	LOCK_TYPE						m_eLockType				= { SAFEBOX_DIAL };
	_ubyte*							m_pLockState			= { nullptr };
	_int*							m_pPassword				= { nullptr };
	_float4x4*						m_pSocketMatrix			= { nullptr };
	_bool*							m_pCameraControl		= { nullptr };

#pragma region SafeBox Variable
private:
	LOCK_ALLOW_KEY					m_eMoveingKey = { LOCK_ALLOW_KEY::END_LOCK_KEY }; /* 현재 키*/
	vector<SAFEBOX_PASSWORD>		m_eCurrentKey; /* 지정한 키 */
	vector<SAFEBOX_PASSWORD>		m_eClearKey; /* 클리어 조건 키*/

	_uint							m_iCurrentKey_Cnt = { 0 };

	/* 1. Roatation Variable */
	_float							m_fRotationAngle = { 0.f };
	_float							m_fPrevRotationAngle = { 0.f };
	_bool							m_isPosition_Register = { false };
	_int							m_iRotationCnt = { 0 };
	_float							m_fWrongTimer = { 0.f };
#pragma endregion


public:
	static CLock_Cabinet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END