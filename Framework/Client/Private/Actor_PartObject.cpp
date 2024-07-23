#include "stdafx.h"
#include "Actor_PartObject.h"

#include "Light.h"

CActor_PartObject::CActor_PartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CActor_PartObject::CActor_PartObject(const CActor_PartObject& rhs)
	: CPartObject{ rhs }
{
}

HRESULT CActor_PartObject::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	ACTOR_PART_DESC*				pDesc = { static_cast<ACTOR_PART_DESC*>(pArg) };
	m_pRootTranslation = pDesc->pRootTranslation;
	m_isBaseObject = pDesc->isBaseObject;
	if (false == pDesc->AnimPrototypeLayerTags.empty())
		m_strAnimLayerTag = pDesc->AnimPrototypeLayerTags.front();

	else
	{
		m_strAnimLayerTag = TEXT("Default");
	}

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;
		
	if (FAILED(Add_Components(pDesc->strModelPrototypeTag)))
		return E_FAIL;

	if (FAILED(Add_Animations(pDesc->AnimPrototypeLayerTags)))
		return E_FAIL;

	if (FAILED(Sort_Animation_Seq()))
		return E_FAIL;

	m_pModelCom->Set_RootBone("root");
	m_pModelCom->Add_Bone_Layer_All_Bone(TEXT("Default"));
	m_pModelCom->Add_AnimPlayingInfo(true, 0, TEXT("Default"), 1.f);
	m_pModelCom->Set_OptimizationCulling(false);
	m_pModelCom->Set_Loop(0, false);
	//	m_pModelCom->Change_Animation(0, m_strAnimLayerTag, 0);

	XMStoreFloat4x4(&m_LocalTransformation, XMMatrixIdentity());

	m_bRender = false;
	m_isPause_Anim = true;

	return S_OK;
}

void CActor_PartObject::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CActor_PartObject::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CActor_PartObject::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (false == m_isPause_Anim)
	{
		if (true == m_isBaseObject)
		{
			//	m_pModelCom->Play_Animations(m_pParentsTransform, fTimeDelta, m_pRootTranslation);
			if (m_strAnimLayerTag != TEXT(""))
				m_pModelCom->Play_Animation_Light(m_pParentsTransform, fTimeDelta);
		}
		else
		{
			//	_float3				vTempDirection = {};
			//	m_pModelCom->Play_Animations(m_pParentsTransform, fTimeDelta, &vTempDirection);
			if (m_strAnimLayerTag != TEXT(""))
				m_pModelCom->Play_Animation_Light(m_pParentsTransform, fTimeDelta);
		}

	}

	if (true == m_bRender)
	{
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_DIR, this);
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_POINT, this);
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);
	}
}

HRESULT CActor_PartObject::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	list<_uint>			NonHideIndices = { m_pModelCom->Get_NonHideMeshIndices() };
	for (auto& i : NonHideIndices)
	{
		if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(i), aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_NormalTexture", static_cast<_uint>(i), aiTextureType_NORMALS)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i))))
			return E_FAIL;

		//	if (FAILED(m_pModelCom->Bind_PrevBoneMatrices(m_pShaderCom, "g_PrevBoneMatrices", static_cast<_uint>(i))))
		//		return E_FAIL;

		if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_AlphaTexture", static_cast<_uint>(i), aiTextureType_METALNESS)))
		{
			_bool isAlphaTexture = false;
			if (FAILED(m_pShaderCom->Bind_RawValue("g_isAlphaTexture", &isAlphaTexture, sizeof(_bool))))
				return E_FAIL;
		}
		else
		{
			_bool isAlphaTexture = true;
			if (FAILED(m_pShaderCom->Bind_RawValue("g_isAlphaTexture", &isAlphaTexture, sizeof(_bool))))
				return E_FAIL;
		}

		if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_AOTexture", static_cast<_uint>(i), aiTextureType_SHININESS)))
		{
			_bool isAOTexture = false;
			if (FAILED(m_pShaderCom->Bind_RawValue("g_isAOTexture", &isAOTexture, sizeof(_bool))))
				return E_FAIL;
		}
		else
		{
			_bool isAOTexture = true;
			if (FAILED(m_pShaderCom->Bind_RawValue("g_isAOTexture", &isAOTexture, sizeof(_bool))))
				return E_FAIL;
		}

		if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_EmissiveTexture", static_cast<_uint>(i), aiTextureType_EMISSIVE)))
		{
			_bool isEmissive = false;
			if (FAILED(m_pShaderCom->Bind_RawValue("g_isEmissiveTexture", &isEmissive, sizeof(_bool))))
				return E_FAIL;
		}
		else
		{
			_bool isEmissive = true;
			if (FAILED(m_pShaderCom->Bind_RawValue("g_isEmissiveTexture", &isEmissive, sizeof(_bool))))
				return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXANIMMODEL::PASS_DEFAULT)))
			return E_FAIL;

		m_pModelCom->Render(static_cast<_uint>(i));
	}

	return S_OK;
}

