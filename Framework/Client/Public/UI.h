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
	virtual void Set_Position(_vector vPos);
	virtual void Size_Stretch(_float fConversionValX, _float fConversionValY);
	virtual void Move(_float3 fMoveMent);

	virtual void Move_State(_float3 fMoveMent, _int iState);

	virtual _float3 ComputeMovement(_uint iSTATE);

	virtual _bool IsMouseHover();
	virtual _bool IsMouseHover(_float& fPosZ);

	virtual _bool IsPTInRect(_float2 fPoint);

	virtual _bool Get_IsChild() { return m_IsChild; }
	virtual void Set_IsChild(_bool IsChild) { m_IsChild = IsChild; }

protected:
	class CShader*				m_pShaderCom = { nullptr };
	class CTexture*				m_pTextureCom = { nullptr };
	class CTexture*				m_pMaskTextureCom = { nullptr };
	class CTexture*				m_pSubMaskTextureCom = { nullptr };
	class CVIBuffer_Rect*		m_pVIBufferCom = { nullptr };

protected:
	_float						m_fX = { 0.f };
	_float						m_fY = { 0.f };
	_float						m_fZ = { 0.f };
	_float						m_fSizeX = { 0.f };
	_float						m_fSizeY = { 0.f };
	_float4x4					m_WorldMatrix = {}, m_ViewMatrix = {}, m_ProjMatrix = {};
	_float						m_fAlpha = { 0.f };

	_vector						m_vPrePos = {};
	_vector						m_vPreScale = {};

	UI_ID						m_eUI_ID = {};

	_bool						m_IsChild = { false };

public:
	virtual CGameObject* Clone(void* pArg) override = 0;
	virtual void Free() override;
};

END