#pragma once

#include "Client_Defines.h"
#include "Part_InteractProps.h"



BEGIN(Client)

class CPannel_Window final : public CPart_InteractProps
{
public:
	enum Pannel_Model
	{
		
	};
private:
	CPannel_Window(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPannel_Window(const CPannel_Window& rhs);
	virtual ~CPannel_Window() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void					Tick(_float fTimeDelta) override;
	virtual void					Late_Tick(_float fTimeDelta) override;
	virtual HRESULT					Render() override;
	virtual HRESULT					Render_LightDepth_Dir()override;
	virtual HRESULT					Render_LightDepth_Point() override;
	virtual HRESULT					Render_LightDepth_Spot()override;

private:
	virtual HRESULT				Add_Components();
	virtual HRESULT				Add_PartObjects() override;
	virtual HRESULT				Initialize_PartObjects() override;
public:

private:
	CShader* m_pShaderNonAnimCom = { nullptr };
	CModel* m_pModelNonAnimCom = { nullptr };
	

public:
	static CPannel_Window* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END