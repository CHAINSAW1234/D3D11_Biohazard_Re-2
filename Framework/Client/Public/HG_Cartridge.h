#pragma once

#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Client)

class CHG_Cartridge final : public CEffect
{
public:
	CHG_Cartridge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHG_Cartridge(const CHG_Cartridge& rhs);
	virtual ~CHG_Cartridge() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	void					SetSize(_float fSizeX, _float fSizeY,_float fSizeZ);
	void					SetWorldMatrix_With_HitNormal(_vector vUp);
	_float4x4				GetWorldMatrix();
	_float4					GetPosition();
	virtual void			Start() override;
	void					Initiate(_float4 vPos,_float4 vDir,_float4 vLook);
private:
	virtual HRESULT			Add_Components();
	virtual HRESULT			Bind_ShaderResources();

private:
	CModel* m_pModelCom = { nullptr };

	_float4 m_vDir;

public:
	static CHG_Cartridge* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END