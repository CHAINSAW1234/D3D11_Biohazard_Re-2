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
	HRESULT				Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void				ResetVariable();
	void				Tick();
	void				Render();

private:
	void				Window_TabWindow_Debuger();

private:
	ImGuiIO								m_ImGuiIO;
	ID3D11Device*						m_pDevice = { nullptr };
	ID3D11DeviceContext*				m_pContext = { nullptr };
	CGameInstance*						m_pGameInstance = { nullptr };

private:
	class CTab_Window*					m_pTabWindow = { nullptr };

private:
	_bool								m_bTabWindow_Debuger = { false };

private: 
	virtual void Free() override;
};

END