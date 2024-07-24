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

class CFlashLight final : public CPartObject
{

private:
	CFlashLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFlashLight(const CFlashLight& rhs);
	virtual ~CFlashLight() = default;

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
	inline _float4x4*				Get_Socket_Ptr() { return m_pSocketMatrix; }
	inline void						Set_Socket_Ptr(_float4x4* pSocketMatrix) { m_pSocketMatrix = pSocketMatrix; }
	inline void						Set_Origin_Translation(_bool isOrigin) { m_isOrigin = isOrigin; }
	inline void						Set_Right_Handed(_bool isRight) { m_isRightHand = isRight; }
	
private:
	CModel*							m_pModelCom = { nullptr };
	CShader*						m_pShaderCom = { nullptr };
	_float4x4*						m_pSocketMatrix = { nullptr };
	wstring							m_strLightTag = { TEXT("Light_Flash") };

	_bool							m_isOrigin = { false };
	_bool							m_isRightHand = { false };

private:
	HRESULT							Add_Components();
	HRESULT							Bind_ShaderResources();

public:
	static CFlashLight* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END