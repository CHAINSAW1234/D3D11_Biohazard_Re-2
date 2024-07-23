#include "stdafx.h"
#include "Camera_Event.h"
#include "Player.h"

CCamera_Event::CCamera_Event(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera{ pDevice, pContext }
{
}

CCamera_Event::CCamera_Event(const CCamera_Event& rhs)
	: CCamera{ rhs }
{
}

HRESULT CCamera_Event::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Event::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_fNear = 0.025f;

	//m_Defaultmatrix = XMMatrixIdentity();

	return S_OK;
}

void CCamera_Event::Tick(_float fTimeDelta)
{
	//Reset_CamPosition();
	__super::Tick(fTimeDelta);

	if (m_isPlay) {
		Play_MCAM(fTimeDelta);
	}
	else {
		;
	}
	__super::Bind_PipeLines();
}

void CCamera_Event::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CCamera_Event::Render()
{
	return S_OK;
}

void CCamera_Event::Active_Camera(_bool isActive)
{
	__super::Active_Camera(isActive);
	
	//if (!m_isActive)
	//	Reset();
}

_float4 CCamera_Event::Get_Position_Float4()
{
	return m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
}

_vector CCamera_Event::Get_Position_Vector()
{
	return m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
}

void CCamera_Event::Reset()
{
	m_pCurrentMCAMList = nullptr;
	m_iCurrentMCAMIndex = 0;
	m_isPlay = false;
	m_fTrackPosition = 0.f;
	m_isActive = false;
	m_iCurrentTranslateFrame = 0;
	m_iCurrentRotationFrame = 0;
	m_iCurrentZoomFrame = 0;
	m_isFinished = false;
	m_isAllFinished = false;
	m_pGameInstance->Active_Camera(g_Level,
		(CCamera*)(m_pGameInstance->Get_GameObject(g_Level, g_strCameraTag, 0)));

	m_pGameInstance->Set_Interpolation_Camera(g_Level,
		(CCamera*)(m_pGameInstance->Get_GameObject(g_Level, g_strCameraTag, 0)), m_fFovy);
}

void CCamera_Event::Change_to_Next(_float fTimeDelta)
{
	++m_iCurrentMCAMIndex;
	m_fTrackPosition = fTimeDelta;
	m_iCurrentTranslateFrame = 0;
	m_iCurrentRotationFrame = 0;
	m_iCurrentZoomFrame = 0;
	m_isFinished = false;

#ifdef _DEBUG

	cout << "Change Camara : " << m_iCurrentMCAMIndex << endl;

#endif
}

HRESULT CCamera_Event::Set_PlayCamlist(const wstring& strCamTag)
{
	vector<MCAM>*		pMCAMList = Find_MCAMList(strCamTag);
	if (nullptr == pMCAMList)
		return E_FAIL;

	m_pCurrentMCAMList = pMCAMList;
	m_iCurrentMCAMIndex = 0;
	m_iCurrentTranslateFrame = 0;
	m_iCurrentRotationFrame = 0;
	m_iCurrentZoomFrame = 0;
	m_isFinished = false;
	m_isPlay = true;
	m_fTrackPosition = 0.f;

	Set_Interpolation(XMConvertToRadians(60.0f));

	m_pGameInstance->Active_Camera(g_Level, this);
	return S_OK;
}

