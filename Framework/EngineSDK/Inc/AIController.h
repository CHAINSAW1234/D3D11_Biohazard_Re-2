#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CAIController : public CBase
{
protected:
	CAIController();
	CAIController(const CAIController& rhs);
	virtual ~CAIController() = default;

public:
	virtual HRESULT					Initialize_Prototype();
	virtual HRESULT					Initialize(void* pArg);

public:
	class CBehaviorTree*			Create_BehaviorTree(_uint* iId);
	class CBehaviorTree*			Get_BehaviorTree(_uint* iId);
	class CPathFinder*				Create_PathFinder();
protected:
	class CGameInstance*			m_pGameInstance = { nullptr };
	vector<class CBehaviorTree*>	m_vecBehaviorTree;
	vector<class CPathFinder*>		m_vecPathFinder;
	
protected:
	_uint m_iBehaviorTree_Count = { 0 };
	_uint m_iPathFinder_Count = { 0 };
public:
	static CAIController* Create();

public:
	virtual void Free() override;
};

END