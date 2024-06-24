#include "stdafx.h"

#include "Customize_UI.h"
#include "TextBox.h"
#include "Player.h"

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

		m_eMap_Location = CustomUIDesc->eMapUI_Type;

		m_wstrDefaultTexturPath = CustomUIDesc->wstrDefaultTexturPath;

		m_wstrMaskPath = CustomUIDesc->wstrMaskPath;

		m_wstrDefaultTexturComTag = CustomUIDesc->wstrDefaultTexturComTag;

		m_wstrMaskComTag = CustomUIDesc->wstrMaskComTag;

		m_isMask = m_Mask[0].isMask;	

		if (FAILED(Add_Components(CustomUIDesc->wstrDefaultTexturComTag, CustomUIDesc->wstrMaskComTag)))
			return E_FAIL;

		m_isPlay = CustomUIDesc->isPlay;

		m_fColorTimer_Limit = CustomUIDesc->fColorTimer_Limit;

		m_iEndingType = CustomUIDesc->iEndingType;

		m_fMaxFrame = CustomUIDesc->fMaxFrame;

		m_isFrame = CustomUIDesc->isFrame;

		m_isLoopStart = CustomUIDesc->isLoopStart;

		m_isLoop = CustomUIDesc->isLoop;

		m_isLoopStop = CustomUIDesc->isLoopStop;

		m_ReStart = CustomUIDesc->ReStart;

		m_iColorMaxNum = CustomUIDesc->iColorMaxNum;

		for (_int i = 0; i <= m_iColorMaxNum; i++)
		{
			m_vColor[i] = CustomUIDesc->vColor[i];
			m_SavePos[i] = CustomUIDesc->SavePos[i];
			m_Mask[i] = CustomUIDesc->Mask[i];
		}

		for (_uint i = 0; i < CustomUIDesc->iTextBoxCount; i++)
		{
			CGameObject* pTextBox = m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_TextBox"), &CustomUIDesc->vecTextBoxDesc[i]);
			if (nullptr == pTextBox)
				return E_FAIL;

			m_vecTextBoxes.push_back(static_cast<CTextBox*>(pTextBox));
		}

		m_IsChild = CustomUIDesc->IsChild;

		/* ▶ 보간이 상관 없다면 */
		if(CustomUIDesc->isLoad == true)
		{
			if (0 == m_iColorMaxNum)
			{
				m_isPlay = false;
			}
		}
	}

	// Shader 초기화
	m_isAlphaChange		= false;
	m_isBlending		= false;
	m_isPush			= false;
	m_fSplit			= 1;
	m_isLoad			= true;

	/* ▶ 만약 보간 과정에서 하얀 게 보인다면 
	> Blending 하는 과정에서 알파를 1로 주지 않았는 지 확인, */

	return S_OK;
}

HRESULT CCustomize_UI::Initialize_TextBox(void* pArg)
{
	return S_OK;
}

void CCustomize_UI::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_isLoad)
	{
		m_isLoad = false;
		Non_Frame();
	}

	if (m_isPlay)
		Color_Frame(fTimeDelta);

	else if(false == m_isTimerControl)
	{
		/* Alpa 값 여부 */
		if (m_isColorChange || m_isAlphaChange || m_isMask)
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

		/* Mask 여부 */
		if (m_isMask)
			m_fMaskTimer += fTimeDelta;
		else
			m_fMaskTimer = 0.f;
	}

	for (auto& iter : m_vecTextBoxes)
	{
		iter->Tick(fTimeDelta);
	}
}

void CCustomize_UI::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);


	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);

	for (auto& iter : m_vecTextBoxes)
	{
		iter->Late_Tick(fTimeDelta);
	}
}

