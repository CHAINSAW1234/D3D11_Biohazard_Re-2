#pragma once

#include "Client_Defines.h"
#include "Decorator_Node.h"

BEGIN(Client)

class CIs_Collision_Prop_Zombie : public CDecorator_Node
{
public:
	enum class COLL_PROP_TYPE { _WINDOW, _DOOR, _END };
private:
	CIs_Collision_Prop_Zombie();
	CIs_Collision_Prop_Zombie(const CIs_Collision_Prop_Zombie& rhs);
	virtual ~CIs_Collision_Prop_Zombie() = default;

public:
	virtual HRESULT					Initialize(COLL_PROP_TYPE ePropType);

	virtual _bool					Condition_Check();

public:
	void							SetBlackBoard(class CBlackBoard_Zombie* pBlackBoard) { m_pBlackBoard = pBlackBoard; }
protected:
	class CBlackBoard_Zombie*		m_pBlackBoard = { nullptr };
	COLL_PROP_TYPE					m_eTargetCollPropType = { COLL_PROP_TYPE::_END };

public:
	static CIs_Collision_Prop_Zombie* Create(COLL_PROP_TYPE ePropType);

public:
	virtual void Free() override;
};

END