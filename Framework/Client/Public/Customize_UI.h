#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "TextBox.h"

BEGIN(Client)

class CCustomize_UI final : public CUI
{
public:
	enum PLAY_BUTTON { PLAY_DEFAULT, PLAY_CHANGE, PLAY_CUT, PLAY_BUTTON_END };

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

	typedef struct CUSTOM_UI_DESC : public CUI::UI_DESC
	{
		wstring							strTexturePath;
		wstring							strTextureComTag;

		_uint							iColorMaxNum = { 0 };
		Value_Color						vColor[10] = {};
		_bool							isPlay = { false };
		_float							fColorTimer_Limit = {0.f};
		_int							iEndingType = {0};
		_int							iTextBox = { 0 };
		_int							iChild = { 0 };
		_bool							IsChild = { false };
		vector<CTextBox::TextBox_DESC>	TextBoxDesc = {};
		_float4x4						SavePos[10] = {};
		_bool							isMask = {};

	}CUSTOM_UI_DESC;

private:
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

private:
	HRESULT Add_Components(const wstring& strModelTag);
	HRESULT Bind_ShaderResources();

private: /* Frame */
	void Color_Frame(_float fTimeDelta);
	void State_Control(_float fTimeDelta);
	void Frame_Defalut(_float fRatio, _float fColorRatio);
	void Frame_Change(_float fRatio, _float fColorRatio);
	void Frame_Cut(_float fRatio, _float fColorRatio);
	_matrix LerpMatrix(_matrix A, _matrix B, _float t); // ���� �� ���

public :
	//HRESULT	Change_Texture(const wstring& strPrototypeTag, const wstring& strTexturePath);

public:
	void PushBack_Child(CGameObject* pGameOBJ);
	void PushBack_TextBox(CGameObject* pGameOBJ);
	CUSTOM_UI_DESC Get_Cutomize_DESC()const;
	void Release_Children();
	void Release_Child(CGameObject* Child);
	CGameObject* Find_Child(CGameObject* Child);
	_bool IsMyChild(CGameObject* Child);


public:// for. Set inline
	void Set_IsChild(_bool IsChild) { m_IsChild = IsChild; }

	/* �÷��� �� � ��� �ִ°�? */
	void Set_MaxColor(_uint _color) { m_iColorMaxNum = _color; }

	void Set_TextureNum(_uint iTextureNum) { m_iTextureNum = iTextureNum; }

	/* �÷��� ���� ���ǵ�*/
	void Set_ColorSpeed(_float _speed) { m_fColorSpeed = _speed; }

	/* Ÿ�̸� ����*/
	void Set_Interver_Timer(_float _timer) { m_fColorTimer_Limit = _timer; }

	/* ���� Ÿ�̸�*/
	void Set_ColorTimer(_float _timer) { m_fCurrentColor_Timer = _timer; }

	void Set_Wave(_bool _wave, _float _speed){
		m_isWave = _wave;
		m_fWaveSpeed = _speed;
	}

	// �÷��� ���� �迭 ũ��
	void isColor_Size(_uint _size, Value_Color _desc, _int _ending){
		m_vColor[_size].vColor = _desc.vColor;
		m_vColor[_size].fBlender_Value = _desc.fBlender_Value;
		m_vColor[_size].isBlender = _desc.isBlender;

		m_vColor[_size].isColorChange = _desc.isColorChange;
		m_vColor[_size].isAlphaChange = _desc.isAlphaChange;

		m_vColor[_size].WaveSpeed = _desc.WaveSpeed;
		m_vColor[_size].isWave = _desc.isWave;

		m_vColor[_size].isPush = _desc.isPush;
		m_vColor[_size].fPushSpeed = _desc.fPushSpeed;
		m_vColor[_size].fPushRotation = _desc.fPushRotation;
		m_vColor[_size].fSplit = _desc.fSplit;

		m_iEndingType = _ending;
	}

	/* ������ ���� �� ���� �����鵵 ��������� �� */
	void Set_Value_Color(Value_Color* _value)
	{
		m_vCurrentColor = _value->vColor;
		m_fBlending = _value->fBlender_Value;
		m_isBlending = _value->isBlender;

		m_isColorChange = _value->isColorChange;
		m_isAlphaChange = _value->isAlphaChange;
		m_isWave = _value->isWave;
		m_fWaveSpeed = _value->isWave;

		m_isPush = _value->isPush;
		m_fPush_Speed = _value->fPushSpeed;
		m_isUVRotation = _value->fPushRotation;
		m_fSplit = _value->fSplit;
	}

	void Set_EndingType(_int i) { m_iEndingType = i; }

	/* �÷� ����� ���ΰ�? */
	void Set_ColorPlay(_uint _play) { m_isPlay = _play; }

	void Set_Color(_bool _color) {
		m_isColorChange = _color;
	}

	void Set_Alpha(_bool _alpha) {
		m_isAlphaChange = _alpha;
	}


