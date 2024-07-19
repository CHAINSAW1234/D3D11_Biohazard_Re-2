#include "Sound_Manager.h"
#include "GameInstance.h"
#include "Transform.h"

CSound_Manager::CSound_Manager()
{
}

HRESULT CSound_Manager::Initialize()
{
	FMOD_RESULT eResult = { FMOD_OK };
	eResult = FMOD_System_Create(&m_pSystem, FMOD_VERSION);
	if (eResult != FMOD_OK)
		return E_FAIL;

	// 1. 시스템 포인터, 2. 사용할 가상채널 수 , 초기화 방식) 
	eResult = FMOD_System_Init(m_pSystem, SOUND_CHANNEL_MAX, FMOD_INIT_NORMAL , NULL);
	if (eResult != FMOD_OK)
		return E_FAIL;

	ZeroMemory(m_pChannelArr, sizeof(FMOD_CHANNEL*) * SOUND_CHANNEL_MAX);

	LoadSoundFile();
	LoadSoundFile_Zombie();

	return S_OK;
}

HRESULT CSound_Manager::Update_Listener(CTransform* pTransform, _float3 vVelocity)
{
	if (nullptr == pTransform)
		return E_FAIL;

	if (pTransform == m_pListener_Transform)
		return S_OK;

	Safe_Release(m_pListener_Transform);
	m_pListener_Transform = pTransform;
	Safe_AddRef(m_pListener_Transform);

	m_vVelocity = vVelocity;

	/*FMOD_3D_ATTRIBUTES					Attributes_desc;
	_vector			vPosition = { pTransform->Get_State_Vector(CTransform::STATE_POSITION) };
	_vector			vUp = { pTransform->Get_State_Vector(CTransform::STATE_UP) };
	_vector			vLook = { pTransform->Get_State_Vector(CTransform::STATE_LOOK) };

	_float3			vPositionFloat3, vUpFloat3 , vLookFloat3;

	XMStoreFloat3(&vPositionFloat3, vPosition);
	XMStoreFloat3(&vUpFloat3, vUp);
	XMStoreFloat3(&vLookFloat3, vLook);

	Attributes_desc.position = vPositionFloat3;
	Attributes_desc.forward = vLookFloat3;
	Attributes_desc.up = vUpFloat3;
	Attributes_desc.velocity = vVelocity;

	FMOD_RESULT eResult = { FMOD_OK };
	eResult = FMOD_System_Set3DListenerAttributes(m_pSystem, 0, &Attributes_desc.position, &Attributes_desc.velocity, &Attributes_desc.forward, &Attributes_desc.up);
	if (eResult != FMOD_OK)
		return E_FAIL;

#ifdef _DEBUG
	eResult = FMOD_System_Get3DListenerAttributes(m_pSystem, 0, &Attributes_desc.position, &Attributes_desc.velocity, &Attributes_desc.forward, &Attributes_desc.up);
	if (eResult != FMOD_OK)
		return E_FAIL;
#endif

	eResult = FMOD_System_Update(m_pSystem);
	if (eResult != FMOD_OK)
		return E_FAIL;*/

	return S_OK;
}

HRESULT CSound_Manager::Stop_Sound_3D(CTransform* pTransform, _uint iSoundIndex)
{
	SOUND_DESC*				pSoundDesc = { Find_SoundDesc_3D(pTransform, iSoundIndex) };
	if (nullptr == pSoundDesc)
		return E_FAIL;

	FMOD_RESULT				eResult = { FMOD_OK };
	eResult = FMOD_Channel_Stop(m_pChannelArr[pSoundDesc->iChannelIndex]);
	if (FMOD_OK != eResult)
		return E_FAIL;

	return S_OK;
}

HRESULT CSound_Manager::Stop_All()
{
	FMOD_RESULT				eResult = { FMOD_OK };
	for (int i = 0; i < SOUND_CHANNEL_MAX; ++i)
	{
		eResult = FMOD_Channel_Stop(m_pChannelArr[i]);
		if (eResult != FMOD_OK)
			return E_FAIL;
	}

	return S_OK;
}

