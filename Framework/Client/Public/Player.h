#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Observer_Handler.h"

BEGIN(Engine)
class CCollider;
class CPartObject;
class CNavigation;
class CFSM;
END

BEGIN(Client)
class CTab_Window;
class CWeapon;

class CPlayer final : public CGameObject, public CObserver_Handler
{
public:
	enum STATE { MOVE, HOLD, SUBHOLD, BITE, DAMAGE };
	enum PART {
		PART_BODY,
		PART_HEAD,
		PART_HAIR,
		PART_LIGHT,
		PART_END
	};

	enum EQUIP_STATE { GUN, SUB, EQUIP_STATE_NONE };
	enum EQUIP { HG, STG, GRENADE, FLASHBANG, NONE};
	enum SETPROPS_LOCATION { SETPROPS_A, SETPROPS_D, SETPROPS_E, SETPROPS_NONE };	//Holster상태일때의 위치
#pragma region ANIMATION
	enum ANIMATION_MOVE {
		ANIM_IDLE, TURN_L180, TURN_R180,
		WALK_F_LOOP, WALK_L_LOOP, WALK_END_LR, WALK_END_RL, WALK_R_LOOP,
		WALK_BACK_L_LOOP, WALK_BACK_B_LOOP, WALK_BACK_R_LOOP,
		JOG_START_L0, JOG_START_L90, JOG_START_L180,
		JOG_START_R0, JOG_START_R90, JOG_START_R180, JOG_END,
		JOG_LCYCLE_LOOP, JOG_STRAIGHT_LOOP, JOG_RCYCLE_LOOP,
		JOG_LCYCLE_SHORT_LOOP, JOG_RCYCLE_SHORT_LOOP,
		STAIR_F_LOOP, STAIR_L_LOOP, STAIR_R_LOOP,
		STAIR_BACK_L_LOOP, STAIR_BACK_B_LOOP, STAIR_BACK_R_LOOP,
		STAIR_RUN_UP_LOOP, STAIR_RUN_DOWN_LOOP,
		LIGHT_ON_OFF, MOVE_END
	};

	enum ANIMATION_HOLD {
		STRAFEL_F, STRAFEL_L, STRAFEL_B, STRAFEL_R, STRAFER_L, STRAFER_R,
		HOLD_START_L0, HOLD_START_L90, HOLD_START_L180,
		HOLD_START_R0, HOLD_START_R90, HOLD_START_R180,
		HOLD_IDLE_LOOP, WHEEL_L180, WHEEL_R180,
		HOLD_SHOT, HOLD_SHOT_NO_AMMO, HOLD_RELOAD,
		HOLSTERTOMOVE, MOVETOHOLSTER, HOLD_END
	};

	enum ANIMASTION_COMMON {
		LADDER_UP_FINE_START, LADDER_UP_FINE_R, LADDER_UP_FINE_L, LADDER_UP_FINE_R_END, LADDER_UP_FINE_L_END,
		HOLD_LEFTHAND_LIGHT,
		LADDER_DOWN_FINE_START, LADDER_DOWN_FINE_L, LADDER_DOWN_FINE_R, LADDER_DOWN_FINE_L_END, LADDER_DOWN_FINE_R_END,
		STEP_DOWM_1M, STEP_UP_1M,
		DOOR_PASS, DOOR_OPEN_JOG, DOOR_LOCK, DOOR_LOCK_L, DOOR_LOCK_R
	};
	
	enum ANIMSET_MOVE { FINE, MOVE_HG, MOVE_STG, FINE_LIGHT, CAUTION, CAUTION_LIGHT, DANGER, DANGER_LIGHT, ANIMSET_MOVE_END };
	enum ANIMSET_HOLD { HOLD_HG, HOLD_STG, HOLD_MLE, HOLD_SUP, ANIMSET_HOLD_END };
	enum ANIMSET_ETC { COMMON , ANIM_BITE, ANIMSET_ETC_END };
#pragma endregion

#pragma region Move Direction
	enum MOVE_DIR
	{
		MD_F,MD_B,MD_R,MD_L,MD_FR,MD_FL,MD_BR,MD_BL,MD_DEFAULT
	};
#pragma endregion

private:
	const static wstring strAnimSetMoveName[ANIMSET_MOVE_END];
	const static wstring strAnimSetHoldName[ANIMSET_HOLD_END];
	const static wstring strAnimSetEtcName[ANIMSET_ETC_END];

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
	virtual void								Start() override;

private:
	void										Priority_Tick_PartObjects(_float fTimeDelta);
	void										Tick_PartObjects(_float fTimeDelta);
	void										Late_Tick_PartObjects(_float fTimeDelta);

private:
	void										Col_Section();

#pragma region 현진 추가

public:
	static wstring								Get_AnimSetMoveName(ANIMSET_MOVE eAnimSetMove) { return strAnimSetMoveName[eAnimSetMove]; }
	static wstring								Get_AnimSetHoldName(ANIMSET_HOLD eAnimSetHold) { return strAnimSetHoldName[eAnimSetHold]; }
	static wstring								Get_AnimSetEtcName(ANIMSET_ETC eAnimEtcHold) { return strAnimSetEtcName[eAnimEtcHold]; }

