#include "stdafx.h"

#include "Customize_UI.h"
#include "GameInstance.h"

CCustomize_UI::CCustomize_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CCustomize_UI::CCustomize_UI(const CCustomize_UI& rhs)
	: CUI{ rhs }
{

}

HRESULT CCustomize_UI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCustomize_UI::Initialize(void* pArg)
{
 	if (pArg != nullptr)
	{
		if (FAILED(__super::Initialize(pArg)))
			return E_FAIL;

		CUSTOM_UI_DESC* CustomUIDesc = (CUSTOM_UI_DESC*)pArg;

		m_strTexturePath = CustomUIDesc->strTexturePath;

		m_strTextureComTag = CustomUIDesc->strTextureComTag;

		m_iColorMaxNum = CustomUIDesc->iColorMaxNum;

		if (m_iColorMaxNum >= 0)
		{
			for (_int i = 0; i <= m_iColorMaxNum; i++)
			{
				m_vColor[i] = CustomUIDesc->vColor[i];
			}
		}

		m_isPlay = CustomUIDesc->isPlay;

		m_fColorTimer_Limit = CustomUIDesc->fColorTimer_Limit;

		m_iEndingType = CustomUIDesc->iEndingType;


		if (FAILED(Add_Components(m_strTextureComTag)))
			return E_FAIL;
	}

	// Shader 초기화
	m_isSelect_Color	= false; 
	m_isSelect			= false;
	m_isAlphaChange		= false;
	m_isBlending		= false;
	m_isPush			= false;
	m_fSplit			= 1;

	return S_OK;
}

HRESULT CCustomize_UI::Initialize_TextBox(void* pArg)
{



	return S_OK;
}

void CCustomize_UI::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fPush_Timer += fTimeDelta;

	/* Color Timer */
	if(m_isPlay)
		Color_Frame(fTimeDelta);

	else
	{
		/* Alpa 값 여부 */
		if (m_isColorChange || m_isAlphaChange)
			m_iShaderPassNum = 1;
		else
			m_iShaderPassNum = 0;

		/* Wave 여부*/
		if (m_isWave)
			m_isWaveTimer += fTimeDelta;
		else
			m_isWaveTimer = 0.0f;

		/* UV Push 여부*/
		if (m_isPush)
			m_fPush_Timer += fTimeDelta;
		else
			m_fPush_Timer = 0.f;
	}


	for (auto& iter : m_vecTextBoxes)
	{
		iter->Tick(fTimeDelta);
	}

}

void CCustomize_UI::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);

	if (0 != m_vecChildUI.size())
	{
		for (auto& iter : m_vecChildUI)
		{
			if (nullptr != iter)
			{
				dynamic_cast<CCustomize_UI*>(iter)->Move_State(ComputeMovement(CUI::UISTATE_POS), CUI::UISTATE_POS);
				dynamic_cast<CCustomize_UI*>(iter)->Move_State(ComputeMovement(CUI::UISTATE_SCALE), CUI::UISTATE_SCALE);
			}
		}
	}

	if (0 != m_vecTextBoxes.size())
	{
		for (auto& iter : m_vecTextBoxes)
		{
			if (nullptr != iter)
			{
				iter->Move_State(ComputeMovement(CUI::UISTATE_POS), CUI::UISTATE_POS);
				iter->Move_State(ComputeMovement(CUI::UISTATE_SCALE), CUI::UISTATE_SCALE);
			}
		}
	}

	__super::Late_Tick(fTimeDelta);

	for (auto& iter : m_vecTextBoxes)
	{
		iter->Late_Tick(fTimeDelta);
	}
}

