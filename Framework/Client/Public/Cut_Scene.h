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

public:
	virtual void								Start() override;

protected:
	virtual	HRESULT								SetUp_Animation_Layer();

public:
	inline void									Play() { m_isPlaying = true; }
	inline void									Stop() { m_isPlaying = false; }

public:
	virtual void								Start_CutScene();
	virtual void								Finish_CutScene();

public:
	inline _bool								Is_Playing() { return m_isPlaying;  }	

protected:
	HRESULT										Add_Actor(const wstring& strPrototypeTag, _uint iActorType, void* pArg);
	HRESULT										Add_PropController(const wstring& strPrototypeTag, _uint iPropType, void* pArg);

	virtual HRESULT								Add_Actors() = 0;
	virtual HRESULT								Add_Props() = 0;
	virtual	HRESULT								Add_Camera_Event() = 0;

public:
	class CActor*								Get_Actor(_uint iActorType);

private:
	void Priority_Tick_Actors(_float fTimeDelta);
	void Tick_Actors(_float fTimeDelta);
	void Late_Tick_Actors(_float fTimeDelta);

protected:
	vector<class CActor*>						m_Actors;
	vector<class CProp_Controller*>				m_PropControllers;
	class CCamera_Event*						m_pEvent_Camera = { nullptr };
	wstring										m_strCamera_Event_Tag = { TEXT("") };

	_bool										m_isPlaying = { false };

public:
	virtual	void Free();
};

END