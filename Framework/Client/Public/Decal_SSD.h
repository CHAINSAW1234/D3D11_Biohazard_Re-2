#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CDecal_SSD : public CGameObject
{
public:
	typedef struct TEMP : public CGameObject::GAMEOBJECT_DESC
	{
		_int iSubType;
		_int iType;
	}DECAL_DESC;
public:
	CDecal_SSD(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDecal_SSD(const CDecal_SSD& rhs);
	virtual ~CDecal_SSD() = default;

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
	virtual void			PlaySound();
	void					SetUsingSound(_bool boolean)
	{
		m_bUsing_Sound = boolean;
	}
	_bool					IsPlayerNearBy();
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
	_bool					m_bBlood = { true };
	_bool					m_bUsing_Sound = { false };
protected:
	virtual HRESULT			Add_Components();
	virtual HRESULT			Bind_ShaderResources();

public:
	static CDecal_SSD* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END