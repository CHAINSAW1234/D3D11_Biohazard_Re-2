#pragma once

#include "Client_Defines.h"
#include "InteractProps.h"

BEGIN(Client)

class CNewpoliceStatue final : public CInteractProps
{
public:
	enum POLICEHALLSTATUE
	{
		POLICEHALLSTATUE_0,
		POLICEHALLSTATUE_1,
		POLICEHALLSTATUE_2,
		POLICEHALLSTATUE_3,
		POLICEHALLSTATUE_END
	};
	enum POLICEHALLSTATUE_PART
	{
		PART_BODY,
		PART_PART,
		PART_END
	};
private:
	CNewpoliceStatue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNewpoliceStatue(const CNewpoliceStatue& rhs);
	virtual ~CNewpoliceStatue() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void					Tick(_float fTimeDelta) override;
	virtual void					Late_Tick(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

private:

	virtual HRESULT				Add_Components();
	virtual HRESULT				Add_PartObjects() override;
	virtual HRESULT				Initialize_PartObjects() override;
	virtual HRESULT				Bind_ShaderResources() override;
public:
	virtual _float4 Get_Object_Pos() override;
private:
	_ubyte  m_eState = { POLICEHALLSTATUE_0 };


public:
	static CNewpoliceStatue* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END