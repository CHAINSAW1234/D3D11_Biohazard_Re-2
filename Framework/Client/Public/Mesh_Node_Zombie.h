#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CMesh_Node_Zombie final : public CBase
{
private:
	CMesh_Node_Zombie();
	CMesh_Node_Zombie(const CMesh_Node_Zombie& rhs);
	virtual ~CMesh_Node_Zombie() = default;

public:
	virtual HRESULT				Initialize(void* pArg);

private:
	_uint						iMeshIndex = {};

public:
	static CMesh_Node_Zombie* Create(void* pArg);
	virtual void Free() override;};

};

END