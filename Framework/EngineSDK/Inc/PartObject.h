#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CPartObject abstract : public CGameObject
{
public:
	typedef struct tagPartObjectDesc: public GAMEOBJECT_DESC
	{
		CTransform*			pParentsTransform = { nullptr };
	}PARTOBJECT_DESC;

protected:
	CPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPartObject(const CPartObject& rhs);
	virtual ~CPartObject() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Priority_Tick(_float fTimeDelta) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual HRESULT			Render_LightDepth() { return S_OK; }
	virtual void			SetCulling(_bool boolean) {}
	virtual void			Create_Cloth() {}
protected:
	void					Update_WorldMatrix();

protected:
	_float4x4				m_WorldMatrix;
	_float4x4				m_PrevWorldMatrix;
	CTransform*				m_pParentsTransform = { nullptr };


public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END