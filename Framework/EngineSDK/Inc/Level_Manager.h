#pragma once

#include "Base.h"


BEGIN(Engine)

class CLevel_Manager final : public CBase
{
private:
	CLevel_Manager();
	virtual ~CLevel_Manager() = default;

public:
	HRESULT Initialize();
	HRESULT Open_Level(_uint iNewLevelID, class CLevel* pNewLevel);
	void Tick(_float fTimeDelta);
	HRESULT Render();


private:
	_uint					m_iCurrentLevelID = { 0 };
	class CLevel*			m_pCurrentLevel = { nullptr };

public:
	static CLevel_Manager* Create();
	virtual void Free() override;
};

END