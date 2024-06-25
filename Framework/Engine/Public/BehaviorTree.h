#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CBehaviorTree final : public CBase
{
private:
	CBehaviorTree();
	CBehaviorTree(const CBehaviorTree& rhs);
	virtual ~CBehaviorTree() = default;

public:
	virtual HRESULT					Initialize_Prototype();
	virtual HRESULT					Initialize();

	class CComposite_Node*				GetRootNode()
	{
		return m_pRootNode;
	}
public:
	void							Initiate(_float fTimeDelta);
protected:
	class CGameInstance*			m_pGameInstance = { nullptr };
	class CComposite_Node*			m_pRootNode = { nullptr };
public:
	static CBehaviorTree* Create();

public:
	virtual void Free() override;
};

END