void CSound_Manager::Change_Sound_3D(CTransform* pTransform, const wstring& strSoundTag, _uint iSoundIndex)
{
	SOUND_DESC*				pSoundDesc = { Find_SoundDesc_3D(pTransform, iSoundIndex) };
	FMOD_SOUND*				pSound = { Find_Sound(strSoundTag) };
	
	if (nullptr == pSoundDesc || nullptr == pSound)
		return;

	FMOD_BOOL				isPlaying = { false };
	FMOD_RESULT				eResult = { FMOD_OK };
	FMOD_Channel_IsPlaying(m_pChannelArr[pSoundDesc->iChannelIndex], &isPlaying);
	if (true == isPlaying)
	{
		Stop_Sound_3D(pTransform, iSoundIndex);
	}

	pSoundDesc->pSound = pSound;
	eResult = FMOD_System_PlaySound(m_pSystem, pSoundDesc->pSound, nullptr , FALSE, &m_pChannelArr[pSoundDesc->iChannelIndex]);
	if (FMOD_OK != eResult)
		return;
}

void CSound_Manager::Set_Volume_3D(CTransform* pTransform, _uint iSoundIndex, _float fVolume)
{
	SOUND_DESC*				pSoundDesc = { Find_SoundDesc_3D(pTransform, iSoundIndex) };
	if (nullptr == pSoundDesc)
		return;

	if (fVolume > SOUND_MAX)
		fVolume = SOUND_MAX;

	if (fVolume < SOUND_MIN)
		fVolume = SOUND_MIN;

	pSoundDesc->fVolume = fVolume;
}

void CSound_Manager::Set_Pause_3D(CTransform* pTransform, _uint iSoundIndex, _bool isPause)
{
	SOUND_DESC*				pSoundDesc = { Find_SoundDesc_3D(pTransform, iSoundIndex) };
	if (nullptr == pSoundDesc)
		return;

	pSoundDesc->bPause = isPause;
}

void CSound_Manager::Set_Distance_3D(CTransform* pTransform, _uint iSoundIndex, _float fMinDistance, _float fMaxDistance)
{
	SOUND_DESC*				pSoundDesc = { Find_SoundDesc_3D(pTransform, iSoundIndex) };
	if (nullptr == pSoundDesc)
		return;

	pSoundDesc->vRange.x = fMinDistance;
	pSoundDesc->vRange.y = fMaxDistance;
}

