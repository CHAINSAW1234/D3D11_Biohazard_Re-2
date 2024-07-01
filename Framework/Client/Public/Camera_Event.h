#pragma once

#include "Client_Defines.h"
#include "Camera.h"
#include "Camera_Event_Struct.h"

BEGIN(Client)
class CCamera_Event final : public CCamera
{
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
	void						SetPlayer(CGameObject* pPlayer);
	void						Set_SocketMatrix(_float4x4* pParentMatrix) { m_pSocketMatrix = pParentMatrix; }
	//void						Set_DefaultMatrix(_float4x4 DefaultMatrix) { memcpy(&m_Defaultmatrix, &DefaultMatrix, sizeof(_float4x4)); }
	void						Reset();
	HRESULT						Set_CurrentMCAM(const wstring& strCamTag);

private:
	void						Load_CamPosition();
	
	HRESULT						Read_CamList(const wstring& strFilePath);
	Header						Read_Header(ifstream& inputFileStream);
	MCAMHeader					Read_CamHeader(ifstream& inputFileStream, streampos Position);

	void						Reset_CamPosition();

	MCAM*						Find_MCAM(const wstring& strPrototypeTag);
	void						Play_MCAM(_float fTimeDelta);

private:
	class						CPlayer* m_pPlayer = { nullptr };
	_float4x4*					m_pParentMatrix = { nullptr };
	_float4x4*					m_pSocketMatrix = { nullptr };
	map<wstring, MCAM>			m_Camlist;

	_bool						m_isPlay = { false };
	MCAM*						m_pCurrentMCAM;
	_float						m_fTrackPosition = { 0.f };
	_float						m_fLinearTimeDelta = { 0.f };
	_float4x4					m_PrePlayerMatrix;

	_float4						m_vCameraPosition;
	_float4						m_vCamera_LookAt_Point;
	_float						m_fLook_Dist_Look = { 1.f };
	_float						m_fUp_Dist_Look = { 0.f };
	_float						m_fRight_Dist_Look = { 0.f };
	_float						m_fLook_Dist_Look_Default = { 1.f };
	_float						m_fUp_Dist_Look_Default = { 0.f };
	_float						m_fRight_Dist_Look_Default = { 0.f };
	_float						m_fLook_Dist_Pos = { 0.f };
	_float						m_fUp_Dist_Pos = { 0.f };
	_float						m_fRight_Dist_Pos = { 0.f };


public:
	static CCamera_Event* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END