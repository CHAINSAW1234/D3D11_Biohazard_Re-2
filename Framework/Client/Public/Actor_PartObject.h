#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Client)

class CActor_PartObject final : public CPartObject
{
public:
	typedef struct tagActorPartObjectDesc : public PARTOBJECT_DESC
	{
		_float3*				pRootTranslation = { nullptr };
		wstring					strModelPrototypeTag = { TEXT("") };
		list<wstring>			AnimPrototypeLayerTags;
		_bool					isBaseObject = { false };
	}ACTOR_PART_DESC;

private:
	CActor_PartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CActor_PartObject(const CActor_PartObject& rhs);
	virtual ~CActor_PartObject() = default;

public:
	virtual HRESULT			Initialize_Prototype() override { return S_OK; }
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Priority_Tick(_float fTimeDelta) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	void					Set_Animation(_uint iPlayingIndex, const wstring& strAnimLayerTag, _uint iAnimIndex);
	void					Set_Animation_Index(_uint iPlayingIndex, _uint iAnimIndex);
	void					Set_Loop(_uint iPlayingIndex, _bool isLoop);
	inline void				Pause_Animation(_bool isPause) { m_isPause_Anim = isPause; }

private:
	HRESULT					Render_LightDepth_Dir() override;
	HRESULT					Render_LightDepth_Spot() override;
	HRESULT					Render_LightDepth_Point() override;

private:
	_float3*				m_pRootTranslation = {};
	CModel*					m_pModelCom = { nullptr };
	CShader*				m_pShaderCom = { nullptr };
	_bool					m_isBaseObject = { false };
	_bool					m_isPause_Anim = { false };
	wstring					m_strAnimLayerTag = { TEXT("") };

private:
	HRESULT					Add_Components(const wstring& strModelPrototypeTag);
	HRESULT					Add_Animations(const list< wstring>& AnimPrototypeLayerTags);
	HRESULT					Bind_ShaderResources();

public:
	static CActor_PartObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual CGameObject* Clone(void* pArg) override { return nullptr; }
	virtual void Free() override;
};

END