#include "stdafx.h"
#include "AnimationEditor.h"
#include "GameObject.h"

#include "AnimTestObject.h"
#include "AnimTestPartObject.h"

CAnimationEditor::CAnimationEditor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEditor{ pDevice, pContext }
{
}

HRESULT CAnimationEditor::Initialize(void* pArg)
{
	if (FAILED(Add_TestObject()))
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Initialize_AnimList()))
		return E_FAIL;
	
	return S_OK;
}

void CAnimationEditor::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	ImGui::Begin("Animation_Editor");

	if (ImGui::Button("Render_Bone_Tags"))
		m_isRenderBoneTags = !m_isRenderBoneTags;	

	Set_Transform_TransformTool();

	Add_PartObject_TestObject();
	Change_Model_TestObject();

	for (auto& pTool : m_Tools)
	{
		pTool.second->Tick(fTimeDelta);
	}

	Update_Backend_Datas();

	ImGui::End();
}

HRESULT CAnimationEditor::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (true == m_isRenderBoneTags)
		Render_BoneTags();

	return S_OK;
}

HRESULT CAnimationEditor::Add_Components()
{
	return S_OK;
}

HRESULT CAnimationEditor::Add_Tools()
{
	Safe_Release(m_pToolCollider);
	m_pToolCollider = nullptr;
	Safe_Release(m_pToolModelSelector);
	m_pToolModelSelector = nullptr;
	Safe_Release(m_pToolAnimList);
	m_pToolAnimList = nullptr;
	Safe_Release(m_pToolTransformation);
	m_pToolTransformation = nullptr;
	Safe_Release(m_pToolPartObject);
	m_pToolPartObject = nullptr;
	Safe_Release(m_pToolBoneLayer);
	m_pToolBoneLayer = nullptr;

	CTool*		pToolCollider = { nullptr };
	CTool*		pToolAnimList = { nullptr };
	CTool*		pToolModelSelector = { nullptr };
	CTool*		pToolTransformtaion = { nullptr };
	CTool*		pToolAnimPlayer = { nullptr };
	CTool*		pToolPartObject = { nullptr };
	CTool*		pToolBoneLayer = { nullptr };

	CTool_AnimPlayer::ANIMPLAYER_DESC		AnimPlayerDesc{};
	if (nullptr != m_pTestObject)
	{
		AnimPlayerDesc.pMoveDir = m_pTestObject->Get_RootTranslation_Ptr();
		AnimPlayerDesc.pTransform = dynamic_cast<CTransform*>(m_pTestObject->Get_Component(TEXT("Com_Transform")));
	}	
	AnimPlayerDesc.pCurrentBoneLayerTag = &m_strCurrentBoneLayerTag;
	AnimPlayerDesc.pCurrentPartObjectTag = &m_strCurrentPartObjectTag;

	CTool_PartObject::TOOL_PARTOBJECT_DESC	PartObjectDesc{};
	if (nullptr != m_pTestObject)
	{
		PartObjectDesc.pTestObject = m_pTestObject;
	}
	PartObjectDesc.pCurrentPartTag = &m_strCurrentPartObjectTag;

	CTool_AnimList::ANIMLIST_DESC			AnimListDesc{};
	AnimListDesc.pCurrentAnimationTag = &m_strCurrentAnimTag;
	AnimListDesc.pCurrentModelTag = &m_strCurrentModelTag;

	CModel_Selector::MODELSELECTOR_DESC		ModelSelectorDesc{};
	ModelSelectorDesc.pCurrentBoneTag = &m_strCurrentBoneTag;
	ModelSelectorDesc.pCurrentModelTag = &m_strCurrentModelTag;

	CTool_BoneLayer::BONELAYER_DESC			BoneLayerDesc{};
	BoneLayerDesc.pCurrentBoneLayerTag = &m_strCurrentBoneLayerTag;
	BoneLayerDesc.pCurrentModelTag = &m_strCurrentModelTag;
	BoneLayerDesc.pCurrentBoneTag = &m_strCurrentBoneTag;
	

	if (FAILED(__super::Add_Tool(&pToolCollider, static_cast<_uint>(CTool::TOOL_TYPE::COLLIDER), TOOL_COLLIDER_TAG)))
		return E_FAIL;
	if (FAILED(__super::Add_Tool(&pToolAnimList, static_cast<_uint>(CTool::TOOL_TYPE::ANIM_LIST), TOOL_ANIMLIST_TAG, &AnimListDesc)))
		return E_FAIL;
	if (FAILED(__super::Add_Tool(&pToolModelSelector, static_cast<_uint>(CTool::TOOL_TYPE::MODEL_SELECTOR), TOOL_MODELSELECTOR_TAG, &ModelSelectorDesc)))
		return E_FAIL;
	if (FAILED(__super::Add_Tool(&pToolTransformtaion, static_cast<_uint>(CTool::TOOL_TYPE::TRANSFORMATION), TOOL_TRANSFORMATION_TAG)))
		return E_FAIL;	
	if (FAILED(__super::Add_Tool(&pToolAnimPlayer, static_cast<_uint>(CTool::TOOL_TYPE::ANIM_PLAYER), TOOL_ANIMPLAYER_TAG, &AnimPlayerDesc)))
		return E_FAIL;
	if (FAILED(__super::Add_Tool(&pToolPartObject, static_cast<_uint>(CTool::TOOL_TYPE::PART_OBJECT), TOOL_PARTOBJECT_TAG, &PartObjectDesc)))
		return E_FAIL;
	if (FAILED(__super::Add_Tool(&pToolBoneLayer, static_cast<_uint>(CTool::TOOL_TYPE::BONE_LAYER), TOOL_BONELAYER_TAG, &BoneLayerDesc)))
		return E_FAIL;

	CTool_Collider*				pToolColliderConvert = dynamic_cast<CTool_Collider*>(pToolCollider);
	CModel_Selector*			pToolModelSelectorConvert = dynamic_cast<CModel_Selector*>(pToolModelSelector);
	CTool_AnimList*				pToolAnimListConvert = dynamic_cast<CTool_AnimList*>(pToolAnimList);
	CTool_Transformation*		pToolTransformationConvert = dynamic_cast<CTool_Transformation*>(pToolTransformtaion);
	CTool_AnimPlayer*			pToolAnimPlayerConvert = dynamic_cast<CTool_AnimPlayer*>(pToolAnimPlayer);
	CTool_PartObject*			pToolPartObjectConvert = dynamic_cast<CTool_PartObject*>(pToolPartObject);
	CTool_BoneLayer*			pToolBoneLayerConvert = dynamic_cast<CTool_BoneLayer*>(pToolBoneLayer);

	if (nullptr == pToolColliderConvert || nullptr == pToolModelSelectorConvert || 
		nullptr == pToolAnimListConvert || nullptr == pToolTransformationConvert || 
		nullptr == pToolAnimPlayerConvert || nullptr == pToolPartObjectConvert || 
		nullptr == pToolBoneLayerConvert)
	{
		MSG_BOX(TEXT("Tool积己 肋给达"));
		return E_FAIL;
	}

	m_pToolCollider = pToolColliderConvert;
	m_pToolModelSelector = pToolModelSelectorConvert;
	m_pToolAnimList = pToolAnimListConvert;
	m_pToolTransformation = pToolTransformationConvert;
	m_pToolAnimPlayer = pToolAnimPlayerConvert;
	m_pToolPartObject = pToolPartObjectConvert;
	m_pToolBoneLayer = pToolBoneLayerConvert;

	return S_OK;
}

