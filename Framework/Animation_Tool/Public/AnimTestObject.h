#pragma once

#include "Tool_Defines.h"
#include "GameObject.h"

BEGIN(Tool)

class CAnimTestObject final : public CGameObject
{
	friend class CEditor;
	friend class CDebuger;
	friend class CTool;

public:
	enum COMPONENT_TYPE { COM_MODEL, COM_END };

private:
	CAnimTestObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAnimTestObject(const CAnimTestObject& rhs);
	virtual ~CAnimTestObject() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Priority_Tick(_float fTimeDelta) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	void					Priority_Tick_PartObjects(_float fTimeDelta);
	void					Tick_PartObjects(_float fTimeDelta);
	void					Late_Tick_PartObjects(_float fTimeDelta);

public:
	class CAnimTestPartObject* Get_PartObject(const wstring& strPartTag);
	map<wstring, class CAnimTestPartObject*>& Get_PartObjects();

public:
	_float3*				Get_RootTranslation_Ptr() { return &m_vRootTranslation; }

public:
	HRESULT					Chanage_Componenet_PartObject(const wstring& strPartTag, CComponent* pComponent, _uint iType);
	HRESULT					Add_PartObject(const wstring& strPartTag);
	HRESULT					Erase_PartObject(const wstring& strPartTag);
	HRESULT					Link_Bone_PartObject(const wstring& strSrcPartTag, const wstring& strDstPartTag, const string& strSrcBoneTag, const string& strDstBoneTag);		//	SrcÀÇ »À¸¦ Dst »À¿¡ ºÎÂø

public:
	void					Set_RootActivePart(const wstring& strRootActivePartTag);
	const wstring&			Get_RootActivePartTag() { return m_strRootActivePartTag; }

private:
	map<wstring, class CAnimTestPartObject*>			m_PartObjects;
	_float3												m_vRootTranslation = {};
	wstring												m_strRootActivePartTag = { TEXT("") };

private:
	HRESULT					Add_Components();

public:
	static CAnimTestObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END