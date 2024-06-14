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
	void							Initialize_BlackBoard();

public: // Setter
	void							SetPlayer(class CPlayer* pPlayer)
	{
		m_pPlayer = pPlayer;
	}

public: // Getter
	class CPlayer*					GetPlayer()
	{
		return m_pPlayer;
	}
	
protected:
	class CGameInstance*			m_pGameInstance = { nullptr };
	class CPathFinder*				m_pPathFinder = { nullptr };
	class CPlayer*					m_pPlayer = { nullptr };
public:
	static CBlackBoard_Zombie* Create();

public:
	virtual void Free() override;
};

END