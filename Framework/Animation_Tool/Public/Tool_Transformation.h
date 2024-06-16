#pragma once

#include "Tool_Defines.h"
#include "Tool.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Tool)

class CTool_Transformation final : public CTool
{
public:
	enum MODE { MODE_PERSPECTIVE, MODE_ORTHO, MODE_END };

protected:
	CTool_Transformation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTool_Transformation() = default;

public:
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void				Tick(_float fTimeDelta) override;

public:
	void						Set_Target(CTransform* pTransform);
	void						Set_Target(CGameObject* pGameObject);
	CTransform*					GEt_Transform() { return m_pTransformCom; }
	void						ReSet_Target();

public:
	CTransform* Get_Target() { return m_pTargetTransform; }

private:
	void						Update_Transform();
	void						Update_Target_Transform();

private:
	void						Set_Origin();

public:
	void						Set_Mode(MODE eMode) { m_eMode = eMode; }

private:
	HRESULT						Add_Componets();

private:
	MODE						m_eMode = { MODE_END };
	CTransform* m_pTransformCom = { nullptr };
	CTransform* m_pTargetTransform = { nullptr };

public:
	static CTool_Transformation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual void Free() override;
};

END