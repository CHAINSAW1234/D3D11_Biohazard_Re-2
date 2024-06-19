#include "stdafx.h"
#include "..\Public\Loader.h"
#include <process.h>

#include "GameInstance.h"
#include "Camera_Free.h"
#include "AnimTestObject.h"
#include "AnimTestPartObject.h"
//	#include "BackGround.h"
#include "Terrain.h"


CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

_uint APIENTRY LoadingMain(void* pArg)
{
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	/* 로더에게 지정된 레벨을 준비해라*/
	CLoader* pLoader = (CLoader*)pArg;

	if (FAILED(pLoader->Start()))
		return 1;

	CoUninitialize();

	return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_Critical_Section);

	/* 스레드를 생성하낟. */
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Start()
{
	EnterCriticalSection(&m_Critical_Section);

	HRESULT		hr = { 0 };

	switch (m_eNextLevelID)
	{
	case LEVEL_LOGO:
		hr = Loading_For_Logo();
		break;
	case LEVEL_TOOL:
		hr = Loading_For_GamePlay();
		break;
	}

	if (FAILED(hr))
		return E_FAIL;

	LeaveCriticalSection(&m_Critical_Section);

	return S_OK;
}

HRESULT CLoader::Loading_For_Logo()
{
	m_strLoadingText = TEXT("텍스쳐를(을) 로딩 중 입니다.");
	/* For.Prototype_Component_Texture_Logo */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Logo/Logo.png")))))
	//	return E_FAIL;


	m_strLoadingText = TEXT("모델를(을) 로딩 중 입니다.");

	m_strLoadingText = TEXT("셰이더를(을) 로딩 중 입니다.");

	m_strLoadingText = TEXT("객체의 원형를(을) 로딩 중 입니다.");

	m_strLoadingText = TEXT("로딩이 완료되었습니다.");

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay()
{
	m_strLoadingText = TEXT("텍스쳐를(을) 로딩 중 입니다.");
	/* Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile%d.dds"), 1))))
		return E_FAIL;

	/*Prototype_Component_Texture_Mask*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Mask.dds"), 1))))
		return E_FAIL;

	/*Prototype_Component_Texture_Brush */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Brush"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Brush.png"), 1))))
		return E_FAIL;




	//			TEST ANIMS


	/* Prototype_Component_Model_LeonBody */
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Test_Animsa"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/TestAnims/em0000_0000_Idle_Loop_A (960 frames).noefbxmulti - em0000_0000_Idle_Loop_A (960 frames).fbx", XMMatrixIdentity()))))
		return E_FAIL;




	m_strLoadingText = TEXT("모델를(을) 로딩 중 입니다.");
	_matrix			TransformMatrix = { XMMatrixIdentity() };
	_matrix			LeonTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
	//	_matrix			LeonTransformMatrix = XMMatrixRotationY(XMConvertToRadians(0.f));

	/* Prototype_Component_police_holl */
	/*if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_police_holl"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/map/police_holl.fbx",
			TransformMatrix))))
		return E_FAIL;*/

#pragma region Player
		/* Prototype_Component_Model_LeonBody */
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_LeonBody"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/LeonTest/LeonBody.fbx",
			LeonTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_LeonFace */
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_LeonFace"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/LeonTest/LeonFace.fbx",
			LeonTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_LeonHair */
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_LeonHair"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/LeonTest/LeonHair.fbx",
			LeonTransformMatrix))))
		return E_FAIL;

#pragma endregion

#pragma region Monster
	/* Prototype_Component_Model_ZombieBody */
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_ZombieBody"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Ex_Default_Zombie/Body.fbx",
			LeonTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_ZombieFace */
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_ZombieFace"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Ex_Default_Zombie/Face.fbx",
			LeonTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_ZombieHat */
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_ZombieHat"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Ex_Default_Zombie/Hat.fbx",
			LeonTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_ZombieShirts */
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_ZombieShirts"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Ex_Default_Zombie/Shirts.fbx",
			LeonTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_ZombiePants */
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_ZombiePants"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Ex_Default_Zombie/Pants.fbx",
			LeonTransformMatrix))))
		return E_FAIL;

#pragma endregion

#pragma region TEST

	/* Prototype_Component_Model_TEST */
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_TEST"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/TEST/Prototype_Component_Model_TEST.fbx",
			LeonTransformMatrix))))
		return E_FAIL;

#pragma endregion


	/* Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;

	/* Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_strLoadingText = TEXT("콜라이더를(을) 로딩 중 입니다.");

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Collider_Sphere"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;

	m_strLoadingText = TEXT("네비게이션를(을) 로딩 중 입니다.");


	m_strLoadingText = TEXT("셰이더를(을) 로딩 중 입니다.");
	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxCube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxModel */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModel.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxModel */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimModel.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxInstance_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxInstance_Rect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_Rect.hlsl"), VTXINSTANCE_RECT::Elements, VTXINSTANCE_RECT::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxInstance_Point */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxInstance_Point"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_Point.hlsl"), VTXINSTANCE_POINT::Elements, VTXINSTANCE_POINT::iNumElements))))
		return E_FAIL;



	m_strLoadingText = TEXT("객체를(을) 로딩 중 입니다.");
	///* For.Prototype_GameObject_Terrain */
	//if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
	//	CTerrain::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	///* For.Prototype_GameObject_Camera_Free */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_AnimTestObject */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AnimTestObject"),
		CAnimTestObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_AnimTestPartObject */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AnimTestPartObject"),
		CAnimTestPartObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	///* For.Prototype_GameObject_Part_Weapon */
	//if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Part_Weapon"),
	//	CWeapon::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;



	m_strLoadingText = TEXT("로딩이 완료되었습니다.");

	m_isFinished = true;

	return S_OK;
}

CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed To Created : CLoader"));

		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);



	DeleteObject(m_hThread);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_Critical_Section);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

