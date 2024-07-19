#include "stdafx.h"
#include "Key_ReaderMachine.h"
#include"Player.h"

#include"ReaderMachine.h"

CKey_ReaderMachine::CKey_ReaderMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPart_InteractProps{ pDevice, pContext }
{
}

CKey_ReaderMachine::CKey_ReaderMachine(const CKey_ReaderMachine& rhs)
	: CPart_InteractProps{ rhs }
{

}

HRESULT CKey_ReaderMachine::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CKey_ReaderMachine::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	if (pArg != nullptr)
	{
		KEY_READER_DESC* pKey_Desc = (KEY_READER_DESC*)pArg;
		m_pPressKeyState = pKey_Desc->pKeyInput;
		m_pKeyState = pKey_Desc->pKeyState;
		memcpy_s(m_pCanPush, sizeof(_bool) * 5 * 3, pKey_Desc->pCanPush, sizeof(_bool) * 5 * 3);
		m_pPush = pKey_Desc->pPush;
		m_pSelectCol = pKey_Desc->pSelectCol;
		m_pSelectRow = pKey_Desc->pSelectRow;
		m_pDoOpen = pKey_Desc->pDoOpen;
		m_pKeyNum = pKey_Desc->pKeyNum;
		memcpy_s(m_iKeyPad, sizeof(_int) * 5 * 3, pKey_Desc->iKeyPad, sizeof(_int) * 5 * 3);
		m_pKeyNum = pKey_Desc->pKeyNum;
		m_pHideKey = pKey_Desc->pHideKey;
	}

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Initialize_Model()))
		return E_FAIL; 

	m_pTransformCom->Set_Scaled(100.f, 100.f, 100.f);
	m_pModelCom->Set_RootBone("RootNode");
	m_pModelCom->Add_Bone_Layer_All_Bone(TEXT("Default"));

	m_pModelCom->Add_AnimPlayingInfo(false, 0, TEXT("Default"), 1.f);

	
	m_pModelCom->Active_RootMotion_Rotation(false);
	



	return S_OK;
}

void CKey_ReaderMachine::Tick(_float fTimeDelta)
{
	m_pTransformCom->Rotation(m_pTransformCom->Get_State_Vector(CTransform::STATE_UP),XMConvertToRadians(180.f));
	if (m_bCheckAnswer)
	{
	
		m_bCheckAnswer = false;
	}

	if (m_pHideKey[0])
	{
		m_pModelCom->Hide_Mesh(m_HidMesh[3], false);

		m_pCanPush[CReaderMachine::ROW_2][CReaderMachine::COL_1] = false;

	}
	if (m_pHideKey[1])
	{
		m_pModelCom->Hide_Mesh(m_HidMesh[0], false);

		m_pCanPush[CReaderMachine::ROW_2][CReaderMachine::COL_2] = false;
	}


}