HRESULT CCustomize_UI::Render()
{
	if (false == m_isRender)
		return E_FAIL;

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

HRESULT CCustomize_UI::Add_Components(const wstring& wstrTextureTag, const wstring& wstrMaskTag)
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(g_Level, wstrTextureTag,
		TEXT("Com_DefaultTexture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(g_Level, wstrMaskTag,
		TEXT("Com_MaskTexture"), (CComponent**)&m_pMaskTextureCom)))
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
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureNum)))
		return E_FAIL;

	if(nullptr != m_pMaskTextureCom)
	{
		if (FAILED(m_pMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", 0)))
			return E_FAIL;
	}
	if (nullptr != m_pSubMaskTextureCom)
	{
		if (FAILED(m_pSubMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskSub_Texture", 0)))
			return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShaderCom, TEXT("Target_Depth"), "g_DepthTexture")))
		return E_FAIL;

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

	// Mask
	if (FAILED(m_pShaderCom->Bind_RawValue("g_isMask", &m_isMask, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fMaskControl", &m_fMaskControl, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fMaskSpeed", &m_fMaskSpeed, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fMaskTime", &m_fMaskTimer, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_MaskType", &m_vMaskType, sizeof(_float2))))
		return E_FAIL;

	/////////////// Client ////////////////////
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightMask_Color", &m_vLightMask_Color, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_isLightMask", &m_isLightMask, sizeof(_bool))))
		return E_FAIL;
	
	// Light
	if (FAILED(m_pShaderCom->Bind_RawValue("g_isLight", &m_isLight, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_LightPosition", &m_fLightPosition, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_LightSize", &m_fLightSize, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CCustomize_UI::CUSTOM_UI_DESC CCustomize_UI::Get_Cutomize_DESC() const
{
	CCustomize_UI::CUSTOM_UI_DESC CustomUIDesc = {};

	CustomUIDesc.wstrDefaultTexturPath = m_wstrDefaultTexturPath;

	CustomUIDesc.wstrDefaultTexturComTag = m_wstrDefaultTexturComTag;

	CustomUIDesc.wstrMaskPath = m_wstrMaskPath;

	CustomUIDesc.wstrMaskComTag = m_wstrMaskComTag;

	CustomUIDesc.worldMatrix = m_pTransformCom->Get_WorldFloat4x4();

	CustomUIDesc.isPlay = m_isPlay;

	CustomUIDesc.fColorTimer_Limit = m_fColorTimer_Limit;

	CustomUIDesc.iEndingType = m_iEndingType;

	CustomUIDesc.fMaxFrame = m_fMaxFrame;

	CustomUIDesc.isFrame = m_isFrame;

	CustomUIDesc.isLoopStart = m_isLoopStart;

	CustomUIDesc.isLoop = m_isLoop;

	CustomUIDesc.isLoopStart = m_isLoopStart;

	CustomUIDesc.ReStart = m_ReStart;

	CustomUIDesc.iColorMaxNum = m_iColorMaxNum;

	for (_int i = 0; i <= m_iColorMaxNum; i++)
	{
		CustomUIDesc.SavePos[i] = m_SavePos[i];

		CustomUIDesc.vColor[i] = m_vColor[i];

		CustomUIDesc.Mask[i] = m_Mask[i];
	}

	CustomUIDesc.iChild = (_int)m_vecChildUI.size();

	for (auto& iter : m_vecTextBoxes)
	{
		CustomUIDesc.vecTextBoxDesc.push_back(iter->Get_TextBoxDesc());
	}

	CustomUIDesc.iTextBoxCount = (_uint)m_vecTextBoxes.size();

	CustomUIDesc.iChild = (_int)m_vecChildUI.size();

	CustomUIDesc.isLoad = true;

	CustomUIDesc.IsChild = m_IsChild;

	return CustomUIDesc;
}

void CCustomize_UI::Release_Children()
{
	for (auto& iter : m_vecChildUI)
	{
		static_cast<CCustomize_UI*>(iter)->Set_IsChild(false);
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

void CCustomize_UI::Set_Dead(_bool bDead)
{
	m_bDead = bDead;

	for (auto& iter : m_vecChildUI)
		iter->Set_Dead(bDead);
}

_float CCustomize_UI::Distance_Player(CGameObject* _obj)
{
	CTransform* pPlayerTrans = static_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag));
	CTransform* pTargetTrans = static_cast<CTransform*>(_obj->Get_Component(g_strTransformTag));

	_vector vDistanceVector = pTargetTrans->Get_State_Vector(CTransform::STATE_POSITION) - pPlayerTrans->Get_State_Vector(CTransform::STATE_POSITION);
	_float fPlayer_Distance = XMVectorGetX(XMVector3Length(vDistanceVector));

	return fPlayer_Distance;
}

void CCustomize_UI::Find_Player()
{
	/* Player 찾기 */
	CGameObject* pPlayerObj = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Player"))->back();

	auto pPlayer = static_cast<CPlayer*>(pPlayerObj);

	m_pPlayer = pPlayer;

	Safe_AddRef(m_pPlayer);
}

void CCustomize_UI::Set_IsLoad(_bool IsLoad)
{
	m_isLoad = IsLoad;

	for (auto& iter : m_vecChildUI)
	{
		static_cast<CCustomize_UI*>(iter)->Set_IsLoad(IsLoad);
	}
}

_bool CCustomize_UI::Select_UI()
{
	_float2 mouse = m_pGameInstance->Get_ViewMousePos();

	_float4 UITrans = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
	_float3 UIScaled = m_pTransformCom->Get_Scaled();

	_float2 LU = { (g_iWinSizeX + UITrans.x) - UIScaled.x, (g_iWinSizeY + UITrans.y) - UIScaled.y };
	_float2 RU = { (g_iWinSizeX + UITrans.x) + UIScaled.x, (g_iWinSizeY + UITrans.y) - UIScaled.y };
	_float2 LD = { (g_iWinSizeX + UITrans.x) - UIScaled.x, (g_iWinSizeY + UITrans.y) - UIScaled.y };
	_float2 RD = { (g_iWinSizeX + UITrans.x) + UIScaled.x, (g_iWinSizeY + UITrans.y) + UIScaled.y };


	if ((mouse.x >= LU.x && mouse.y >= LU.y) &&
		(mouse.x <= RU.x && mouse.y >= RU.y) &&
		(mouse.x >= LD.x && mouse.y <= LD.y) &&
		(mouse.x <= RD.x && mouse.y <= RD.y))
		return true;

	return false;
}


HRESULT CCustomize_UI::Change_Texture(const wstring& strPrototypeTag, const wstring& strComponentTag)
{
	if (nullptr == m_pGameInstance->Find_Prototype(g_Level, strPrototypeTag))
		return E_FAIL;

	if (TEXT("Com_DefaultTexture") == strComponentTag)
	{
		Safe_Release(m_pTextureCom);
		m_pTextureCom = nullptr;
		if (FAILED(__super::Change_Component(g_Level, strPrototypeTag, strComponentTag, (CComponent**)&m_pTextureCom)))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

void CCustomize_UI::Non_Frame(_uint i)
{
	/* ▶ 보간이 필요하지 않다면
		=> 정확하게는 Client가 직접 Play를 지정해주기 위해서,
		Shader에서 실시간으로 변환시켜줄 변수 = 내가 처음 지정했던 변수
		를 넣어주는 과정임 */
	m_vCurrentColor = m_vColor[i].vColor;
	m_fBlending = m_vColor[i].fBlender_Value;
	m_fSplit = m_vColor[i].fSplit;
	m_fWaveSpeed = m_vColor[i].WaveSpeed;
	m_isUVRotation = m_vColor[i].fPushRotation;
	m_fPush_Speed.x = m_vColor[i].fPushSpeed.x;
	m_fPush_Speed.y = m_vColor[i].fPushSpeed.y;

	m_isColorChange = m_vColor[i].isColorChange;
	m_isAlphaChange = m_vColor[i].isAlphaChange;
	m_isBlending = m_vColor[i].isBlender;
	m_isPush = m_vColor[i].isPush;
	m_isWave = m_vColor[i].isWave;

	m_isMask = m_Mask[i].isMask;
	m_fMaskSpeed = m_Mask[i].fMaskSpeed;
	m_vMaskType = m_Mask[i].vMaskType;
	m_fMaskControl = m_Mask[i].fMaskControl;

	/* World Matrix Change */
	m_pTransformCom->Set_WorldMatrix(m_SavePos[i]);

}

void CCustomize_UI::Color_Frame(_float fTimeDelta)
{
	/* ▶ 실시간 상태 변경 */
	State_Control(fTimeDelta);

	/* ▶ 보간 값 */
	m_fCurrentColor_Timer += fTimeDelta;

	_float      fRatio = m_fCurrentColor_Timer / m_fColorTimer_Limit;
	_float      fColorRatio = m_fCurrentColor_Timer * m_fColorSpeed;

	/* ▶ 보간 Type */
	if (m_iEndingType == PLAY_BUTTON::PLAY_DEFAULT)
	{
		Frame_Defalut(fRatio, fColorRatio);
	}
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
	}

	Frame_Reset();
}

void CCustomize_UI::State_Control(_float fTimeDelta)
{
	/* Alpa 값 여부 */
	if (m_isColorChange || m_isAlphaChange || m_isMask)
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
		_matrix saveMatrix = {};
		_float4x4 myFloat4x4 = {};
		saveMatrix = LerpMatrix(XMLoadFloat4x4(&m_SavePos[m_iColorCurNum]), XMLoadFloat4x4(&m_SavePos[0]), fRatio);
		XMStoreFloat4x4(&myFloat4x4, saveMatrix);
		m_pTransformCom->Set_WorldMatrix(myFloat4x4);
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

		/* Mask */
		/* World Matrix Change */
		_matrix saveMatrix = {};
		_float4x4 myFloat4x4 = {};
		saveMatrix = LerpMatrix(XMLoadFloat4x4(&m_SavePos[m_iColorCurNum]), XMLoadFloat4x4(&m_SavePos[m_iColorCurNum + 1]), fRatio);
		XMStoreFloat4x4(&myFloat4x4, saveMatrix);
		m_pTransformCom->Set_WorldMatrix(myFloat4x4);
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
		m_pTransformCom->Set_WorldMatrix(m_SavePos[m_iColorMaxNum]);

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
		_matrix saveMatrix = {};
		_float4x4 myFloat4x4 = {};
		saveMatrix = LerpMatrix(XMLoadFloat4x4(&m_SavePos[m_iColorCurNum]), XMLoadFloat4x4(&m_SavePos[m_iColorCurNum + 1]), fRatio);
		XMStoreFloat4x4(&myFloat4x4, saveMatrix);
		m_pTransformCom->Set_WorldMatrix(myFloat4x4);

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
			result.r[i].m128_f32[j] = A.r[i].m128_f32[j] + t * (B.r[i].m128_f32[j] - A.r[i].m128_f32[j]);
	}

	return result;
}

void CCustomize_UI::Frame_Stop(_bool _stop)
{
	if (true == _stop)
	{
		m_isKeepPlay = true;
		return;
	}
	else
		m_iColorCurNum = 0;
}

void CCustomize_UI::Frame_Reset()
{
	/* ▶ 만약 Play 멈추고 싶다면 m_isKeepPlay 함수를 사용하면 된다.*/
	if (true == m_isKeepPlay)
	{
		m_iColorCurNum = m_iColorMaxNum;
		return;
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

void CCustomize_UI::Move(_float3 fMoveMent)
{
	_vector	vPosition = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
	vPosition += XMLoadFloat3(&fMoveMent);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

	for (auto& iter : m_vecTextBoxes)
		iter->Move(fMoveMent);

	for (auto& iter : m_vecChildUI)
		static_cast<CCustomize_UI*>(iter)->Move(fMoveMent);
}

void CCustomize_UI::Set_Position(_vector vPos)
{
	_vector vprePos = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	_float3 fMovement = {};
	XMStoreFloat3(&fMovement, vPos - vprePos);
	fMovement.z = 0.f;

	for (auto& iter : m_vecTextBoxes)
		iter->Move(fMovement);

	for (auto& iter : m_vecChildUI)
		static_cast<CCustomize_UI*>(iter)->Move(fMovement);
}

void CCustomize_UI::PushBack_Child(CGameObject* pGameOBJ)
{
	m_vecChildUI.push_back(pGameOBJ);
	Safe_AddRef(pGameOBJ);
	static_cast<CCustomize_UI*>(pGameOBJ)->Set_IsChild(true);
}

void CCustomize_UI::PushBack_TextBox(CGameObject* pGameOBJ)
{
	if (nullptr == pGameOBJ)
		return;

	m_vecTextBoxes.push_back(static_cast<CTextBox*>(pGameOBJ));
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

	Safe_Release(m_pPlayer);

}

HRESULT CCustomize_UI::CreatUI_FromDat(ifstream& inputFileStream, CGameObject* pGameParentsObj, wstring PrototypeTag, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCustomize_UI::CUSTOM_UI_DESC CustomizeUIDesc;

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.isLoad), sizeof(_bool));

	_char DefaultTexturePath[MAX_PATH] = "";
	_char DefaultTextureTag[MAX_PATH] = "";
	_char MaskTexturePath[MAX_PATH] = "";
	_char MaskTextureTag[MAX_PATH] = "";

	inputFileStream.read(reinterpret_cast<_char*>(DefaultTexturePath), sizeof(_char) * MAX_PATH);
	inputFileStream.read(reinterpret_cast<_char*>(DefaultTextureTag), sizeof(_char) * MAX_PATH);
	inputFileStream.read(reinterpret_cast<_char*>(MaskTexturePath), sizeof(_char) * MAX_PATH);
	inputFileStream.read(reinterpret_cast<_char*>(MaskTextureTag), sizeof(_char) * MAX_PATH);

	CustomizeUIDesc.wstrDefaultTexturPath = wstring(DefaultTexturePath, DefaultTexturePath + strlen(DefaultTexturePath));
	CustomizeUIDesc.wstrDefaultTexturComTag = wstring(DefaultTextureTag, DefaultTextureTag + strlen(DefaultTextureTag));
	CustomizeUIDesc.wstrMaskPath = wstring(MaskTexturePath, MaskTexturePath + strlen(MaskTexturePath));
	CustomizeUIDesc.wstrMaskComTag = wstring(MaskTextureTag, MaskTextureTag + strlen(MaskTextureTag));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.worldMatrix), sizeof(_float4x4));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.vSize), sizeof(_float2));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.isPlay), sizeof(_bool));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.fColorTimer_Limit), sizeof(_float));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.iEndingType), sizeof(_int));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.fMaxFrame), sizeof(_float));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.isFrame), sizeof(_bool));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.isLoopStart), sizeof(_bool));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.isLoop), sizeof(_bool));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.isLoopStop), sizeof(_bool));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.ReStart), sizeof(_bool));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.iColorMaxNum), sizeof(_uint));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.iTextBoxCount), sizeof(_uint));

	for (_uint i = 0; i <= CustomizeUIDesc.iColorMaxNum; i++)
	{
		inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.SavePos[i]), sizeof(_float4x4));

		inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.vColor[i]), sizeof(CCustomize_UI::Value_Color));

		inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.Mask[i]), sizeof(CCustomize_UI::Value_Mask));
	}

	for (_uint i = 0; i < CustomizeUIDesc.iTextBoxCount; i++)
	{
		CTextBox::TextBox_DESC TextBoxDesc = {};

		_tchar FontString[MAX_PATH] = L"";

		_char FontType[MAX_PATH] = "";

		inputFileStream.read(reinterpret_cast<_char*>(FontString), sizeof(_tchar) * MAX_PATH);

		inputFileStream.read(reinterpret_cast<_char*>(FontType), sizeof(_char) * MAX_PATH);

		TextBoxDesc.wstrText = FontString;

		TextBoxDesc.wstrFontType = wstring(FontType, FontType + strlen(FontType));

		inputFileStream.read(reinterpret_cast<_char*>(&TextBoxDesc.vFontColor), sizeof(_vector));

		inputFileStream.read(reinterpret_cast<_char*>(&TextBoxDesc.iFontSize), sizeof(_uint));

		inputFileStream.read(reinterpret_cast<_char*>(&TextBoxDesc.vPos), sizeof(_float3));

		inputFileStream.read(reinterpret_cast<_char*>(&TextBoxDesc.vSize), sizeof(_float2));

		inputFileStream.read(reinterpret_cast<_char*>(&TextBoxDesc.isOuterLine), sizeof(_bool));

		inputFileStream.read(reinterpret_cast<_char*>(&TextBoxDesc.vOutLineColor), sizeof(_vector));

		CustomizeUIDesc.vecTextBoxDesc.push_back(TextBoxDesc);
	}

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.IsChild), sizeof(_bool));

	if (0 == CustomizeUIDesc.fMaxFrame && TEXT("") != CustomizeUIDesc.wstrDefaultTexturPath)
	{
		/* For.Prototype_Component_Texture_ */
		if (FAILED(CGameInstance::Get_Instance()->Add_Prototype(g_Level, CustomizeUIDesc.wstrDefaultTexturComTag,
			CTexture::Create(pDevice, pContext, CustomizeUIDesc.wstrDefaultTexturPath)))) {
			int a = 0;
		}
	}


	else if (0 < CustomizeUIDesc.fMaxFrame && TEXT("") != CustomizeUIDesc.wstrDefaultTexturPath)
	{
		/* For.Prototype_Component_Texture_ */
		if (FAILED(CGameInstance::Get_Instance()->Add_Prototype(g_Level, CustomizeUIDesc.wstrDefaultTexturComTag,
			CTexture::Create(pDevice, pContext, CustomizeUIDesc.wstrDefaultTexturPath, (_uint)CustomizeUIDesc.fMaxFrame)))) {
			int a = 0;
		}
	}

	if (TEXT("") != CustomizeUIDesc.wstrMaskPath)
	{
		/* For.Prototype_Component_Texture_ */
		if (FAILED(CGameInstance::Get_Instance()->Add_Prototype(g_Level, CustomizeUIDesc.wstrMaskComTag,
			CTexture::Create(pDevice, pContext, CustomizeUIDesc.wstrMaskPath)))) {
			int a = 0;
		}
	}

	//객체 생성
	if (FAILED(CGameInstance::Get_Instance()->Add_Clone(g_Level, TEXT("Layer_UI"), PrototypeTag, &CustomizeUIDesc)))
	{
		MSG_BOX(TEXT("Failed to Add Clone UI"));
		return E_FAIL;
	}

	CGameObject* pGameObj = CGameInstance::Get_Instance()->Find_Layer(g_Level, TEXT("Layer_UI"))->back();

	if (nullptr != pGameParentsObj)
	{
		static_cast<CCustomize_UI*>(pGameParentsObj)->PushBack_Child(pGameObj);
	}

	/*자식이 있으면 자식 정보 read*/

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.iChild), sizeof(_int));

	if (true == 0 < CustomizeUIDesc.iChild)
	{
		for (_int i = 0; i < CustomizeUIDesc.iChild; i++)
		{
			if (FAILED(CreatUI_FromDat(inputFileStream, pGameObj, PrototypeTag, pDevice, pContext)))
				return E_FAIL;
		}
	}

	if (nullptr == pGameParentsObj)
	{
		inputFileStream.close();
		return S_OK;
	}


	return S_OK;
}

