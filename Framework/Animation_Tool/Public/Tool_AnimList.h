#pragma once

#include "Tool_Defines.h"
#include "Tool.h"

BEGIN(Engine)
class CAnimation;
END

BEGIN(Tool)

class CTool_AnimList : public CTool
{
private:
	CTool_AnimList();
	virtual ~CTool_AnimList() = default;

public:
	virtual HRESULT						Initialize(void* pArg) override;
	virtual void						Tick(_float fTimeDelta) override;

public:
	CAnimation*							Get_Animation(const string& strAnimTag);

private:
	map<string, CAnimation*>			m_Animations;

public:
	static CTool_AnimList* Create(void* pArg);
	virtual void Free();
};

END