void CKey_ReaderMachine::Late_Tick(_float fTimeDelta)
{
	if (m_bDead)
		return;



	switch (*m_pKeyState)
	{
	case CReaderMachine::READERMACHINE_KEY_STATIC:
		m_pModelCom->Change_Animation(0, TEXT("Default"), 0);
		break;

	case CReaderMachine::READERMACHINE_KEY_LIVE:
	{
		m_pModelCom->Change_Animation(0, TEXT("Default"), 0);

		switch (*m_pPressKeyState)
		{
		case CReaderMachine::KEY_NOTHING:
			if (m_fGoalLength[BONE_KEYB].z < 0.f)
				m_fGoalLength[BONE_KEYB].z = 0.f;
			if (m_fGoalLength[BONE_KEYENTER].z < 0.f)
				m_fGoalLength[BONE_KEYENTER].z = 0.f;
			m_fGoalLength[m_iKeyPad[*m_pSelectRow][(*m_pSelectCol)]].x = 0.005f;
			break;

		case CReaderMachine::KEY_W:
			m_fGoalLength[m_iKeyPad[*m_pSelectRow][(*m_pSelectCol)]].x = 0.f;

			*m_pSelectRow -= 1;
			if (*m_pSelectRow < CReaderMachine::ROW_0)
				*m_pSelectRow = CReaderMachine::ROW_4;
			*m_pPressKeyState = CReaderMachine::KEY_NOTHING;
			break;

		case CReaderMachine::KEY_A:
			m_fGoalLength[m_iKeyPad[*m_pSelectRow][(*m_pSelectCol)]].x = 0.f;

			*m_pPressKeyState = CReaderMachine::KEY_NOTHING;
			*m_pSelectCol -= 1;
			if (*m_pSelectCol < CReaderMachine::COL_0)
				*m_pSelectCol = CReaderMachine::COL_2;
			break;

		case CReaderMachine::KEY_S:
			m_fGoalLength[m_iKeyPad[*m_pSelectRow][(*m_pSelectCol)]].x = 0.f;

			*m_pPressKeyState = CReaderMachine::KEY_NOTHING;
			*m_pSelectRow += 1;
			if (*m_pSelectRow > CReaderMachine::ROW_4)
				*m_pSelectRow = CReaderMachine::ROW_0;
			break;

		case CReaderMachine::KEY_D:
			m_fGoalLength[m_iKeyPad[*m_pSelectRow][(*m_pSelectCol)]].x = 0.f;

			*m_pPressKeyState = CReaderMachine::KEY_NOTHING;
			*m_pSelectCol += 1;
			if (*m_pSelectCol > CReaderMachine::COL_2 )
				*m_pSelectCol = CReaderMachine::COL_0;
			break;

		case CReaderMachine::KEY_SPACE:
			*m_pPressKeyState = CReaderMachine::KEY_NOTHING;
			if (m_iKeyPad[*m_pSelectRow][(*m_pSelectCol)] == CReaderMachine::PAD_DELETE)
			{
				if (m_pPush[*m_pKeyNum] == -1)
				{
					*m_pKeyNum--;
					if (*m_pKeyNum < CReaderMachine::NUM_0)
						*m_pKeyNum = CReaderMachine::NUM_0;

					m_fGoalLength[m_iKeyPad[*m_pSelectRow][(*m_pSelectCol)]] = _float3(0.f, 0.f, -0.01f);
				}
				else
					m_pPush[*m_pKeyNum] = -1;

			}
			else if (m_iKeyPad[*m_pSelectRow][(*m_pSelectCol)] == CReaderMachine::PAD_ENTER)
			{
				m_fGoalLength[m_iKeyPad[*m_pSelectRow][(*m_pSelectCol)]] = _float3(0.f, 0.f, -0.01f);
				if (Check_PadFull())
					*m_pDoOpen = true;


				break;
					
			}
			else
			{
				if (Check_PadFull())
					break;
				if (Check_CanPush())
				{
					m_fGoalLength[m_iKeyPad[*m_pSelectRow][(*m_pSelectCol)]] += _float3(0.f, 0.f, -0.005f);

					m_pPush[*m_pKeyNum] = m_iKeyPad[*m_pSelectRow][(*m_pSelectCol)];
					(*m_pKeyNum)++;
					if (*m_pKeyNum > CReaderMachine::NUM_2)
						*m_pKeyNum = CReaderMachine::NUM_0;
				}
				break;
			}

		}

	}
	break;

	case CReaderMachine::READERMACHINE_KEY_WRONG:
		*m_pKeyState = CReaderMachine::READERMACHINE_KEY_LIVE;
		for (_int i = 0; i < CReaderMachine::PAD_END; i++)
		{
			m_fGoalLength[i] = _float3(0.f, 0.f, 0.0f);
		}
		break;

	case CReaderMachine::READERMACHINE_KEY_CORRECT:
		for (_int i = 0; i < CReaderMachine::PAD_END; i++)
		{
			m_fGoalLength[i] = _float3(0.f, 0.f, 0.0f);
		}
		*m_pKeyState = CReaderMachine::READERMACHINE_KEY_LIVE;

		break;
	}
	for (_int i = 0; i < READERMACHINE_BONE_END; i++)
	{
		m_fCurLength[i] = XMVectorLerp(m_fCurLength[i], m_fGoalLength[i], fTimeDelta * 3.f);

		_matrix			TranslationMatrix = m_pTransformCom->Get_WorldMatrix()* m_pTransformCom->Get_WorldMatrix_Inverse();
		TranslationMatrix.r[3] = m_fCurLength[i];
		m_pModelCom->Add_Additional_Transformation_World(m_strBoneTag[i], TranslationMatrix);

		
	}
	_float4 fTransform4 = m_pParentsTransform->Get_State_Float4(CTransform::STATE_POSITION);
	_float3 fTransform3 = _float3{ fTransform4.x,fTransform4.y,fTransform4.z };
	_float3				vDirection = { };

	m_pModelCom->Play_Animations(m_pParentsTransform, fTimeDelta, &vDirection);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_POINT, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_DIR, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);



	_matrix			WorldMatrix = { m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pSocketMatrix) * (m_pParentsTransform->Get_WorldMatrix()) };
	XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
	Get_SpecialBone_Rotation(); // for UI
}

