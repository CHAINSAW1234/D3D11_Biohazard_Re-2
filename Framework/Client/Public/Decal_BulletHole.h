#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CDecal_BulletHole : public CGameObject
{
public:
	typedef struct TEMP : public CGameObject::GAMEOBJECT_DESC
	{
		_int iSubType;
		_int iType;
	}DECAL_DESC;
public:
	CDecal_BulletHole(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDecal_BulletHole(const CDecal_BulletHole& rhs);
	virtual ~CDecal_BulletHole() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;
	void					SetWorldMatrix(_float4x4 WorldMatrix);
	void					SetWorldMatrix_With_HitNormal(_vector vUp);
	void					SetPosition(_float4 vPos);
	void					LookAt(_float4 vDir);
protected:
	class CModel*			m_pModelCom = { nullptr };
	class CShader*			m_pShaderCom = { nullptr };
	class CTexture*			m_pTextureCom = { nullptr };
	class CVIBuffer_Rect*	m_pVIBufferCom = { nullptr };

protected:
	_float					m_fSizeX = { 0.f };
	_float					m_fSizeY = { 0.f };
	_float					m_fSizeZ = { 0.f };
	_float3					m_vExtent;
	_float4					m_vNormal;
	_int					m_iFrame = { 0 };
protected:
	virtual HRESULT			Add_Components();
	virtual HRESULT			Bind_ShaderResources();

public:
	static CDecal_BulletHole* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END