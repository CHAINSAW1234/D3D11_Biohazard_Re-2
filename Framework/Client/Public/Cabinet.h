#pragma once

#include "Client_Defines.h"
#include "InteractProps.h"



BEGIN(Client)

class CCabinet final : public CInteractProps
{
public:
	enum CABINET_TYPE
	{
		TYPE_NORMAL,
		TYPE_SAFEBOX,
		TYPE_ELECTRIC,
		TYPE_LEON,
	};
	enum CABINET_STATE
	{
		//ĳ���_�ٵ� �ִ�
		CABINET_CLOSED,
		CABINET_OPEN,
		CABINET_OPENED,
		CABINET_END,
	};
	enum LOCK_STATE
	{
		STATIC_LOCK, // �׳� ��������
		LIVE_LOCK, //���� Ǫ�� �ൿ�� �Ҷ� => ���� ��������
		WRONG_LOCK, // ���� Ʋ���� ��
		CLEAR_LOCK // ���� Ǯ������ 
	};
	enum CABINET_PART
	{
		PART_BODY,
		PART_ITEM,
		PART_LOCK,
		PART_LOCK1, //���²�
		PART_END
	};
private:
	CCabinet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCabinet(const CCabinet& rhs);
	virtual ~CCabinet() = default;

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
	void Active();

public:
	virtual _float4 Get_Object_Pos() override;


private:
	_bool				m_bReonDesk = { false };
	_bool				m_bItemDead = { true };
	_bool				m_bObtain = { false };
	_bool				m_bOpened = { false };
	_bool				m_bLock =	{ false };
	_bool				m_bActivity = { false };
	_bool				m_bCamera = { false };

	_ubyte			m_eState = { CABINET_CLOSED };
	_ubyte			m_eLockState = { STATIC_LOCK };
	_int				m_eCabinetType = { TYPE_NORMAL };

public:
	static CCabinet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END