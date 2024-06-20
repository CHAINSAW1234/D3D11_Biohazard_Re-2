#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
class CPartObject;
class CNavigation;
class CFSM;
END

BEGIN(Client)

class CPlayer final : public CGameObject
{
public:
	enum STATE { MOVE, HOLD, DAMAGE };

	enum PART {
		PART_BODY,
		PART_HEAD,
		PART_HAIR,
		PART_WEAPON,
		PART_END
	};

#pragma region ANIMATION
	enum ANIMATION_MOVE {
		ANIM_IDLE, TURN_L180, TURN_R180, TURN_L0, TURN_R0, TURN_L90, TURN_R90,
		WALK_F_LOOP, WALK_L_LOOP, WALK_END_LR, WALK_END_RL, WALK_R_LOOP,
		WALK_BACK_L_LOOP, WALK_BACK_B_LOOP, WALK_BACK_R_LOOP,
		JOG_START_L0, JOG_START_L90, JOG_START_L180,
		JOG_START_R0, JOG_START_R90, JOG_START_R180, JOG_END,
		JOG_LCYCLE_LOOP, JOG_STRAIGHT_LOOP, JOG_RCYCLE_LOOP,
		JOG_LCYCLE_SHORT_LOOP, JOG_RCYCLE_SHORT_LOOP,
		STAIR_F_LOOP, STAIR_L_LOOP, STAIR_R_LOOP,
		STAIR_BACK_L_LOOP, STAIR_BACK_B_LOOP, STAIR_BACK_R_LOOP,
		STAIR_RUN_UP_LOOP, STAIR_RUN_DOWN_LOOP,
		PIVOTTURN_L0, PIVOTTURN_L90, PIVOTTURN_L180,
		PIVOTTURN_R0, PIVOTTURN_R90, PIVOTTURN_R180,
		LIGHT_ON_OFF,
		// 여기부터 Hold
		STRAFEL_F, STRAFEL_L, STRAFEL_B, STRAFEL_R, STRAFER_L, STRAFER_R,
		HOLD_START_L0, HOLD_START_L90, HOLD_START_L180,
		HOLD_START_R0, HOLD_START_R90, HOLD_START_R180,
		HOLD_IDLE_LOOP, WHEEL_L180, WHEEL_R180,
		HOLD_SHOT, HOLD_SHOT_NO_AMMO, HOLD_RELOAD,
		HOLSTERTOMOVE, MOVETOHOLSTER,


		MOVE_END
	};

#pragma endregion

#pragma region Move Direction
	enum MOVE_DIR
	{
		MD_F,MD_B,MD_R,MD_L,MD_FR,MD_FL,MD_BR,MD_BL,MD_DEFAULT
	};
#pragma endregion

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

#pragma region 현진 추가
public:
	CModel*										Get_Body_Model();
	_float3*									Get_Body_RootDir();
	_bool										Get_Spotlight() { return m_isSpotlight; }
	DWORD										Get_Direction() { return m_dwDirection; }	// 플레이어 이동 상하좌우 계산
	void										Set_Spotlight(_bool isSpotlight) { m_isSpotlight = isSpotlight; }
	void										Set_TurnSpine(_bool isTurnSpine) { m_isTurnSpine = isTurnSpine;  m_fSpineTurnLerfTimeDelta = 0.f; }

	void										Change_State(STATE eState);

	_float										Get_CamDegree(); //카메라와 플레이어 간의 각도 계산

	HRESULT										Add_FSM_States();
	void										Update_FSM();

	void										Update_Direction();
	void										Turn_Spine(_float fTimeDelta);

private:
	_bool m_isSpotlight = { false };
	DWORD m_dwDirection = { 0 };
	_bool m_isTurnSpine = { false };
	_float m_fSpineTurnLerfTimeDelta = { 1.f };
	_float m_fCurSpineTurnAxis;
	_float m_fTargetSpineTurnAxis;

	friend class CPlayer_State_Move_Walk;
	friend class CPlayer_State_Move_Jog;
	friend class CPlayer_State_Hold_Idle;
#pragma endregion

#pragma region 예은 추가 
public:
	_int										Get_Player_ColIndex() { return m_iCurCol; }
	_int										Get_Player_Direction() { return m_iDir; }
	_bool										Get_Player_RegionChange() { return m_bChange; }
	_bool*									Get_Player_Interact_Ptr() { return &m_bInteract; }
private:
	_bool										m_bInteract = { false };
	_bool										m_bChange = { true };
	_int										m_iCurCol = { 0 };
	_int										m_iDir = { 0 };
	_int										m_iPreCol = { 1 };
	_float										m_fTimeTEST = { 0.f };
#pragma endregion

	vector<CPartObject*>						m_PartObjects;
	_ubyte										m_eState = {};
	CCollider*									m_pColliderCom = { nullptr };
	CNavigation*								m_pNavigationCom = { nullptr };
	CFSM*										m_pFSMCom = {nullptr};


	_float3										m_vRootTranslation = {};

public: //For Shot
	void										RayCast_Shoot();

public:	//For Camera
	void										Calc_Camera_LookAt_Point(_float fTimeDelta);
	HRESULT										Ready_Camera();
	void										Load_CameraPosition();
	void										RayCasting_Camera();
	void										Calc_YPosition_Camera();
	void										Calc_Camera_Transform(_float fTimeDelta);
	void										SetMoveDir();
	void										ResetCamera();

private:
	class CCamera_Free*							m_pCamera = { nullptr };
	_float4										m_vCameraPosition;
	_float4										m_vCamera_LookAt_Point;
	_float										m_fLook_Dist_Look = { 1.f };
	_float										m_fUp_Dist_Look = { 0.f };
	_float										m_fRight_Dist_Look = { 0.f };
	_float										m_fLook_Dist_Look_Default = { 1.f };
	_float										m_fUp_Dist_Look_Default = { 0.f };
	_float										m_fRight_Dist_Look_Default = { 0.f };
	_float										m_fLook_Dist_Pos = { 0.f };
	_float										m_fUp_Dist_Pos = { 0.f };
	_float										m_fRight_Dist_Pos = { 0.f };
	MOVE_DIR									m_eMoveDir = { MD_DEFAULT };
	MOVE_DIR									m_ePrevMoveDir = { MD_DEFAULT };
	CTransform*									m_pTransformCom_Camera = { nullptr };
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
	_bool										m_bMove = { false };
	_bool										m_bTurnAround = { false };
	_float4										m_vTurnAround_Look_Vector;
	_float										m_fTurnAround_Time = { 0.f };
	_float										m_fLerpAmount_Right = { 0.f };
	_float										m_fLerpAmount_Up = { 0.f };
	_float										m_fLerpAmount_Look = { 0.f };
	_bool										m_bCollision_Lerp = { false };

	_bool										m_bMouseCursorClip = { true };


	/* For. TurnSpine */
	_float										m_fSpineRotate_PerSec = { XMConvertToRadians(90.f) };
	_float3										m_vPreHeadDir = { 0.f, 0.f, 1.f };

	class CModel*								m_pBodyModel = { nullptr };
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