HRESULT CAnimationEditor::Add_TestObject()
{	
	Safe_Release(m_pTestObject);
	m_pTestObject = nullptr;

	if (LEVEL::LEVEL_TOOL == m_pGameInstance->Get_CurrentLevel())
	{
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL, TEXT("Layer_TestObject"), TEXT("Prototype_GameObject_AnimTestObject"))))
			return E_FAIL;

		list<CGameObject*>*		pObjectList = { m_pGameInstance->Find_Layer(LEVEL_TOOL, TEXT("Layer_TestObject")) };
		CAnimTestObject*		pTestObject = { dynamic_cast<CAnimTestObject*>(pObjectList->back()) };
		if (nullptr == pTestObject)
			return E_FAIL;

		m_pTestObject = pTestObject;
		Safe_AddRef(m_pTestObject);
	}	

	return S_OK;
}

void CAnimationEditor::Add_PartObject_TestObject()
{
	static _char			szPartTag[MAX_PATH] = {};
	ImGui::InputText("PartTag : ", szPartTag, static_cast<size_t>(sizeof(szPartTag)));

	if (ImGui::Button("Add_PartObject"))
	{
		_tchar		szTemp[MAX_PATH] = { L"" };
		MultiByteToWideChar(CP_ACP, 0, szPartTag, (_uint)strlen(szPartTag), szTemp, MAX_PATH);

		m_pTestObject->Add_PartObject(szTemp);
	}
}

