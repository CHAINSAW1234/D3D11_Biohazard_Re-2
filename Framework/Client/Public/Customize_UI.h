#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "TextBox.h"

BEGIN(Client)

class CCustomize_UI abstract : public CUI
{
public: 
	/* Tool */
	enum PLAY_BUTTON { PLAY_DEFAULT, PLAY_CHANGE, PLAY_CUT, PLAY_BUTTON_END };

public :
	/* Client*/
	enum class ITEM_BOX_TYPE { DEFAULT_BOX, SELECT_BOX, END_BOX };
	enum class HPBAR_TYPE { MAIN_BAR, BACKGROUND_BAR, END_BAR };
	enum class INVENTORY_TYPE { MAIN_INVEN, SUB_INVEN, END_INVEN };
	
	typedef struct color
	{
		_float4		vColor = {};
		_float		fBlender_Value = {};
		_bool		isBlender = { false };
		_bool		isColorChange = { false };
		_bool		isAlphaChange = { false };

		_float		WaveSpeed = { 0.f };
		_bool		isWave = { false };

		_bool		isPush = { false };
		_float2		fPushSpeed = {};
		_float		fPushRotation = { 0.f };
		_float		fSplit = { 0.f };

	}Value_Color;

	typedef struct mask
	{
		_bool isMask = { false };
		_float2 fMaskControl = { 0.f, 0.f };
		_float fMaskSpeed = { 0.f };
		_float2 vMaskType = { 0.f, 0.f };

	}Value_Mask;

	typedef struct CUSTOM_UI_DESC : public CUI::UI_DESC
	{
		/* Tool */
		wstring							wstrDefaultTexturComTag = { TEXT("") };
		wstring							wstrDefaultTexturPath = { TEXT("") };

		wstring							wstrMaskComTag = { TEXT("") };
		wstring							wstrMaskPath = { TEXT("") };

		_uint							iColorMaxNum = { 0 };
		_bool							isPlay = { false };
		_float							fColorTimer_Limit = { 0.f };
		_int							iEndingType = { 0 };

		_float							fMaxFrame = { 0.f };
		_bool							isFrame = { false };
		_bool							isLoopStart = { false };
		_bool							isLoop = { false };
		_bool							isLoopStop = { false };
		_bool							ReStart = { false };

		Value_Color						vColor[10] = {};
		_float4x4						SavePos[10] = {};
		Value_Mask						Mask[10] = {};

		_uint							iTextBoxCount = { 0 };
		vector<CTextBox::TextBox_DESC>	vecTextBoxDesc;

		_int							iChild = { 0 };
		_bool							IsChild = { false };

		_bool							isLoad = { false };

		/* +추가 */
		_int							iWhich_Child = { 0 };
		CGameObject*					pSupervisor;
		CGameObject*					pImmediateSuperior;

		ITEM_NUMBER						eItem_Number;

		/* Client */
		ITEM_BOX_TYPE					eBox_Type;
		HPBAR_TYPE						eHPBar_Type;
		INVENTORY_TYPE					eInventory_Type;

		LOCATION_MAP_VISIT				eMapUI_Type;
		wstring							wstrFileName = { TEXT("") };
		wstring							wstrItemName = { TEXT("") };

		_int							iInven_Type;

		_uint							iDefaultTexturLevel = { 3 };

	}CUSTOM_UI_DESC;

protected :
	CCustomize_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCustomize_UI(const CCustomize_UI& rhs);
	virtual ~CCustomize_UI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT	Initialize_TextBox(void* pArg);
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected :
	HRESULT Add_Components(const wstring& wstrTextureTag, const wstring& wstrMaskTag, _uint iDefaultTexturLevel);
	HRESULT Bind_ShaderResources();
	HRESULT Change_Texture(const wstring& strPrototypeTag, const wstring& strComponentTag);

protected :
	/* Frame */
	void Non_Frame(_uint i = 0);
	void Color_Frame(_float fTimeDelta);
	void State_Control(_float fTimeDelta);
	void Frame_Defalut(_float fRatio, _float fColorRatio);
	void Frame_Change(_float fRatio, _float fColorRatio);
	void Frame_Cut(_float fRatio, _float fColorRatio);
	_matrix LerpMatrix(_matrix A, _matrix B, _float t); // 보간 값 계산

