#include "stdafx.h"
#include "Tool_Selector.h"

CTool_Selector::CTool_Selector()
{
}

HRESULT CTool_Selector::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CTool_Selector::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CTool_Selector::Free()
{
	__super::Free();
}