HRESULT CCamera_Event::Add_CamList(const wstring& strCamLayerTag, const wstring& strFilePath)
{

	if (nullptr != Find_MCAMList(strCamLayerTag)) {
		return E_FAIL;
	}

	ifstream inputFileStream{ strFilePath, std::ios::binary };
	if (!inputFileStream) {
		return E_FAIL;
	}

	uint32_t magic = {};
	inputFileStream.seekg(4);
	inputFileStream.read(reinterpret_cast<char*>(&magic), sizeof(magic));

	if (magic == 1835098989) {
		MSG_BOX(TEXT("Warning : Read_CamList -> magic == 1835098989"));
		return E_FAIL;
	}

	// 1. read header
	Header header = Read_Header(inputFileStream);

	vector<MCAM> vec;

	for (size_t i = 0; i < header.mcamCount; i++) {
		MCam mcam = {};

		uint64_t position = header.mcamEntry[i];


		mcam.CAMHeader = Read_CamHeader(inputFileStream, position);

		// Read translation frames
		inputFileStream.seekg(mcam.CAMHeader.TranslateHeader.FramesOffset + position, std::ios::beg);
		mcam.TranslationFrame = vector<uint16_t>(mcam.CAMHeader.TranslateHeader.numFrames);
		if (mcam.CAMHeader.TranslateHeader.s2 == 64) {
			inputFileStream.read(reinterpret_cast<char*>(mcam.TranslationFrame.data()), sizeof(uint16_t) * mcam.CAMHeader.TranslateHeader.numFrames);
		}
		else {
			vector<uint8_t> temp(mcam.CAMHeader.TranslateHeader.numFrames);
			inputFileStream.read(reinterpret_cast<char*>(temp.data()), sizeof(uint8_t) * mcam.CAMHeader.TranslateHeader.numFrames);
			for (_uint i = 0; i < mcam.CAMHeader.TranslateHeader.numFrames; ++i) {
				mcam.TranslationFrame[i] = temp[i];
			}
		}
		
		inputFileStream.seekg(mcam.CAMHeader.TranslateHeader.DataOffset + position, std::ios::beg);
		mcam.Translations = vector<_float3>(mcam.CAMHeader.TranslateHeader.numFrames);
		inputFileStream.read(reinterpret_cast<char*>(mcam.Translations.data()), sizeof(_float3) * mcam.CAMHeader.TranslateHeader.numFrames);

		//for (auto& vTraslation : mcam.Translations) {
		//	vTraslation = XMVector3Transform(vTraslation, XMMatrixRotationY(XMConvertToRadians(180)));
		//}

		// Read Rotation frames
		inputFileStream.seekg(mcam.CAMHeader.RotationHeader.FramesOffset + position, std::ios::beg);
		mcam.RotationFrame = vector<uint16_t>(mcam.CAMHeader.RotationHeader.numFrames);
		if (mcam.CAMHeader.RotationHeader.s2 == 64) {
			inputFileStream.read(reinterpret_cast<char*>(mcam.RotationFrame.data()), sizeof(uint16_t) * mcam.CAMHeader.RotationHeader.numFrames);
		}
		else {
			vector<uint8_t> temp(mcam.CAMHeader.RotationHeader.numFrames);
			inputFileStream.read(reinterpret_cast<char*>(temp.data()), sizeof(uint8_t) * mcam.CAMHeader.RotationHeader.numFrames);
			for (_uint i = 0; i < mcam.CAMHeader.RotationHeader.numFrames; ++i) {
				mcam.RotationFrame[i] = temp[i];
			}
		}
		
		
		inputFileStream.seekg(mcam.CAMHeader.RotationHeader.DataOffset + position, std::ios::beg);
		mcam.Rotations = vector<_float4>(mcam.CAMHeader.RotationHeader.numFrames);
		inputFileStream.read(reinterpret_cast<char*>(mcam.Rotations.data()), sizeof(_float4) * mcam.CAMHeader.RotationHeader.numFrames);

		//for (auto& vRotation : mcam.Rotations) {
		//	vRotation = XMLoadFloat4(&vRotation) * XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), XMConvertToRadians(180.0f)) ;
		//}

		// Read Zoom frames
		inputFileStream.seekg(mcam.CAMHeader.ZoomHeader.FramesOffset + position, std::ios::beg);
		mcam.ZoomFrame = vector<uint16_t>(mcam.CAMHeader.ZoomHeader.numFrames);
		if (mcam.CAMHeader.ZoomHeader.s2 == 64) {
			inputFileStream.read(reinterpret_cast<char*>(mcam.ZoomFrame.data()), sizeof(uint16_t) * mcam.CAMHeader.ZoomHeader.numFrames);
		}
		else {
			vector<uint8_t> temp(mcam.CAMHeader.ZoomHeader.numFrames);
			inputFileStream.read(reinterpret_cast<char*>(temp.data()), sizeof(uint8_t) * mcam.CAMHeader.ZoomHeader.numFrames);
			for (_uint i = 0; i < mcam.CAMHeader.ZoomHeader.numFrames; ++i) {
				mcam.ZoomFrame[i] = temp[i];
			}


					}
		
		
		inputFileStream.seekg(mcam.CAMHeader.ZoomHeader.DataOffset + position, std::ios::beg);
		mcam.Zooms = vector<_float3>(mcam.CAMHeader.ZoomHeader.numFrames);
		inputFileStream.read(reinterpret_cast<char*>(mcam.Zooms.data()), sizeof(_float3) * mcam.CAMHeader.ZoomHeader.numFrames);

		vec.emplace_back(mcam);
	}

	m_Camlist.emplace(strCamLayerTag, vec);
	
	return S_OK;
}

