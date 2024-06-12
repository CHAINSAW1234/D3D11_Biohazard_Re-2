#pragma once

#include "Tool_Defines.h"
#include "PartObject.h"

BEGIN(Tool)

class CAnimTestPartObject final : public CPartObject
{
public:
	typedef struct tagAnimTestPartObjectDesc : public PARTOBJECT_DESC
	{
		_float3*			pRootTranslation = { nullptr };
	}ANIMTESTPART_DESC;

public:
	enum COMPONENT_TYPE { COM_MODEL, COM_END };

private:
	CAnimTestPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAnimTestPartObject(const CAnimTestPartObject& rhs);
	virtual ~CAnimTestPartObject() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Priority_Tick(_float fTimeDelta) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	HRESULT					Render_LightDepth_Dir()override;
	HRESULT					Render_LightDepth_Spot()override;
	HRESULT					Render_LightDepth_Point() override;

public:
	HRESULT					Chanage_Componenet(CComponent* pComponent, COMPONENT_TYPE eType);
	CModel*					Get_CurrentModelComponent() { return m_pModelCom; }
	void					Set_RootBone(string strRootBoneTag);

private:
	_float3*				m_pRootTranslation = {};
	CModel*					m_pModelCom = { nullptr };
	CShader*				m_pShaderCom = { nullptr };

private:
	HRESULT					Add_Components();
	HRESULT					Bind_ShaderResources();

public:
	static CAnimTestPartObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END