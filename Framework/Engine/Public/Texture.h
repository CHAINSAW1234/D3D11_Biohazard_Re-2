#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTexture final : public CComponent
{
public:
	typedef struct Texture_Desc
	{
		_uint iWidth;
		_uint iHeight;
		_uint iCountX;
		_uint iCountY;
	}TEXTURE_DESC;
private:
	CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTexture(const CTexture& rhs);
	virtual ~CTexture() = default;

public:
	virtual HRESULT							Initialize_Prototype(const wstring& strTextureFilePath, _uint iNumTexture, TEXTURE_DESC* Desc = nullptr);
	virtual HRESULT							Initialize(void* pArg) override;

public:
	IMG_SIZE								GetImgSize()
	{
		return m_TextureSize;
	}
	pair<_uint, _uint>						GetDivideCount()
	{
		return m_DivideCount;
	}
public:
	HRESULT									Bind_ShaderResource(class CShader* pShader, const _char* pConstantName, _uint iTextureIndex = 0);
	HRESULT									Bind_ShaderResources(class CShader* pShader, const _char* pConstantName);

private:
	vector<ID3D11ShaderResourceView*>		m_Textures;
	_uint									m_iNumTextures = { 0 };
	IMG_SIZE								m_TextureSize;
	pair<_uint, _uint>						m_DivideCount;
public:
	static CTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strTextureFilePath, _uint iNumTexture = 1, TEXTURE_DESC* pDesc = nullptr);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END