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
	virtual HRESULT								Initialize_Prototype() override;
	virtual HRESULT								Initialize(void* pArg) override;
	virtual void								Priority_Tick(_float fTimeDelta) override;
	virtual void								Tick(_float fTimeDelta) override;
	virtual void								Late_Tick(_float fTimeDelta) override;
	virtual HRESULT								Render() override;

private:
	void										Priority_Tick_PartObjects(_float fTimeDelta);
	void										Tick_PartObjects(_float fTimeDelta);
	void										Late_Tick_PartObjects(_float fTimeDelta);
	
private:
	void										Col_Section();

#pragma region 예은 추가 
public:
	_int										Get_Player_ColIndex() { return m_iCurCol; }
	_int										Get_Player_Direction() { return m_iDir; }
	_bool										Get_Player_RegionChange() { return m_bChange; }
private:


	_bool											m_bChange = { true };
	_int											m_iCurCol = {0};
	_int											m_iDir = {0};
	_int											m_iPreCol = {0};
	_float										m_fTimeTEST = { 0.f };
#pragma endregion

	vector<CPartObject*>						m_PartObjects;
	_ubyte										m_eState = {};
	CCollider*									m_pColliderCom = { nullptr };
	CNavigation*								m_pNavigationCom = { nullptr };

	_float3										m_vRootTranslation = {};

	//For Camera
public:
	void										Calc_Camera_LookAt_Point(_float fTimeDelta);
	HRESULT										Ready_Camera();
	void										Load_CameraPosition();
	void										RayCasting_Camera();
	void										Calc_YPosition_Camera();
private:
	class CCamera_Free* m_pCamera = { nullptr };
	_float4										m_vCameraPosition;
	_float4										m_vCamera_LookAt_Point;
	_float										m_fLook_Dist = { 1.f };
	_float										m_fUp_Dist = { 0.f };
	_float										m_fRight_Dist = { 0.f };
	CTransform* m_pTransformCom_Camera = { nullptr };
	_float										m_fMouseSensor = { 0.0f };

	_float4										m_vLookPoint_To_Position_Dir;
	_float4										m_vPrev_Position;
	_float										m_fRayDist = { 0.f };
	_float4										m_vRayDir;
	_float										m_fLerpTime = { 1.f };
	_bool										m_bLerp = { false };
	_bool										m_bLerp_Move = { false };
	_bool										m_bMove_Forward = { false };
	_bool										m_bMove_Backward = { false };
	_bool										m_bMove_Right = { false };
	_bool										m_bMove_Left = { false };
	_bool										m_bAim = { false };
	_bool										m_bRotate_Delay = { false };
	_float										m_fRotate_Delay_Amount = { 1.f };
	_bool										m_bRotate_Delay_Start = { false };
	_float										m_fRotate_Amount_X = { 0.f };
	_float										m_fRotate_Amount_Y = { 0.f };
	_float										m_fPrev_Rotate_Amount_X = { 0.f };
	_float										m_fPrev_Rotate_Amount_Y = { 0.f };
	_float										m_fTotal_Rotate_Amount_Y = { 0.f };
	_float4										m_vOrigin_LookAt_Point;
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