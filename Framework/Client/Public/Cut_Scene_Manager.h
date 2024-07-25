#pragma once

#include "Base.h"
#include "Client_Defines.h"

BEGIN(Client)

class CCut_Scene_Manager :
    public CBase
{
	DECLARE_SINGLETON(CCut_Scene_Manager)

private:
	CCut_Scene_Manager();
	virtual ~CCut_Scene_Manager() = default;

public:
	enum class CF_ID { _92, _93, _94, _95, _120, _150, _151, _152, _153, _190, _END };

public:
	HRESULT				Initialize();

	HRESULT				Add_CutScene(CF_ID eCutScene, class CCut_Scene* pCutScene);

public:
	void				Play_CutScene(CF_ID eCutScene);

private:
	vector<class CCut_Scene*>				m_CutScenes;

public:
	virtual void Free() override;
};

END