HRESULT CKey_ReaderMachine::Render()
{
	if (m_bClear)
		return S_OK;
	if (m_bRender == false)
		return S_OK;
	else
		m_bRender = false;
	
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


		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		m_pModelCom->Render(static_cast<_uint>(i));
	}


	return S_OK;
}

HRESULT CKey_ReaderMachine::Add_Components()
{
	/* For.Com_Body_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Body_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	
	/* For.Com_Body_Model */
	if (FAILED(__super::Add_Component(g_Level, m_strModelComponentName,
		TEXT("Com_Body_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;
#ifdef _DEBUG
#ifdef UI_POS
	CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

	ColliderDesc.fRadius = _float(20.f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
	/* For.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Body_Collider"), (CComponent**)&m_pColliderCom[Part_INTERACTPROPS_COL_SPHERE], &ColliderDesc)))
		return E_FAIL;
#endif
#endif

	return S_OK;
}

HRESULT CKey_ReaderMachine::Add_PartObjects()
{

	return S_OK;
}

HRESULT CKey_ReaderMachine::Initialize_PartObjects()
{
	return S_OK;
}

void CKey_ReaderMachine::Get_SpecialBone_Rotation()
{

}

HRESULT CKey_ReaderMachine::Initialize_Model()
{
	vector<string>			MeshTags = { m_pModelCom->Get_MeshTags() };

	vector<string>			ResultMeshTags;
	vector<string>			HidMeshs;
	m_HidMesh.emplace_back("Group_102");
	m_HidMesh.emplace_back("Group_103");
	m_HidMesh.emplace_back("Group_124");
	m_HidMesh.emplace_back("Group_125");
	for (auto& strMeshTag : MeshTags)
	{
		if ((strMeshTag.find("Group_102") == string::npos) && (strMeshTag.find("Group_103") == string::npos) && (strMeshTag.find("Group_124") == string::npos) && (strMeshTag.find("Group_125") == string::npos))
			ResultMeshTags.push_back(strMeshTag);
		else
			HidMeshs.push_back(strMeshTag);
	}
	m_HidMesh = HidMeshs;
	for (auto& strMeshTag : MeshTags)
	{
		m_pModelCom->Hide_Mesh(strMeshTag, true);
	}

	for (auto& strMeshTag : ResultMeshTags)
	{
		m_pModelCom->Hide_Mesh(strMeshTag, false);
	}


	return S_OK;
}

_bool CKey_ReaderMachine::Check_PadFull()
{
	for (_int i = 0; i < CReaderMachine::NUM_END; i++)
	{
		if (m_pPush[i] == -1)
			return false;


	}

	return true;
}

_bool CKey_ReaderMachine::Check_CanPush()
{
	if (false == m_pCanPush[*m_pSelectRow][*m_pSelectCol])
		return true;

	return false;
}


CKey_ReaderMachine* CKey_ReaderMachine::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CKey_ReaderMachine* pInstance = new CKey_ReaderMachine(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CKey_ReaderMachine"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CKey_ReaderMachine::Clone(void* pArg)
{
	CKey_ReaderMachine* pInstance = new CKey_ReaderMachine(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CKey_ReaderMachine"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CKey_ReaderMachine::Free()
{
	__super::Free();

}
