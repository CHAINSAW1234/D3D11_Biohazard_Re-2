#pragma once

#include "Client_Defines.h"
#include "Part_InteractProps.h"

BEGIN(Client)

class CMedal_BigStatue final : public CPart_InteractProps
{
public :
	enum MEDAL_TYPE
	{
		MEDAL_UNICORN,
		MEDAL_VIRGIN01,
		MEDAL_VIRGIN02,
		MEDAL_END
	};

	typedef struct tag_MedalBigStatue_desc : public CPart_InteractProps::PART_INTERACTPROPS_DESC
	{
		_ubyte		eMedelType = { MEDAL_TYPE::MEDAL_END };
		_bool*		eMedalRenderType;
		_float4x4* pParentWorldMatrix = { nullptr };

	}MEDAL_BIGSTATUE_DESC;

private:
	CMedal_BigStatue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMedal_BigStatue(const CMedal_BigStatue& rhs);
	virtual ~CMedal_BigStatue() = default;

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
	virtual void				Get_SpecialBone_Rotation() override;
	HRESULT						Initialize_Model();

public :
	void						Set_Render_MedalType(_ubyte _eMedalType) { }

private:
	_ubyte						m_eMedelType			= { MEDAL_TYPE::MEDAL_END };
	_bool*						m_eRender_MedalType		= { nullptr };
	_float4x4*					m_pParentWorldMatrix = { nullptr };
	_bool							m_isTransformControl = { false };
public:
	static CMedal_BigStatue* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END