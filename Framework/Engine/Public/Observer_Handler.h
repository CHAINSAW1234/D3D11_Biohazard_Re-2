#pragma once
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CObserver_Handler abstract/* : public CBase*/
{
protected:
	CObserver_Handler();
	virtual ~CObserver_Handler() = default;

public:
	virtual void RegisterObserver(class CObserver* observer/*, _uint iLevelIndex*/);
	virtual void RemoveObserver(class CObserver* observer/*, _uint iLevelIndex*/);
	virtual void NotifyObserver();

private:
	vector<class CObserver*>	m_vecObserver;
	//
	//public:
	//	virtual void Free() override;
};

END