	/* ���� ������ ����*/
	void Set_EditColor(_float4 _color, _bool _bender, _float _blending = 0.f) {

		m_vCurrentColor = _color;
		m_isBlending = _bender;

		if (m_isBlending)
			m_fBlending = _blending;
	}

	/* UV�� �� ���ΰ�? */
	void Set_PushUV(_bool _push, _float _speedX, _float _speedY, _float _rotation, _float _split = 1) {
		m_isPush = _push;
		m_fPush_Speed = _float2(_speedX, _speedY);
		m_isUVRotation = _rotation;
		m_fSplit = _split;
	}

	void ColorChange(_float4 _color, _uint i) {
		m_isColorChange = true;
		m_vColor[i].vColor = _color;
	}

	void ColorChange_Stop() {
		m_isColorChange = false;
		m_isAlphaChange = false;
		m_isBlending = false;
	}

	/* Transform World Matrix �Ѱ��� : �̰� ���� */
	void Set_StoreTransform(_uint i, _float4x4 _trans) { m_SavePos[i] = _trans; }

	void Set_UIID(UI_ID _eID) { m_eUI_ID = _eID; }

public:/* for.Get Inline */
	/* ���� Ÿ�̸�*/
	_float Get_ColorTimer() const { return m_fCurrentColor_Timer; }
	/* Ÿ�̸� ����*/
	_float Get_Interver_Timer() const { return m_fColorTimer_Limit; }
	/* ������*/
	Value_Color Get_Value_Color(_uint i) const { return m_vColor[i]; }

	vector<CGameObject*>* Get_Child() { return &m_vecChildUI; }

	_bool Get_IsChild() const { return m_IsChild; }

	vector<class CTextBox*>* Get_vecTextBoxes() { return &m_vecTextBoxes; }

	UI_ID Get_UIID() const { return m_eUI_ID; }

	_float Get_CurrentRatioTime() const { return m_fCurrentColor_Timer; }

	_int  Get_MaxSize() const { return m_iColorMaxNum; }

	_int Get_EndingType() const { return m_iEndingType; }

	Value_Color* Get_Current_Value() const
	{
		Value_Color _desc = {};

		_desc.vColor = m_vCurrentColor;
		_desc.fBlender_Value = m_fBlending;
		_desc.isBlender = m_isBlending;

		_desc.isColorChange = m_isColorChange;
		_desc.isAlphaChange = m_isAlphaChange;
		_desc.isWave = m_isWave;
		_desc.WaveSpeed = m_fWaveSpeed;

		_desc.isPush = m_isPush ;
		_desc.fPushSpeed = m_fPush_Speed;
		_desc.fPushRotation = m_isUVRotation ;
		_desc.fSplit = m_fSplit;

		return (&_desc);
	}

private:
	_int						m_iTextBox		= { 0 };
	vector<class CTextBox*>		m_vecTextBoxes;

private:
	_bool						m_IsChild		= { false };//�� �ڽ��̳�..?
	vector<CGameObject*>		m_vecChildUI;

private:
	wstring						m_strTexturePath = { L"" };
	wstring						m_strTextureComTag = { L"" };
	_uint						m_iTextureNum	= { 0 };
	_uint						m_iShaderPassNum = { 0 };
	
private : /* NY : Shader ���� */
	Value_Color					m_vColor[10]			= {};		// ���� Edit �󿡼� �������� �÷�
	_float4x4					m_SavePos[10]			= {};

	_float						m_fColorTimer_Limit		= { 0.f };	// �÷� change ���� �ð�
	_float						m_fCurrentColor_Timer	= { 0.f };	// �÷� ���� �ð�
	_float4						m_vCurrentColor			= {};		// ���� Edit ����
	_float						m_fColorSpeed			= { 0.f };

	_int						m_iColorMaxNum			= { -1 };
	_uint						m_iColorCurNum			= { 0 };

	_bool						m_isColorBack			= { false }; // Color �ǰ���
	_bool						m_isPlay				= { false };

	// Shader ����
	_bool						m_isSelect_Color		= { false };

private : /* 1.Color */
	_bool						m_isColorChange			= { false };
	_bool						m_isAlphaChange			= { false };
	_bool						m_isBlending			= { false };
	_float						m_fBlending				= { 0.f };

private : /* 2. Wave*/
	_bool						m_isWave				= { false };
	_float						m_isWaveTimer			= { 0.f };
	_float						m_fWaveSpeed			= { 0.f };

private : /* 3. Push */
	_bool						m_isPush				= { false };
	_float						m_fPush_Timer			= { 0.f };
	_float2						m_fPush_Speed			= { 0.f, 0.f };
	_float						m_fSplit				= { 0.f };
	_float						m_isUVRotation			= { 0.f };

	_int						m_iEndingType			= { 0 };

	_float						m_fMaskTimer			= { 0 };

private : /* Client Variable */
	_bool						m_isMask				= { false };

public:
	static CCustomize_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END