	// =============================== GET ===============================
	CModel*									Get_Body_Model();
	CModel*									Get_Weapon_Model();
	_float3*										Get_Body_RootDir();
	_bool											Get_isBite() { return m_isBite; }
	_bool											Get_Spotlight() { return m_isSpotlight; }
	_int											Get_Hp() { return m_iHp; }
	CWeapon*									Get_Weapon() { return m_pWeapon; }
	EQUIP										Get_Equip() { return m_eEquip; }
	ITEM_NUMBER									Get_Equip_As_ITEM_NUMBER();
	DWORD										Get_Direction() { return m_dwDirection; }	// 플레이어 이동 상하좌우 계산
	_float										Get_CamDegree(); //카메라와 플레이어 간의 각도 계산
	_float4										Get_MuzzlePosition();
	wstring										Get_BiteLayerTag() { return m_strBiteLayerTag; }
	_int											Get_BiteAnimIndex() { return m_iBiteAnimIndex; }
	_int											Get_MaxBullet();
	// =============================== SET ===============================
	void										Set_isBite(_bool isBite) { m_isBite = isBite; }
	void										Set_Spotlight(_bool isSpotlight); 
	void										Requst_Change_Equip(EQUIP eEquip);
	void										Set_Equip(EQUIP* eEquip);
	void										Set_Equip_Gun(EQUIP* eEquip);
	void										Set_Equip_Sub(EQUIP* eEquip);
	void										Set_Hp(_int iHp);					
	void										Set_TurnSpineDefualt(_bool isTurnSpineDefault) { m_isTurnSpineDefault = isTurnSpineDefault; }
	void										Set_TurnSpineHold(_bool isTurnSpineHold) { m_isTurnSpineHold = isTurnSpineHold;}
	void										Set_TurnSpineLight(_bool isTurnSpineLight) { m_isTurnSpineLight = isTurnSpineLight; }
	void										Set_Gravity(_bool isGravity); 
	void Set_Position(_float4 vPos);
	void Set_Position(_fvector vPos);

	// ============================ CHANGE == SET ============================
	void										Change_Body_Animation_Move(_uint iPlayingIndex, _uint iAnimIndex);
	void										Change_Body_Animation_Hold(_uint iPlayingIndex, _uint iAnimIndex);
	void										Change_State(STATE eState);
	void										Change_AnimSet_Move(ANIMSET_MOVE eAnimSetMove) { m_eAnimSet_Move = eAnimSetMove; }
	void										Change_AnimSet_Hold(ANIMSET_HOLD eAnimSetHold) { m_eAnimSet_Hold = eAnimSetHold; }
	void										Change_Player_State_Bite(_int iAnimIndex, const wstring& strBiteLayerTag, _float4x4 Interpolationmatrix, _float fTotalInterpolateTime);
	void										Change_Equip_State(EQUIP_STATE eEquip_State);
	void										Change_WeaponLocation_To_Holster(CWeapon* pWeapon);
	void										Request_NextBiteAnimation(_int iAnimIndex);
	void										Shot();
	void										Throw_Sub();
	void										Reload();
	void										Stop_UpperBody();


	// ============================ CHECK = ISABLE ============================
	_bool										IsShotAble();
	_bool										IsReloadAble();
	
