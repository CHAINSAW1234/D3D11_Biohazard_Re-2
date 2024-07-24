#pragma once

#include "Client_Defines.h"
#include "Camera.h"
#include "Camera_Event_Struct.h"

BEGIN(Client)
class CCamera_Event final : public CCamera
{
	friend class CCut_Scene;

private:
	CCamera_Event(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Event(const CCamera_Event& rhs);
	virtual ~CCamera_Event() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void				Tick(_float fTimeDelta) override;
	virtual void				Late_Tick(_float fTimeDelta) override;
	virtual HRESULT				Render() override;
	virtual void				Active_Camera(_bool isActive) override;

	_float4						Get_Position_Float4();
	_vector						Get_Position_Vector();
	HRESULT						Set_PlayCamlist(const wstring& strCamTag);
	HRESULT						Add_CamList(const wstring& strCamLayerTag, const wstring& strFilePath);

public:
	void						Play_MCAM(_float fTimeDelta);

private:
	Header						Read_Header(ifstream& inputFileStream);
	MCAMHeader					Read_CamHeader(ifstream& inputFileStream, streampos Position);

	vector<MCAM>*				Find_MCAMList(const wstring& strCamListTag);
	MCAM*						Find_MCAM(const wstring& strCamListTag, _uint iIndex);

	void						Reset();
	void						Change_to_Next(_float fTimeDelta = 0.f);

public:
	inline _bool				Is_Finsihed() { return m_isFinished; }
	inline _bool				Is_All_Finsihed() { return m_isAllFinished; }

private:
	map<wstring, vector<MCAM>>	m_Camlist;

	_bool						m_isPlay = { false };
	vector<MCAM>*				m_pCurrentMCAMList;
	_uint						m_iCurrentMCAMIndex;

	_float						m_fTrackPosition = { 0.f };

	_uint						m_iCurrentTranslateFrame = { 0 };
	_uint						m_iCurrentRotationFrame = { 0 };
	_uint						m_iCurrentZoomFrame = { 0 };

	_float						m_fLinearTimeDelta = { 0.f };
	_float4x4					m_PrePlayerMatrix;

	_bool						m_isFinished = { false };
	_bool						m_isAllFinished = { false };


public:
	static CCamera_Event* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END