HRESULT CActor_PartObject::Set_Animation(_uint iPlayingIndex, const wstring& strAnimLayerTag, _uint iAnimIndex)
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	_uint				iNumAnims = { static_cast<_uint>(m_pModelCom->Get_Animations(strAnimLayerTag).size()) };
	if (iNumAnims <= iAnimIndex)
		return E_FAIL;

	m_pModelCom->Change_Animation(iPlayingIndex, strAnimLayerTag, iAnimIndex);

#ifdef _DEBUG

	list<string>			AnimationTags = { m_pModelCom->Get_Animation_Tags(strAnimLayerTag) };
	list<string>::iterator			iter = { AnimationTags.begin() };
	for (_uint i = 0; i < iAnimIndex; ++i)
	{
		++iter;
	}
	cout << "ChangeAnim : " << *iter << endl;

#endif

	return S_OK;
}

void CActor_PartObject::Set_Animation_Seq(_uint iPlayingIndex, _uint iSeqLevel)
{
	if (nullptr == m_pModelCom)
		return;

	wstring				strAnimLayerTag = { m_pModelCom->Get_CurrentAnimLayerTag(iPlayingIndex) };

	unordered_map<wstring, vector<string>>::iterator			iter = { m_Animations_Seq.find(strAnimLayerTag) };
	if (iter == m_Animations_Seq.end())
		return;

	_uint				iNumSequence = { static_cast<_uint>(iter->second.size()) };
	if (iNumSequence <= iSeqLevel)
		return;

	string				strAnimTag = { iter->second[iSeqLevel] };

#ifdef _DEBUG

	cout << "ChangeAnim : " << strAnimTag << endl;

#endif

	m_pModelCom->Change_Animation(iPlayingIndex, strAnimLayerTag, strAnimTag);
	m_pModelCom->Set_TrackPosition(iPlayingIndex, 0.f, false);
}

void CActor_PartObject::Set_Loop(_uint iPlayingIndex, _bool isLoop)
{
	if (nullptr == m_pModelCom)
		return;

	m_pModelCom->Set_Loop(iPlayingIndex, isLoop);
}

HRESULT CActor_PartObject::Render_LightDepth_Dir()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (m_bRagdoll)
	{
		auto WorldMat = m_pParentsTransform->Get_WorldFloat4x4();
		WorldMat._41 = 0.f;
		WorldMat._42 = 0.f;
		WorldMat._43 = 0.f;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &WorldMat)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
			return E_FAIL;
	}

	if (m_pGameInstance->Get_ShadowLight(CPipeLine::DIRECTION) != nullptr) {

		const CLight* pLight = m_pGameInstance->Get_ShadowLight(CPipeLine::DIRECTION);
		const LIGHT_DESC* pDesc = pLight->Get_LightDesc(0);

		if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &pDesc->ViewMatrix[0])))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &pDesc->ProjMatrix)))
			return E_FAIL;

		list<_uint>			NonHideIndices = { m_pModelCom->Get_NonHideMeshIndices() };
		for (auto& i : NonHideIndices)
		{
			if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(i), aiTextureType_DIFFUSE)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i))))
				return E_FAIL;

			/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이ㅏ터를 다 던져야한다. */
			if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXANIMMODEL::PASS_LIGHTDEPTH)))
				return E_FAIL;

			m_pModelCom->Render(static_cast<_uint>(i));
		}
	}

	return S_OK;
}

