#pragma once

#include "Client_Defines.h"
#include "Event.h"

BEGIN(Client)

class CEvent_Base abstract : public CEvent
{
protected:
	CEvent_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CEvent_Base() = default;

public:
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual CEvent::STATE Tick(_float fTimeDelta) override;

protected:
	HRESULT CEvent_Base::Load_Collider(const wstring& strFile, const wstring& strColLayerTag, _int iLevel, _float4 vColliderColor = { 1.f, 1.f, 1.f, 1.f });

	// load_eventprop같은 경우에는 map보다 나중에 생성되므로 옥토트리에 의해 컬링된다 
	// => 미리 생성해서 layer로 가지고 있다가 처리해야하나?
private:

public:
	virtual void Free() override;
};

END