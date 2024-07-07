#pragma once

#include "Client_Defines.h"
#include "Task_Node.h"
#include "Body_Zombie.h"

#define ZOMBIE_DOOR_OPEN_TOTAL_INTERPOLATE_TO_WINDOW_TIME		0.5f

BEGIN(Client)

class COpen_Door_Zombie : public CTask_Node
{
private:
	COpen_Door_Zombie();
	COpen_Door_Zombie(const COpen_Door_Zombie& rhs);
	virtual ~COpen_Door_Zombie() = default;

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
	class CBlackBoard_Zombie*		m_pBlackBoard = { nullptr };

	PLAYING_INDEX					m_eBasePlayingIndex = { PLAYING_INDEX::INDEX_0 };
	const wstring					m_strAnimLayerTag = { TEXT("Gimmick_Door") };
	_int							m_iAnimIndex = { -1 };

	_float							m_fAccLinearInterpolateTime = { 0.f };
	_float4x4						m_InterpolateDeltaMatrix = {};

public:
	static COpen_Door_Zombie* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END