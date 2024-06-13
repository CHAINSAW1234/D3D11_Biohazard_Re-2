#include "stdafx.h"
#include "CTest.h"

CTest::CTest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice ,pContext }
{
}

CTest::CTest(const CPartObject& rhs)
	: CPartObject{ rhs }
{
}

HRESULT CTest::Initialize_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CTest::Initialize(void* pArg)
{
	return E_NOTIMPL;
}

void CTest::Priority_Tick(_float fTimeDelta)
{
}

void CTest::Tick(_float fTimeDelta)
{
}

void CTest::Late_Tick(_float fTimeDelta)
{
}

HRESULT CTest::Render()
{
	return E_NOTIMPL;
}

void CTest::Update_WorldMatrix()
{
}

CTest* CTest::Create()
{
	return nullptr;
}

CGameObject* CTest::Clone(void* pArg)
{
	return nullptr;
}

void CTest::Free()
{
}
