#pragma once

#include "Client_Defines.h"
#include "InteractProps.h"



BEGIN(Client)

class CChair final : public CInteractProps
{
public:
	enum CHAIR_TYPE
	{
		CHAIR_ZOMBIE,  //sm45_018
		CHAIR_BARRIGATE,
		CHAIR_END
	};
	enum CHAIR_PART
	{
		PART_BODY,
		PART_END
	};
	enum ZOMBIECHAIR_STATE
	{
		Zom_Static,
		Zom_4631_Start,
		Zom_4632_Start
	};
	enum BARRIGATE_STATE
	{
		BARRI_MOVE,
		BARRI_STATIC,
	};
private:
	CChair(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CChair(const CChair& rhs);
	virtual ~CChair() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void					Start() override;
	virtual void					Tick(_float fTimeDelta) override;
	virtual void					Late_Tick(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

private:
	virtual HRESULT				Add_Components();
	virtual HRESULT				Add_PartObjects() override;
	virtual HRESULT				Initialize_PartObjects() override;
	virtual HRESULT				Bind_ShaderResources() override;

	void								Zombie_Tick(_float fTimeDelta);
	void								Barrigate_Tick(_float fTimeDelta);

private:
	void Barrigate_Active();

public:
	virtual _float4 Get_Object_Pos() override;


private:
	_bool				m_bActivity = { false };
	_bool					m_bCamera = { false };

	_ubyte			m_eBarriState = { BARRI_STATIC };
	_ubyte			m_eZombieState = { Zom_Static };
	_ubyte			m_eType= { CHAIR_ZOMBIE };

	class CDoor* m_pDoor = { nullptr };

public:
	static CChair* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END