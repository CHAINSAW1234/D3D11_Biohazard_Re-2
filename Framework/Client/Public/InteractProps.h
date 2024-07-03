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
		INTERACTPROPS_COL_AABB,
		INTERACTPROPS_COL_SPHERE,
		INTERACTPROPS_COL_OBB,
		INTERACTPROPS_COL_END
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

	}BIG_STATUE_DESC;

	typedef struct Event_Prop_Desc
	{

	}EVENT_PROP_DESC;


	typedef struct ITEM_Prop_Desc
	{
		_int iItemIndex = { 0 };
		wstring Name = { TEXT("") };

	}ITEM_PROP_DESC;

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
	virtual void										Start() override { return; };
	virtual HRESULT										Render() override;


	void												Priority_Tick_PartObjects(_float fTimeDelta);
	void												Tick_PartObjects(_float fTimeDelta);
	void												Late_Tick_PartObjects(_float fTimeDelta);



	virtual HRESULT										Render_LightDepth_Dir() override;
	virtual	HRESULT										Render_LightDepth_Spot() override;
	virtual HRESULT										Render_LightDepth_Point() override;
	
public:
	_bool*												Get_Activity() { return &m_bActivity; }

#pragma region NY
	_bool*												ComeClose_toPlayer(_float _come); /* NY : 해당 거리까지 Obj에 플레이어가 다가갔는 지 확인 */
	_bool*												Selector_Rendering() { return &m_isSelector_Rendering;  }

	/*To NY*/
	virtual _float4									Get_Object_Pos() = 0;
	_int													Get_PropType() { return m_tagPropDesc.iPropType; } // 프롭타입이라 쓰고 arg라 읽는다. // 문의 지역 enum을 반환한다.


	_bool												Get_Interact_With_Player_Once() { return m_bFirstInteract; }
	_int												Get_iItemIndex() { return m_iItemIndex; }
	_int												Get_Floor() { return m_tagPropDesc.iFloor; }
	_int												Get_Region() { return m_tagPropDesc.iRegionNum; }
	_int												Get_Type() { return m_tagPropDesc.iPropType; }
	void												Set_Region(_int iRegion) { m_tagPropDesc.iRegionNum = iRegion; }
	virtual _bool									Attack_Prop(class CTransform* pTransfromCom = nullptr) { return false; };
private :
	_bool												m_isSelector_Rendering = { false };
	_bool												m_isNYResult				= { false };

protected:
	_int												m_iItemIndex = { -1 };
	_bool												m_bActivity = { true };
	_bool												m_bOnce = { true };
	_bool												m_bDoorOnce = { false };
	_bool												m_bBlock = { false };
	_bool												m_bFirstInteract = { false };// 한번 접촉하면 계속 true
	_bool												m_bShadow = { true };
	_bool												m_bVisible = { true };
	_bool												m_bCol = { false }; // 충돌이 되었다
	_float												m_fTimeDelay = { 0.f };
	_float												m_fDistance = { 0.f };
	CModel*												m_pModelCom = { nullptr };
	CShader*											m_pShaderCom = { nullptr };
	CCollider*											m_pColliderCom[INTERACTPROPS_COL_END] = { nullptr,nullptr,nullptr };
	
	class CPlayer*										m_pPlayer = { nullptr };
	_bool*												m_pPlayerInteract = { nullptr };//player의 m_bInteract 변수 포인터
	CTransform*											m_pPlayerTransform = { nullptr };

	_float3												m_vRootTranslation = {};
	INTERACTPROPS_DESC 									m_tagPropDesc ={};
	vector<CPartObject*>								m_PartObjects;

protected:
	void												Check_Player();
	_float											Check_Player_Distance();
	_float											Check_Player_Distance(_float4 vPos);
	_float3											Get_Collider_World_Pos(_float3 vPos);
	_bool												Check_Col_Sphere_Player();
	_bool												Check_Col_OBB_Player();
	_bool												Check_Col_AABB_Player();
	_bool												Visible();
	_float											Get_PlayerLook_Degree();

	virtual HRESULT										Add_Components();
	virtual HRESULT										Add_PartObjects();
	virtual HRESULT										Initialize_PartObjects();
	virtual HRESULT										Bind_ShaderResources();
public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END