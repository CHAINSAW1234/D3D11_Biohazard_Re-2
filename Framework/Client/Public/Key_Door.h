#pragma once

#include "Client_Defines.h"
#include "Part_InteractProps.h"

BEGIN(Client)

class CKey_Door final : public CPart_InteractProps
{
public :
	typedef struct tag_keyDoor_desc : public CPart_InteractProps::PART_INTERACTPROPS_DESC
	{
		_ubyte eEmblemKey;
		_ubyte* EmblemAnim;
		_float4x4* pParentWorldMatrix;

	}KEY_DOOR;

private:
	CKey_Door(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CKey_Door(const CKey_Door& rhs);
	virtual ~CKey_Door() = default;

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
	HRESULT						Initialize_Model();

public:
	void						Set_Socket(_float4x4* pSocketMatrix) { m_pSocketMatrix = pSocketMatrix; }

private :
	_float4x4*					m_pSocketMatrix		= { nullptr };
	_ubyte*						m_pEmblemAnim;
	_float4x4*					m_pParentWorldMatrix;

	_bool						m_isKeyUsing		= { false };
	_bool						m_isKeyRender		= { false };

public:
	static CKey_Door* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END