#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Logo final : public CLevel
{
private:
	CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Logo() = default;

public:
	virtual	HRESULT				Initialize() override;
	virtual void				Tick(_float fTimeDelta) override;

	virtual HRESULT				Render() override;

private:
	HRESULT						Ready_Layer_BackGround(const wstring& strLayerTag);

private : /* UI */
	HRESULT						Ready_Layer_UI(const wstring& strLayerTag);
	void						UI_Distinction(wstring& selectedFilePath);
	void						CreatFromDat(ifstream& inputFileStream, wstring strListName, CGameObject* pGameParentsObj, wstring fileName, _int iWhich_Child = 0);

public:
	static CLevel_Logo*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END