#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CPartObject;
class CCollider;
class CBone;
END

BEGIN(Client)

class CInteractProps abstract : public CGameObject
{
public:
	const static _int iMaxNum = 50;
	enum INTERACTPROPS_COL
	{
		INTER_COL_NORMAL,
		INTER_COL_DOUBLE,
		INTER_COL_TRIPLE,
		INTER_COL_END
	};
	enum PROPS_DISTANCE_RANGE
	{
		RANGE_MAX,
		RANGE_MIN,
		RANGE_END,
	};
	enum INTERACTPROPS_COL_STEP
	{
		COL_STEP0, // 인지 가능 표시 UI를 띄울 범위
		COL_STEP1, // 상호작용 가능 표시 UI를 띄울 범위
		COL_STEP2, // 그냥 상호작용해버리는 범위
		COL_STEP_END,
	};

	enum INTERACT_GIMMICK_TYPE
	{
		LOCK_GIMMICK,
		KEY_GIMMICK,
		NONE_GIMMICK
	};

	typedef struct Door_Desc
	{
		//lock의 여부, 엠블럼||사슬
		_bool		bLock = { false };
		_int		iLockType = { 0 };
		_int		iEmblemType = { 0 };
	}DOOR_DESC;

	typedef struct Cabinet_Desc
	{
		//lock의 여부, 자물쇠, 아이템
		_bool		bLock = { false };
		_int		iLockType = { 0 };
		_int		iLockNum[10] = { -1, };

		_bool		bItem = { false };
		_int		iItemIndex = { 0 };
		wstring Name = { TEXT("") };

	}CABINET_DESC;

	typedef struct Window_Desc
	{

	}WINDOW_DESC;

	typedef struct Shutter_Desc
	{
		// 몇번째 레버와 동작

	}SHUTTER_DESC;

	typedef struct Hall_Statue_Desc
	{

	}HALL_STATUE_DESC;

	typedef struct Statue_Desc
	{

	}STATUE_DESC;

	typedef struct BIG_Statue_Desc
	{
		_int		iLockNum[10] = { -1, };

	}BIG_STATUE_DESC;

	typedef struct Event_Prop_Desc
	{

	}EVENT_PROP_DESC;


	typedef struct ITEM_Prop_Desc
	{
		_int iItemIndex = { 0 };
		wstring Name = { TEXT("") };

	}ITEM_PROP_DESC;

