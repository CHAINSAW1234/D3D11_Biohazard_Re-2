#pragma once

#include "Client_Defines.h"
#include "Task_Node.h"
#include "Body_Zombie.h"

BEGIN(Client)

class CHold_Out_Hand_Window_Zombie : public CTask_Node
{
private:
	CHold_Out_Hand_Window_Zombie();
	CHold_Out_Hand_Window_Zombie(const CHold_Out_Hand_Window_Zombie& rhs);
	virtual ~CHold_Out_Hand_Window_Zombie() = default;

public:
	virtual HRESULT					Initialize(void* pArg);

	virtual void					Enter() override;
	virtual _bool					Execute(_float fTimeDelta) override;
	virtual void					Exit() override;

private:
	void							Change_Animation(_float fTimeDelta);

public:
	inline void						SetBlackBoard(class CBlackBoard_Zombie* pBlackBoard) { m_pBlackBoard = pBlackBoard; }

protected:
	class CBlackBoard_Zombie* m_pBlackBoard = { nullptr };

	_int							m_iBasePlayingIndex = { -1 };
	const wstring					m_strAnimLayerTag = { TEXT("Gimmick_Window") };

public:
	static CHold_Out_Hand_Window_Zombie* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END