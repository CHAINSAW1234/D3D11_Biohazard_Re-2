#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CBone;
class CModel;
class CShader;
class CCollider;
END

BEGIN(Client)

class CKnife final : public CPartObject
{

private:
	CKnife(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CKnife(const CKnife& rhs);
	virtual ~CKnife() = default;

public:
	virtual HRESULT					Initialize_Prototype() override;
	virtual HRESULT					Initialize(void* pArg) override;
	virtual void					Tick(_float fTimeDelta) override;
	virtual void					Late_Tick(_float fTimeDelta) override;
	virtual HRESULT					Render() override;

	virtual HRESULT					Render_LightDepth_Dir()override;
	virtual HRESULT					Render_LightDepth_Point() override;
	virtual HRESULT					Render_LightDepth_Spot()override;

public:
	inline _float4x4* Get_Socket_Ptr() { return m_pSocketMatrix; }
	inline void						Set_Socket_Ptr(_float4x4* pSocketMatrix) { m_pSocketMatrix = pSocketMatrix; }
	virtual void Set_Render(_bool isRender);
	void Set_Active(_bool isActive);
private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	_float4x4* m_pSocketMatrix = { nullptr };
	_bool m_isActive = { false };
private:
	HRESULT							Add_Components();
	HRESULT							Bind_ShaderResources();

public:
	static CKnife* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END