Header CCamera_Event::Read_Header(ifstream& inputFileStream)
{
	Header header = {};

	inputFileStream.seekg(0);
	inputFileStream.read(reinterpret_cast<char*>(&header.version), sizeof(header.version));
	inputFileStream.read(reinterpret_cast<char*>(&header.magic), sizeof(header.magic));
	skipToNextLine(inputFileStream);
	inputFileStream.read(reinterpret_cast<char*>(&header.uknLong1), sizeof(header.uknLong1));
	inputFileStream.read(reinterpret_cast<char*>(&header.idxOffset), sizeof(header.idxOffset));
	inputFileStream.read(reinterpret_cast<char*>(&header.uknLong3), sizeof(header.uknLong3));
	inputFileStream.read(reinterpret_cast<char*>(&header.uknLong4), sizeof(header.uknLong4));
	inputFileStream.read(reinterpret_cast<char*>(&header.mcamCount), sizeof(header.mcamCount));
	header.name = readWString(inputFileStream);
	skipToNextLine(inputFileStream);

	header.mcamEntry.resize(header.mcamCount);
	inputFileStream.read(reinterpret_cast<char*>(header.mcamEntry.data()), sizeof(uint64_t) * header.mcamCount);
	skipToNextLine(inputFileStream);

	return header;
}

MCAMHeader CCamera_Event::Read_CamHeader(ifstream& inputFileStream, streampos Position)
{
	MCAMHeader mcam;

	inputFileStream.seekg(Position, std::ios::beg);
	inputFileStream.read(reinterpret_cast<char*>(&mcam.version), sizeof(mcam.version));
	inputFileStream.read(reinterpret_cast<char*>(&mcam.magic), sizeof(mcam.magic));
	skipToNextLine(inputFileStream);
	inputFileStream.read(reinterpret_cast<char*>(&mcam.offsets), sizeof(mcam.offsets));
	inputFileStream.read(reinterpret_cast<char*>(&mcam.uknShort), sizeof(mcam.uknShort));
	inputFileStream.read(reinterpret_cast<char*>(&mcam.frameRate), sizeof(mcam.frameRate));
	inputFileStream.read(reinterpret_cast<char*>(&mcam.frameCount), sizeof(mcam.frameCount));
	inputFileStream.read(reinterpret_cast<char*>(&mcam.blending), sizeof(mcam.blending));
	mcam.name3 = readWString(inputFileStream);
	skipToNextLine(inputFileStream);

	inputFileStream.seekg(mcam.offsets[0] +  Position, std::ios::beg);

	uint64_t trash = {};
	inputFileStream.read(reinterpret_cast<char*>(&trash), sizeof(uint16_t));
	inputFileStream.read(reinterpret_cast<char*>(&trash), sizeof(uint16_t));
	inputFileStream.read(reinterpret_cast<char*>(&trash), sizeof(uint32_t));
	inputFileStream.read(reinterpret_cast<char*>(&trash), sizeof(uint32_t));
	inputFileStream.read(reinterpret_cast<char*>(&trash), sizeof(uint32_t));
	inputFileStream.read(reinterpret_cast<char*>(&trash), sizeof(uint64_t));

	inputFileStream.read(reinterpret_cast<char*>(&mcam.TranslateHeader), sizeof(TrackHeader));
	inputFileStream.read(reinterpret_cast<char*>(&mcam.RotationHeader), sizeof(TrackHeader));


	inputFileStream.seekg(mcam.offsets[1] + Position, std::ios::beg);


	inputFileStream.read(reinterpret_cast<char*>(&trash), sizeof(uint16_t));
	inputFileStream.read(reinterpret_cast<char*>(&trash), sizeof(uint16_t));
	inputFileStream.read(reinterpret_cast<char*>(&trash), sizeof(uint32_t));
	inputFileStream.read(reinterpret_cast<char*>(&trash), sizeof(uint32_t));
	inputFileStream.read(reinterpret_cast<char*>(&trash), sizeof(uint32_t));
	inputFileStream.read(reinterpret_cast<char*>(&trash), sizeof(uint64_t));

	inputFileStream.read(reinterpret_cast<char*>(&mcam.ZoomHeader), sizeof(TrackHeader));
	return mcam;
}

vector<MCAM>* CCamera_Event::Find_MCAMList(const wstring& strCamListTag)
{
	for (auto& pair : m_Camlist) {
		if (pair.first.find(strCamListTag) != wstring::npos) {
			return &pair.second;
		}
	}

	return nullptr;
}