	void Frame_Stop(_bool _stop);
	void Frame_Reset();

public:
	virtual void Move(_float3 fMoveMent) override;
	virtual void Set_Position(_vector vPos) override;


public:
	virtual void Frame_Change_ValueColor(_uint iChange_FrameNum) {
		m_vCurrentColor =	m_vColor[iChange_FrameNum].vColor;
		m_fBlending =		m_vColor[iChange_FrameNum].fBlender_Value;
		m_isBlending =		m_vColor[iChange_FrameNum].isBlender;

		m_isColorChange =	m_vColor[iChange_FrameNum].isColorChange;
		m_isAlphaChange =	m_vColor[iChange_FrameNum].isAlphaChange;
		m_isWave =			m_vColor[iChange_FrameNum].isWave;
		m_fWaveSpeed =		m_vColor[iChange_FrameNum].WaveSpeed;

		m_isPush =			m_vColor[iChange_FrameNum].isPush;
		m_fPush_Speed =		m_vColor[iChange_FrameNum].fPushSpeed;
		m_isUVRotation =	m_vColor[iChange_FrameNum].fPushRotation;
		m_fSplit =			m_vColor[iChange_FrameNum].fSplit;
	}

	virtual void Child_Frame_Change_ValueColor(_uint iChild, _uint iChange_FrameNum) {
		static_cast<CCustomize_UI*>(m_vecChildUI[iChild])->Frame_Change_ValueColor(iChange_FrameNum);
	}

public:
	void PushBack_Child(CGameObject* pGameOBJ);
	void PushBack_TextBox(CGameObject* pGameOBJ);
	CUSTOM_UI_DESC Get_Cutomize_DESC()const;
	void Release_Children();
	void Release_Child(CGameObject* Child);
	CGameObject* Find_Child(CGameObject* Child);
	_bool IsMyChild(CGameObject* Child);

public :
	_bool				Select_UI();
	virtual void		Set_Dead(_bool bDead) override;
	_float				Distance_Player(CGameObject* _obj);
	_float				Distance_Player(_float4 _pos);
	void				Find_Player();


public : /* Client */
	_bool				Get_Children()		{ return m_IsChild;  }
	ITEM_BOX_TYPE		Get_ItemBox_Type()	{ return m_eBox_Type;  }
	INVENTORY_TYPE		Get_Inven_Type()	{ return m_eInventory_Type;  }


public: /* Mask */
	/* 저장할 Light State*/
	void Set_StoreMask(_uint i, Value_Mask* _mask)
	{
		m_Mask[i] = *_mask;
	}

	/* 현재 바뀌는 Light State*/
	void Set_CurrentMask(Value_Mask* _mask)
	{
		m_isMask = _mask->isMask;
		m_fMaskControl = _mask->fMaskControl;
		m_fMaskSpeed = _mask->fMaskSpeed;
		m_vMaskType = _mask->vMaskType;
	}

	Value_Mask		Get_Value_Mask(_uint i) const { return m_Mask[i]; }


public:// for. Set inline
	void Set_IsChild(_bool IsChild) { m_IsChild = IsChild; }

	/* 컬러가 총 몇개 들어 있는가? */
	void Set_MaxColor(_uint _color) { m_iColorMaxNum = _color; }

	void Set_TextureNum(_uint iTextureNum) { m_iTextureNum = iTextureNum; }

	void Set_ChildTextureNum(_uint iChildNum, _uint iTextureNum){
		static_cast<CCustomize_UI*>(m_vecChildUI[iChildNum])->Set_TextureNum(iTextureNum);
	}

	/* 컬러를 돌릴 스피드*/
	void Set_ColorSpeed(_float _speed) { m_fColorSpeed = _speed; }

	/* 타이머 간격*/
	void Set_Interver_Timer(_float _timer) { m_fColorTimer_Limit = _timer; }

	/* 현재 타이머*/
	void Set_ColorTimer(_float _timer) { m_fCurrentColor_Timer = _timer; }

	void Set_Wave(_bool _wave, _float _speed) {
		m_isWave = _wave;
		m_fWaveSpeed = _speed;
	}

	// 컬러를 넣을 배열 크기
	void isColor_Size(_uint _size, Value_Color _desc, _int _ending)
	{
		m_vColor[_size] = _desc;
		m_iEndingType = _ending;
	}

	/* 프레임 변경 시 현재 변수들도 변경해줘야 함 */
	void Set_Value_Color(Value_Color* _value)
	{
		m_vCurrentColor = _value->vColor;
		m_fBlending = _value->fBlender_Value;
		m_isBlending = _value->isBlender;

		m_isColorChange = _value->isColorChange;
		m_isAlphaChange = _value->isAlphaChange;
		m_isWave = _value->isWave;
		m_fWaveSpeed = _value->WaveSpeed;

		m_isPush = _value->isPush;
		m_fPush_Speed = _value->fPushSpeed;
		m_isUVRotation = _value->fPushRotation;
		m_fSplit = _value->fSplit;
	}

	void Set_EndingType(_int i) { m_iEndingType = i; }

	/* 컬러 재생할 것인가? */
	void Set_ColorPlay(_uint _play) { m_isPlay = _play; }

	void Set_Color(_bool _color) {
		m_isColorChange = _color;
	}

	void Set_Alpha(_bool _alpha) {
		m_isAlphaChange = _alpha;
	}


