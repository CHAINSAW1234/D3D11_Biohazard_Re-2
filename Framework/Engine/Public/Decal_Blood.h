#pragma once
#include "Decal.h"


BEGIN(Engine)

class ENGINE_DLL CDecal_Blood : public CDecal
{
public:
	typedef struct
	{
		_uint iNumIndices;
		_uint iMeshIndex;
	}DECAL_BLOOD_DESC;
protected:
	CDecal_Blood(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDecal_Blood(const CDecal_Blood& rhs);
	virtual ~CDecal_Blood() = default;

public:
	virtual void	Add_Skinned_Decal(AddDecalInfo Info,RAYCASTING_INPUT Input) override;
	void			Set_Model(class CModel* pModel)
	{
		m_pModel = pModel;
	}
	virtual void	Bind_Resource_DecalInfo() override;
	virtual _uint	Staging_DecalInfo_RayCasting() override;
	virtual void	Staging_Calc_Decal_Info() override;
	virtual void	Calc_Decal_Info(CALC_DECAL_INPUT Input) override;
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	virtual HRESULT			Add_Components();
	virtual HRESULT			Bind_ShaderResources();

private:
	class CModel*			m_pModel = { nullptr };
	class CMesh*			m_pMesh = { nullptr };
public:
	static CDecal_Blood* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END