HRESULT CCustomize_UI::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(m_iShaderPassNum)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCustomize_UI::Add_Components(const wstring& strModelTag)
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, strModelTag,
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCustomize_UI::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	//_float4x4			WorldMatrix = { XMMatrixScaling(400.f, 200.f, 1.f) };
	//if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
	//	return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureNum)))
		return E_FAIL;

	// Edit
	if (FAILED(m_pShaderCom->Bind_RawValue("g_SelectColor", &m_isSelect_Color, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_GreenColor", &m_isSelect, sizeof(_bool))))
		return E_FAIL;

	// Color Change
	if (FAILED(m_pShaderCom->Bind_RawValue("g_ColorValu", &m_vCurrentColor, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_ColorChange", &m_isColorChange, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_AlpaChange", &m_isAlphaChange, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Blending", &m_isBlending, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_BlendingStrength", &m_fBlending, sizeof(_float))))
		return E_FAIL;
	
	// Wave Change
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Speed", &m_fWaveSpeed, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Wave", &m_isWave, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("gTime", &m_isWaveTimer, sizeof(float))))
		return E_FAIL;

	// UP Psuh 
	if (FAILED(m_pShaderCom->Bind_RawValue("g_isPush", &m_isPush, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPush_Timer", &m_fPush_Timer, sizeof(float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_UVSpeed", &m_fPush_Speed, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_RotationSpeed", &m_isUVRotation, sizeof(float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Split", &m_fSplit, sizeof(float))))
		return E_FAIL;
	
	return S_OK;
}

CCustomize_UI::CUSTOM_UI_DESC CCustomize_UI::Get_Cutomize_DESC() const
{
	CCustomize_UI::CUSTOM_UI_DESC CustomUIDesc = {};

	CustomUIDesc.strTexturePath = m_strTexturePath;

	CustomUIDesc.iColorMaxNum = m_iColorMaxNum;

	memcpy(&CustomUIDesc.fWorldMatrix, m_pTransformCom->Get_WorldFloat4x4_Ptr(), sizeof(_float4x4));

	for (_int i = 0; i < m_iColorMaxNum; i++)
	{
		CustomUIDesc.vColor[i] = m_vColor[i];
	}

	for (auto& iter : m_vecTextBoxes)
	{
		CustomUIDesc.TextBoxDesc.push_back(iter->Get_TextBoxDesc());
	}
		

	CustomUIDesc.isPlay = true;

	CustomUIDesc.fColorTimer_Limit = m_fColorTimer_Limit;

	CustomUIDesc.iEndingType = m_iEndingType;

	CustomUIDesc.iTextBox = m_iTextBox;
	
	CustomUIDesc.iChild = (_int)m_vecChildUI.size();

	CustomUIDesc.IsChild = m_IsChild;

	return CustomUIDesc;
}

void CCustomize_UI::Release_Children()
{
	for (auto& iter : m_vecChildUI)
	{
		dynamic_cast<CCustomize_UI*>(iter)->Set_IsChild(false);
		Safe_Release(iter);
	}
	m_vecChildUI.clear();
}

void CCustomize_UI::Release_Child(CGameObject* Child)
{
	auto iter = find(m_vecChildUI.begin(), m_vecChildUI.end(), Child);

	if (iter != m_vecChildUI.end())
	{
		Safe_Release(*iter);
		m_vecChildUI.erase(iter);
	}
}

CGameObject* CCustomize_UI::Find_Child(CGameObject* Child)
{
	for (auto& iter : m_vecChildUI)
	{
		if (iter == Child)
		{
			return iter;
		}
	}

	return nullptr;
}

_bool CCustomize_UI::IsMyChild(CGameObject* Child)
{
	for (auto& iter : m_vecChildUI)
	{
		if (iter == Child)
		{
			return true;
		}
	}

	return false;
}

//HRESULT CCustomize_UI::Change_Texture(const wstring& strPrototypeTag, const wstring& strTexturePath)
//{
//	Safe_Release(m_pTextureCom);
//	m_pTextureCom = nullptr;
//
//	m_strTexturePath = strTexturePath;
//
//	/* For.Com_Texture */
//	if (FAILED(__super::Change_Component(LEVEL_GAMEPLAY, strPrototypeTag, TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
//		return E_FAIL;
//
//	return S_OK;
//}

