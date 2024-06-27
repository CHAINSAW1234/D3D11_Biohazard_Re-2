#include "stdafx.h"
#include "Decal_Blood.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"
#include "Engine_Struct.h"
#include "Mesh.h"

CDecal_Blood::CDecal_Blood(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CDecal{ pDevice, pContext }
{
}

CDecal_Blood::CDecal_Blood(const CDecal_Blood& rhs)
	: CDecal{ rhs }
{

}

void CDecal_Blood::Add_Skinned_Decal(AddDecalInfo Info)
{

}

HRESULT CDecal_Blood::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDecal_Blood::Initialize(void* pArg)
{
	GAMEOBJECT_DESC		GameObjectDesc{};

	GameObjectDesc.fSpeedPerSec = 10.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	/*decalLookupTexture = Demo::resourceManager->CreateTexture(desc, "Decal lookup map");
	if (!decalLookupTextures[numDecalLookupTextures])
		return false;
	subModel.materialDT.AddTextureSrv(decalLookupTextures[numDecalLookupTextures]);*/

	//DXGI_FORMAT_R32_UINT

	decalConstData.subModelInfo.firstIndex = 0;
	decalConstData.subModelInfo.numTris = m_pMesh->GetNumIndices()/3;
	decalConstData.subModelInfo.decalLookupMapWidth = static_cast<float>(512);
	decalConstData.subModelInfo.decalLookupMapHeight = static_cast<float>(512);

	return S_OK;
}

void CDecal_Blood::Tick(_float fTimeDelta)
{
	
}

void CDecal_Blood::Late_Tick(_float fTimeDelta)
{
	
}

HRESULT CDecal_Blood::Render()
{
	return S_OK;
}

HRESULT CDecal_Blood::Add_Components()
{
	return S_OK;
}

HRESULT CDecal_Blood::Bind_ShaderResources()
{
	return S_OK;
}

CDecal_Blood* CDecal_Blood::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDecal_Blood* pInstance = new CDecal_Blood(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CDecal_Blood"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CDecal_Blood::Clone(void* pArg)
{
	CDecal_Blood* pInstance = new CDecal_Blood(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CDecal_Blood"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDecal_Blood::Free()
{
	__super::Free();
}
