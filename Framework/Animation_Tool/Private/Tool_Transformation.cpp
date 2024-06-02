#include "stdafx.h"
#include "Tool_Transformation.h"

#include "ImGuizmo.h"

CTool_Transformation::CTool_Transformation()
{
}

HRESULT CTool_Transformation::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

void CTool_Transformation::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	static _bool isChanged = { false };

	Update_WorldMatrix();
	Update_Transform();
}

void CTool_Transformation::Set_Target(CTransform* pTransform)
{
	if (nullptr == pTransform)
	{
		ReSet_Target();
		return;
	}

	if (nullptr != m_pTargetTransform)
	{
		Safe_Release(m_pTargetTransform);
	}

	m_pTargetTransform = pTransform;

	Safe_AddRef(m_pTargetTransform);
}

void CTool_Transformation::Set_Target(CGameObject* pGameObject)
{
	m_p
}

void CTool_Transformation::ReSet_Target()
{
	Safe_Release(m_pTargetTransform);
	m_pTargetTransform = nullptr;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
}

void CTool_Transformation::Update_Transform()
{
	if (nullptr == m_pTargetTransform)
		return;

	_float4x4		WorldFloat4x4 = { m_pTargetTransform->Get_WorldFloat4x4() };

	ImGuizmo::BeginFrame();

	static ImGuizmo::OPERATION CurrentGizmoOperation(ImGuizmo::ROTATE);
	static ImGuizmo::MODE CurrentGizmoMode(ImGuizmo::WORLD);

	/*if (m_pGameInstance->Get_KeyState('V') == DOWN)
		CurrentGizmoOperation = ImGuizmo::SCALE;

	if (m_pGameInstance->Get_KeyState('R') == DOWN)
		CurrentGizmoOperation = ImGuizmo::ROTATE;

	if (m_pGameInstance->Get_KeyState('T') == DOWN)
		CurrentGizmoOperation = ImGuizmo::TRANSLATE;*/


	if (ImGui::RadioButton("Scale", CurrentGizmoOperation == ImGuizmo::SCALE))
		CurrentGizmoOperation = ImGuizmo::SCALE;

	ImGui::SameLine();

	if (ImGui::RadioButton("Rotation", CurrentGizmoOperation == ImGuizmo::ROTATE))
		CurrentGizmoOperation = ImGuizmo::ROTATE;

	ImGui::SameLine();

	if (ImGui::RadioButton(u8"Translation", CurrentGizmoOperation == ImGuizmo::TRANSLATE))
		CurrentGizmoOperation = ImGuizmo::TRANSLATE;

	float matrixTranslation[3], matrixRotation[3], matrixScale[3];

	_bool			isChanged = { false };

	ImGuizmo::DecomposeMatrixToComponents((_float*)&WorldFloat4x4, matrixTranslation, matrixRotation, matrixScale);

	if (ImGui::InputFloat3("Scale", matrixScale))
		isChanged = true;

	if (ImGui::InputFloat3("Rotation", matrixRotation))
		isChanged = true;

	if (ImGui::InputFloat3("Translation", matrixTranslation))
		isChanged = true;

	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, (_float*)&WorldFloat4x4);

	if (CurrentGizmoOperation != ImGuizmo::SCALE)
	{
		if (ImGui::RadioButton("LocalSpace", CurrentGizmoMode == ImGuizmo::LOCAL))
			CurrentGizmoMode = ImGuizmo::LOCAL;
		ImGui::SameLine();
		if (ImGui::RadioButton("WorldSpace", CurrentGizmoMode == ImGuizmo::WORLD))
			CurrentGizmoMode = ImGuizmo::WORLD;
	}

	static _bool isUseSnap = { false };
	ImGui::Checkbox("Grab", &isUseSnap);
	ImGui::SameLine();

	_float3 vSnap = { 0.f, 0.f, 0.f };
	switch (CurrentGizmoOperation)
	{
	case ImGuizmo::TRANSLATE:
		ImGui::InputFloat3("Translate Grab", &vSnap.x);
		break;
	case ImGuizmo::ROTATE:
		ImGui::InputFloat("Rotate Grab", &vSnap.x);
		break;
	case ImGuizmo::SCALE:
		ImGui::InputFloat("Scale Grab", &vSnap.x);
		break;
	}

	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

	_float4x4	ProjMatrix, ViewMatrix;

	if (MODE_PERSPECTIVE == m_eMode)
	{
		ProjMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);
		ViewMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
	}

	ImGuizmo::Manipulate((_float*)&ViewMatrix, (_float*)&ProjMatrix, CurrentGizmoOperation, CurrentGizmoMode, (_float*)&WorldFloat4x4, NULL, isUseSnap ? &vSnap.x : NULL);

	//if(true == isChanged && m_pTargetTransform)
	m_pTargetTransform->Set_WorldMatrix(WorldFloat4x4);
}

void CTool_Transformation::Update_WorldMatrix()
{
	if (nullptr != m_pTargetTransform)
		m_WorldMatrix = m_pTargetTransform->Get_WorldFloat4x4();
}

CTool_Transformation* CTool_Transformation::Create(void* pArg)
{
	CTool_Transformation* pInatnace = new CTool_Transformation();

	if (FAILED(pInatnace->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CTool_Transformation"));

		Safe_Release(pInatnace);
	}

	return pInatnace;
}

void CTool_Transformation::Free()
{
	__super::Free();

	Safe_Release(m_pTargetTransform);
}