HRESULT CActor_PartObject::Render_LightDepth_Spot()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (m_bRagdoll)
	{
		auto WorldMat = m_pParentsTransform->Get_WorldFloat4x4();
		WorldMat._41 = 0.f;
		WorldMat._42 = 0.f;
		WorldMat._43 = 0.f;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &WorldMat)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
			return E_FAIL;
	}

	if (m_pGameInstance->Get_ShadowLight(CPipeLine::SPOT) != nullptr) {

		const CLight* pLight = m_pGameInstance->Get_ShadowLight(CPipeLine::SPOT);
		const LIGHT_DESC* pDesc = pLight->Get_LightDesc(0);

		if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &pDesc->ViewMatrix[0])))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &pDesc->ProjMatrix)))
			return E_FAIL;

		list<_uint>			NonHideIndices = { m_pModelCom->Get_NonHideMeshIndices() };
		for (auto& i : NonHideIndices)
		{
			if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(i), aiTextureType_DIFFUSE)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXANIMMODEL::PASS_LIGHTDEPTH)))
				return E_FAIL;

			m_pModelCom->Render(static_cast<_uint>(i));
		}
	}

	Update_WorldMatrix();

	return S_OK;
}

HRESULT CActor_PartObject::Render_LightDepth_Point()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (m_bRagdoll)
	{
		auto WorldMat = m_pParentsTransform->Get_WorldFloat4x4();
		WorldMat._41 = 0.f;
		WorldMat._42 = 0.f;
		WorldMat._43 = 0.f;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &WorldMat)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
			return E_FAIL;
	}

	list<LIGHT_DESC*> LightDescList = m_pGameInstance->Get_ShadowPointLightDesc_List();
	_int iIndex = 0;
	for (auto& pLightDesc : LightDescList) {
		const _float4x4* pLightViewMatrices;
		_float4x4 LightProjMatrix;
		pLightViewMatrices = pLightDesc->ViewMatrix;
		LightProjMatrix = pLightDesc->ProjMatrix;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_LightIndex", &iIndex, sizeof(_int))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_Matrices("g_LightViewMatrix", pLightViewMatrices, 6)))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_LightProjMatrix", &LightProjMatrix)))
			return E_FAIL;

		list<_uint>			NonHideIndices = { m_pModelCom->Get_NonHideMeshIndices() };
		for (auto& i : NonHideIndices)
		{
			if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(i), aiTextureType_DIFFUSE)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXANIMMODEL::PASS_LIGHTDEPTH_CUBE)))
				return E_FAIL;

			m_pModelCom->Render(static_cast<_uint>(i));
		}

		++iIndex;
	}

	return S_OK;
}

HRESULT CActor_PartObject::Add_Components(const wstring& strModelPrototypeTag)
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(g_Level, strModelPrototypeTag,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CActor_PartObject::Add_Animations(const list<wstring>& AnimPrototypeLayerTags)
{
	for (auto& strAnimPrototypeLayerTag : AnimPrototypeLayerTags)
	{
		if (FAILED(m_pModelCom->Add_Animations(strAnimPrototypeLayerTag, strAnimPrototypeLayerTag)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CActor_PartObject::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CActor_PartObject::Sort_Animation_Seq()
{
	list<wstring>				AnimLayerTags = { m_pModelCom->Get_AnimationLayer_Tags() };
	for (auto& strAnimLayerTag : AnimLayerTags)
	{
		if (strAnimLayerTag == TEXT("Default") || strAnimLayerTag == TEXT(""))
			continue;

		list<string>			AnimationTags = { m_pModelCom->Get_Animation_Tags(strAnimLayerTag) };
		_uint					iLevel = { 0 };
		vector<string>			SeqAnimTags;
		for (auto& strAnimTag : AnimationTags)
		{
			if (strAnimTag == "")
			{
				SeqAnimTags.push_back("");
				++iLevel;
				continue; 
			}
			_uint				iPos = { static_cast<_uint>(strAnimTag.rfind(L'_')) };
			_uint				iSeqLevel = { static_cast<_uint>(stoi(strAnimTag.substr(iPos + 1))) };

			while (iSeqLevel != iLevel)
			{
				SeqAnimTags.push_back("");
				++iLevel;
			}

			SeqAnimTags.push_back(strAnimTag);
			++iLevel;
		}

		m_Animations_Seq.emplace(strAnimLayerTag, SeqAnimTags);
	}

	return S_OK;
}

CActor_PartObject* CActor_PartObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CActor_PartObject* pInstance = { new CActor_PartObject(pDevice, pContext) };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CActor_PartObject"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CActor_PartObject::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