void CCustomize_UI::Color_Frame(_float fTimeDelta)
{
	/* ▶ 실시간 상태 변경 */
	State_Control(fTimeDelta);	

	/* ▶ 보간 값 */
	m_fCurrentColor_Timer += fTimeDelta;

	_float      fRatio = m_fCurrentColor_Timer / m_fColorTimer_Limit;
	_float      fColorRatio = m_fCurrentColor_Timer * m_fColorSpeed;

	/* ▶ 보간이 필요하지 않다면 */
	if (0 == m_iColorMaxNum)
	{
		m_vCurrentColor = _float4(1, 1, 1, 1);/*m_vColor[0].vColor*/
		m_fBlending = m_vColor[0].fBlender_Value;
		m_fSplit = m_vColor[0].fSplit;
		m_fWaveSpeed = m_vColor[0].WaveSpeed;
		m_isUVRotation = m_vColor[0].fPushRotation;
		m_fPush_Speed.x = m_vColor[0].fPushSpeed.x;
		m_fPush_Speed.y = m_vColor[0].fPushSpeed.y;

		m_isColorChange = m_vColor[0].isColorChange;
		m_isAlphaChange = m_vColor[0].isAlphaChange;
		m_isBlending = m_vColor[0].isBlender;
		m_isPush = m_vColor[0].isPush;
		m_isWave = m_vColor[0].isWave;

		/* World Matrix Change */
		//m_pTransformCom->Set_WorldMatrix(m_SavePos[0]);
	}
	/* ▶ 보간 Type */
	if (m_iEndingType == PLAY_BUTTON::PLAY_DEFAULT)
	{
		Frame_Defalut(fRatio, fColorRatio);
	}
/* ▶ 보간 Type */
	else 
	{
		if (m_iEndingType == PLAY_BUTTON::PLAY_DEFAULT)
		{
			Frame_Defalut(fRatio, fColorRatio);
		}

		else if (m_iEndingType == PLAY_BUTTON::PLAY_CHANGE)
		{
			Frame_Change(fRatio, fColorRatio);
		}

		else if (m_iEndingType == PLAY_BUTTON::PLAY_CUT) // Clear
		{
			Frame_Cut(fRatio, fColorRatio);
		}

		/* ▶ 컬러 간의 간격*/
		if (m_fCurrentColor_Timer >= m_fColorTimer_Limit)
		{
			if (m_iColorCurNum >= 10)
				m_iColorCurNum = 0;

			if (m_iEndingType == PLAY_BUTTON::PLAY_CHANGE)
			{
				// 모든 배열을 도달했다면
				if ((_int)m_iColorCurNum > m_iColorMaxNum)
					m_iColorCurNum = (_uint)m_iColorMaxNum;
			}

			m_iColorCurNum++;

			// 모든 배열을 도달했다면
			if ((_int)m_iColorCurNum > m_iColorMaxNum)
				m_iColorCurNum = 0;

			m_fCurrentColor_Timer = 0.f;
		}
	}
}

void CCustomize_UI::State_Control(_float fTimeDelta)
{
	/* Alpa 값 여부 */
	if (m_isColorChange || m_isAlphaChange)
		m_iShaderPassNum = 1;
	else
		m_iShaderPassNum = 0;

	/* Wave 여부*/
	if (m_isWave)
		m_isWaveTimer += fTimeDelta;
	else
		m_isWaveTimer = 0.0f;

	/* UV Push 여부*/
	if (m_isPush)
		m_fPush_Timer += fTimeDelta;
	else
		m_fPush_Timer = 0.f;

	if (0.f == m_fColorTimer_Limit)
		return;

	if (!m_isPush)
		m_fSplit = 1.f;
}

