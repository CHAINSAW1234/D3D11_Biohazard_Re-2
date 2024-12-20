#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_GamePlay final : public CLevel
{
private:
	CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_GamePlay() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Lights();
	HRESULT Ready_EnvSounds();
	HRESULT Ready_Layer_Camera(const wstring& strLayerTag);
	HRESULT Ready_Layer_BackGround(const wstring& strLayerTag);
	
	HRESULT Ready_Managers();
	HRESULT Ready_TabWindow();
	HRESULT Ready_LandObject();
	HRESULT Ready_PropManager();
	HRESULT Ready_Layer_Player(const wstring& strLayerTag/*, CLandObject::LANDOBJECT_DESC& LandObjectDesc*/);
	HRESULT Ready_Layer_Monster(const wstring& strLayerTag);
	HRESULT Ready_Layer_LandBackGround(const wstring& strLayerTag);
	HRESULT Ready_RegionCollider();
	HRESULT Ready_CutScene();
	HRESULT Ready_EnvCube();

	HRESULT Ready_Layer_Effect(const wstring& strLayerTag);
	HRESULT Ready_Layer_UI(const wstring& strLayerTag);

	HRESULT Ready_Decal(const wstring& strLayerTag);
	
private:
	void UI_Distinction(wstring& selectedFilePath);
	void CreatFromDat(ifstream& inputFileStream, wstring strListName, CGameObject* pGameParentsObj, wstring fileName, _int iWhich_Child = 0, CGameObject* pSupervisor = nullptr );
	
	wstring StringToWstring(const std::string& strString)
	{
		std::setlocale(LC_ALL, "");
		size_t requiredSize = 0;
		mbstowcs_s(&requiredSize, nullptr, 0, strString.c_str(), strString.size());
		std::vector<wchar_t> buffer(requiredSize);
		mbstowcs_s(&requiredSize, buffer.data(), buffer.size(), strString.c_str(), strString.size());
		return std::wstring(buffer.data());
	}

private:
	HRESULT Load_Collider(const wstring& strFile, const wstring& strColLayerTag, _float4 vColliderColor = { 1.f, 1.f, 1.f, 1.f });
	HRESULT Load_Layer(const wstring& strFilePath, _uint iLevel);
	HRESULT Load_Object(const wstring& strFilePath, const wstring& strLayerName, _uint iLevel);
	HRESULT Load_Monster(const wstring& strFilePath, const wstring& strLayerName, _uint iLevel);

private:
	HRESULT SetUp_DeadMonsters();


#pragma region 나옹
private:
	_bool m_isMapType = { false };
#pragma endregion

#pragma region 예은 추가 (임시임)
private:
	//class CProp_Manager* m_pPropManager = { nullptr };
	_int iCurIndex = { 0 };
	_int iPreIndex = { 0 };
#pragma endregion

	class CCall_Center*				m_pCall_Center = { nullptr };
	class CCut_Scene_Manager*		m_pCutSceneManager = { nullptr };
	class CBGM_Player*				m_pBGM_Player = { nullptr };

	//CTexture* m_pTexture = { nullptr };
	class CEnvCube* m_pCubeMap = { nullptr };
public:
	static CLevel_GamePlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END