MCAM* CCamera_Event::Find_MCAM(const wstring& strCamListTag, _uint iIndex)
{
	for (auto& pair : m_Camlist) {
		if (pair.first.find(strCamListTag) != wstring::npos) {
			if (iIndex >= pair.second.size())
				return nullptr;

			return &pair.second[iIndex];
		}
	}

	return nullptr;
}

void CCamera_Event::Play_MCAM(_float fTimeDelta)
{
	if (true == m_isFinished)
		return;

	//	cout << m_iCurrentTranslateFrame << endl;
	if (nullptr == m_pCurrentMCAMList)
		return;

	if (m_iCurrentMCAMIndex >= m_pCurrentMCAMList->size())
		return;
	
	MCAM CurrentMCAM = (*m_pCurrentMCAMList)[m_iCurrentMCAMIndex];

	m_fTrackPosition += fTimeDelta * CurrentMCAM.CAMHeader.frameRate;

	if (m_fTrackPosition > CurrentMCAM.CAMHeader.frameCount)
	{
		m_isFinished = true;

		if (m_iCurrentMCAMIndex >= m_pCurrentMCAMList->size() - 1) {

			m_isAllFinished = true;
		}

		return;

		// 이 카메라는 이제 끝이여
	}

	_float3 vTranslation;

	while (1) {
		if(m_iCurrentTranslateFrame >= CurrentMCAM.CAMHeader.TranslateHeader.numFrames -1 ||
			m_fTrackPosition < CurrentMCAM.TranslationFrame[m_iCurrentTranslateFrame + 1])
			break;

		++m_iCurrentTranslateFrame;
	}

	if (m_iCurrentTranslateFrame >= CurrentMCAM.CAMHeader.TranslateHeader.numFrames - 1) {
		vTranslation = CurrentMCAM.Translations[CurrentMCAM.CAMHeader.TranslateHeader.numFrames - 1];
	}
	else {
		vTranslation = XMVectorLerp(CurrentMCAM.Translations[m_iCurrentTranslateFrame],
			CurrentMCAM.Translations[m_iCurrentTranslateFrame + 1],
			(m_fTrackPosition - (_float)CurrentMCAM.TranslationFrame[m_iCurrentTranslateFrame]) / ((_float)CurrentMCAM.TranslationFrame[m_iCurrentTranslateFrame + 1] - (_float)CurrentMCAM.TranslationFrame[m_iCurrentTranslateFrame]));
	}

	while (1) {
		if (m_iCurrentRotationFrame >= CurrentMCAM.CAMHeader.RotationHeader.numFrames - 1 ||
			m_fTrackPosition < CurrentMCAM.RotationFrame[m_iCurrentRotationFrame + 1])
			break;

		++m_iCurrentRotationFrame;
	}

	_float4 vRotation;

	if (m_iCurrentRotationFrame >= CurrentMCAM.CAMHeader.RotationHeader.numFrames - 1) {
		vRotation = CurrentMCAM.Rotations[CurrentMCAM.CAMHeader.RotationHeader.numFrames - 1];
	}
	else {
		vRotation = XMQuaternionSlerp(CurrentMCAM.Rotations[m_iCurrentRotationFrame],
			CurrentMCAM.Rotations[m_iCurrentRotationFrame + 1], 
			(m_fTrackPosition - (_float)CurrentMCAM.RotationFrame[m_iCurrentRotationFrame]) / ((_float)CurrentMCAM.RotationFrame[m_iCurrentRotationFrame + 1] - (_float)CurrentMCAM.RotationFrame[m_iCurrentRotationFrame]));
	}

	_float vZoom;

	while (1) {
		if (m_iCurrentZoomFrame >= CurrentMCAM.CAMHeader.ZoomHeader.numFrames - 1 ||
			m_fTrackPosition < CurrentMCAM.ZoomFrame[m_iCurrentZoomFrame + 1])
			break;

		++m_iCurrentZoomFrame;
	}


	if (m_iCurrentZoomFrame >= CurrentMCAM.CAMHeader.ZoomHeader.numFrames - 1) {
		vZoom = CurrentMCAM.Zooms[CurrentMCAM.CAMHeader.ZoomHeader.numFrames - 1].x;
	}
	else {
		vZoom = XMVectorGetX(XMVectorLerp(CurrentMCAM.Zooms[m_iCurrentZoomFrame],
			CurrentMCAM.Zooms[m_iCurrentZoomFrame + 1], 
			(m_fTrackPosition - (_float)CurrentMCAM.ZoomFrame[m_iCurrentZoomFrame]) / ((_float)CurrentMCAM.ZoomFrame[m_iCurrentZoomFrame + 1] - (_float)CurrentMCAM.ZoomFrame[m_iCurrentZoomFrame])));
	}

	m_fFovy = XMConvertToRadians(60.f) * vZoom;

#pragma region 축회전 해봣는데 안댐
	_vector				vTransaltionWorld = { XMVectorSetW(XMLoadFloat3(&vTranslation), 1.f)};
	vTransaltionWorld.m128_f32[0] = vTransaltionWorld.m128_f32[0] * -1.f;
	//	_vector				vTransaltionWorld = { XMVectorSetW(XMVector3TransformCoord(vTranslation, TransformationMatrix), 1.f) };

	_vector				vQuaternion = { XMLoadFloat4(&vRotation) };

	// 평면의 법선 벡터를 쿼터니언으로 변환
	_vector				vPlaneQuaternion = XMVectorSelect(XMVectorZero(), XMVectorSet(0.f, 1.f, 0.f, 0.f), g_XMSelect1110);
	_vector				vFippedQuaternion = { XMQuaternionMultiply(vQuaternion, vPlaneQuaternion) };
	vPlaneQuaternion = XMVectorSelect(XMVectorZero(), XMVectorSet(0.f, 0.f, 1.f, 0.f), g_XMSelect1110);
	vFippedQuaternion = { XMQuaternionMultiply(vFippedQuaternion, vPlaneQuaternion) };

	vQuaternion = vFippedQuaternion;

	_matrix					WorldMatrix = { XMMatrixAffineTransformation(XMVectorSet(1.f, 1.f, 1.f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f),  vQuaternion, vTransaltionWorld)};

	_matrix					RotationLookMatrix = { XMMatrixRotationAxis(XMVector3Normalize(WorldMatrix.r[CTransform::STATE_LOOK]), XMConvertToRadians(180.f)) };

	_vector					vPosition = { WorldMatrix.r[CTransform::STATE_POSITION] };
	WorldMatrix.r[CTransform::STATE_POSITION] = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	WorldMatrix = WorldMatrix * RotationLookMatrix;
	WorldMatrix.r[CTransform::STATE_POSITION] = vPosition;
	//	_matrix					WorldMatrix = { XMMatrixAffineTransformation(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f),  vQuaternion, vTransaltionWorld)};
	//	WorldMatrix = WorldMatrix* TransformationMatrix;

	m_pTransformCom->Set_WorldMatrix(WorldMatrix);
#pragma endregion
	//_matrix CombinedMatrix = XMMatrixAffineTransformation(XMVectorSet(1.f, 1.f, 1.f, 0.f),
	//	XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 0.f),
	//	vDeltaTranslation); // rotation 죽임 

	//_matrix CombinedMatrix = XMMatrixAffineTransformation(XMVectorSet(1.f, 1.f, 1.f, 0.f),
	//	XMVectorSet(0.f, 0.f, 0.f, 1.f), XMQuaternionConjugate(vDeltaRotation),
	//	XMVectorSet(0.f, 0.f, 0.f, 0.f)); // translation  죽임

	
	//_matrix				TransformationMatrix = { XMMatrixRotationY(XMConvertToRadians(180.f)) };

	//	_vector				vQuaternion = { XMLoadFloat4(&vRotation) };
	//_vector				vTransaltionWorld = { XMVectorSetW(XMVector3TransformNormal(vTranslation, TransformationMatrix), 1.f) };

	//	_matrix				WorldMatrix = { XMMatrixAffineTransformation(XMVectorSet(1.f, 1.f, 1.f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), vQuaternion, vTransaltionWorld) };

	
	//	_matrix				RotationMatrix = { XMMatrixRotationRollPitchYawFromVector(vRotation) };
		
	//WorldMatrix = WorldMatrix * ScaleMatrix;

	//	m_pTransformCom->Set_WorldMatrix(WorldMatrix);

	//	m_fFovy = XMConvertToRadians(120.f);

	//	m_pTransformCom->Set_WorldMatrix( CombinedMatrix * XMMatrixRotationY(XMConvertToRadians(180.f)));

	//	m_pTransformCom->Set_WorldMatrix(ResultMatrix);
}

CCamera_Event* CCamera_Event::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Event* pInstance = new CCamera_Event(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCamera_Free"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_Event::Clone(void* pArg)
{
	CCamera_Event* pInstance = new CCamera_Event(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CCamera_Event"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Event::Free()
{
	__super::Free();
}