HRESULT CSound_Manager::Update_Sounds()
{
	if (FAILED(Update_Listener_Camera()))
		return E_FAIL;

	if (FAILED(Update_Sounds_Mode()))
		return E_FAIL;
	if (FAILED(Update_Sounds_Position()))
		return E_FAIL;
	if (FAILED(Update_Sounds_Volume()))
		return E_FAIL;
	if (FAILED(Update_Sounds_Paused()))
		return E_FAIL;
	if (FAILED(Update_Sounds_Distance()))
		return E_FAIL;

	if (FAILED(Update_Sounds_System()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSound_Manager::Update_Listener_Camera()
{
	FMOD_3D_ATTRIBUTES					Attributes_desc;
	_matrix			CameraWorldMatrix = { CGameInstance::Get_Instance()->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_VIEW) };

	_vector			vPosition = { CameraWorldMatrix.r[CTransform::STATE_POSITION] };
	_vector			vUp = { CameraWorldMatrix.r[CTransform::STATE_UP] };
	_vector			vLook = { CameraWorldMatrix.r[CTransform::STATE_LOOK] };

	_float3			vPositionFloat3, vUpFloat3, vLookFloat3;

	XMStoreFloat3(&vPositionFloat3, vPosition);
	XMStoreFloat3(&vUpFloat3, vUp);
	XMStoreFloat3(&vLookFloat3, vLook);

	Attributes_desc.position = vPositionFloat3;
	Attributes_desc.forward = vLookFloat3;
	Attributes_desc.up = vUpFloat3;
	Attributes_desc.velocity = m_vVelocity;

	FMOD_RESULT		eResult = { FMOD_OK };
	eResult = FMOD_System_Set3DListenerAttributes(m_pSystem, 0, &Attributes_desc.position, &Attributes_desc.velocity, &Attributes_desc.forward, &Attributes_desc.up);
	if (eResult != FMOD_OK)
		return E_FAIL;

#ifdef _DEBUG
	eResult = FMOD_System_Get3DListenerAttributes(m_pSystem, 0, &Attributes_desc.position, &Attributes_desc.velocity, &Attributes_desc.forward, &Attributes_desc.up);
	if (eResult != FMOD_OK)
		return E_FAIL;
#endif
}

HRESULT CSound_Manager::Update_Sounds_Position()
{
	if (nullptr == m_pListener_Transform)
		return S_OK;

	_vector				vListenerPosition = { m_pListener_Transform->Get_State_Vector(CTransform::STATE_POSITION) };
	_matrix				CameraWorldMatrix = { CGameInstance::Get_Instance()->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_VIEW) };
	_vector				vCameraPosition = { CameraWorldMatrix.r[CTransform::STATE_POSITION] };

	FMOD_RESULT			eResult = { FMOD_OK };

	for (auto& Pair : m_Sound_Descs_3D)
	{
		CTransform*				pTransform = { Pair.first };
		if (nullptr == pTransform)
			continue;

		_vector					vPosition = { pTransform->Get_State_Vector(CTransform::STATE_POSITION) };
		_vector					vLook = { pTransform->Get_State_Vector(CTransform::STATE_LOOK) };
		_float3					vLookFloat3;
		XMStoreFloat3(&vLookFloat3, vLook);

		_vector					vDirectionFromListener = { vPosition - vListenerPosition };
		_vector					vDirectionFromCamera = { vPosition - vCameraPosition };

		_float					fDistanceFromListner = { XMVectorGetX(XMVector3Length(vDirectionFromListener)) };
		_float					fDistanceFromCamera = { XMVectorGetX(XMVector3Length(vDirectionFromCamera)) };

		_vector					vVirtualPosition = { (vDirectionFromCamera / fDistanceFromCamera) * fDistanceFromListner };
		_float3					vVirtualPositionFloat3;
		XMStoreFloat3(&vVirtualPositionFloat3, vVirtualPosition);

		for (auto& SoundDesc : Pair.second)
		{
			if (nullptr == m_pChannelArr[SoundDesc.iChannelIndex])
				continue;

			FMOD_BOOL		isPlaying = { false };
			FMOD_Channel_IsPlaying(m_pChannelArr[SoundDesc.iChannelIndex], &isPlaying);
			if (false == isPlaying)
				continue;

			SoundDesc.vPos = FMOD_VECTOR(vVirtualPositionFloat3);
			SoundDesc.vSpeedDir = FMOD_VECTOR(vLookFloat3);

			eResult = FMOD_Channel_Set3DAttributes(m_pChannelArr[SoundDesc.iChannelIndex], &SoundDesc.vPos, &SoundDesc.vSpeedDir);
			if (eResult != FMOD_OK)
				return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CSound_Manager::Update_Sounds_Volume()
{
	FMOD_RESULT			eResult = { FMOD_OK };
	FMOD_BOOL			isPlaying = { false };

	for (auto& Pair : m_Sound_Descs_3D)
	{
		for (auto& SoundDesc : Pair.second)
		{
			if (nullptr == m_pChannelArr[SoundDesc.iChannelIndex])
				continue;

			FMOD_BOOL		isPlaying = { false };
			FMOD_Channel_IsPlaying(m_pChannelArr[SoundDesc.iChannelIndex], &isPlaying);
			if (false == isPlaying)
				continue;

			eResult = FMOD_Channel_SetVolume(m_pChannelArr[SoundDesc.iChannelIndex], SoundDesc.fVolume);
			if (FMOD_OK != eResult)
				return E_FAIL;			
		}
	}

	return S_OK;
}

HRESULT CSound_Manager::Update_Sounds_Mode()
{
	FMOD_RESULT			eResult = { FMOD_OK };
	FMOD_BOOL			isPlaying = { false };

	for (auto& Pair : m_Sound_Descs_3D)
	{
		for (auto& SoundDesc : Pair.second)
		{
			if (nullptr == m_pChannelArr[SoundDesc.iChannelIndex])
				continue;

			FMOD_BOOL		isPlaying = { false };
			FMOD_Channel_IsPlaying(m_pChannelArr[SoundDesc.iChannelIndex], &isPlaying);
			if (false == isPlaying)
				continue;

			eResult = FMOD_Channel_SetMode(m_pChannelArr[SoundDesc.iChannelIndex], SoundDesc.eMode);
			/*if (eResult != FMOD_OK)
				return E_FAIL;*/
		}
	}

	return S_OK;
}

HRESULT CSound_Manager::Update_Sounds_Paused()
{
	FMOD_RESULT			eResult = { FMOD_OK };
	FMOD_BOOL			isPlaying = { false };

	for (auto& Pair : m_Sound_Descs_3D)
	{
		for (auto& SoundDesc : Pair.second)
		{
			if (nullptr == m_pChannelArr[SoundDesc.iChannelIndex])
				continue;

			FMOD_BOOL		isPlaying = { false };
			FMOD_Channel_IsPlaying(m_pChannelArr[SoundDesc.iChannelIndex], &isPlaying);
			if (false == isPlaying)
				continue;

			eResult = FMOD_Channel_SetPaused(m_pChannelArr[SoundDesc.iChannelIndex], SoundDesc.bPause);
			if (eResult != FMOD_OK)
				return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CSound_Manager::Update_Sounds_Distance()
{
	FMOD_RESULT			eResult = { FMOD_OK };
	FMOD_BOOL			isPlaying = { false };

	for (auto& Pair : m_Sound_Descs_3D)
	{
		for (auto& SoundDesc : Pair.second)
		{
			if (nullptr == m_pChannelArr[SoundDesc.iChannelIndex])
				continue;

			FMOD_BOOL		isPlaying = { false };
			FMOD_Channel_IsPlaying(m_pChannelArr[SoundDesc.iChannelIndex], &isPlaying);
			if (false == isPlaying)
				continue;

			eResult = FMOD_Channel_Set3DMinMaxDistance(m_pChannelArr[SoundDesc.iChannelIndex], SoundDesc.vRange.x, SoundDesc.vRange.y);
			if (eResult != FMOD_OK)
				return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CSound_Manager::Update_Sounds_System()
{
	FMOD_RESULT				eResult = { FMOD_OK };
	eResult = FMOD_System_Update(m_pSystem);
	if (eResult != FMOD_OK)
		return E_FAIL;

	return S_OK;
}

HRESULT CSound_Manager::Add_Object_Sound(CTransform* pTransform, _uint iNumChannel)
{
	unordered_map<CTransform*, vector<SOUND_DESC>>::iterator				iter = { m_Sound_Descs_3D.find(pTransform) };
	if (iter != m_Sound_Descs_3D.end())
		return E_FAIL;

	vector<SOUND_DESC>				Sound_Descs;
	Sound_Descs.resize(iNumChannel);

	for (auto& SoundDesc : Sound_Descs)
	{
		SoundDesc.iChannelIndex = m_iNumUsningChannel++;
		SoundDesc.fVolume = SOUND_DEFAULT_VOLUME;
		SoundDesc.eMode = FMOD_3D | FMOD_3D_LINEARROLLOFF;
		SoundDesc.vRange.x = SOUND_DEFAULT_MIN_DISTANCE;
		SoundDesc.vRange.y = SOUND_DEFAULT_MAX_DISTANCE;
		SoundDesc.bPause = false;
	}

	m_Sound_Descs_3D.emplace(pTransform, Sound_Descs);

	return S_OK;
}

void CSound_Manager::LoadSoundFile()
{
	_tfinddata64_t fd;
	__int64 handle = _tfindfirst64(L"../Bin/Resources/Sound/*.*", &fd);
	if (handle == -1 || handle == 0)
		return;

	_int iResult = 0;

	char szCurPath[MAX_PATH] = "../Bin/Resources/Sound/";
	char szFullPath[MAX_PATH] = "";
	char szFilename[MAX_PATH];
	while (iResult != -1)
	{
		WideCharToMultiByte(CP_UTF8, 0, fd.name, -1, szFilename, sizeof(szFilename), NULL, NULL);
		strcpy_s(szFullPath, szCurPath);
		strcat_s(szFullPath, szFilename);
		FMOD_SOUND*				pSound = { nullptr };
		FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_3D, 0, &pSound);//|FMOD_3D_HEADRELATIVE  | FMOD_3D_INVERSEROLLOFF
		if (eRes == FMOD_OK)
		{
			_int iLength = (_int)strlen(szFilename) + 1;

			TCHAR pSoundKey[MAX_PATH] = { TEXT("") };
			MultiByteToWideChar(CP_ACP, 0, szFilename, iLength, pSoundKey, iLength);
			m_Soundmap.emplace(pSoundKey, pSound);
		}
		iResult = _tfindnext64(handle, &fd);
	}

	FMOD_System_Update(m_pSystem);
	_findclose(handle);
}

void CSound_Manager::LoadSoundFile_Zombie()
{
	_tfinddata64_t fd;
	__int64 handle = _tfindfirst64(L"../Bin/Resources/Sound/em/*.*", &fd);
	if (handle == -1 || handle == 0)
		return;

	_int iResult = 0;

	char szCurPath[MAX_PATH] = "../Bin/Resources/Sound/em/";
	char szFullPath[MAX_PATH] = "";
	char szFilename[MAX_PATH];
	while (iResult != -1)
	{
		WideCharToMultiByte(CP_UTF8, 0, fd.name, -1, szFilename, sizeof(szFilename), NULL, NULL);
		strcpy_s(szFullPath, szCurPath);
		strcat_s(szFullPath, szFilename);
		FMOD_SOUND*					pSound = { nullptr };
		FMOD_RESULT					eRes = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_3D, 0, &pSound);//|FMOD_3D_HEADRELATIVE  | FMOD_3D_INVERSEROLLOFF
		if (eRes == FMOD_OK)
		{
			_int iLength = (_int)strlen(szFilename) + 1;

			TCHAR pSoundKey[MAX_PATH] = { TEXT("") };
			MultiByteToWideChar(CP_ACP, 0, szFilename, iLength, pSoundKey, iLength);
			m_Soundmap.emplace(pSoundKey, pSound);
		}
		iResult = _tfindnext64(handle, &fd);
	}

	FMOD_System_Update(m_pSystem);
	_findclose(handle);
}

FMOD_SOUND* CSound_Manager::Find_Sound(const wstring& strSoundTag)
{
	auto& iter = m_Soundmap.find(strSoundTag);

	if (iter == m_Soundmap.end())
		return nullptr;

	return iter->second;
}

SOUND_DESC* CSound_Manager::Find_SoundDesc_3D(CTransform* pTransform, _uint iSoudIndex)
{
	if (nullptr == pTransform)
		return nullptr;

	unordered_map<CTransform*, vector<SOUND_DESC>>::iterator			iter = { m_Sound_Descs_3D.find(pTransform) };	
	if (iter == m_Sound_Descs_3D.end())
		return nullptr;

	if (iter->second.size() <= iSoudIndex)
		return nullptr;

	return &iter->second[iSoudIndex];
}

CSound_Manager* CSound_Manager::Create()
{
	CSound_Manager* pInstance = new CSound_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CSound_Manager"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSound_Manager::Free()
{
	__super::Free();

	for (auto& Pair : m_Soundmap)
	{
		FMOD_Sound_Release(Pair.second);
	}
	m_Soundmap.clear();

	Safe_Release(m_pListener_Transform);

	FMOD_System_Release(m_pSystem);
	FMOD_System_Close(m_pSystem);
}

