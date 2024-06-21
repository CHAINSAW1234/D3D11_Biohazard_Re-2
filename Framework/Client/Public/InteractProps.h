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
		INTERACTPROPS_COL_AABB ,
		INTERACTPROPS_COL_SPHERE ,
		INTERACTPROPS_COL_OBB ,
		INTERACTPROPS_COL_END
	};


public:
	typedef struct tagInteractProps_desc: public CGameObject::GAMEOBJECT_DESC
	{
		_int BelongIndexs2[iMaxNum];
		_int iPropsType;
		_int iRegion;

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
	virtual HRESULT										Render() override;


	void												Priority_Tick_PartObjects(_float fTimeDelta);
	void												Tick_PartObjects(_float fTimeDelta);
	void												Late_Tick_PartObjects(_float fTimeDelta);



	virtual HRESULT										Render_LightDepth_Dir() override;
	virtual	HRESULT										Render_LightDepth_Spot() override;
	virtual HRESULT										Render_LightDepth_Point() override;
	
public:
	_bool*												Get_Activity() { return &m_bActivity; }


protected:
	_bool												m_bActivity = { true };
	_bool												m_bShadow = { true };
	_bool												m_bVisible = { true };
	_bool												m_bCol = { false }; // 충돌이 되었다
	_float												m_fTimeTest = { 0.f };
	CModel*												m_pModelCom = { nullptr };
	CShader*											m_pShaderCom = { nullptr };
	CCollider*											m_pColliderCom[INTERACTPROPS_COL_END] = { nullptr,nullptr,nullptr };
	
	class CPlayer*										m_pPlayer = { nullptr };
	_bool*												m_pPlayerInteract = { nullptr };
	CTransform*											m_pPlayerTransform = { nullptr };

	_int												m_iPropsType = { 0 };
	_float3												m_vRootTranslation = {};
	INTERACTPROPS_DESC 									m_tagPropDesc ={};
	vector<CPartObject*>								m_PartObjects;

	
protected:
	void														Check_Player();
	void														Check_Col_Sphere_Player();
	_bool														Visible();
	virtual HRESULT										Add_Components();
	virtual HRESULT										Add_PartObjects();
	virtual HRESULT										Initialize_PartObjects();
	virtual HRESULT										Bind_ShaderResources();

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END