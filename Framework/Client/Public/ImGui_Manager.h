#pragma once

#include "Client_Defines.h"
#include "Base.h"


BEGIN(Client)

class CImgui_Manager  final : public CBase
{
	DECLARE_SINGLETON(CImgui_Manager)

public:
	CImgui_Manager();
	virtual ~CImgui_Manager() = default;


public:
	HRESULT				Initialize();
	void				Set_GraphicDevice(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void				ResetVariable();
	void				Tick();
	void				Render();

private:
	void				Window_TabWindow_Debuger();
	void				Window_Player_Debuger();

private:
	wstring				Classify_ItemNum_To_String(ITEM_NUMBER eItemNum);
	ITEM_NUMBER			Classify_String_To_ItemNum(wstring wstrItemNum);

private:
	string				WStringToString(const std::wstring& wstr);

private:
	ImGuiIO								m_ImGuiIO;
	ID3D11Device*						m_pDevice = { nullptr };
	ID3D11DeviceContext*				m_pContext = { nullptr };
	CGameInstance*						m_pGameInstance = { nullptr };

private:
	class CTab_Window*					m_pTabWindow = { nullptr };
	class CPlayer*						m_pPlayer = { nullptr };

private:
	_bool								m_bTabWindow_Debuger = { false };
	_bool								m_bPlayer_Debuger = { false };

private:/*for. Tab_Window*/
	_int								m_icurrent_Item = { 0 };
	wstring								m_wstrCurrent_Item = { TEXT("") };
	vector<wstring>						m_vecItemNums;

	_int								m_iItemCount = { 1 };

private:
	_int								m_iPlayerHP = { 5 };


private:
	HRESULT Refer_Player();
	HRESULT Refer_Tabwindow();


public: 
	virtual void Free() override;
};

END