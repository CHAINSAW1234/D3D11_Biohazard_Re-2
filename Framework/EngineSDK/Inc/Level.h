#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CLevel abstract : public CBase
{
protected:
	CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel() = default;

public:
	virtual HRESULT						Initialize();
	virtual void						Tick(_float fTimeDelta);
	virtual HRESULT						Render();
	virtual HRESULT Load_Layer(const wstring& strFilePath, _uint iLevel);
	virtual HRESULT Load_Object(const wstring& strFilePath, const wstring& strLayerName, _uint iLevel);
	virtual HRESULT Load_Light(const wstring& strFilePath, _uint iLevel);

protected:
	ID3D11Device*						m_pDevice = { nullptr };
	ID3D11DeviceContext*				m_pContext = { nullptr };
	class CGameInstance*				m_pGameInstance = { nullptr };

public:
	virtual void Free() override;
};

END