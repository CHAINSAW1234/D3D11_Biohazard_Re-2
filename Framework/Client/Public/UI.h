#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CUI abstract : public CGameObject
{
public:
	enum UISTATE {UISTATE_POS, UISTATE_SCALE, UISTATE_END};
	typedef struct UI_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		_float4x4		fWorldMatrix;

		_float3			vPos = {};
		_float2			vSize = {};
		_float			fAlpha = { 0.f };

	}UI_DESC;

protected:
	CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI(const CUI& rhs);
	virtual ~CUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override = 0;

public:
	virtual void Set_Size(_float fSizeX, _float fSizeY);
	virtual void Set_Position(_float fPosX, _float fPosY, _float fPosZ);
	virtual void Size_Stretch(_float fConversionValX, _float fConversionValY);

	virtual void Move_State(_float3 fMoveMent, _int iState);

	virtual _float3 ComputeMovement(_uint iSTATE);

	virtual _bool IsMouseHover();
	virtual _bool IsMouseHover(_float& fPosZ);

protected:
	class CShader*				m_pShaderCom = { nullptr };
<<<<<<< HEAD
	class CTexture*				m_pMask_TextureCom = { nullptr };
=======
>>>>>>> parent of 6f7c5be (Customize_UI Load, Value)
	class CTexture*				m_pTextureCom = { nullptr };
	class CVIBuffer_Rect*		m_pVIBufferCom = { nullptr };

protected:
	_float					m_fX = { 0.f };
	_float					m_fY = { 0.f };
	_float					m_fZ = { 0.f };
	_float					m_fSizeX = { 0.f };
	_float					m_fSizeY = { 0.f };
	_float4x4				m_WorldMatrix = {}, m_ViewMatrix = {}, m_ProjMatrix = {};
	_float					m_fAlpha = { 0.f };

	_vector					m_vPrePos = {};
	_vector					m_vPreScale = {};

	_bool					m_isSelect = { false };

	UI_ID					m_eUI_ID = {};

//protected:
//	virtual HRESULT Add_Components();
//	virtual HRESULT Bind_ShaderResources() = 0;

public:
	virtual CGameObject* Clone(void* pArg) override = 0;
	virtual void Free() override;
};

END