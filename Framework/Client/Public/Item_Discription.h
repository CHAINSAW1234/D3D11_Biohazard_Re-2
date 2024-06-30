#pragma once
#include "UI.h"


BEGIN(Client)

class CItem_Discription final : public CUI
{
private:
	enum ITEM_DISCRIPTION{NAME, CLASSIFY, DISCRIPTION};

protected:
	CItem_Discription(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItem_Discription(const CItem_Discription& rhs);
	virtual ~CItem_Discription() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_Item_Number(ITEM_NUMBER eItem_Num) { m_eItemNumber = eItem_Num; }

private:
	class CTextBox* m_pItemName = { nullptr };
	class CTextBox* m_pItemClassify = { nullptr };
	class CTextBox* m_pItemDiscription = { nullptr };

private:
	/*for.ItemDiscription*/
	unordered_map<ITEM_NUMBER, vector<wstring>>		m_mapItemDiscription;

	ITEM_NUMBER										m_eItemNumber = { ITEM_NUMBER_END };


private:
	_bool m_isAlphaControl = { false };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

	HRESULT Load_Item_Discription();

	HRESULT Create_TextBoxes();

public:
	static CItem_Discription* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END