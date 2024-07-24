#pragma once

#include "Client_Defines.h"
#include "InteractProps.h"



BEGIN(Client)

class CBigStatue final : public CInteractProps
{
public:
	enum BIGSTATUE_STATE
	{
		STATE_PLAY,
		STATE_STATIC
	};
	enum BIGSTATUE_TYPE
	{
		BIGSTATUE_UNICON,
		BIGSTATUE_WOMAN,
		BIGSTATUE_LION,
	};

	enum BIGSTATUE_PART
	{
		PART_BODY,
		PART_MINI_STATUE, //�޴��� ������ �̴� ����
		PART_MINI_PARTS_STATUE, //�޴��� ������ �̴� ����
		PART_MEDAL, // �޴�
		PART_DIAL, //���� 
		PART_END,
	};
	enum LOCK_KEY_INPUT
	{
		KEY_NOTHING,
		KEY_W,
		KEY_A,
		KEY_S,
		KEY_D,
	};
	enum LOCK_STATE
	{
		STATIC_LOCK, // �׳� ��������0 0
		LIVE_LOCK, //���� Ǫ�� �ൿ�� �Ҷ� => ���� ��������1
		WRONG_LOCK, // ���� Ʋ���� ��2 1
		CLEAR_LOCK // ���� Ǯ������ 3 2
	};
private:
	CBigStatue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBigStatue(const CBigStatue& rhs);
	virtual ~CBigStatue() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void				Tick(_float fTimeDelta) override;
	virtual void				Late_Tick(_float fTimeDelta) override;
	virtual HRESULT				Render() override;
	virtual void Do_Interact_Props() override;

private:
	virtual HRESULT				Add_Components();
	virtual HRESULT				Add_PartObjects() override;
	virtual HRESULT				Initialize_PartObjects() override;
	virtual HRESULT				Bind_ShaderResources() override;


private:
	void						Active();
			
public:
	virtual _float4 Get_Object_Pos() override;
private:
	_bool					m_bCamera = { false };
	_bool					m_bCameraReset = { false };
	_bool					m_bAutoOpen = { false };
	_bool					m_bMove = { false };
	_float				m_fDelayLockTime = { 0.f };
	_ubyte				m_eType = { BIGSTATUE_UNICON };
	_bool				m_bActivity = { false };
	_float				m_fTime = { 0.f };
	_ubyte				m_eState = { STATE_STATIC };
	_int					m_iPassWord[10] = { 1, };

private :
	_ubyte				m_eLockState = { STATIC_LOCK };
	_ubyte				m_eKeyInput = { KEY_NOTHING };


	_ubyte				m_eStatue_ItemType = { 0 };
	_bool				m_isPut_HandItem = { false };
	_bool				m_isMedalAnim = { false };

public:
	static CBigStatue* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END