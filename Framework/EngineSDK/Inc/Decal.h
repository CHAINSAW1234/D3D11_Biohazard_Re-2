#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CDecal : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
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
	virtual void	Add_Skinned_Decal(AddDecalInfo Info, RAYCASTING_INPUT Input) {}
	virtual void	Bind_Resource_DecalInfo() {}
	virtual _uint	Staging_DecalInfo_RayCasting() { return 0; }
	virtual void	Calc_Decal_Info(CALC_DECAL_INPUT Input) {}
	virtual void	Staging_Calc_Decal_Info() {}

public:
	ID3D11UnorderedAccessView* GetDecalInfo_Uav()
	{
		return m_pUAV_DecalInfo;
	}
protected:
	class CModel* m_pModelCom = { nullptr };
	class CShader* m_pShaderCom = { nullptr };
	class CTexture* m_pTextureCom = { nullptr };
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	class DecalConstData m_DecalConstData;
	class DecalInfo* m_DecalInfo = { nullptr };
	ID3D11Buffer* m_pSB_DecalInfo = { nullptr };
	ID3D11Buffer* m_pCB_DecalConstData = { nullptr };
	ID3D11UnorderedAccessView* m_pUAV_DecalInfo = { nullptr };
	ID3D11Buffer* m_pStaging_Buffer_Decal_Info = { nullptr };
protected:
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY, m_fSizeZ;

protected:
	virtual HRESULT			Add_Components();
	virtual HRESULT			Bind_ShaderResources();

public:
	static CDecal* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END