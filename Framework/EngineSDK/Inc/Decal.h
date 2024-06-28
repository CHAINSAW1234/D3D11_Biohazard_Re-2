#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CDecal : public CGameObject
{
public:
	typedef struct Decal_Desc : public CGameObject::GAMEOBJECT_DESC
	{
		_int iSubType;
		_int iType;
	}DECAL_DESC;
protected:
	CDecal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDecal(const CDecal& rhs);
	virtual ~CDecal() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void	Add_Skinned_Decal(AddDecalInfo Info) {}
protected:
	class CModel* m_pModelCom = { nullptr };
	class CShader* m_pShaderCom = { nullptr };
	class CTexture* m_pTextureCom = { nullptr };
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	struct DecalConstData decalConstData;
protected:
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY,m_fSizeZ;

protected:
	virtual HRESULT			Add_Components();
	virtual HRESULT			Bind_ShaderResources();

public:
	static CDecal* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END