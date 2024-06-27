#pragma once

#include "Client_Defines.h"
#include "Decal.h"

BEGIN(Engind)
class CModel;
class CMesh;
END

BEGIN(Client)

class CDecal_Blood : public CDecal
{
public:
	typedef struct Decal_Desc: public CGameObject::GAMEOBJECT_DESC
	{
		_int iSubType;
		_int iType;
	}DECAL_DESC;
protected:
	CDecal_Blood(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDecal_Blood(const CDecal_Blood& rhs);
	virtual ~CDecal_Blood() = default;

public:
	virtual void	Add_Skinned_Decal(struct AddDecalInfo Info) override;
	void			Set_Model(class CModel* pModel)
	{
		m_pModel = pModel;
	}
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
	class CZombie*			m_pZombie = { nullptr };
	class CModel*			m_pModel = { nullptr };
	class CMesh*			m_pMesh = { nullptr };
public:
	static CDecal_Blood* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END