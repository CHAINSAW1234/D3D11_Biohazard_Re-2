#pragma once

#include "Tool_Defines.h"
#include "Level.h"


/* 1. ������ �ʿ��� ��ü���� �����Ѵ�. */
/* 2. ������ �ݺ������� �����Ͽ� ȭ�鿡 �����ش�. */

BEGIN(Tool)

class CLevel_Tool final : public CLevel
{
private:
	CLevel_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Tool() = default;

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
	HRESULT Ready_Layer_Effect(const wstring& strLayerTag);


public:
	static CLevel_Tool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END