#pragma once

#include "Transform.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	typedef struct tagGameObjectDesc: public CTransform::TRANSFORM_DESC
	{		
		//map툴과 파싱재료(prototype과 clone할 때 필요)
		_bool bAnim = { false };
		wstring strModelComponent = { TEXT("") };
		wstring strObjectPrototype = { TEXT("") };
		string strGamePrototypeName = { "" };
		_int iIndex = { 0 };
		_float4x4 worldMatrix;

		vector<_int> BelongIndexs;
		_int iRegionDir = {0};
		//
	}GAMEOBJECT_DESC;

protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	class CComponent*							Get_Component(const wstring& strComTag);

public:
	virtual HRESULT								Initialize_Prototype();
	virtual HRESULT								Initialize(void* pArg);
	virtual void								Priority_Tick(_float fTimeDelta);
	virtual void								Tick(_float fTimeDelta);
	virtual void								Late_Tick(_float fTimeDelta);
	virtual HRESULT								Render();
	virtual HRESULT								Render_LightDepth_Dir() { return S_OK; }
	virtual HRESULT								Render_LightDepth_Spot() { return S_OK; }
	virtual HRESULT								Render_LightDepth_Point() { return S_OK; }
	virtual HRESULT								Render_Distortion() { return S_OK; }
	virtual HRESULT								Render_Emissive() { return S_OK; }
	virtual HRESULT								Render_Font() { return S_OK; }

protected:
	ID3D11Device*								m_pDevice = { nullptr };
	ID3D11DeviceContext*						m_pContext = { nullptr };

	class CGameInstance*						m_pGameInstance = { nullptr };
	CTransform*									m_pTransformCom = { nullptr };

	//Physics_Component
	class CCharacter_Controller*				m_pController = { nullptr };
	class CRigid_Dynamic*						m_pRigid_Body = { nullptr };
	_int										m_iIndex_CCT = { 0 };
	_int										m_iIndex_RigidBody = { 0 };
protected:
	map<const wstring, class CComponent*>		m_Components;

protected:
	HRESULT Add_Component(_uint iLevelIndex, const wstring& strPrototypeTag, const wstring& strComponentTag, class CComponent** ppOut, void* pArg = nullptr);
	HRESULT Change_Component(_uint iLevelIndex, const wstring& strPrototypeTag, const wstring& strComponentTag, CComponent** ppOut, void* pArg = nullptr );

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END