#include "stdafx.h"
#include "AnimationEditor.h"

CAnimationEditor::CAnimationEditor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEditor{ pDevice, pContext }
{
}

HRESULT CAnimationEditor::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CAnimationEditor::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

HRESULT CAnimationEditor::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAnimationEditor::Add_Components()
{
	return S_OK;
}

HRESULT CAnimationEditor::Add_Tools()
{
	return S_OK;
}

CAnimationEditor* CAnimationEditor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CAnimationEditor* pInstance = new CAnimationEditor(pDevice, pContext);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Create : CMapEditor"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimationEditor::Free()
{
	__super::Free();
}
