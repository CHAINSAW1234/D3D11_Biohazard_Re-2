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
	HRESULT Ready_Layer_Camera(const wstring& strLayerTag);
	HRESULT Ready_Layer_BackGround(const wstring& strLayerTag);
	
	HRESULT Ready_LandObject();
	HRESULT Ready_Layer_Player(const wstring& strLayerTag/*, CLandObject::LANDOBJECT_DESC& LandObjectDesc*/);
	HRESULT Ready_Layer_Monster(const wstring& strLayerTag);
	HRESULT Ready_Layer_LandBackGround(const wstring& strLayerTag);
	HRESULT Ready_RegionCollider();

	HRESULT Ready_Layer_Effect(const wstring& strLayerTag);
private:
	HRESULT Load_Collider(const wstring& strFile, const wstring& strColLayerTag);

public:
	static CLevel_GamePlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END