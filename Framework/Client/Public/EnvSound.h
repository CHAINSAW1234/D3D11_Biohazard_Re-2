#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CEnvSound final : public CGameObject
{
public:
	typedef struct EnvSound_desc :public CGameObject::GAMEOBJECT_DESC
	{
		wstring		wstrSoundName = {TEXT("")};
		_bool			iBelongCol[50] = {false,};
		_float4x4		WorldMatrix = {};
	}ENVSOUND_DESC;
	enum ENV_SOUND_CHENNEL
	{
		ENV_CHENNEL_LOCAL0,
		ENV_CHENNEL_LOCAL1,
		ENV_CHENNEL_LOCAL2,
		ENV_CHENNEL_LOCAL3,
		ENV_CHENNEL_END,
	};

private:
	CEnvSound(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEnvSound(const CEnvSound& rhs);
	virtual ~CEnvSound() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void				Start() override;
	virtual void				Tick(_float fTimeDelta) override;
	virtual void				Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	void							Set_SoundRegion(_int iPlayerRegion)	{ 
		m_iCurRegion = iPlayerRegion;
		if (m_iCurRegion != m_iPreRegion && m_fRegionChangeTimeDelay == 0.f)
			m_fRegionChangeTimeDelay = 2.f;
	}

private:

	_int							m_iCurRegion = { 0 };
	_int							m_iPreRegion = { 0 };
	_float						m_fRegionChangeTimeDelay = { 0.f };
	_float						m_fRegionChangeTimeDelay2 = { 0.f };
	_float						m_fMaxSound = { 0.4f }; // 지역이 바뀔때 이 둘을 보간한다.
	_float						m_fMinSound = { 0.2f }; //
	_float						m_fCurSound = { 0.4f }; //
	_float						m_fSoundCoolTime[ENV_CHENNEL_END] = {};
	_float						m_fSoundTime[ENV_CHENNEL_END] = {};

	CTransform*				m_pEnvSoundTransformCom[ENV_CHENNEL_END] = { nullptr,nullptr,nullptr ,nullptr };
	//map<wstring, vector<ENVSOUND_DESC>>		m_EnvSoundMap; // 사운드 제목, <속한 구역, 위치들> -> 특정 구역에 소리가 난다.
	map<_int, vector<ENVSOUND_DESC>>			m_EnvSoundMap; //구역 / 사운드와 속한 곳, pos
	map<_int, wstring>										m_BGMSoundMap;// 구역, 사운드 명
	class CPlayer*												m_pPlayer = { nullptr };
private:
	HRESULT							Add_Components();
	HRESULT							Bind_ShaderResources();

public:
	static CEnvSound* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END