HRESULT CCustomize_UI::CreatUI_FromDat(ifstream& inputFileStream, CGameObject* pGameParentsObj, wstring PrototypeTag, CGameObject** ppOut, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCustomize_UI::CUSTOM_UI_DESC CustomizeUIDesc;

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.isLoad), sizeof(_bool));

	_char DefaultTexturePath[MAX_PATH] = "";
	_char DefaultTextureTag[MAX_PATH] = "";
	_char MaskTexturePath[MAX_PATH] = "";
	_char MaskTextureTag[MAX_PATH] = "";

	inputFileStream.read(reinterpret_cast<_char*>(DefaultTexturePath), sizeof(_char) * MAX_PATH);
	inputFileStream.read(reinterpret_cast<_char*>(DefaultTextureTag), sizeof(_char) * MAX_PATH);
	inputFileStream.read(reinterpret_cast<_char*>(MaskTexturePath), sizeof(_char) * MAX_PATH);
	inputFileStream.read(reinterpret_cast<_char*>(MaskTextureTag), sizeof(_char) * MAX_PATH);

	CustomizeUIDesc.wstrDefaultTexturPath = wstring(DefaultTexturePath, DefaultTexturePath + strlen(DefaultTexturePath));
	CustomizeUIDesc.wstrDefaultTexturComTag = wstring(DefaultTextureTag, DefaultTextureTag + strlen(DefaultTextureTag));
	CustomizeUIDesc.wstrMaskPath = wstring(MaskTexturePath, MaskTexturePath + strlen(MaskTexturePath));
	CustomizeUIDesc.wstrMaskComTag = wstring(MaskTextureTag, MaskTextureTag + strlen(MaskTextureTag));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.worldMatrix), sizeof(_float4x4));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.vSize), sizeof(_float2));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.isPlay), sizeof(_bool));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.fColorTimer_Limit), sizeof(_float));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.iEndingType), sizeof(_int));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.fMaxFrame), sizeof(_float));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.isFrame), sizeof(_bool));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.isLoopStart), sizeof(_bool));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.isLoop), sizeof(_bool));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.isLoopStop), sizeof(_bool));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.ReStart), sizeof(_bool));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.iColorMaxNum), sizeof(_uint));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.iTextBoxCount), sizeof(_uint));

	for (_uint i = 0; i <= CustomizeUIDesc.iColorMaxNum; i++)
	{
		inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.SavePos[i]), sizeof(_float4x4));

		inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.vColor[i]), sizeof(CCustomize_UI::Value_Color));

		inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.Mask[i]), sizeof(CCustomize_UI::Value_Mask));
	}

	for (_uint i = 0; i < CustomizeUIDesc.iTextBoxCount; i++)
	{
		CTextBox::TextBox_DESC TextBoxDesc = {};

		_tchar FontString[MAX_PATH] = L"";

		_char FontType[MAX_PATH] = "";

		inputFileStream.read(reinterpret_cast<_char*>(FontString), sizeof(_tchar) * MAX_PATH);

		inputFileStream.read(reinterpret_cast<_char*>(FontType), sizeof(_char) * MAX_PATH);

		TextBoxDesc.wstrText = FontString;

		TextBoxDesc.wstrFontType = wstring(FontType, FontType + strlen(FontType));

		inputFileStream.read(reinterpret_cast<_char*>(&TextBoxDesc.vFontColor), sizeof(_vector));

		inputFileStream.read(reinterpret_cast<_char*>(&TextBoxDesc.iFontSize), sizeof(_uint));

		inputFileStream.read(reinterpret_cast<_char*>(&TextBoxDesc.vPos), sizeof(_float3));

		inputFileStream.read(reinterpret_cast<_char*>(&TextBoxDesc.vSize), sizeof(_float2));

		inputFileStream.read(reinterpret_cast<_char*>(&TextBoxDesc.isOuterLine), sizeof(_bool));

		inputFileStream.read(reinterpret_cast<_char*>(&TextBoxDesc.vOutLineColor), sizeof(_vector));

		CustomizeUIDesc.vecTextBoxDesc.push_back(TextBoxDesc);
	}

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.IsChild), sizeof(_bool));

	if (0 == CustomizeUIDesc.fMaxFrame && TEXT("") != CustomizeUIDesc.wstrDefaultTexturPath)
	{
		/* For.Prototype_Component_Texture_ */
		if (FAILED(CGameInstance::Get_Instance()->Add_Prototype(g_Level, CustomizeUIDesc.wstrDefaultTexturComTag,
			CTexture::Create(pDevice, pContext, CustomizeUIDesc.wstrDefaultTexturPath)))) {
			int a = 0;
		}
	}


	else if (0 < CustomizeUIDesc.fMaxFrame && TEXT("") != CustomizeUIDesc.wstrDefaultTexturPath)
	{
		/* For.Prototype_Component_Texture_ */
		if (FAILED(CGameInstance::Get_Instance()->Add_Prototype(g_Level, CustomizeUIDesc.wstrDefaultTexturComTag,
			CTexture::Create(pDevice, pContext, CustomizeUIDesc.wstrDefaultTexturPath, (_uint)CustomizeUIDesc.fMaxFrame)))) {
			int a = 0;
		}
	}

	if (TEXT("") != CustomizeUIDesc.wstrMaskPath)
	{
		/* For.Prototype_Component_Texture_ */
		if (FAILED(CGameInstance::Get_Instance()->Add_Prototype(g_Level, CustomizeUIDesc.wstrMaskComTag,
			CTexture::Create(pDevice, pContext, CustomizeUIDesc.wstrMaskPath)))) {
			int a = 0;
		}
	}

	//객체 생성
	if (FAILED(CGameInstance::Get_Instance()->Add_Clone(g_Level, TEXT("Layer_UI"), PrototypeTag, &CustomizeUIDesc)))
	{
		MSG_BOX(TEXT("Failed to Add Clone UI"));
		return E_FAIL;
	}

	CGameObject* pGameObj = CGameInstance::Get_Instance()->Find_Layer(g_Level, TEXT("Layer_UI"))->back();

	if (nullptr != pGameParentsObj)
	{
		static_cast<CCustomize_UI*>(pGameParentsObj)->PushBack_Child(pGameObj);
	}

	/*자식이 있으면 자식 정보 read*/

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.iChild), sizeof(_int));

	if (true == 0 < CustomizeUIDesc.iChild)
	{
		for (_int i = 0; i < CustomizeUIDesc.iChild; i++)
		{
			if (FAILED(CreatUI_FromDat(inputFileStream, pGameObj, PrototypeTag, ppOut, pDevice, pContext)))
				return E_FAIL;
		}
	}

	if (nullptr == pGameParentsObj)
	{
		inputFileStream.close();
		*ppOut = pGameObj;
		return S_OK;
	}


	return S_OK;
}

