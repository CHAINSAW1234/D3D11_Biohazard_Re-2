#pragma once
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CObserver abstract /*: public CBase*/
{
protected:
	CObserver();
	virtual ~CObserver() = default;

public:
	virtual void				OnNotify() = 0;
	//virtual void Free() override {}
};

END