void CCustomize_UI::Frame_Defalut(_float fRatio, _float fColorRatio)
{
	/*만약 현재 변수가 마지막이라면 0 번째와 Chagne*/
	if ((_int)m_iColorCurNum >= m_iColorMaxNum)
	{
		/* Color */
		_vector vStartColor = XMLoadFloat4(&m_vColor[m_iColorCurNum].vColor);
		_vector vEndColor = XMLoadFloat4(&m_vColor[0].vColor);
		_vector vCurrentColor = XMVectorLerp(vStartColor, vEndColor, fColorRatio);
		XMStoreFloat4(&m_vCurrentColor, vCurrentColor);

		if (!m_vColor[m_iColorCurNum].isPush)
			m_vColor[m_iColorCurNum].fSplit = 1.f;

		/* 분할 : Splite*/
		_float start = m_vColor[m_iColorCurNum].fSplit;
		_float end = m_vColor[0].fSplit;
		m_fSplit = start * (1 - fRatio) + end * fRatio;

		/* Blending*/
		start = m_vColor[m_iColorCurNum].fBlender_Value;
		end = m_vColor[0].fBlender_Value;
		m_fBlending = start * (1 - fRatio) + end * fRatio;

		/* Wave*/
		start = m_vColor[m_iColorCurNum].WaveSpeed;
		end = m_vColor[0].WaveSpeed;
		m_fWaveSpeed = start * (1 - fRatio) + end * fRatio;

		/*Rotation*/
		start = m_vColor[m_iColorCurNum].fPushRotation;
		end = m_vColor[0].fPushRotation;
		m_isUVRotation = start * (1 - fRatio) + end * fRatio;

		/*Position.x*/
		start = m_vColor[m_iColorCurNum].fPushSpeed.x;
		end = m_vColor[0].fPushSpeed.x;
		m_fPush_Speed.x = start * (1 - fRatio) + end * fRatio;

		/*Position.y*/
		start = m_vColor[m_iColorCurNum].fPushSpeed.y;
		end = m_vColor[0].fPushSpeed.y;
		m_fPush_Speed.y = start * (1 - fRatio) + end * fRatio;

		/* World Matrix Change */
		//_matrix saveMatrix = {};
		//_float4x4 myFloat4x4 = {};
		//saveMatrix = LerpMatrix(XMLoadFloat4x4(&m_SavePos[m_iColorCurNum]), XMLoadFloat4x4(&m_SavePos[0]), fRatio);
		//XMStoreFloat4x4(&myFloat4x4, saveMatrix);
		//m_pTransformCom->Set_WorldMatrix(myFloat4x4);

	}

	else
	{
		/* Color */
		_vector vStartColor = XMLoadFloat4(&m_vColor[m_iColorCurNum].vColor);
		_vector vEndColor = XMLoadFloat4(&m_vColor[m_iColorCurNum + 1].vColor);
		_vector vCurrentColor = XMVectorLerp(vStartColor, vEndColor, fColorRatio);
		XMStoreFloat4(&m_vCurrentColor, vCurrentColor);

		/* 분할 : Splite*/
		_float start = m_vColor[m_iColorCurNum].fSplit;
		_float end = m_vColor[m_iColorCurNum + 1].fSplit;
		m_fSplit = start * (1 - fRatio) + end * fRatio;

		if (!m_vColor[m_iColorCurNum + 1].isPush)
			m_vColor[m_iColorCurNum + 1].fSplit = 1.f;

		/* Blending*/
		start = m_vColor[m_iColorCurNum].fBlender_Value;
		end = m_vColor[m_iColorCurNum + 1].fBlender_Value;
		m_fBlending = start * (1 - fRatio) + end * fRatio;

		/* Wave*/
		start = m_vColor[m_iColorCurNum].WaveSpeed;
		end = m_vColor[m_iColorCurNum + 1].WaveSpeed;
		m_fWaveSpeed = start * (1 - fRatio) + end * fRatio;

		/*Rotation*/
		start = m_vColor[m_iColorCurNum].fPushRotation;
		end = m_vColor[m_iColorCurNum + 1].fPushRotation;
		m_isUVRotation = start * (1 - fRatio) + end * fRatio;

		/*Position.x*/
		start = m_vColor[m_iColorCurNum].fPushSpeed.x;
		end = m_vColor[m_iColorCurNum + 1].fPushSpeed.x;
		m_fPush_Speed.x = start * (1 - fRatio) + end * fRatio;

		/*Position.y*/
		start = m_vColor[m_iColorCurNum].fPushSpeed.y;
		end = m_vColor[m_iColorCurNum + 1].fPushSpeed.y;
		m_fPush_Speed.y = start * (1 - fRatio) + end * fRatio;

		/* World Matrix Change */
		//_matrix saveMatrix = {};
		//_float4x4 myFloat4x4 = {};
		//saveMatrix = LerpMatrix(XMLoadFloat4x4(&m_SavePos[m_iColorCurNum]), XMLoadFloat4x4(&m_SavePos[m_iColorCurNum + 1]), fRatio);
		//XMStoreFloat4x4(&myFloat4x4, saveMatrix);
		//m_pTransformCom->Set_WorldMatrix(myFloat4x4);
	}

	m_isColorChange = m_vColor[m_iColorCurNum].isColorChange;
	m_isAlphaChange = m_vColor[m_iColorCurNum].isAlphaChange;
	m_isBlending = m_vColor[m_iColorCurNum].isBlender;

	if (m_vColor[m_iColorCurNum + 1].isPush == true)
		m_isPush = true;
	else
		m_isPush = m_vColor[m_iColorCurNum].isPush;

	if (m_vColor[m_iColorCurNum + 1].isWave == true)
		m_isWave = true;
	else
		m_isWave = m_vColor[m_iColorCurNum].isWave;
}

