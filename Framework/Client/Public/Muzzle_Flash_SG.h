#pragma once

#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Client)

class CMuzzle_Flash_SG final : public CEffect
{
public:
	CMuzzle_Flash_SG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMuzzle_Flash_SG(const CMuzzle_Flash_SG& rhs);
	virtual ~CMuzzle_Flash_SG() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	virtual void			Compute_CurrentUV();
	virtual void			Setup_Billboard() override;
public:
	void					SetSize(_float fSizeX, _float fSizeY);
	virtual void			SetPosition(_float4 Pos) override;
	virtual void			Set_Render(_bool boolean) override
	{
		m_bRender = boolean;
		m_FrameTime = GetTickCount64();
		m_iFrame = 0;
		m_fAlpha_Delta_Sum = 0.f;
	}

private:
	virtual HRESULT			Add_Components();
	virtual HRESULT			Bind_ShaderResources();

private:
	_float2									m_vFirstFrame_Size;
	_float2									m_vSecondFrame_Size;
	_float2									m_vThirdFrame_Size;
	_int									m_iMainFrame = { 0 };
	class CMuzzle_Light_SG*					m_pMuzzle_Light = { nullptr };
	vector<class CMuzzle_Spark_SG*>			m_vecMuzzle_Spark_SG = { nullptr };
	_int									m_iSparkIndex = { 0 };
public:
	static CMuzzle_Flash_SG* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END