	typedef struct Reader_Prop_Desc
	{


	}READER_PROP_DESC;

public:
	typedef struct tagInteractProps_desc : public CGameObject::GAMEOBJECT_DESC
	{
		_int BelongIndexs2[iMaxNum];
		DOOR_DESC tagDoor = {};
		CABINET_DESC tagCabinet = {};
		WINDOW_DESC tagWindow = {};
		SHUTTER_DESC tagShutter = {};
		HALL_STATUE_DESC tagHallStatue = {};
		STATUE_DESC tagStatue = {};
		BIG_STATUE_DESC tagBigStatue = {};
		ITEM_PROP_DESC tagItemDesc = {};
		READER_PROP_DESC tagReaderDesc = {};

	}INTERACTPROPS_DESC;

protected:
	CInteractProps(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInteractProps(const CInteractProps& rhs);
	virtual ~CInteractProps() = default;

public:
	virtual HRESULT										Initialize_Prototype() override;
	virtual HRESULT										Initialize(void* pArg) override;
	virtual void										Priority_Tick(_float fTimeDelta) override;
	virtual void										Tick(_float fTimeDelta) override;
	virtual void										Late_Tick(_float fTimeDelta) override;
	virtual void										Start() override;
	virtual HRESULT										Render() override;


	void												Priority_Tick_PartObjects(_float fTimeDelta);
	void												Tick_PartObjects(_float fTimeDelta);
	void												Late_Tick_PartObjects(_float fTimeDelta);

	void												Camera_Active(_int ePart, _float3 vRatio, INTERACT_GIMMICK_TYPE _layoutType, _float4 vPos = _float4{ 0.f,0.f,0.f,1.f });

	void												Reset_Camera();
	


	virtual HRESULT								Render_LightDepth_Dir() override;
	virtual	HRESULT								Render_LightDepth_Spot() override;
	virtual HRESULT								Render_LightDepth_Point() override;
	
public:
	INTERACTPROPS_DESC*					Get_DESC() { return &m_tagPropDesc; }
	_bool*											Get_Activity() { return &m_bActivity; }

	_bool*											ComeClose_toPlayer(_float _come); /* NY : 해당 거리까지 Obj에 플레이어가 다가갔는 지 확인 */
	_bool*											Selector_Rendering() { return &m_isSelector_Rendering;  }

	/*To NY*/
	virtual _float4										Get_Object_Pos() = 0;
	_int												Get_PropType() { return m_tagPropDesc.iPropType; } // 프롭타입이라 쓰고 arg라 읽는다. // 문의 지역 enum을 반환한다.


	_bool												Get_Interact_With_Player_Once() { return m_bFirstInteract; }
	_int												Get_iItemIndex() { return m_iItemIndex; }
	_int												Get_Floor() { return m_tagPropDesc.iFloor; }
	_int												Get_Region() { return m_tagPropDesc.iRegionNum; }
	_int												Get_Type() { return m_tagPropDesc.iPropType; }
	void												Set_Region(_int iRegion) { m_tagPropDesc.iRegionNum = iRegion; }
	virtual _bool									Attack_Prop(class CTransform* pTransfromCom = nullptr) { return false; };

	_float											Get_PlayerLook_Degree();
	
#pragma region 나옹
	/* Function */
	_bool												Create_Selector_UI();
	void												Opreate_Selector_UI(_bool _Interact, _float4 _pos);
	void												Select_UI();
	/* Variable */
	class CSelector_UI*									m_pSelector = { nullptr }; /* 사용 중인 Selector Obj */

#pragma endregion
#pragma region	For 창균오빠 - interact_Props() && GET
public:
	_int												Get_NeedItem_Index() const { return m_iNeedItem; }
	virtual void										Do_Interact_Props() { return ; }
	virtual CGameObject*										Get_Item_Props() { return this ; }

public:
	void												Set_OutOfControll(_bool isOutOfControll) { m_isOutOfControll = isOutOfControll; }

protected:
	_int												m_iNeedItem = {-1};// 필요 없으면 -1 => 이건 문제가 안되는게 어차피 interact_props함수를 interactprops에서 부를 것이기 때문이다.
#pragma endregion

private :
	_bool												m_isSelector_Rendering = { false };
	_bool												m_isNYResult			= { false };

protected:
	_bool												m_isCamera_Reset = { false };
	_bool												m_bActivity = { false };
	_bool												m_bOnce = { false };
	_bool												m_bBlock = { false };
	_bool												m_bFirstInteract = { false };// 한번 접촉하면 계속 true
	_bool												m_bShadow = { true };
	_bool												m_bVisible = { true };
	_bool												m_bCol[INTER_COL_END][COL_STEP_END] = { {false,false,false},{false,false,false} };
	_int												m_iItemIndex = { -1 };
	_float												m_fTimeDelay = { 0.f };
	_float												m_fDistance = { 0.f };
	_float											m_fVolumeRangeDistance[RANGE_END];
	CModel*												m_pModelCom = { nullptr };
	CShader*											m_pShaderCom = { nullptr };
	CCollider*											m_pColliderCom[INTER_COL_END][COL_STEP_END] = { {nullptr,nullptr,nullptr},{nullptr,nullptr,nullptr} };
	
	class CPlayer*										m_pPlayer = { nullptr };
	_bool*												m_pPlayerInteract = { nullptr };//player의 m_bInteract 변수 포인터
	CTransform*											m_pPlayerTransform = { nullptr }; 
	CCollider*											m_pPlayerCol = { nullptr };

	class CCamera_Free*									m_pCamera = { nullptr };
	CTransform*											m_pCameraTransform = { nullptr };

	class CCamera_Gimmick*								m_pCameraGimmick = { nullptr };
	CTransform*											m_pCameraGimmickTransform = { nullptr };

	_float3												m_vRootTranslation = {};
	INTERACTPROPS_DESC 									m_tagPropDesc ={};
	vector<CPartObject*>								m_PartObjects;

	_bool												m_isOutOfControll = { false };
	_bool												m_bSoundCueSign = { false };
protected:
	void												Check_Player();
	_float												Check_Player_Distance();
	_float												Check_Player_Distance(_float4 vPos);
	_float4												Get_Collider_World_Pos(_float4 vPos);
	_bool												Check_Col_Player(INTERACTPROPS_COL eInterCol, INTERACTPROPS_COL_STEP eStepCol);
	void												Tick_Col();
	_bool												Visible();

	void Change_Sound(const wstring& strSoundTag, _uint iSoundIndex)
	{
		m_pGameInstance->Change_Sound_3D(m_pTransformCom, strSoundTag, iSoundIndex, true);
	}
	void Change_Same_Sound(const wstring& strSoundTag, _uint iSoundIndex)
	{
		m_pGameInstance->Change_Sound_3D(m_pTransformCom, strSoundTag, iSoundIndex);
	}

	void Stop_Sound(_uint iSoundIndex)
	{
		m_pGameInstance->Stop_Sound_3D(m_pTransformCom, iSoundIndex);
	}

#ifdef		_DEBUG
	void												Add_Col_DebugCom();

#endif

	_bool												Activate_Col(_float4 vActPos); //카메라 look에 대해 판단

	virtual HRESULT										Add_Components();
	virtual HRESULT										Add_PartObjects();
	virtual HRESULT										Initialize_PartObjects();
	virtual HRESULT										Bind_ShaderResources();

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END