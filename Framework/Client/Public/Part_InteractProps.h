#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CModel;
class CShader;
END

BEGIN(Client)

class CPart_InteractProps abstract : public CPartObject
{
public:
	typedef struct tagPart_InteractProps_Desc : public CPartObject::PARTOBJECT_DESC
	{
		const _ubyte*	pState;
		_float3*				pRootTranslation = { nullptr };
		wstring				strModelComponentName = { TEXT("") };

	}PART_INTERACTPROPS_DESC;
protected:
	CPart_InteractProps(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPart_InteractProps(const CPart_InteractProps& rhs);
	virtual ~CPart_InteractProps() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void				Tick(_float fTimeDelta) override;
	virtual void				Late_Tick(_float fTimeDelta) override;
	virtual HRESULT				Render() override;


	virtual HRESULT					Render_LightDepth_Dir()override;
	virtual HRESULT					Render_LightDepth_Point() override;
	virtual HRESULT					Render_LightDepth_Spot()override;


protected:
	_bool						m_bShadow = { true };
	_float						m_fTimeTest = { 0.f };
	CModel*						m_pModelCom = { nullptr };
	CShader*					m_pShaderCom = { nullptr };
	class CPlayer*				m_pPlayer = { nullptr };
	_int						m_iPropsType = { 0 };
	wstring						m_strModelComponentName = { TEXT("") };

protected:
	HRESULT						Add_Components();
	virtual HRESULT				Add_PartObjects();
	virtual HRESULT				Initialize_PartObjects();

protected:
	HRESULT					Bind_ShaderResources_Anim();
	HRESULT					Bind_ShaderResources_NonAnim();

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END