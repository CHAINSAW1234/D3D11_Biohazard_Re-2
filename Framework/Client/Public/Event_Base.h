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

	// load_eventprop���� ��쿡�� map���� ���߿� �����ǹǷ� ����Ʈ���� ���� �ø��ȴ� 
	// => �̸� �����ؼ� layer�� ������ �ִٰ� ó���ؾ��ϳ�?
private:

public:
	virtual void Free() override;
};

END