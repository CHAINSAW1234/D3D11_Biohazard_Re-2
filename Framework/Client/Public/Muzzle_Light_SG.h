#pragma once

#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Client)

class CMuzzle_Light_SG final : public CEffect
{
public:
	CMuzzle_Light_SG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMuzzle_Light_SG(const CMuzzle_Light_SG& rhs);
	virtual ~CMuzzle_Light_SG() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	virtual void			Setup_Billboard() override;
public:
	void					SetSize(_float fSizeX, _float fSizeY);
	virtual void			SetPosition(_float4 Pos) override;

private:
	virtual HRESULT			Add_Components();
	virtual HRESULT			Bind_ShaderResources();

public:
	static CMuzzle_Light_SG* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END