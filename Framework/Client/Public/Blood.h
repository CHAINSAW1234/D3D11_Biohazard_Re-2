#pragma once

#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Client)

class CBlood final : public CEffect
{
public:
	CBlood(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBlood(const CBlood& rhs);
	virtual ~CBlood() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	void					SetSize(_float fSizeX, _float fSizeY,_float fSizeZ);
	void					SetWorldMatrix_With_HitNormal(_vector vUp);
	_float4x4				GetWorldMatrix();

private:
	virtual HRESULT			Add_Components();
	virtual HRESULT			Bind_ShaderResources();

private:
	CModel* m_pModelCom = { nullptr };
	CModel* m_pModelCom_2 = { nullptr };
	CModel* m_pModelCom_3 = { nullptr };
	CModel* m_pModelCom_4 = { nullptr };
	CModel* m_pModelCom_5 = { nullptr };
	CModel* m_pModelCom_6 = { nullptr };
	CModel* m_pModelCom_7 = { nullptr };
	CModel* m_pModelCom_8 = { nullptr };
	CModel* m_pModelCom_9 = { nullptr };
	CModel* m_pModelCom_10 = { nullptr };
	CModel* m_pModelCom_11 = { nullptr };

	_float					m_fSize_X_Default = { 0.f };
	_float					m_fSize_Y_Default = { 0.f };
	_float					m_fSize_Z_Default = { 0.f };

public:
	static CBlood* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END