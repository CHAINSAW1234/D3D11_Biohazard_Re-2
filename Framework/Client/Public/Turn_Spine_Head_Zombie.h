#pragma once

#include "Client_Defines.h"
#include "Task_Node.h"
#include "Body_Zombie.h"
#include "Zombie.h"

BEGIN(Client)

class CTurn_Spine_Head_Zombie : public CTask_Node
{
private:
	CTurn_Spine_Head_Zombie();
	CTurn_Spine_Head_Zombie(const CTurn_Spine_Head_Zombie& rhs);
	virtual ~CTurn_Spine_Head_Zombie() = default;

public:
	virtual HRESULT					Initialize(void* pArg);

	virtual void					Enter() override;
	virtual _bool					Execute(_float fTimeDelta) override;
	virtual void					Exit() override;

private:
	void							Set_Spine_To_Head_AdditionalMatrices(_float fTimeDelta);

public:
	void							SetBlackBoard(class CBlackBoard_Zombie* pBlackBoard) { m_pBlackBoard = pBlackBoard; }

private:
	class CBlackBoard_Zombie*		m_pBlackBoard = { nullptr };

	const string					m_strSpineBoneTag = { "spine_0" };
	const string					m_strHeadBoneTag = { "head" };

	_float3							m_vPreLookDirection = {};

	list<_uint>						m_Child_JointIndices;

	_bool							m_isStart = { false };

	_float							m_fMaxAngle = { 0.f };
	_float							m_fPreAngle = { 0.f };
	_float							m_fRotatePerSec = { 0.f };

public:
	static CTurn_Spine_Head_Zombie* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END