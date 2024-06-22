#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CLoader final : public CBase
{
private:
	CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLoader() = default;

public:
	HRESULT Initialize(LEVEL eNextLevelID);
	HRESULT Start();
	_bool isFinished() const {
		return m_isFinished;
	}
	void Output() {
		SetWindowText(g_hWnd, m_strLoadingText.c_str());
	}

private:
	HRESULT Load_Prototype();
	HRESULT Loading_For_Static_Component();

	HRESULT Load_Field_Prototype(const wstring& filePath);

private:
	HRESULT Ready_Layer_UI(const wstring& strLayerTag);
	void UI_Distinction(wstring& selectedFilePath);
	void CreatFromDat(ifstream& inputFileStream, wstring strListName, CGameObject* pGameParentsObj, wstring fileName, _int iWhich_Child = 0);

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	HANDLE						m_hThread;
	CRITICAL_SECTION			m_Critical_Section;
	LEVEL						m_eNextLevelID = { LEVEL_END };
	wstring						m_strLoadingText;
	_bool						m_isFinished = { false };

	CGameInstance*				m_pGameInstance = { nullptr };

private:
	HRESULT Loading_For_Logo();
	HRESULT Loading_For_GamePlay();

private:
	HRESULT	Load_Animations();


public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID);
	virtual void Free() override;
};

END