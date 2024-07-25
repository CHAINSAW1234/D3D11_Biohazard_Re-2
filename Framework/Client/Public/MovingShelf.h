#pragma once

#include "Client_Defines.h"
#include "InteractProps.h"



BEGIN(Client)

class CMovingShelf final : public CInteractProps
{
public:
	enum SHELF_TYPE
	{
		SHELF_197_RICKER,
		SHELF_232_MOVE,
		SHELF_TYPE_END
	};
	enum SHELF_STATE
	{
		SHELF_FINISH, // �� �Ѿ�߸� ���� ()
		SHELF_MOVE, // ������ ����(�÷��̾ Ȧ���ϰ� �ִ� ����)
		SHELF_START, // �����̷� �ϴ� ����(�÷��̾ ������ ����)
		SHELF_STOP, // ���� ���� ���� (Ȧ������ �ʰ� ��������)
		SHELF_STATIC, // ������ ����
		SHELF_END,
	};
	enum SHELF_PART
	{
		PART_BODY,
		PART_END
	};
private:
	CMovingShelf(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMovingShelf(const CMovingShelf& rhs);
	virtual ~CMovingShelf() = default;

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
	virtual HRESULT				Bind_ShaderResources() override;

private:
	void								Active();
	void								Find_HoldUI();

public:
	virtual _float4						Get_Object_Pos() override;
	_float4x4							Get_WorldMatrix() { return m_pTransformCom->Get_WorldFloat4x4(); }
	_int								Get_Shelf_Type() { return m_eType; }
	_int								Get_Anim_State() { return m_eState; }
	void								Set_Anim_State(_int eState) { m_eState = eState; }

private:
	_ubyte							m_eType = { SHELF_232_MOVE };
	_ubyte							m_eState = { SHELF_STATIC };

	class CHold_UI*					m_pHold_UI = { nullptr };


public:
	static CMovingShelf* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END