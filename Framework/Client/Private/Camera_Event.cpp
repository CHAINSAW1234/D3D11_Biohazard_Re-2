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

	if (FAILED(Read_CamList(TEXT("../Bin/DataFiles/mcamlist/em0000_maincam00.mcamlist.13"))))
		return E_FAIL;

	Load_CamPosition();

	m_Defaultmatrix = XMMatrixIdentity();

	return S_OK;
}

void CCamera_Event::Tick(_float fTimeDelta)
{
	Reset_CamPosition();


	if (m_isPlay) {
		Play_MCAM(fTimeDelta);
	}
	else {
		_float3 vRight = m_Defaultmatrix.Right() * m_fRight_Dist_Pos;
		_float3 vUp = m_Defaultmatrix.Up() * (m_fUp_Dist_Pos + CONTROLLER_GROUND_GAP);
		_float3 vLook = -m_Defaultmatrix.Forward() * m_fLook_Dist_Pos;

		_matrix Default = m_Defaultmatrix;
			Default.r[3] = XMVectorSetW(m_Defaultmatrix.Translation() + vRight + vUp + vLook, 1.f);

		m_pTransformCom->Set_WorldMatrix(Default);
		m_pTransformCom->Look_At(XMVectorSetW(m_Defaultmatrix.Translation() + _float3(0.f,CONTROLLER_GROUND_GAP, 0.f), 1.f));
	}
	__super::Bind_PipeLines();
}

void CCamera_Event::Late_Tick(_float fTimeDelta)
{
}

HRESULT CCamera_Event::Render()
{
	return S_OK;
}

void CCamera_Event::Active_Camera(_bool isActive)
{
	__super::Active_Camera(isActive);
	
	if (!m_isActive)
		Reset();
}

_float4 CCamera_Event::Get_Position_Float4()
{
	return m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
}

_vector CCamera_Event::Get_Position_Vector()
{
	return m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
}

void CCamera_Event::SetPlayer(CGameObject* pPlayer)
{
	auto Player = dynamic_cast<CPlayer*>(pPlayer);
	if (Player)
		m_pPlayer = Player;

	m_pParentMatrix = m_pPlayer->Get_Transform()->Get_WorldFloat4x4_Ptr();
}

void CCamera_Event::Reset()
{
	m_pCurrentMCAM = nullptr;
	m_isPlay = false;
	m_fTrackPosition = 0.f;
}

HRESULT CCamera_Event::Set_CurrentMCAM(const wstring& strCamTag)
{
	MCAM* pMCAM = Find_MCAM(strCamTag);
	if (nullptr == pMCAM)
		return E_FAIL;

	m_pCurrentMCAM = pMCAM;
	m_isPlay = true;
	m_fTrackPosition = 0.f;
	//m_isActive = true;	--> Player가 해줌

	return S_OK;
}

void CCamera_Event::Load_CamPosition()
{
	string filePath = "../Camera_Position/Camera_Position";

	//File Import
	ifstream File(filePath, std::ios::binary | std::ios::in);

	File.read((char*)&m_vCameraPosition, sizeof(_float4));
	File.read((char*)&m_fRight_Dist_Look, sizeof(_float));
	File.read((char*)&m_fUp_Dist_Look, sizeof(_float));
	File.read((char*)&m_fLook_Dist_Look, sizeof(_float));

	m_fRight_Dist_Look_Default = m_fRight_Dist_Look;
	m_fUp_Dist_Look_Default = m_fUp_Dist_Look;
	m_fLook_Dist_Look_Default = m_fLook_Dist_Look;

	m_fUp_Dist_Look -= CONTROLLER_GROUND_GAP;

	m_fLook_Dist_Pos = m_vCameraPosition.z;
	m_fRight_Dist_Pos = m_vCameraPosition.x;
	m_fUp_Dist_Pos = m_vCameraPosition.y - CONTROLLER_GROUND_GAP;

	File.close();
}

