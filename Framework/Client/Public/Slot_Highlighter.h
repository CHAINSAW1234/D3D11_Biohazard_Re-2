#pragma once
#include "Customize_UI.h"


BEGIN(Client)

class CSlot_Highlighter final : public CCustomize_UI
{
private:
	enum SH_ROLE { CURSOR_SH, GLITTER_SH, SH_NONE };

protected:
	CSlot_Highlighter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSlot_Highlighter(const CSlot_Highlighter& rhs);
	virtual ~CSlot_Highlighter() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	void FirstTick_Seting();
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void Set_Dead(_bool bDead) override;

	void ResetPosition(_float4 fResetPos);

	void Set_DragShadow(_bool IsDragShadow);

private:
	void Set_SH_Role(SH_ROLE eSHRoloe) {
		m_eSHRole = eSHRoloe;
	}
	SH_ROLE Get_SH_Role() {
		return m_eSHRole;
	}


private:
	_vector			m_vOriginDiff = {};
	CTransform*		m_pCursorTranform = { nullptr };

	_bool			m_isDragShadow = { false };

	SH_ROLE			m_eSHRole = { SH_NONE };
	
public:
	static CSlot_Highlighter* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END