#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CPart_Breaker_Zombie final : public CBase
{
private:
	CPart_Breaker_Zombie();
	CPart_Breaker_Zombie(const CPart_Breaker_Zombie& rhs);
	virtual ~CPart_Breaker_Zombie() = default;

public:
	
private:

public:
	static CPart_Breaker_Zombie* Create(void* pArg);
	virtual void Free() override;
};

END