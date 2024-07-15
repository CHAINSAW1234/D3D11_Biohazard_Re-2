#pragma once

#include "Client_Defines.h"
#include "Task_Node.h"
#include "Body_Zombie.h"

BEGIN(Client)

class CPivot_Turn_Zombie : public CTask_Node
{
public:
	CPivot_Turn_Zombie();
	CPivot_Turn_Zombie(const CPivot_Turn_Zombie& rhs);
	virtual ~CPivot_Turn_Zombie() = default;

public:
	virtual HRESULT					Initialize(void* pArg);

	virtual void					Enter() override;
	virtual _bool					Execute(_float fTimeDelta) override;
	virtual void					Exit() override;

private:
	void							Change_Animation(_float fTimeDelta);

	_bool							Is_StateFinished();

public:
	void							SetBlackBoard(class CBlackBoard_Zombie* pBlackBoard)
	{
		m_pBlackBoard = pBlackBoard;
	}

protected:
	class CBlackBoard_Zombie*								m_pBlackBoard = { nullptr };

	_int													m_iBasePlayingIndex = { -1 };

	unordered_map<wstring, unordered_set<_uint>>			m_StartAnimIndicesLayer;
	unordered_map<wstring, unordered_set<_uint>>			m_LoopAnimIndicesLayer;

	const PLAYING_INDEX				m_eBasePlayingIndex = { PLAYING_INDEX::INDEX_0 };
	const PLAYING_INDEX				m_eBlendPlayingIndex = { PLAYING_INDEX::INDEX_2 };

	const wstring					m_strBoneLayerTag = { BONE_LAYER_DEFAULT_TAG };
	const wstring					m_strAnimLayerTag = { TEXT("Ordinary_PivotTurn") };

public:
	static CPivot_Turn_Zombie* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END