HRESULT CCamera_Event::Read_CamList(const wstring& strFilePath)
{
	ifstream inputFileStream{ strFilePath, std::ios::binary };
	if (!inputFileStream) {
		return E_FAIL;
	}

	uint32_t magic;
	inputFileStream.seekg(4);
	inputFileStream.read(reinterpret_cast<char*>(&magic), sizeof(magic));

	if (magic == 1835098989) {
		MSG_BOX(TEXT("Warning : Read_CamList -> magic == 1835098989"));
		return E_FAIL;
	}
	
	// 1. read header
	Header header = Read_Header(inputFileStream);

	for (size_t i = 0; i < header.mcamCount; i++) {
		MCam mcam = {};

		uint64_t position = header.mcamEntry[i];

		mcam.CAMHeader = Read_CamHeader(inputFileStream, position);

		// Read translation frames
		inputFileStream.seekg(mcam.CAMHeader.TranslateHeader.DataOffset + position, std::ios::beg);
		mcam.Translations = vector<_float3>(mcam.CAMHeader.TranslateHeader.numFrames);
		inputFileStream.read(reinterpret_cast<char*>(mcam.Translations.data()), sizeof(_float3) * mcam.CAMHeader.TranslateHeader.numFrames);

		// Read Rotation frames
		inputFileStream.seekg(mcam.CAMHeader.RotationHeader.DataOffset + position, std::ios::beg);
		mcam.Rotations = vector<_float4>(mcam.CAMHeader.RotationHeader.numFrames);
		inputFileStream.read(reinterpret_cast<char*>(mcam.Rotations.data()), sizeof(_float4) * mcam.CAMHeader.RotationHeader.numFrames);

		// Read Zoom frames
		inputFileStream.seekg(mcam.CAMHeader.ZoomHeader.DataOffset + position, std::ios::beg);
		mcam.Zooms = vector<_float3>(mcam.CAMHeader.ZoomHeader.numFrames);
		inputFileStream.read(reinterpret_cast<char*>(mcam.Zooms.data()), sizeof(_float3) * mcam.CAMHeader.ZoomHeader.numFrames);

		if (nullptr== Find_MCAM(mcam.CAMHeader.name3)) {
			m_Camlist.emplace(mcam.CAMHeader.name3, mcam);
		}

	}

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

	uint64_t trash;
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

void CCamera_Event::Reset_CamPosition()
{
	m_Defaultmatrix = m_pPlayer->Get_Transform()->Get_WorldMatrix_Pure();

	m_fRight_Dist_Look = m_fRight_Dist_Look_Default;
	m_fUp_Dist_Look = m_fUp_Dist_Look_Default;
	m_fLook_Dist_Look = m_fLook_Dist_Look_Default;
}

MCAM* CCamera_Event::Find_MCAM(const wstring& strCamTag)
{
	for (auto& pair : m_Camlist) {
		if (pair.first.find(strCamTag) != wstring::npos) {
			return &pair.second;
		}
	}

	return nullptr;
}

void CCamera_Event::Play_MCAM(_float fTimeDelta)
{
	if (nullptr == m_pCurrentMCAM)
		return;

	m_fTrackPosition += fTimeDelta * m_pCurrentMCAM->CAMHeader.frameRate;
	
	if (m_fTrackPosition > m_pCurrentMCAM->CAMHeader.frameCount) {
		m_fTrackPosition = m_pCurrentMCAM->CAMHeader.frameCount;
		m_pPlayer->Swap_Camera();
		Reset();
		return;
		// 이 카메라는 이제 끝이여
	}

	_float fTranslationFrame = m_fTrackPosition;
	_float3 vTranslation; 

	if (fTranslationFrame > m_pCurrentMCAM->CAMHeader.TranslateHeader.numFrames -1) {
		vTranslation = m_pCurrentMCAM->Translations[m_pCurrentMCAM->CAMHeader.TranslateHeader.numFrames - 1];
	}
	else {
		vTranslation = XMVectorLerp(m_pCurrentMCAM->Translations[int(floor(fTranslationFrame))],
			m_pCurrentMCAM->Translations[int(floor(fTranslationFrame)) + 1], fTranslationFrame - floor(fTranslationFrame));
	}

	_float fRotationFrame = m_fTrackPosition;
	_float4 vRotation;

	if (fRotationFrame > m_pCurrentMCAM->CAMHeader.RotationHeader.numFrames - 1) {
		vRotation = m_pCurrentMCAM->Rotations[m_pCurrentMCAM->CAMHeader.RotationHeader.numFrames - 1];
	}
	else {
		vRotation = XMQuaternionSlerp(m_pCurrentMCAM->Rotations[int(floor(fRotationFrame))],
			m_pCurrentMCAM->Rotations[int(floor(fRotationFrame)) + 1], fRotationFrame - floor(fRotationFrame));
	}

	_float fZoomFrame = m_fTrackPosition;
	_float vZoom;

	if (fZoomFrame > m_pCurrentMCAM->CAMHeader.ZoomHeader.numFrames - 1) {
		vZoom = m_pCurrentMCAM->Zooms[m_pCurrentMCAM->CAMHeader.ZoomHeader.numFrames - 1].x;
	}
	else {
		vZoom = XMVectorGetX(XMVectorLerp(m_pCurrentMCAM->Zooms[int(floor(fTranslationFrame))],
			m_pCurrentMCAM->Zooms[int(floor(fTranslationFrame)) + 1], fTranslationFrame - floor(fTranslationFrame)));
	}

	_matrix CombinedMatrix = XMMatrixAffineTransformation(XMVectorSet(1.f, 1.f, 1.f, 0.f),
		XMVectorSet(0.f, 0.f, 0.f, 1.f), XMQuaternionConjugate(vRotation), vTranslation);

	_float3 vRight = m_Defaultmatrix.Right() * 1.5;
	_float3 vUp = m_Defaultmatrix.Up() * 1.7;
	_float3 vLook = m_Defaultmatrix.Forward() * 0.2;

	_matrix Default = m_Defaultmatrix;
	Default.r[3] = XMVectorSetW(m_Defaultmatrix.Translation()  + vRight + vUp  -  vLook, 1.f);

	//m_pTransformCom->Set_WorldMatrix(Default);
	//m_pTransformCom->Look_At(XMVectorSetW(m_Defaultmatrix.Translation() + _float3(0.f, CONTROLLER_GROUND_GAP, 0.f), 1.f));
	//
	//Default = m_pTransformCom->Get_WorldFloat4x4();


	
	_matrix finalMatrix;

	finalMatrix = XMMatrixRotationY(XMConvertToRadians(180)) * CombinedMatrix
		* Default;
//		* m_pPlayer->Get_Transform()->Get_WorldMatrix()
//		* XMMatrixTranslationFromVector(m_pPlayer->Get_Transform()->Get_State_Vector(CTransform::STATE_LOOK) * -100.f);

	m_pTransformCom->Set_WorldMatrix(finalMatrix);
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
