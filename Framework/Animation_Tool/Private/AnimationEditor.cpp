#include "stdafx.h"
#include "AnimationEditor.h"
#include "GameObject.h"

CAnimationEditor::CAnimationEditor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEditor{ pDevice, pContext }
{
}

HRESULT CAnimationEditor::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CAnimationEditor::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	ImGui::Begin("Animation_Editor");

	if (ImGui::Button("Render_Bone_Tags"))
		m_isRenderBoneTags = !m_isRenderBoneTags;


	if (ImGui::RadioButton("Active_Root_XZ", m_isActiveRoot_XZ))
		m_isActiveRoot_XZ = !m_isActiveRoot_XZ;
	ImGui::SameLine();
	if (ImGui::RadioButton("Active_Root_Y", m_isActiveRoot_Y))
		m_isActiveRoot_Y = !m_isActiveRoot_Y;
	ImGui::SameLine();
	if (ImGui::RadioButton("Active_Root_Rotate", m_isActiveRoot_Rotate))
		m_isActiveRoot_Rotate = !m_isActiveRoot_Rotate;


	ImGui::End();
}

HRESULT CAnimationEditor::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if(true == m_isRenderBoneTags)
		Render_BoneTags();

	return S_OK;
}

HRESULT CAnimationEditor::Add_Components()
{
	return S_OK;
}

HRESULT CAnimationEditor::Add_Tools()
{
	CTool*		pToolCollider = { nullptr };
	CTool*		pToolAnimList = { nullptr };
	CTool*		pToolModelSelector = { nullptr };
	CTool*		pToolTransformtaion = { nullptr };

	if (FAILED(__super::Add_Tool(&pToolCollider, static_cast<_uint>(CTool::TOOL_TYPE::COLLIDER), TOOL_COLLIDER_TAG)))
		return E_FAIL;
	if (FAILED(__super::Add_Tool(&pToolAnimList, static_cast<_uint>(CTool::TOOL_TYPE::ANIM_LIST), TOOL_ANIMLIST_TAG)))
		return E_FAIL;
	if (FAILED(__super::Add_Tool(&pToolModelSelector, static_cast<_uint>(CTool::TOOL_TYPE::MODEL_SELECTOR), TOOL_MODELSELECTOR_TAG)))
		return E_FAIL;
	if (FAILED(__super::Add_Tool(&pToolTransformtaion, static_cast<_uint>(CTool::TOOL_TYPE::TRANSFORMATION), TOOL_TRANSFORMATION_TAG)))
		return E_FAIL;	

	m_pToolCollider = dynamic_cast<CTool_Collider*>(pToolCollider);
	m_pToolModelSelector = dynamic_cast<CModel_Selector*>(pToolCollider);
	m_pToolAnimList = dynamic_cast<CTool_AnimList*>(pToolCollider);
	m_pToolTransformation = dynamic_cast<CTool_Transformation*>(pToolCollider);

	return S_OK;
}

void CAnimationEditor::Set_Context(CGameObject* pGameObject)
{
	if (nullptr != m_pContext)
		Safe_Release(m_pContext);
	m_pContext = nullptr;

	m_pContext = pGameObject;
	Safe_AddRef(m_pContext);
}

void CAnimationEditor::Render_BoneTags()
{
	map<string, _float4x4>			CombinedMatrices = { m_pToolModelSelector->Get_BoneCombinedMatrices() };

	//	_matrix							WorldMatrix = { XMLoadFloat4x4(&m_WorldMatrix) };
	_matrix							WorldMatrix = { XMMatrixIdentity() };
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

		_float2			vScreenSpacePositionFloat2 = {};
		XMStoreFloat2(&vScreenSpacePositionFloat2, vScreenSpacePosition);

		_tchar			szTemp[MAX_PATH] = { L"" };
		MultiByteToWideChar(CP_ACP, 0, strBoneTag.c_str(), (_uint)strlen(strBoneTag.c_str()), szTemp, MAX_PATH);
		wstring			wstrBoneTag = { szTemp };
		m_pGameInstance->Render_Font(TEXT("Font_Default"), wstrBoneTag, vScreenSpacePositionFloat2, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f);
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

	Safe_Release(m_pContext);

	Safe_Release(m_pToolCollider);
	Safe_Release(m_pToolModelSelector);
	Safe_Release(m_pToolAnimList);
	Safe_Release(m_pToolTransformation);
}
