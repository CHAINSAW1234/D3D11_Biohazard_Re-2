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

#pragma region CubeMap
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_CubeMap"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/CubeMap/st4_101_0_00.dds")))))
		return E_FAIL;

#pragma endregion

#pragma region Model
	m_strLoadingText = TEXT("Now Loading ... Model");

	_matrix			TransformMatrix = { XMMatrixIdentity() };
	_matrix			LeonTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
	_matrix			WeaponTransformMatrix = { XMMatrixRotationY(XMConvertToRadians(90.f)) };
	WeaponTransformMatrix *= XMMatrixRotationX(XMConvertToRadians(-90.f));
	_matrix			LightTransformMatrix = { XMMatrixRotationX(XMConvertToRadians(30.f)) };
#pragma region Players Model 

	/* Prototype_Component_Model_LeonBody */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_LeonBody"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/LeonTest/LeonBody.fbx",
			LeonTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_LeonFace */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_LeonFace"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/LeonTest/LeonFace.fbx",
			LeonTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_LeonHair */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_LeonHair"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/LeonTest/LeonHair.fbx",
			LeonTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_HandGun */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_HandGun"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Weapon/HandGun/HandGun.fbx",
			WeaponTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_FlashLight */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_FlashLight"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapon/FlashLight/FlashLight.fbx",
			TransformMatrix))))
		return E_FAIL;






	/* Prototype_Component_Model_FlashLight */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_7800"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/pl7800/pl7800.fbx",
			LeonTransformMatrix))))
		return E_FAIL;


	/* Prototype_Component_Model_FlashLight */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_7850"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/pl7850/pl7850.fbx",
			LeonTransformMatrix))))
		return E_FAIL;


	/* Prototype_Component_Model_FlashLight */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_7870"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/pl7870/pl7870.fbx",
			LeonTransformMatrix))))
		return E_FAIL;


	/* Prototype_Component_Model_FlashLight */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_7880"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/pl7880/pl7880.fbx",
			LeonTransformMatrix))))
		return E_FAIL;
#pragma endregion

