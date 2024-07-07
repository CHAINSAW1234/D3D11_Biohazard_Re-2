#pragma once

#include "Client_Defines.h"
#include "Task_Node.h"
#include "Body_Zombie.h"

#define ZOMBIE_ATTACK_WINDOW_NEED_TIME						2.f
#define ZOMBIE_KNOCK_TOTAL_INTERPOLATE_TO_WINDOW_TIME		0.5f

BEGIN(Client)

class CKnock_Window_Zombie : public CTask_Node
{
private:
	CKnock_Window_Zombie();
	CKnock_Window_Zombie(const CKnock_Window_Zombie& rhs);
	virtual ~CKnock_Window_Zombie() = default;

public:
	virtual HRESULT					Initialize(void* pArg);

	virtual void					Enter() override;
	virtual _bool					Execute(_float fTimeDelta) override;
	virtual void					Exit() override;

private:
	void							Change_Animation(_float fTimeDelta);
	void							Set_TargetInterpolate_Matrix();

public:
	inline void						SetBlackBoard(class CBlackBoard_Zombie* pBlackBoard) { m_pBlackBoard = pBlackBoard; }

protected:
	class CBlackBoard_Zombie*		m_pBlackBoard = { nullptr };

	PLAYING_INDEX					m_eBasePlayingIndex = { PLAYING_INDEX::INDEX_0 };
	const wstring					m_strAnimLayerTag = { TEXT("Gimmick_Window") };

	_float							m_fAccKnockTime = { 0.f };
	DIRECTION						m_eStartDirectionToWindow = { DIRECTION::_END };

	_float							m_fAccLinearInterpolateTime = { 0.f };
	_float4x4						m_DeltaInterpolateMatrix = {};

public:
	static CKnock_Window_Zombie* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END