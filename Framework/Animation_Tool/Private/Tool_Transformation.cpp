#include "stdafx.h"
#include "Tool_Transformation.h"

#include "ImGuizmo.h"
#include "GameObject.h"

CTool_Transformation::CTool_Transformation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CTool{ pDevice, pContext }
{
}

HRESULT CTool_Transformation::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Componets()))
		return E_FAIL;

	m_strCollasingTag = "Tool_Transform";

	m_pTransformCom->Set_Scaled(0.05f, 0.05f, 0.05f);

	return S_OK;
}

void CTool_Transformation::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	static _bool isChanged = { false };
	ImGui::Begin("Transformation");

	Set_Origin();
	Update_Transform();
	Update_Target_Transform();

	ImGui::End();
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
	if (nullptr == pGameObject)
		return;

	CTransform* pTransform = { dynamic_cast<CTransform*>(pGameObject->Get_Component(TEXT("Com_Transform"))) };
	if (nullptr == pTransform)
		return;

	Safe_Release(m_pTargetTransform);
	m_pTargetTransform = nullptr;

	m_pTargetTransform = pTransform;
	Safe_AddRef(m_pTargetTransform);
}

void CTool_Transformation::ReSet_Target()
{
	Safe_Release(m_pTargetTransform);
	m_pTargetTransform = nullptr;
}

void CTool_Transformation::Update_Transform()
{
	ImGui::NewLine();

	if (nullptr != m_pTargetTransform)
	{
		_matrix			TargetWorldMatrix = { m_pTargetTransform->Get_WorldMatrix() };
		m_pTransformCom->Set_WorldMatrix(TargetWorldMatrix);
	}

	_float4x4		WorldFloat4x4 = { m_pTransformCom->Get_WorldFloat4x4() };

	ImGuizmo::BeginFrame();

	static ImGuizmo::OPERATION		CurrentGizmoOperation(ImGuizmo::ROTATE);
	static ImGuizmo::MODE			CurrentGizmoMode(ImGuizmo::WORLD);


	if (ImGui::RadioButton("Scale", CurrentGizmoOperation == ImGuizmo::SCALE))
		CurrentGizmoOperation = ImGuizmo::SCALE;
	ImGui::SameLine();

	if (ImGui::RadioButton("Rotation", CurrentGizmoOperation == ImGuizmo::ROTATE))
		CurrentGizmoOperation = ImGuizmo::ROTATE;
	ImGui::SameLine();

	if (ImGui::RadioButton(u8"Translation", CurrentGizmoOperation == ImGuizmo::TRANSLATE))
		CurrentGizmoOperation = ImGuizmo::TRANSLATE;


	_float			matrixTranslation[3], matrixRotation[3], matrixScale[3];
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

	ImGuiIO& io = { ImGui::GetIO() };
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

	_float4x4	ProjMatrix, ViewMatrix;

	ProjMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);
	ViewMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);

	ImGuizmo::Manipulate(&ViewMatrix.m[0][0], &ProjMatrix.m[0][0], CurrentGizmoOperation, CurrentGizmoMode, &WorldFloat4x4.m[0][0], NULL, isUseSnap ? &vSnap.x : NULL);

	m_pTransformCom->Set_WorldMatrix(WorldFloat4x4);
}

void CTool_Transformation::Update_Target_Transform()
{
	if (nullptr == m_pTargetTransform)
		return;

	_matrix			WorldMatrix = { m_pTransformCom->Get_WorldMatrix() };
	m_pTargetTransform->Set_WorldMatrix(WorldMatrix);
}

void CTool_Transformation::Set_Origin()
{
	if (nullptr == m_pTransformCom)
		return;

	if (ImGui::Button("Set Position Zero"))
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorZero());

		if (nullptr != m_pTargetTransform)
			m_pTargetTransform->Set_State(CTransform::STATE_POSITION, XMVectorZero());
	}
}

HRESULT CTool_Transformation::Add_Componets()
{
	m_pTransformCom = CTransform::Create(m_pDevice, m_pContext);
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Initialize(nullptr)))
		return E_FAIL;

	return S_OK;
}

CTool_Transformation* CTool_Transformation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CTool_Transformation* pInatnace = { new CTool_Transformation(pDevice, pContext) };

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