void CCustomize_UI::Frame_Change(_float fRatio, _float fColorRatio)
{
	if (!m_vColor[m_iColorCurNum].isPush)
		m_vColor[m_iColorCurNum].fSplit = 1.f;

	if ((_int)m_iColorCurNum >= m_iColorMaxNum)
	{
		m_vCurrentColor = m_vColor[m_iColorMaxNum].vColor;
		m_fSplit = m_vColor[m_iColorMaxNum].fSplit;
		m_fWaveSpeed = m_vColor[m_iColorMaxNum].WaveSpeed;
		m_isUVRotation = m_vColor[m_iColorMaxNum].fPushRotation;
		m_fPush_Speed.x = m_vColor[m_iColorMaxNum].fPushSpeed.x;
		m_fPush_Speed.y = m_vColor[m_iColorMaxNum].fPushSpeed.y;
		m_fBlending = m_vColor[m_iColorMaxNum].fBlender_Value;
		/* World Matrix Change */
		//m_pTransformCom->Set_WorldMatrix(m_SavePos[m_iColorMaxNum]);

		m_isColorChange = m_vColor[m_iColorMaxNum].isColorChange;
		m_isAlphaChange = m_vColor[m_iColorMaxNum].isAlphaChange;
		m_isBlending = m_vColor[m_iColorMaxNum].isBlender;
		m_isPush = m_vColor[m_iColorMaxNum].isPush;
		m_isWave = m_vColor[m_iColorMaxNum].isWave;
	}

	else
	{
		/* Color */
		_vector vStartColor = XMLoadFloat4(&m_vColor[m_iColorCurNum].vColor);
		_vector vEndColor = XMLoadFloat4(&m_vColor[m_iColorCurNum + 1].vColor);
		_vector vCurrentColor = XMVectorLerp(vStartColor, vEndColor, fColorRatio);
		XMStoreFloat4(&m_vCurrentColor, vCurrentColor);

		/* 분할 : Splite*/
		_float start = m_vColor[m_iColorCurNum].fSplit;
		_float end = m_vColor[m_iColorCurNum + 1].fSplit;
		m_fSplit = start * (1 - fRatio) + end * fRatio;

		/* Blending*/
		start = m_vColor[m_iColorCurNum].fBlender_Value;
		end = m_vColor[m_iColorCurNum + 1].fBlender_Value;
		m_fBlending = start * (1 - fRatio) + end * fRatio;

		/* Wave*/
		start = m_vColor[m_iColorCurNum].WaveSpeed;
		end = m_vColor[m_iColorCurNum + 1].WaveSpeed;
		m_fWaveSpeed = start * (1 - fRatio) + end * fRatio;

		/*Rotation*/
		start = m_vColor[m_iColorCurNum].fPushRotation;
		end = m_vColor[m_iColorCurNum + 1].fPushRotation;
		m_isUVRotation = start * (1 - fRatio) + end * fRatio;

		/*Position.x*/
		start = m_vColor[m_iColorCurNum].fPushSpeed.x;
		end = m_vColor[m_iColorCurNum + 1].fPushSpeed.x;
		m_fPush_Speed.x = start * (1 - fRatio) + end * fRatio;

		/*Position.y*/
		start = m_vColor[m_iColorCurNum].fPushSpeed.y;
		end = m_vColor[m_iColorCurNum + 1].fPushSpeed.y;
		m_fPush_Speed.y = start * (1 - fRatio) + end * fRatio;

		/* World Matrix Change */
		//_matrix saveMatrix = {};
		//_float4x4 myFloat4x4 = {};
		//saveMatrix = LerpMatrix(XMLoadFloat4x4(&m_SavePos[m_iColorCurNum]), XMLoadFloat4x4(&m_SavePos[m_iColorCurNum + 1]), fRatio);
		//XMStoreFloat4x4(&myFloat4x4, saveMatrix);
		//m_pTransformCom->Set_WorldMatrix(myFloat4x4);

		m_isColorChange = m_vColor[m_iColorCurNum].isColorChange;
		m_isAlphaChange = m_vColor[m_iColorCurNum].isAlphaChange;
		m_isBlending = m_vColor[m_iColorCurNum].isBlender;
		m_isPush = m_vColor[m_iColorCurNum].isPush;
		m_isWave = m_vColor[m_iColorCurNum].isWave;
	}
}

