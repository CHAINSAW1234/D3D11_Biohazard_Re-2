#pragma once

#include "Client_Defines.h"
#include "InteractProps.h"

BEGIN(Client)

class CNewpoliceStatue final : public CInteractProps
{
public:
	enum POLICEHALLSTATUE
	{
		POLICEHALLSTATUE_0,
		POLICEHALLSTATUE_1,
		POLICEHALLSTATUE_2,
		POLICEHALLSTATUE_3,
		POLICEHALLSTATUE_END
	};

	enum POLICEHALLSTATUE_PART
	{
		PART_BODY,
		PART_PART1,
		PART_PART2,
		PART_PART3,
		PART_END
	};

private:
	CNewpoliceStatue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNewpoliceStatue(const CNewpoliceStatue& rhs);
	virtual ~CNewpoliceStatue() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void				Tick(_float fTimeDelta) override;
	virtual void				Late_Tick(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

private:

	virtual HRESULT				Add_Components();
	virtual HRESULT				Add_PartObjects() override;
	virtual HRESULT				Initialize_PartObjects() override;
	virtual HRESULT				Bind_ShaderResources() override;
	virtual void					Do_Interact_Props() override;
public:
	virtual _float4 Get_Object_Pos() override;

private :
	void				Animation_BaseOn_MedalType();
	void				Active();
private:
	_bool				m_eMedalRender[3]	= { false };
	_bool				m_isGiveMedal		= { false };
	_float			m_fZoomOut = { 0.f };
	_uint				m_iEXCode = { 0 };
	_uint				m_iPrevMedalCnt = { 0 }; 

	_ubyte				m_eState			= { POLICEHALLSTATUE_0 };
	_ubyte				m_eMedalType;

	_bool				m_bCamera			= { false };

public:
	static CNewpoliceStatue* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END