#pragma region Monsters Model

	_matrix				ZombieTrnasformMatrix = { XMMatrixRotationY(XMConvertToRadians(180.f)) };

	//////////////////////////////////////////////////
	//	BODY
	//////////////////////////////////////////////////
	/* Prototype_Component_Model_Zombie_Body_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Body_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Body_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Body_Female */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Body_Female"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Female/Body_Female.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Body_Male_Big */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Body_Male_Big"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male_Big/Body_Male_Big.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	//////////////////////////////////////////////////
	//	HAT
	//////////////////////////////////////////////////
	/* Prototype_Component_Model_Zombie_Hat00_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Hat00_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Hat00_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Hat01_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Hat01_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Hat01_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	//////////////////////////////////////////////////
	//	Face
	//////////////////////////////////////////////////
	/* Prototype_Component_Model_Zombie_Face00_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Face00_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Face00_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Face01_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Face01_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Face01_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Face02_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Face02_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Face02_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Face03_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Face03_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Face03_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Face04_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Face04_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Face04_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Face05_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Face05_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Face05_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Face06_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Face06_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Face06_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Face07_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Face07_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Face07_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Face08_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Face08_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Face08_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Face09_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Face09_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Face09_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Face10_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Face10_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Face10_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	//////////////////////////////////////////////////
	//	Pants
	//////////////////////////////////////////////////
	/* Prototype_Component_Model_Zombie_Pants00_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Pants00_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Pants00_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Pants01_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Pants01_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Pants01_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Pants02_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Pants02_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Pants02_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Pants03_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Pants03_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Pants03_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Pants04_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Pants04_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Pants04_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Pants05_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Pants05_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Pants05_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Pants06_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Pants06_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Pants06_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	//////////////////////////////////////////////////
	//	Shirts
	//////////////////////////////////////////////////
	/* Prototype_Component_Model_Zombie_Shirts00_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Shirts00_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Shirts00_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Shirts01_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Shirts01_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Shirts01_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Shirts02_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Shirts02_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Shirts02_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Shirts03_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Shirts03_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Shirts03_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Shirts04_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Shirts04_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Shirts04_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Shirts05_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Shirts05_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Shirts05_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Shirts06_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Shirts06_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Shirts06_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Shirts08_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Shirts08_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Shirts08_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Shirts09_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Shirts09_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Shirts09_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Shirts11_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Shirts11_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Shirts11_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Shirts12_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Shirts12_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Shirts12_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Shirts70_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Shirts70_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Shirts70_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Shirts72_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Shirts72_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Shirts72_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Shirts73_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Shirts73_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Shirts73_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Shirts81_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zombie_Shirts81_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Shirts81_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

#pragma endregion

	m_strLoadingText = TEXT("Now Loading ... Animations");

#pragma region Animation Load 


	if (FAILED(Load_Animations()))
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


HRESULT CLoader::Load_Animations()
{
	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Player_Move_Fine"), "../Bin/Resources/Animations/Player/Body/move/move_fine/")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Player_Move_Stg"), "../Bin/Resources/Animations/Player/Body/move/move_stg/")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Player_Move_Light"), "../Bin/Resources/Animations/Player/Body/move/move_stlight/")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Player_Move_Caution"), "../Bin/Resources/Animations/Player/Body/move/move_caution/")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Player_Move_Caution_Light"), "../Bin/Resources/Animations/Player/Body/move/move_caution_stlight/")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Player_Move_Danger"), "../Bin/Resources/Animations/Player/Body/move/move_danger/")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Player_Move_Danger_Light"), "../Bin/Resources/Animations/Player/Body/move/move_danger_stlight/")))
		return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Player_Move_Common"), "../Bin/Resources/Animations/Player/Body/move/move_danger_common/")))
	//	return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Player_Hold_Hg"), "../Bin/Resources/Animations/Player/Body/hold/hg_hold/")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Player_Hold_Stg"), "../Bin/Resources/Animations/Player/Body/hold/stg_hold/")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Player_Hold_Sup"), "../Bin/Resources/Animations/Player/Body/hold/sup_hold/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Player_Bite"), "../Bin/Resources/Animations/Player/Body/bite/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Player_Common"), "../Bin/Resources/Animations/Player/Body/move/move_common/")))
		return E_FAIL;

#pragma region Default Zombie Ordinary Anims

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Ordinary_Box_ClimbOver"), "../Bin/Resources/Animations/Body_Zombie/Ordinary/Box_ClimbOver/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Ordinary_ETC"), "../Bin/Resources/Animations/Body_Zombie/Ordinary/ETC/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Ordinary_Hold"), "../Bin/Resources/Animations/Body_Zombie/Ordinary/Hold/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Ordinary_Idle"), "../Bin/Resources/Animations/Body_Zombie/Ordinary/Idle/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Ordinary_PivotTurn"), "../Bin/Resources/Animations/Body_Zombie/Ordinary/PivotTurn/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Ordinary_Stairs_PivotTurn"), "../Bin/Resources/Animations/Body_Zombie/Ordinary/Stairs_PivotTurn/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Ordinary_Stairs_Walk"), "../Bin/Resources/Animations/Body_Zombie/Ordinary/Stairs_Walk/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Ordinary_StandUp"), "../Bin/Resources/Animations/Body_Zombie/Ordinary/StandUp/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Ordinary_Walk"), "../Bin/Resources/Animations/Body_Zombie/Ordinary/Walk/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Ordinary_Walk_Lose"), "../Bin/Resources/Animations/Body_Zombie/Ordinary/Walk_Lose/")))
		return E_FAIL;

#pragma endregion

#pragma region Default Zombie Add Anims

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Add_Arm_L"), "../Bin/Resources/Animations/Body_Zombie/Add/Arm_L/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Add_Arm_R"), "../Bin/Resources/Animations/Body_Zombie/Add/Arm_R/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Add_Body"), "../Bin/Resources/Animations/Body_Zombie/Add/Body/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Add_Head"), "../Bin/Resources/Animations/Body_Zombie/Add/Head/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Add_Leg_L"), "../Bin/Resources/Animations/Body_Zombie/Add/Leg_L/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Add_Leg_R"), "../Bin/Resources/Animations/Body_Zombie/Add/Leg_R/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Add_Shoulder_L"), "../Bin/Resources/Animations/Body_Zombie/Add/Shoulder_L/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Add_Shoulder_R"), "../Bin/Resources/Animations/Body_Zombie/Add/Shoulder_R/")))
		return E_FAIL;

#pragma endregion

#pragma region Default Zombie Bite Anims

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Bite_Push_Down"), "../Bin/Resources/Animations/Body_Zombie/Bite/Push_Down/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Bite_Creep"), "../Bin/Resources/Animations/Body_Zombie/Bite/Creep/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Bite_Default_Front"), "../Bin/Resources/Animations/Body_Zombie/Bite/Default_Front/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Bite_Default_Back"), "../Bin/Resources/Animations/Body_Zombie/Bite/Default_Back/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Bite_Lightly_Hold"), "../Bin/Resources/Animations/Body_Zombie/Bite/Lightly_Hold/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Bite_ETC"), "../Bin/Resources/Animations/Body_Zombie/Bite/ETC/")))
		return E_FAIL;

#pragma endregion

#pragma region Default Zombie Damage Anims

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Damage_Burst"), "../Bin/Resources/Animations/Body_Zombie/Damage/Burst/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Damage_Default"), "../Bin/Resources/Animations/Body_Zombie/Damage/Default/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Damage_Down"), "../Bin/Resources/Animations/Body_Zombie/Damage/Down/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Damage_Electric_Shock"), "../Bin/Resources/Animations/Body_Zombie/Damage/Electric_Shock/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Damage_Knockback"), "../Bin/Resources/Animations/Body_Zombie/Damage/Knockback/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Damage_Lost"), "../Bin/Resources/Animations/Body_Zombie/Damage/Lost/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Damage_Stairs_Down"), "../Bin/Resources/Animations/Body_Zombie/Damage/Stairs_Down/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Damage_Stairs_Up"), "../Bin/Resources/Animations/Body_Zombie/Damage/Stairs_Up/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Damage_Stun"), "../Bin/Resources/Animations/Body_Zombie/Damage/Stun/")))
		return E_FAIL;

#pragma endregion

#pragma region Default Zombie Dead Anims

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Dead_Default"), "../Bin/Resources/Animations/Body_Zombie/Dead/Default/")))
		return E_FAIL;

#pragma endregion

#pragma region Default Zombie Lost Anims

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Lost_Bite"), "../Bin/Resources/Animations/Body_Zombie/Lost/Bite/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Lost_Dead2_Idle"), "../Bin/Resources/Animations/Body_Zombie/Lost/Dead2_Idle/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Lost_Hold"), "../Bin/Resources/Animations/Body_Zombie/Lost/Hold/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Lost_Idle"), "../Bin/Resources/Animations/Body_Zombie/Lost/Idle/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Lost_Turn"), "../Bin/Resources/Animations/Body_Zombie/Lost/Turn/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Lost_TurnOver"), "../Bin/Resources/Animations/Body_Zombie/Lost/TurnOver/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Lost_Walk"), "../Bin/Resources/Animations/Body_Zombie/Lost/Walk/")))
		return E_FAIL;

#pragma endregion

#pragma region Default Zombie Sick Anims

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Sick_FlashGranade"), "../Bin/Resources/Animations/Body_Zombie/Sick/FlashGranade/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Sick_Knife"), "../Bin/Resources/Animations/Body_Zombie/Sick/Knife/")))
		return E_FAIL;

#pragma endregion

#pragma region Default Zombie Undiscovered Anims

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Undiscovered_Cage"), "../Bin/Resources/Animations/Body_Zombie/Undiscovered/Cage/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Undiscovered_Capture"), "../Bin/Resources/Animations/Body_Zombie/Undiscovered/Capture/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Undiscovered_Celling_Fall"), "../Bin/Resources/Animations/Body_Zombie/Undiscovered/Celling_Fall/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Undiscovered_Dead"), "../Bin/Resources/Animations/Body_Zombie/Undiscovered/Dead/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Undiscovered_Dead_Pose"), "../Bin/Resources/Animations/Body_Zombie/Undiscovered/Dead_Pose/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Undiscovered_Eat"), "../Bin/Resources/Animations/Body_Zombie/Undiscovered/Eat/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Undiscovered_Fance"), "../Bin/Resources/Animations/Body_Zombie/Undiscovered/Fance/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Undiscovered_HeadBang"), "../Bin/Resources/Animations/Body_Zombie/Undiscovered/HeadBang/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Undiscovered_Lounge"), "../Bin/Resources/Animations/Body_Zombie/Undiscovered/Lounge/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Undiscovered_Prison"), "../Bin/Resources/Animations/Body_Zombie/Undiscovered/Prison/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Undiscovered_Railing_Fall"), "../Bin/Resources/Animations/Body_Zombie/Undiscovered/Railing_Fall/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Undiscovered_Railing_Stund"), "../Bin/Resources/Animations/Body_Zombie/Undiscovered/Stund/")))
		return E_FAIL;

#pragma endregion

#pragma region Default Zombie Gimmick Anims

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Gimmick_Window"), "../Bin/Resources/Animations/Body_Zombie/Gimmick/Window/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Gimmick_Door"), "../Bin/Resources/Animations/Body_Zombie/Gimmick/Door/")))
		return E_FAIL;

#pragma endregion

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

