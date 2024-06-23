#pragma once

#include "Tool_Defines.h"
#include "Tool.h"

BEGIN(Engine)
class CAnimation;
END

BEGIN(Tool)

class CTool_AnimLibrary : public CTool
{
public:
	typedef struct tagAnimLibraryDesc
	{
		string*									pCurrentAnimationTag = { nullptr };
		string*									pCurrentModelTag = { nullptr };
		wstring*								pCurrentAnimLayerTag = { nullptr };
	}ANIMLIBRARY_DESC;

private:
	CTool_AnimLibrary(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTool_AnimLibrary() = default;

public:
	virtual HRESULT								Initialize(void* pArg) override;
	virtual void								Tick(_float fTimeDelta) override;

	void										Set_CurrentModel(CModel* pModel);

private:	/* For.SHowList */
	void										Show_Default();
	void										Show_Func();

	void										Add_Animations_To_CurrentModel();

private:
	//	wstring => 애니메이션 프로토타입 묶음 태그
	//	string => 애니메이션 각각의 태그
	vector<wstring>								m_AnimPrototypeTags;
	CModel*										m_pCurrentModel = { nullptr };

	string*										m_pCurrentModelTag = { nullptr };
	wstring*									m_pCurrentAnimLayerTag = { nullptr };

public:
	static CTool_AnimLibrary* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual void Free();
};

END