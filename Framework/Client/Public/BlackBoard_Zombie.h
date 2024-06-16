#pragma once

#include "Client_Defines.h"
#include "BlackBoard.h"

BEGIN(Client)

class CBlackBoard_Zombie : public CBlackBoard
{
public:
	CBlackBoard_Zombie();
	CBlackBoard_Zombie(const CBlackBoard_Zombie& rhs);
	virtual ~CBlackBoard_Zombie() = default;

public:
	virtual HRESULT					Initialize_Prototype();
	virtual HRESULT					Initialize(void* pArg);
	void							Initialize_BlackBoard(class CZombie* pAI);

public: // Setter
	void							SetPlayer(class CPlayer* pPlayer)
	{
		m_pPlayer = pPlayer;
	}
	class CZombie* GetAI()
	{
		return m_pAI;
	}

public: // Getter
	class CPlayer*					GetPlayer()
	{
		return m_pPlayer;
	}
	
protected:
	class CPathFinder*				m_pPathFinder = { nullptr };
	class CPlayer*					m_pPlayer = { nullptr };
	class CZombie*					m_pAI = { nullptr };

public:
	static CBlackBoard_Zombie* Create();

public:
	virtual void Free() override;
};

END