#pragma once

#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Client)

class CHit_Props final : public CEffect
{
public:
	CHit_Props(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHit_Props(const CHit_Props& rhs);
	virtual ~CHit_Props() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	virtual void			Compute_CurrentUV();
	virtual void			Setup_Billboard() override;
public:
	void					SetSize(_float fSizeX, _float fSizeY);
	virtual void			SetPosition(_float4 Pos) override;

private:
	virtual HRESULT			Add_Components();
	virtual HRESULT			Bind_ShaderResources();

public:
	static CHit_Props* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END