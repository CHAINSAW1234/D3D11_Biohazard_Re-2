#pragma once

#include "Client_Defines.h"
#include "Task_Node.h"
#include "Zombie.h"
#include "Body_Zombie.h"

BEGIN(Client)

class CBite_Lost_Zombie : public CTask_Node
{
public:
	enum class ZOMBIE_BITE_LOST_ANIM_TYPE { _DIRECT_1, _DIRECT1_EM, _DIRECT_2, _END };

private:
	CBite_Lost_Zombie();
	CBite_Lost_Zombie(const CBite_Lost_Zombie& rhs);
	virtual ~CBite_Lost_Zombie() = default;

public:
	virtual HRESULT					Initialize(void* pArg);

	virtual void					Enter() override;
	virtual _bool					Execute(_float fTimeDelta) override;
	virtual void					Exit() override;

private:
	void							Change_Animation();

public:
	void							SetBlackBoard(class CBlackBoard_Zombie* pBlackBoard) { m_pBlackBoard = pBlackBoard; }
private:
	class CBlackBoard_Zombie*		m_pBlackBoard = { nullptr };

	const wstring					m_strLostBiteAnimLayerTag = { TEXT("Lost_Bite") };
	const wstring					m_strBiteETCAnimLayerTag = { TEXT("Bite_ETC") };
	const PLAYING_INDEX				m_ePlayingIndex = { PLAYING_INDEX::INDEX_0 };

	_bool							m_isSendMSG_To_Player = { false };

	_float							m_fAccLinearTime_HalfMatrix = { 0.f };
	_float							m_fTotalLinearTime_HalfMatrix = { 0.f };

	_float4x4						m_Delta_Matrix_To_HalfMatrix;

	ZOMBIE_BITE_LOST_ANIM_TYPE		m_eBite_Lost_Anim_Type = { ZOMBIE_BITE_LOST_ANIM_TYPE::_END };

public:
	static CBite_Lost_Zombie* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END