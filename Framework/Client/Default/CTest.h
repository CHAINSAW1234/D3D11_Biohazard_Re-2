#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Client)
class CTest :
    public CPartObject
{
public:
	typedef struct tagPartObjectDesc : public GAMEOBJECT_DESC
	{
		CTransform* pParentsTransform = { nullptr };
	}PARTOBJECT_DESC;

protected:
	CTest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTest(const CPartObject& rhs);
	virtual ~CTest() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Priority_Tick(_float fTimeDelta) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual HRESULT			Render_LightDepth() { return S_OK; }

protected:
	void					Update_WorldMatrix();

protected:
	_float4x4				m_WorldMatrix;
	_float4x4				m_PrevWorldMatrix;
	CTransform* m_pParentsTransform = { nullptr };


public:
	static CTest* Create();
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END