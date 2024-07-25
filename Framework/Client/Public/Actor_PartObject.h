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
	HRESULT					Set_Animation(_uint iPlayingIndex, const wstring& strAnimLayerTag, _uint iAnimIndex);
	void					Set_Animation_Seq(_uint iPlayingIndex, _uint iSequenceLevel);
	void					Set_Loop(_uint iPlayingIndex, _bool isLoop);

	_bool					Is_LinkAuto() { return m_isLink_Auto; }
	_bool					Is_Finish_Seq(_uint iSequence);
	void					Set_LinkAuto(_bool isLinkAuto) { m_isLink_Auto = isLinkAuto; }
	void					Set_Pause_Anim(_bool isPause) { m_isPause_Anim = isPause; }
	void					Set_Local_Transformation(_fmatrix TransformationMatrix) { XMStoreFloat4x4(&m_LocalTransformation, TransformationMatrix); }

	void					Play_Pose_FirstTick();
	void					Play_Animation_Light(_float fTimeDelta) { m_pModelCom->Play_Animation_Light(m_pTransformCom, fTimeDelta); }

	void					Set_AdditionalRotation_RootBone(_bool isSetAdditionalRotation, _fmatrix RotationMatrix);
	void					Set_Animation_Light(_bool isLight) { m_isAnimLight = isLight; }

	_bool					Is_Active_Sequence(_uint iSeqLevel);

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
	unordered_map<wstring, vector<string>>				m_Animations_Seq;	

	_bool					m_isLink_Auto = { true };
	_bool					m_isAnimLight = { true };
	_bool					m_isSetAdditionalRotation = { false };
	_float4x4				m_AdditionalRotation_Root = {};

	_float4x4				m_LocalTransformation = {};

private:
	HRESULT					Add_Components(const wstring& strModelPrototypeTag);
	HRESULT					Add_Animations(const list< wstring>& AnimPrototypeLayerTags);
	HRESULT					Bind_ShaderResources();
	HRESULT					Sort_Animation_Seq();

public:
	static CActor_PartObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual CGameObject* Clone(void* pArg) override { return nullptr; }
	virtual void Free() override;
};

END