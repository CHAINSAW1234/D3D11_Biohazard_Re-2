#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CDecal : public CGameObject
{
public:
	typedef struct TEMP : public CGameObject::GAMEOBJECT_DESC
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
	void			SetWorldMatrix(_float4x4 WorldMatrix);
public:
	virtual void	Add_Skinned_Decal(AddDecalInfo Info, RAYCASTING_INPUT Input) {}
	virtual void	Bind_Resource_DecalInfo() {}
	virtual _uint	Staging_DecalInfo_RayCasting(_float* pDist) { return 0; }
	virtual void	Calc_Decal_Info(CALC_DECAL_INPUT Input) {}
	virtual void	Staging_Calc_Decal_Info() {}
	virtual void	Bind_Resource_DecalMap(CALC_DECAL_MAP_INPUT Input) {}
	virtual void	Bind_DecalMap(class CShader* pShader) {}
public:
	ID3D11UnorderedAccessView* GetDecalInfo_Uav()
	{
		return m_pUAV_DecalInfo;
	}
	void						SetNumVertices(_uint iNumVertices)
	{
		m_iNumVertices = iNumVertices;
	}
	virtual HRESULT				Init_Decal_Texture(_uint iLevel) { return S_OK; }
protected:
	class CModel* m_pModelCom = { nullptr };
	class CShader* m_pShaderCom = { nullptr };
	class CTexture* m_pTextureCom = { nullptr };
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	DecalConstData m_DecalConstData;
	DecalInfo* m_DecalInfo = { nullptr };
	ID3D11Buffer* m_pSB_DecalInfo = { nullptr };
	ID3D11Buffer* m_pCB_DecalConstData = { nullptr };

	ID3D11Buffer*				m_pSB_DecalMap = nullptr;
	ID3D11ShaderResourceView*	m_pSRV_DecalMap = nullptr;
	ID3D11UnorderedAccessView*  m_pUAV_DecalInfo = { nullptr };
	ID3D11UnorderedAccessView*  m_pUAV_DecalMap = { nullptr };
	ID3D11RenderTargetView*		m_pRTV_DecalMap = { nullptr };
	ID3D11Buffer*				m_pStaging_Buffer_Decal_Info = { nullptr };
	ID3D11Buffer*				m_pStaging_Buffer_Decal_Map = { nullptr };

	_float3						m_vExtent;
	_uint						m_iNumVertices = { 0 };

	_float2*					m_pDecal_Map = { nullptr };
protected:
	_float					m_fSizeX;
	_float					m_fSizeY;
	_float					m_fSizeZ;

protected:
	virtual HRESULT			Add_Components();
	virtual HRESULT			Bind_ShaderResources();

public:
	static CDecal* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END