	/*// 현재 결정할 색상*/
	void Set_EditColor(_float4 _color, _bool _bender, _float _blending = 0.f) {

		m_vCurrentColor = _color;
		m_isBlending = _bender;

		if (m_isBlending)
			m_fBlending = _blending;
	}

	/* UV를 밀 것인가? */
	void Set_PushUV(_bool _push, _float _speedX, _float _speedY, _float _rotation, _float _split = 1) {
		m_isPush = _push;
		m_fPush_Speed = _float2(_speedX, _speedY);
		m_isUVRotation = _rotation;
		m_fSplit = _split;
	}

	/* 컬러만 저장하고자 할 때 */
	void ColorChange(_float4 _color, _uint i) {
		m_isColorChange = true;
		m_vColor[i].vColor = _color;
	}

	void ColorChange_Stop() {
		m_isColorChange = false;
		m_isAlphaChange = false;
		m_isBlending = false;
	}

	/* Transform World Matrix 넘겨줌 : 이거 저장 */
	void Set_StoreTransform(_uint i, _float4x4 _trans) { m_SavePos[i] = _trans; }

	void Set_UIID(UI_ID _eID) { m_eUI_ID = _eID; }

	void Set_MaxFrame(_int iMaxFrame) {
		m_fMaxFrame = iMaxFrame * 1.f;
	}

	void Set_IsMultiTex(_bool IsMultiTex) {
		m_isMultiTex = IsMultiTex;
	}

	void Set_InvenType(INVENTORY_TYPE eInvenType) { m_eInventory_Type = eInvenType; }

	void Set_IsLoad(_bool IsLoad);

	void Set_Text(_uint iTextNum, wstring wstrSetText)
	{
		m_vecTextBoxes[iTextNum]->Set_Text(wstrSetText);
	}

	void Set_MyChild_Text(_uint iChildNum, _uint iTextNum, wstring wstrSetText) {
		static_cast<CCustomize_UI*>(m_vecChildUI[iChildNum])->Set_Text(iTextNum, wstrSetText);
	}

public:/* for.Get Inline */
	_float4x4* Get_StoreTransform(_uint i) { return &m_SavePos[i]; }
	/* 현재 타이머*/
	_float Get_ColorTimer() const { return m_fCurrentColor_Timer; }
	/* 타이머 간격*/
	_float Get_Interver_Timer() const { return m_fColorTimer_Limit; }
	/* 모음집*/
	Value_Color Get_Value_Color(_uint i) const { return m_vColor[i]; }

	vector<CGameObject*>* Get_Child() { return &m_vecChildUI; }

	_bool Get_IsChild() const { return m_IsChild; }

	vector<class CTextBox*>* Get_vecTextBoxes() { return &m_vecTextBoxes; }

	UI_ID Get_UIID() const { return m_eUI_ID; }

	_float Get_CurrentRatioTime() const { return m_fCurrentColor_Timer; }

	_int  Get_MaxSize() const { return m_iColorMaxNum; }

	_int Get_EndingType() const { return m_iEndingType; }

	/* 현재 color 값*/
	Value_Color* Get_Current_ColorValue() const
	{
		Value_Color _desc = {};

		_desc.vColor = m_vCurrentColor;
		_desc.fBlender_Value = m_fBlending;
		_desc.isBlender = m_isBlending;

		_desc.isColorChange = m_isColorChange;
		_desc.isAlphaChange = m_isAlphaChange;
		_desc.isWave = m_isWave;
		_desc.WaveSpeed = m_fWaveSpeed;

		_desc.isPush = m_isPush;
		_desc.fPushSpeed = m_fPush_Speed;
		_desc.fPushRotation = m_isUVRotation;
		_desc.fSplit = m_fSplit;

		return (&_desc);
	}

	/* 현재 Mask 값*/
	Value_Mask* Get_Current_MaskValue() const
	{
		Value_Mask _desc = {};

		_desc.fMaskControl = m_fMaskControl;
		_desc.fMaskSpeed = m_fMaskSpeed;
		_desc.isMask = m_isMask;
		_desc.vMaskType = m_vMaskType;

		return (&_desc);
	}