void CAnimationEditor::Change_Model_TestObject()
{
	if (nullptr == m_pToolModelSelector)
		return;

	CModel*			pModel = { m_pToolModelSelector->Get_CurrentSelectedModel() };
	if (nullptr == pModel)
		return;

	map<wstring, class CAnimTestPartObject*>&		PartObjects = { m_pTestObject->Get_PartObjects() };

	for (auto& Pair : PartObjects)
	{
		const wchar_t*		szPartTag = { Pair.first.c_str() };
		_char				szTemp[MAX_PATH] = { "" };

		WideCharToMultiByte(CP_UTF8, 0, szPartTag, (_uint)lstrlen(szPartTag), szTemp, MAX_PATH, NULL, NULL);
		if (ImGui::Button(string(string("Change_Part_") + string(szTemp)).c_str()))
		{
			m_pTestObject->Chanage_Componenet_PartObject(szPartTag, pModel, CAnimTestPartObject::COMPONENT_TYPE::COM_MODEL);
		}
	}
}

void CAnimationEditor::Show_PartObjectTags()
{

}

void CAnimationEditor::Update_Backend_Datas()
{
	Update_BoneLayerTool();
	Update_AnimPlayerTool();
	Update_PartObjectTool();
}

void CAnimationEditor::Update_BoneLayerTool()
{
	if (nullptr == m_pToolBoneLayer || nullptr == m_pToolModelSelector)
		return;

	CModel*			pModel = { m_pToolModelSelector->Get_CurrentSelectedModel() };
	m_pToolBoneLayer->Set_Current_Model(pModel);
}

void CAnimationEditor::Update_TestObject()
{
	if (nullptr == m_pTestObject)
		return;

	//	m_pTestObject->
}

void CAnimationEditor::Set_Transform_TransformTool()
{
	if (nullptr == m_pToolTransformation)
		return;

	m_pToolTransformation->Set_Target(m_pTestObject);
}

void CAnimationEditor::Update_AnimPlayerTool()
{
	if (nullptr == m_pToolAnimPlayer || nullptr == m_pToolModelSelector || nullptr == m_pToolAnimList)
		return;

	CModel*						pModel = { m_pToolModelSelector->Get_CurrentSelectedModel() };
	m_pToolAnimPlayer->Set_Current_Model(pModel);

	CAnimation*					pAnimation = { m_pToolAnimList->Get_CurrentAnimation() };
	m_pToolAnimPlayer->Set_Current_Animation(pAnimation);
}

void CAnimationEditor::Update_PartObjectTool()
{
	if (nullptr == m_pToolPartObject || nullptr == m_pToolAnimList)
		return;

	CAnimation*					pAnimation = { m_pToolAnimList->Get_CurrentAnimation() };
	m_pToolPartObject->Set_CurrentAnimation(pAnimation);
}