void CCustomize_UI::ExtractData_FromDat(ifstream& inputFileStream, vector<CUSTOM_UI_DESC>* pvecdesc, _bool IsFirst)
{
	CCustomize_UI::CUSTOM_UI_DESC CustomizeUIDesc;

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.isLoad), sizeof(_bool));

	_char DefaultTexturePath[MAX_PATH] = "";
	_char DefaultTextureTag[MAX_PATH] = "";
	_char MaskTexturePath[MAX_PATH] = "";
	_char MaskTextureTag[MAX_PATH] = "";

	inputFileStream.read(reinterpret_cast<_char*>(DefaultTexturePath), sizeof(_char) * MAX_PATH);
	inputFileStream.read(reinterpret_cast<_char*>(DefaultTextureTag), sizeof(_char) * MAX_PATH);
	inputFileStream.read(reinterpret_cast<_char*>(MaskTexturePath), sizeof(_char) * MAX_PATH);
	inputFileStream.read(reinterpret_cast<_char*>(MaskTextureTag), sizeof(_char) * MAX_PATH);

	CustomizeUIDesc.wstrDefaultTexturPath = wstring(DefaultTexturePath, DefaultTexturePath + strlen(DefaultTexturePath));
	CustomizeUIDesc.wstrDefaultTexturComTag = wstring(DefaultTextureTag, DefaultTextureTag + strlen(DefaultTextureTag));
	CustomizeUIDesc.wstrMaskPath = wstring(MaskTexturePath, MaskTexturePath + strlen(MaskTexturePath));
	CustomizeUIDesc.wstrMaskComTag = wstring(MaskTextureTag, MaskTextureTag + strlen(MaskTextureTag));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.worldMatrix), sizeof(_float4x4));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.vSize), sizeof(_float2));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.isPlay), sizeof(_bool));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.fColorTimer_Limit), sizeof(_float));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.iEndingType), sizeof(_int));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.fMaxFrame), sizeof(_float));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.isFrame), sizeof(_bool));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.isLoopStart), sizeof(_bool));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.isLoop), sizeof(_bool));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.isLoopStop), sizeof(_bool));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.ReStart), sizeof(_bool));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.iColorMaxNum), sizeof(_uint));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.iTextBoxCount), sizeof(_uint));

	for (_uint i = 0; i <= CustomizeUIDesc.iColorMaxNum; i++)
	{
		inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.SavePos[i]), sizeof(_float4x4));

		inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.vColor[i]), sizeof(CCustomize_UI::Value_Color));

		inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.Mask[i]), sizeof(CCustomize_UI::Value_Mask));
	}

	for (_uint i = 0; i < CustomizeUIDesc.iTextBoxCount; i++)
	{
		CTextBox::TextBox_DESC TextBoxDesc = {};

		_tchar FontString[MAX_PATH] = L"";

		_char FontType[MAX_PATH] = "";

		inputFileStream.read(reinterpret_cast<_char*>(FontString), sizeof(_tchar) * MAX_PATH);

		inputFileStream.read(reinterpret_cast<_char*>(FontType), sizeof(_char) * MAX_PATH);

		TextBoxDesc.wstrText = FontString;

		TextBoxDesc.wstrFontType = wstring(FontType, FontType + strlen(FontType));

		inputFileStream.read(reinterpret_cast<_char*>(&TextBoxDesc.vFontColor), sizeof(_vector));

		inputFileStream.read(reinterpret_cast<_char*>(&TextBoxDesc.iFontSize), sizeof(_uint));

		inputFileStream.read(reinterpret_cast<_char*>(&TextBoxDesc.vPos), sizeof(_float3));

		inputFileStream.read(reinterpret_cast<_char*>(&TextBoxDesc.vSize), sizeof(_float2));

		inputFileStream.read(reinterpret_cast<_char*>(&TextBoxDesc.isOuterLine), sizeof(_bool));

		inputFileStream.read(reinterpret_cast<_char*>(&TextBoxDesc.vOutLineColor), sizeof(_vector));

		CustomizeUIDesc.vecTextBoxDesc.push_back(TextBoxDesc);
	}

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.IsChild), sizeof(_bool));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.iChild), sizeof(_int));

	pvecdesc->push_back(CustomizeUIDesc);

	if (true == 0 < CustomizeUIDesc.iChild)
	{
		for (_int i = 0; i < CustomizeUIDesc.iChild; i++)
		{
			ExtractData_FromDat(inputFileStream, pvecdesc, false);
		}
	}

	if (true == IsFirst)
	{
		inputFileStream.close();
	}
}
