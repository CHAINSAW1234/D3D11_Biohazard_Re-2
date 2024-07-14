#pragma once

#include "Client_Defines.h"
#include "Task_Node.h"
#include "Body_Zombie.h"

BEGIN(Client)

class CTurn_Lost_Zombie : public CTask_Node
{
public:
	enum class ZOMBIE_TURN_LOST_TYPE { _A, _C, _C_SYM, _D, _D_SYM, _E, _E_SYM, _F, _F_SYM, _G, _G_SYM, _H, _I, _J, _END };

public:
	CTurn_Lost_Zombie();
	CTurn_Lost_Zombie(const CTurn_Lost_Zombie& rhs);
	virtual ~CTurn_Lost_Zombie() = default;

public:
	virtual HRESULT					Initialize(void* pArg);

	virtual void					Enter() override;
	virtual _bool					Execute(_float fTimeDelta) override;
	virtual void					Exit() override;

private:
	void							Change_Animation(_float fTimeDelta);

public:
	void							SetBlackBoard(class CBlackBoard_Zombie* pBlackBoard) { m_pBlackBoard = pBlackBoard; }

protected:
	class CBlackBoard_Zombie*		m_pBlackBoard = { nullptr };

	const PLAYING_INDEX				m_eBasePlayingIndex = { PLAYING_INDEX::INDEX_0 };

	const wstring					m_strBoneLayerTag = { BONE_LAYER_DEFAULT_TAG };
	const wstring					m_strAnimLayerTag = { TEXT("Lost_Turn") };
	ZOMBIE_TURN_LOST_TYPE			m_eTurnAnimType = { ZOMBIE_TURN_LOST_TYPE::_END };
	_bool							m_isRight = { false };

public:
	static CTurn_Lost_Zombie* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END