HRESULT CAnimationEditor::Initialize_AnimList()
{
	map<string, CModel*>						Models = { m_pToolModelSelector->Get_Models() };
	map<string, map<string, CAnimation*>>		ModelAnimations;
	for (auto& Pair : Models)
	{
		string							strModelTag = { Pair.first };
		CModel*							pModel = { Pair.second };

		vector<CAnimation*>				vecAnimations = { pModel->Get_Animations() };
		map<string, CAnimation*>		mapAnimations;

		for (auto& pAnimation : vecAnimations)
		{
			string		strAnimationTag = { pAnimation->Get_Name() };

			mapAnimations.emplace(strAnimationTag, pAnimation);
		}		

		ModelAnimations.emplace(Pair.first, mapAnimations);
	}

	m_pToolAnimList->Add_Animations(ModelAnimations);

	return S_OK;
}

void CAnimationEditor::Render_BoneTags()
{
	map<string, _float4x4>			CombinedMatrices = { m_pToolModelSelector->Get_BoneCombinedMatrices() };

	//	_matrix							WorldMatrix = { XMLoadFloat4x4(&m_WorldMatrix) };
	CTransform*						pTransform = { dynamic_cast<CTransform*>(m_pTestObject->Get_Component(TEXT("Com_Transform"))) };
	if (nullptr == pTransform)
		return;

	_matrix							WorldMatrix = { pTransform->Get_WorldMatrix() };
	_matrix							ViewMatrix = { m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW) };
	_matrix							ProjMatrix = { m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ) };
	_matrix							WVPMatrix = { WorldMatrix * ViewMatrix * ProjMatrix };

	for (auto& Pair : CombinedMatrices)
	{
		string			strBoneTag = { Pair.first };
		_matrix			CombinedMatrix = { XMLoadFloat4x4(&Pair.second) };
		_matrix			ProjSpaceCombinedMatrix = { CombinedMatrix * WVPMatrix };

		_vector			vProjSpacePosition = { ProjSpaceCombinedMatrix.r[CTransform::STATE_POSITION] };
		vProjSpacePosition = { vProjSpacePosition / XMVectorGetW(vProjSpacePosition) };

		_vector			vScreenSpacePosition = {
			XMVectorGetX(vProjSpacePosition) * static_cast<_float>(g_iWinSizeX) * 0.5f + static_cast<_float>(g_iWinSizeX * 0.5f),
			(XMVectorGetY(vProjSpacePosition) * static_cast<_float>(g_iWinSizeY) * 0.5f - static_cast<_float>(g_iWinSizeY * 0.5f)) * -1.f,
			0.f, 0.f
		};

		if (fabsf(vScreenSpacePosition.m128_f32[0]) < g_iWinSizeX * 0.5f * 0.1f ||
			fabsf(vScreenSpacePosition.m128_f32[1]) < g_iWinSizeY * 0.5f * 0.1f)
			continue;

		_float2			vScreenSpacePositionFloat2 = {};
		XMStoreFloat2(&vScreenSpacePositionFloat2, vScreenSpacePosition);

		_tchar			szTemp[MAX_PATH] = { L"" };
		MultiByteToWideChar(CP_ACP, 0, strBoneTag.c_str(), (_uint)strlen(strBoneTag.c_str()), szTemp, MAX_PATH);
		wstring			wstrBoneTag = { szTemp };

		m_pGameInstance->Render_Font_Scaled(TEXT("Font_Default"), wstrBoneTag, vScreenSpacePositionFloat2, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, 0.35f);
	}
}

CAnimationEditor* CAnimationEditor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CAnimationEditor*		pInstance = { new CAnimationEditor(pDevice, pContext) };
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Create : CAnimationEditor"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimationEditor::Free()
{
	__super::Free();

	Safe_Release(m_pToolCollider);
	Safe_Release(m_pToolModelSelector);
	Safe_Release(m_pToolAnimList);
	Safe_Release(m_pToolTransformation);
	Safe_Release(m_pToolAnimPlayer);
	Safe_Release(m_pToolPartObject);
	Safe_Release(m_pToolBoneLayer);

	Safe_Release(m_pTestObject);
}
