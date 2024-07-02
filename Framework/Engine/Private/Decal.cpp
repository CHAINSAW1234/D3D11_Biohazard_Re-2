#include "Decal.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"

CDecal::CDecal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CDecal::CDecal(const CDecal& rhs)
	: CGameObject{ rhs }
{

}

HRESULT CDecal::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDecal::Initialize(void* pArg)
{
	GAMEOBJECT_DESC		GameObjectDesc{};

	GameObjectDesc.fSpeedPerSec = 10.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	return S_OK;
}

void CDecal::Tick(_float fTimeDelta)
{

}

void CDecal::Late_Tick(_float fTimeDelta)
{

}

HRESULT CDecal::Render()
{
	return S_OK;
}

void CDecal::SetWorldMatrix(_float4x4 WorldMatrix)
{
	m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&WorldMatrix));
}

HRESULT CDecal::Add_Components()
{
	return S_OK;
}

HRESULT CDecal::Bind_ShaderResources()
{
	return S_OK;
}

CDecal* CDecal::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDecal* pInstance = new CDecal(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CDecal"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CDecal::Clone(void* pArg)
{
	CDecal* pInstance = new CDecal(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CDecal"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDecal::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Delete(m_DecalInfo);
	Safe_Release(m_pSB_DecalInfo);
	Safe_Release(m_pUAV_DecalInfo);
	Safe_Release(m_pCB_DecalConstData);
	Safe_Release(m_pStaging_Buffer_Decal_Info);
	Safe_Release(m_pStaging_Buffer_Decal_Map);
	Safe_Release(m_pSB_DecalMap);
	Safe_Release(m_pSRV_DecalMap);
	Safe_Release(m_pUAV_DecalMap);
	Safe_Release(m_pRTV_DecalMap);
	Safe_Delete_Array(m_pDecal_Map);
}
