#pragma once
#include "Customize_UI.h"


BEGIN(Client)

class CInventory_Slot final : public CCustomize_UI
{
protected:
	CInventory_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInventory_Slot(const CInventory_Slot& rhs);
	virtual ~CInventory_Slot() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Change_Tool() override;
public:
	_bool Get_IsFilled() const {
		return m_isFilled;
	}

	void Set_IsFilled(_bool isFilled) {
		m_isFilled = isFilled;
	}



private:
	CTexture* m_pEmptyTexture = { nullptr };
	CTexture* m_pFilledTexture = { nullptr };

	_bool m_isFilled = { false };
	_bool m_PriorisFilled = { false };

public:
	static CInventory_Slot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END