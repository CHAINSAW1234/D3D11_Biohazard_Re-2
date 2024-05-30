#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
END

BEGIN(Client)

class CProps final : public CGameObject
{
public:
	typedef struct Props_Desc : public CGameObject::GAMEOBJECT_DESC
	{
		_int iPropsType;
		_float4 Position;
	}PROPS_DESC;
private:
	CProps(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CProps(const CProps& rhs);
	virtual ~CProps() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void				Tick(_float fTimeDelta) override;
	virtual void				Late_Tick(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

	virtual HRESULT				Render_LightDepth_Dir() override;
	virtual	HRESULT				Render_LightDepth_Spot() override;
	virtual HRESULT				Render_LightDepth_Point() override;

private:
	CModel*						m_pModelCom = { nullptr };
	CShader*					m_pShaderCom = { nullptr };

	_int						m_iPropsType = { 0 };
	_bool						m_bVisible = { false };
private:
	HRESULT						Add_Components();
	HRESULT						Bind_ShaderResources();

public:
	static CProps* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END