void CCustomize_UI::Frame_Cut(_float fRatio, _float fColorRatio)
{
	m_vCurrentColor = m_vColor[m_iColorCurNum].vColor;
	m_fBlending = m_vColor[m_iColorCurNum].fBlender_Value;
	m_fSplit = m_vColor[m_iColorCurNum].fSplit;
	m_fWaveSpeed = m_vColor[m_iColorCurNum].WaveSpeed;
	m_isUVRotation = m_vColor[m_iColorCurNum].fPushRotation;
	m_fPush_Speed.x = m_vColor[m_iColorCurNum].fPushSpeed.x;
	m_fPush_Speed.y = m_vColor[m_iColorCurNum].fPushSpeed.y;

	m_isColorChange = m_vColor[m_iColorCurNum].isColorChange;
	m_isAlphaChange = m_vColor[m_iColorCurNum].isAlphaChange;
	m_isBlending = m_vColor[m_iColorCurNum].isBlender;
	m_isPush = m_vColor[m_iColorCurNum].isPush;
	m_isWave = m_vColor[m_iColorCurNum].isWave;

	/* World Matrix Change */
	m_pTransformCom->Set_WorldMatrix(m_SavePos[m_iColorCurNum]);
}

_matrix CCustomize_UI::LerpMatrix(_matrix A, _matrix B, _float t)
{
	_matrix result;

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			// A와 B 행렬의 (i, j) 요소에 대해 선형 보간 수행
			result.r[i].m128_f32[j] = A.r[i].m128_f32[j] + t * (B.r[i].m128_f32[j] - A.r[i].m128_f32[j]);
		}
	}

	return result;
}


void CCustomize_UI::PushBack_Child(CGameObject* pGameOBJ)
{
	m_vecChildUI.push_back(pGameOBJ);
	Safe_AddRef(pGameOBJ);
	dynamic_cast<CCustomize_UI*>(pGameOBJ)->Set_IsChild(true);
}

void CCustomize_UI::PushBack_TextBox(CGameObject* pGameOBJ)
{
	if (nullptr == pGameOBJ)
		return;

	m_vecTextBoxes.push_back(dynamic_cast<CTextBox*>(pGameOBJ));
}

CCustomize_UI* CCustomize_UI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCustomize_UI* pInstance = new CCustomize_UI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCustomize_UI"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CCustomize_UI::Clone(void* pArg)
{
	CCustomize_UI* pInstance = new CCustomize_UI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CCustomize_UI"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCustomize_UI::Free()
{
	__super::Free();

	for (auto& pTextBox : m_vecTextBoxes)
		Safe_Release(pTextBox);
	m_vecTextBoxes.clear();

	for (auto& pChildUI : m_vecChildUI)
		Safe_Release(pChildUI);
	m_vecChildUI.clear();
}