	// ============================ UPDATE ============================
	void										Update_InterplationMatrix(_float fTimeDelta);

private:
	void										Update_FSM();
	void										Update_KeyInput_Reload();
	void										Update_LightCondition();				// 현재 빛 상태에 따라 라이트를 키고 끔, 애니메이션 처리
	void										Update_Equip();
	void										Update_AnimSet();
	void										Update_Direction();
	void										Turn_Spine_Default(_float fTimeDelta);		// Idle 상태에서 카메라 반대쪽으로 머리 돌리기
	void										Turn_Spine_Hold(_float fTimeDelta);		// Hold 상태에서의 카메라 보기
	void										Turn_Spine_Light(_float fTimeDelta);		// Light 상태일때의 카메라 보기

	// ============================ INITIALIZE ============================
	HRESULT										Add_FSM_States();

public:
	void										Swap_Camera();

private:
	_int										m_iMaxHp = { 5 };
	_int										m_iHp = { 5 };

	_bool										m_isSpotlight = { false };
	DWORD										m_dwDirection = { 0 };

	_bool										m_isTurnSpineDefault = { false };
	_bool										m_isTurnSpineHold = { false };
	_bool										m_isTurnSpineLight = { false };

	ANIMSET_MOVE								m_eAnimSet_Move = { FINE };
	ANIMSET_HOLD								m_eAnimSet_Hold = { HOLD_HG };

	_bool										m_isBite = { false };
	_uint										m_eBiteType;
	_int										m_iBiteAnimIndex = { -1 };
	_float4x4									m_vBiteInterpolateMatrix;
	_float										m_fTotalInterpolateTime = 0.f;
	_float										m_fCurrentInterpolateTime = 0.f;
	wstring										m_strBiteLayerTag;

	EQUIP_STATE									m_eEquip_State = { GUN };
	_bool										m_isRequestChangeEquip = { false };				// 무기 교체 요청 들어옴
	EQUIP										m_eTargetEquip = { NONE };						// 플레이어 애님셋 교체에 관련된 장비
	EQUIP										m_eEquip = { NONE };								// 플레이어 애님셋과 관련된 장비
	EQUIP										m_eEquip_Gun = { NONE };							// 인벤토리에서 처리하는 장비된 무기
	EQUIP										m_eEquip_Sub = { NONE };							// 인벤토리에서 처리하는 장비된 Sub무기
	_int										m_SetProps[SETPROPS_NONE] = {};

	CWeapon*									m_pWeapon = { nullptr };
	vector<CWeapon*>							m_Weapons;

	class CCamera_Event*						m_pCamera_Event = { nullptr };

	friend class CPlayer_State_SubHold_Start;
	friend class CPlayer_State_Move_Walk;
	friend class CPlayer_State_Move_Jog;
	friend class CPlayer_State_Hold_Idle;
#pragma endregion

#pragma region 나옹 추가
public:
	/* For. Getter Inline*/
	_bool*										Col_Event_UI(class CCustomCollider* pCustom);
	_int*										Get_Hp_Ptr() { return &m_iHp;  }
	void										Set_Tutorial_Start(UI_TUTORIAL_TYPE i)
	{ 
		m_isTutorial_Notify = true;
		m_isPlayer_FirstBehavior[(_int)i] = true;
		m_eTutial_Type = i;
	}
	
	_bool*										Get_Tutorial_Notify()	{ return &m_isTutorial_Notify; }
	UI_TUTORIAL_TYPE*							Get_Tutorial_Type()		{ return &m_eTutial_Type; }
	_byte										Get_Player_State()		{ return m_eState; } /* Player 상태 반환 */

	/* For. Fuction */
	void										Player_First_Behavior();

	/* For. Variable */
	UI_TUTORIAL_TYPE							m_eTutial_Type					= { UI_TUTORIAL_TYPE::TUTORIAL_END };

	_bool										m_isNYResult;
	_bool										m_isPlayer_FirstBehavior[100] = { false };
	_bool										m_isTutorial_Notify = { false };
#pragma

#pragma region 예은 추가 
public:
	_bool													Get_Player_RegionChange() { return m_bChange; }
	_int													Get_Player_ColIndex() { return m_iCurCol; }
	_int													Get_Player_Direction() { return m_iDir; }
	_int													Get_Player_Floor() { return m_iFloor; } /* 현재 플레이어의 층수 */
	_int													Get_Player_Region() { return m_iRegion; } /* 현재 존재하는 지역 */
	_bool*												Get_Player_Interact_Ptr() { return &m_bInteract; }
	_bool*												Get_Player_Region_Array() { return m_bRegion; }
	enum PLAYER_LADDER_BEAVE			{LADDER_BEHAVE_NOTHING, LADDER_BEHAVE_UP, LADDER_BEHAVE_DOWN};
	enum PLAYER_DOOR_BEHAVE				{ DOOR_BEHAVE_NOTHING, DOOR_BEHAVE_OPEN, DOOR_BEHAVE_LOCK};
	enum PLAYER_LEVER_BEHAVE				{LEVER_BEHAVE_NOTHING, LEVER_BEHAVE_DOWN};

