#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
END

BEGIN(Client)

class CCustomCollider final: public CGameObject
{
public:
	typedef struct tagColDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_int				iColNum = {0};
		_int				iDir = {0};

		_float4x4		worldMatrix = { };
	}COLLIDER_DESC;

private:
	CCustomCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCustomCollider(const CCustomCollider& rhs);
	virtual ~CCustomCollider() = default;
	
	
public:
	virtual HRESULT							Initialize_Prototype() override;
	virtual HRESULT							Initialize(void* pArg) override;
	virtual void							Priority_Tick(_float fTimeDelta);
	virtual void							Tick(_float fTimeDelta) override;
	virtual void							Late_Tick(_float fTimeDelta) override;
	virtual HRESULT							Render() override;

public:
	_int									Get_Index() { return m_iIndex; }
	_float4									Get_Position() { return m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION); }
	_int									Get_Col() { return m_iColNum; }
	_int									Get_Dir() { return m_iDir; }
	_int*									Get_Col_Ptr() { return &m_iColNum; }
	_int*									Get_Dir_Ptr() { return &m_iDir; }
private:
	HRESULT									Add_Components(COLLIDER_DESC* pCol);
private:
	_int									m_iColNum = { 0 };
	_int									m_iDir = { 0 };

	_int									m_iIndex = { 0 };
	CCollider*								m_pColliderCom = {nullptr};

public:
	static CCustomCollider*					Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*					Clone(void* pArg) override;
	virtual void							Free() override;

};

END