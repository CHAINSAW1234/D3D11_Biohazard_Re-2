#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class COctree;
END

BEGIN(Client)

class CMap final : public CGameObject
{
public:
	const static _int iMaxNum = 50;
public:
	typedef struct tagProp_desc: public CGameObject::GAMEOBJECT_DESC
	{
		_int BelongIndexs2[iMaxNum];
		_int iPropsType;
	}PROPS_DESC;
private:
	CMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMap(const CMap& rhs);
	virtual ~CMap() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void				Tick(_float fTimeDelta) override;
	virtual void				Late_Tick(_float fTimeDelta) override;
	virtual HRESULT				Render() override;
	virtual HRESULT				Render_Blend() override;
	virtual void				Start() override;

	virtual HRESULT				Render_LightDepth_Dir() override;
	virtual	HRESULT				Render_LightDepth_Spot() override;
	virtual HRESULT				Render_LightDepth_Point() override;

private:
	HRESULT						Initialize_Room_Finder();

private:
	_bool						m_isBlend = { false };


	_bool						m_bOctotree = { false };
	_bool						m_bShadow = { true };
	_float						m_fTimeDelay = { 0.f };
	CModel*						m_pModelCom = { nullptr };
	CShader*					m_pShaderCom = { nullptr };
	class CPlayer*				m_pPlayer = { nullptr };
	_int						m_iPropsType = { 0 };
	_bool						m_bVisible = { false };
	PROPS_DESC 					m_tagPropDesc ={};

	_bool						m_isSetUp_LinkRoom = { false };

	class CRoom_Finder*			m_pRoom_Finder = { nullptr };
	COctree*					m_pOctree = { nullptr };
private:
	HRESULT						Add_Components();
	HRESULT						Bind_ShaderResources();

public:
	static CMap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END