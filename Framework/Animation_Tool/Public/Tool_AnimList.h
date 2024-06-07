#pragma once

#include "Tool_Defines.h"
#include "Tool_Selector.h"

BEGIN(Engine)
class CAnimation;
END

BEGIN(Tool)

class CTool_AnimList : public CTool_Selector
{
private:
	CTool_AnimList(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTool_AnimList() = default;

public:
	virtual HRESULT						Initialize(void* pArg) override;
	virtual void						Tick(_float fTimeDelta) override;

public:
	CAnimation*							Get_Animation(const string& strAnimTag);
	CAnimation*							Get_CurrentAnimation();
	void								Add_Animation(CAnimation* pAnimation);

private:
	map<string, CAnimation*>			m_Animations;
	string								m_strCurrentAnimTag;

public:
	static CTool_AnimList* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual void Free();
};

END