#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CCut_Scene abstract: public CGameObject
{
protected:
	CCut_Scene(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCut_Scene(const CCut_Scene& rhs);
	virtual ~CCut_Scene() = default;

public:
	virtual HRESULT								Initialize_Prototype() override;
	virtual HRESULT								Initialize(void* pArg) override;
	virtual void								Priority_Tick(_float fTimeDelta) override;
	virtual void								Tick(_float fTimeDelta) override;
	virtual void								Late_Tick(_float fTimeDelta) override;
	virtual HRESULT								Render() override;

protected:
	virtual	HRESULT								SetUp_Animation_Layer();

public:
	inline void									Play() { m_isPlaying = true; }
	inline void									Stop() { m_isPlaying = false; }

public:
	virtual void								Start() = 0;
	virtual void								Finish() = 0;

public:
	inline _bool								Is_Playing() { return m_isPlaying;  }	

protected:
	HRESULT										Add_Actor(const wstring& strPrototypeTag, _uint iActorType, void* pArg);
	virtual HRESULT								Add_Actors() = 0;

public:
	class CActor*								Get_Actor(_uint iActorType);

private:
	void Priority_Tick_Actors(_float fTimeDelta);
	void Tick_Actors(_float fTimeDelta);
	void Late_Tick_Actors(_float fTimeDelta);

protected:
	vector<class CActor*>						m_Actors;

	_bool										m_isPlaying = { false };

public:
	virtual	void Free();
};

END