	_int	Get_TextureNum() const { return m_iTextureNum; }
	_int	Get_ChildTextureNum(_uint iChild) { return static_cast<CCustomize_UI*>(m_vecChildUI[iChild])->Get_TextureNum(); }

public : /* Clinet */
	_bool	Get_IsRender() const { return m_isRender; }
	virtual void    Set_IsRender(_bool isRender) { m_isRender = isRender; 
	for (auto& iter : m_vecChildUI){
		static_cast<CCustomize_UI*>(iter)->Set_IsRender(isRender);}}

protected :
	vector<class CTextBox*>		m_vecTextBoxes;

protected :
	_bool						m_IsChild = { false };//나 자식이냐..?
	vector<CGameObject*>		m_vecChildUI;


protected :
	wstring						m_wstrMaskPath					= { TEXT("") }; // 텍스쳐 페스
	wstring						m_wstrMaskComTag				= { TEXT("") };
	wstring						m_wstrDefaultTexturPath			= { TEXT("") }; // 텍스쳐 페스
	wstring						m_wstrDefaultTexturComTag		= { TEXT("") };
	_uint						m_iTextureNum					= { 0 };
	_uint						m_iShaderPassNum				= { 0 };

protected :
	_uint						m_iRenderGroup = { static_cast<_uint>(CRenderer::RENDER_UI) };

#pragma region NY : Shader 변수
protected :
	_bool						m_isMovePoint	= { false };
	Value_Color					m_vColor[10]	 = {};	// 현재 Edit 상에서 보여지는 컬러
	_float4x4					m_SavePos[10]	= {};

	_float						m_fColorTimer_Limit = {};	// 컬러 change 제한 시간
	_float						m_fCurrentColor_Timer = {};	// 컬러 현재 시간
	_float4						m_vCurrentColor = {};	// 현재 Edit 색상
	_float						m_fColorSpeed = {};

	_int						m_iColorMaxNum = { -1 };
	_uint						m_iColorCurNum = { 0 };

	_bool						m_isColorBack = { false }; // Color 되감기
	_bool						m_isPlay = { false };
	_bool						m_isLoad = { false };

	// Shader 변수
	_bool						m_isColorChange = { false };
	_bool						m_isAlphaChange = { false };
	_bool						m_isBlending = { false };
	_float						m_fBlending = {};

	_bool						m_isWave = { false };
	_float						m_isWaveTimer = {};
	_float						m_fWaveSpeed = {};

	_bool						m_isPush = {};
	_float						m_fPush_Timer = {};
	_float2						m_fPush_Speed = {};
	_float						m_fSplit = {};
	_float						m_isUVRotation = {};

	_int						m_iEndingType = {};

protected :
	// Mask
	Value_Mask					m_Mask[10];
	_bool						m_isMask = {};
	_float2						m_fMaskControl = {};
	_float						m_fMaskTimer = {};
	_float						m_fMaskSpeed = {};
	_float2						m_vMaskType = {};

protected :
	/* Frame*/
	_float						m_fFrame = { 0.f };
	_float						m_fMaxFrame = { 0.f };
	_bool						m_isFrame = { false };

	_bool						m_isMultiTex = { false };

	/* Texture */
	_uint						m_iDefaultTexture = { 0 };
	_uint						m_iMaskTexture = { 0 };

	/* Loop */
	_bool						m_isLoopStart = { false };
	_bool						m_isLoop = { false };
	_bool						m_isLoopStop = { false };
	_bool						m_ReStart = { false };
#pragma endregion

	_bool						m_isMap = { false };

	CGameObject*				m_pSupervisor			= { nullptr };
	CGameObject*				m_pImmediateSuperior	= { nullptr };

protected : /* Client*/
	class CPlayer*				m_pPlayer = { nullptr };
	
	/* Render 관련*/
	_bool						m_isRender = { true };
	_float4						m_vOriginTextColor = {};
	_float4						m_vOriginColor = {};

	_int						m_iWhich_Child = { 0 }; /* 몇 번째 자식인 지를 설명할 때 사용 */

	/* 1. inventory Item */
	ITEM_BOX_TYPE				m_eBox_Type = { ITEM_BOX_TYPE::END_BOX };
	INVENTORY_TYPE				m_eInventory_Type = { INVENTORY_TYPE::END_INVEN };

	/* 2. HP Bar*/
	_bool						m_isTimerControl = { false };
	_float						m_fShiftMaskUV_X = {};
	_float4						m_vLightMask_Color;
	_bool						m_isLightMask = { false };

	/* Timer */
	_float						m_fInterpolation_Timer = {}; /* 보간 타이머 */

	/* Light */
	_float2						m_fLightPosition = {};
	_float						m_fLightSize = {};
	_bool						m_isLight = {};

	_bool						m_isKeepPlay = {};

public:
	static HRESULT CreatUI_FromDat(ifstream& inputFileStream, CGameObject* pGameParentsObj, wstring PrototypeTag, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	
	//생성한 객체를 포인터로 받고싶으면 사용하시오
	static HRESULT CreatUI_FromDat(ifstream& inputFileStream, CGameObject* pGameParentsObj, wstring PrototypeTag, CGameObject** ppOut, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	
	//복잡한 구조면 사용 못함
	static void ExtractData_FromDat(ifstream& inputFileStream, vector<CUSTOM_UI_DESC>* pvecdesc, _bool IsFirst);

	virtual CGameObject* Clone(void* pArg) override = 0;
	virtual void Free() override;
};

END