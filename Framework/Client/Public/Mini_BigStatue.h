#pragma once

#include "Client_Defines.h"
#include "Part_InteractProps.h"



BEGIN(Client)

class CMini_BigStatue final : public CPart_InteractProps
{

public:
	enum class Mini_Anim { Static, Move };
	
	enum class PARTS_TYPE
	{
		MINI_BODY,
		MINI_PARTS,
		MINI_END
	};

	typedef struct tag_MiniStatus_desc : public CPart_InteractProps::PART_INTERACTPROPS_DESC
	{
		_ubyte				eMiniType = {};
		_ubyte				eParts_Type = {};

		_float4x4*			vParts_WorldMatrix = {};
		_bool*				isMedalAnim = { false };
		_bool*				pMove = { nullptr };

	}BODY_MINI_STATUE_DESC;

private:
	CMini_BigStatue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMini_BigStatue(const CMini_BigStatue& rhs);
	virtual ~CMini_BigStatue() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void				Tick(_float fTimeDelta) override;
	virtual void				Late_Tick(_float fTimeDelta) override;

	virtual HRESULT				Render() override;
	virtual HRESULT				Render_LightDepth_Dir()override;
	virtual HRESULT				Render_LightDepth_Point() override;
	virtual HRESULT				Render_LightDepth_Spot()override;

private:
	virtual HRESULT				Add_Components();
	virtual HRESULT				Add_PartObjects() override;
	virtual HRESULT				Initialize_PartObjects() override;

private:
	void						Unicon_Statue(_float fTimeDelta);
	void						Woman_Statue(_float fTimeDelta);
	void						Lion_Statue(_float fTimeDelta);

public:
	_float4x4*					Get_WorldMatrix_Ptr() { return &m_WorldMatrix; }

private :
	/* Init Option */
	_float4x4*					m_ParentWorldMatrix		= {};
	_ubyte						m_ePartsType			= {};

	/* Anim */
	Mini_Anim					m_eAnim					= { Mini_Anim::Static };
	_ubyte						m_eMiniType				= {};
	_bool	*					m_isMove				= { false };
	
	/* Transform */
	_float						m_fAdditionalHeight		= { 0.f };
	_float						m_fAdditionalHeight_D	= { 0.f };
	_float						m_fAdditionalZ			= { 0.f };

	_bool	*					m_isMoveStart			= { nullptr };
	_bool						m_isMoveEnd				= { false };
	/* Render */
	_float						m_fZTimer				= { 0.f };
	_float						m_fRotationAngle		= { 0.f };
	_bool*						m_isMedalAnim			= { };

public:
	static CMini_BigStatue* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END