	PLAYER_DOOR_BEHAVE						Get_Door_Setting() { return (PLAYER_DOOR_BEHAVE)m_iDoor_Setting; }
	PLAYER_LADDER_BEAVE						Get_Ladder_Setting() { return (PLAYER_LADDER_BEAVE)m_iLadder_Setting; }
	PLAYER_LEVER_BEHAVE						Get_Lever_Setting() { return (PLAYER_LEVER_BEHAVE)m_iLever_Setting; } //추
	_float												Get_Door_Degree() { return m_fDoor_Degree; }
	_float4x4											Get_Ladder_WorldMatrix() { return m_LadderWorldMatrix; }
	_float4x4											Get_Lever_WorldMatrix() { return m_Lever_WorldMatrix; } //가
	void													Set_Door_Setting(_int iDoor_Setting, _float fDoorDegree = 0.f) {m_iDoor_Setting = iDoor_Setting; m_fDoor_Degree = fDoorDegree;};
	void													Set_Ladder_Setting(_int iLadder_Setting, _float4x4 LadderWorldMatrix = _float4x4()) { m_iLadder_Setting = iLadder_Setting; m_LadderWorldMatrix = LadderWorldMatrix; }
	void													Set_Lever_Setting(_int iLever_Setting, _float4x4 LeverWorldMatrix = _float4x4()) { m_iLever_Setting = iLever_Setting; m_Lever_WorldMatrix = LeverWorldMatrix; } //욤


private:
	_int											m_iDoor_Setting = { DOOR_BEHAVE_NOTHING };
	_float										m_fDoor_Degree = { 0.f };

	_int											m_iLadder_Setting = { LADDER_BEHAVE_NOTHING };
	_float4x4									m_LadderWorldMatrix = { _float4x4() };

	_int											m_iLever_Setting = { LEVER_BEHAVE_NOTHING };
	_float4x4									m_Lever_WorldMatrix = { _float4x4() };


	_bool											m_bInteract = { false }; //플레이어가 상호작용을 시도한
	_bool											m_bChange = { true };
	_int											m_iCurCol = { 0 };
	_int											m_iRegion = { 0 };
	_int											m_iDir = { 0 };
	_int											m_iPreCol = { 1 };
	_int											m_iFloor = { 2 };
	_float										m_fTimeTEST = { 0.f };
	_bool											m_bRegion[100] = { false, };
#pragma endregion


#pragma region 창균 추가
public:
	void										PickUp_Item(CGameObject* pPickedUp_Item); //TabWindow의 PickUp_Item함수 호출용
	void										Set_isCamTurn(_bool isCamTurn) { m_isCamTurn = isCamTurn; }

private:
	_bool										m_isCamTurn = { false };
	class CTab_Window*				m_pTabWindow = { nullptr };
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
	void										Apply_Recoil(_float fTimeDelta);

public:
	void										Set_ManualMove(_bool isManualMove);

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

	_bool										m_bRecoil = { false };
	_float										m_fRecoil_Rotate_Amount_X = { 0.f };
	_float										m_fRecoil_Rotate_Amount_Y = { 0.f };
	_float										m_fRecoil_Rotate_Amount_X_Current = { 0.f };
	_float										m_fRecoil_Rotate_Amount_Y_Current = { 0.f };
	_float										m_fRecoil_Lerp_Time = { 0.f };
	_float										m_fRecoil_Lerp_Time_Omega = { 0.f };

	_bool										m_isManualMove = { false };

#pragma region Effect
public:
	void	Ready_Effect();
	void	Release_Effect();
	void	Tick_Effect(_float fTimeDelta);
	void	Late_Tick_Effect(_float fTimeDelta);
private:
	class CMuzzle_Flash*						m_pMuzzle_Flash = { nullptr };
	class CMuzzle_Flash_SG*						m_pMuzzle_Flash_SG = { nullptr };
#pragma endregion
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