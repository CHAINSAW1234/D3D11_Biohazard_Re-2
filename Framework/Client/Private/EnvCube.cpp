#include "stdafx.h"
#include "..\Public\EnvCube.h"

CEnvCube::CEnvCube(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject{ pDevice, pContext }
{
}

CEnvCube::CEnvCube(const CEnvCube & rhs)
	: CGameObject{ rhs }
{

}

HRESULT CEnvCube::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEnvCube::Initialize(void * pArg)
{
	GAMEOBJECT_DESC		GameObjectDesc{};

	GameObjectDesc.fSpeedPerSec = 10.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;		

	return S_OK;
}

void CEnvCube::Tick(_float fTimeDelta)
{
	return;
}

void CEnvCube::Late_Tick(_float fTimeDelta)
{
	//m_pTransformCom->Set_WorldMatirx(m_CubeList[m_iCurRegion][m_iCubTextureNum]);

	//m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CEnvCube::Render()
{
	//if (FAILED(Bind_ShaderResources()))
	//	return E_FAIL;

	//m_pShaderCom->Begin(0);

	//m_pVIBufferCom->Bind_Buffers();

	//m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CEnvCube::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxCube"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	filesystem::path FullPath(TEXT("../Bin/Data/EnvCubeSetting.dat"));



	ifstream ifs(FullPath.c_str(), ios::binary);

	if (true == ifs.fail())
	{
		MSG_BOX(TEXT("Failed To OpenFile"));

		return E_FAIL;
	}
	_int iEntireTextures;
	ifs.read(reinterpret_cast<_char*>(&iEntireTextures), sizeof(_int));
	
	/* For.Com_Texture */
	for (_int i = 0; i < iEntireTextures; i++)
	{
		_tchar szPrototypeComName[MAX_PATH];
		_tchar szPath[MAX_PATH];
		_int iTextureNum;
		ifs.read(reinterpret_cast<_char*>(&szPrototypeComName), sizeof(_tchar) * MAX_PATH);
		ifs.read(reinterpret_cast<_char*>(&szPath), sizeof(_tchar) * MAX_PATH);
		ifs.read(reinterpret_cast<_char*>(&iTextureNum), sizeof(_int));
		wstring strTag = TEXT("Com_Texture")+ m_pGameInstance->StringToWstring(to_string(i));
		vector<_float4x4> Cubvec;
		Cubvec.resize(iTextureNum);
		CTexture* pTexture = { nullptr };
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, szPrototypeComName,
			strTag, (CComponent**)&pTexture)))
			return E_FAIL;
		m_TextureComMap.insert({ i, pTexture });
		m_CubeList.insert({ i, Cubvec });
		m_TextureNumMap.insert({ i, iTextureNum });
	}

	m_iEntireTexture = iEntireTextures;
	ifs.close();


	filesystem::path Full(TEXT("../Bin/Data/EnvCube.dat"));

	ifstream ifst(Full.c_str(), ios::binary);

	if (true == ifst.fail())
	{
		MSG_BOX(TEXT("Failed To OpenFile"));

		return E_FAIL;
	}
	_int iEntires;
	ifst.read(reinterpret_cast<_char*>(&iEntires), sizeof(_int));

	/* For.Com_Texture */

	for (_int i = 0; i < iEntires; i++)
	{
		Set_CubeRegion(i);
		_int iMaxNum;
		ifst.read(reinterpret_cast<_char*>(&iMaxNum), sizeof(_int));

		for (_int j = 0; j < iMaxNum; j++)
		{
			Set_CubeTextureNum(j);
			_float4x4 WorldMatrix;
			ifst.read(reinterpret_cast<_char*>(&WorldMatrix), sizeof(_float4x4));
			Set_WorldMatrix(WorldMatrix);
		}

	}

	ifst.close();
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}



HRESULT CEnvCube::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;	

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_TextureComMap[m_iCurRegion]->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iCubTextureNum)))
		return E_FAIL;

	return S_OK;
}

CEnvCube * CEnvCube::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEnvCube*		pInstance = new CEnvCube(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CEnvCube"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject * CEnvCube::Clone(void * pArg)
{
	CEnvCube*		pInstance = new CEnvCube(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CEnvCube"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEnvCube::Free()
{
	__super::Free();

	for (_int i = 0; i < m_iEntireTexture; i++)
	{
		Safe_Release(m_TextureComMap[i]);
		m_TextureComMap[i] = nullptr;
	}
	Safe_Release(m_pShaderCom);	
	Safe_Release(m_pVIBufferCom);
}
