#pragma once

#include "Client_Defines.h"
#include "Part_InteractProps.h"



BEGIN(Client)

class CLock_Cabinet final : public CPart_InteractProps
{
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
		_int iLockType = { SAFEBOX_DIAL };
		_ubyte* pLockState = { nullptr };
	}BODY_LOCK_CABINET_DESC;
private:
	CLock_Cabinet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLock_Cabinet(const CLock_Cabinet& rhs);
	virtual ~CLock_Cabinet() = default;

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
private:
	void								Safebox_Late_Tick(_float fTimeDelta);
	void								OpenLocker_Late_Tick(_float fTimeDelta);
public:
	void								Set_Socket(_float4x4* pSocketMatrix) { m_pSocketMatrix = pSocketMatrix; }

public:
	
private:
	LOCK_TYPE					m_eLockType = { SAFEBOX_DIAL };
	_ubyte*							m_pLockState = { nullptr };

	_float4x4*						m_pSocketMatrix = { nullptr };
	_float m_fRotationAngle = { 0 };
	_bool m_isRoation = { false };

public:
	static CLock_Cabinet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END