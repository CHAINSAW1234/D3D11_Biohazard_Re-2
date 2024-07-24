#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
END

BEGIN(Client)

class CItem_Mesh_Viewer final : public CGameObject
{
public:
	enum OPERATION_TYPE { EXAMIN, PICKUPITEM, SECON_PICKUPITEM, EXAMIN_PUZZLE, PUZZLE_SUCCESS ,OPER_TYPE_END };

private:
	enum BUTTON_STATE { BUTTON_IDLE, PRESSED, BUTTON_STATE_END };
	enum LAMP_STATE { LAMP_IDLE, SUCCEED, FAILED, LAMP_STATE_END };
	enum PUZZLE_PROGRESS { PUZZLE_IDLE, PUZZLE_SUCCEEC, PUZZLE_FAILED, PUZZLE_PROGRESS_END };

private:
	CItem_Mesh_Viewer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItem_Mesh_Viewer(const CItem_Mesh_Viewer& rhs);
	virtual ~CItem_Mesh_Viewer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Start() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	void PopUp_Operation(_float fTimeDelta);
	void Idle_Operation(_float fTimeDelta);
	void Hide_Operation(_float fTimeDelta);
	
	//-1:실패 0:진행 1:성공
	_int Check_Puzzle_Success(); 
	void PS_Button_Resset();

public:
	void Set_Operation(UI_OPERRATION eOperation, ITEM_NUMBER eCallItemType, _uint iOperateType);
	void Set_ScaleByItemNum(ITEM_NUMBER eCallItemType);
	void Set_Weapon_Accessories(ITEM_NUMBER eCallItemType, _uint iAccessories);
	OPERATION_TYPE Get_OperationType() const { return m_eOperType; }

	
private:
	vector<CModel*>		m_vecModelCom;
	vector<wstring>		m_vecModelTag;
	CShader*			m_pShaderCom = { nullptr };
	CShader*			m_pAnimShaderCom = { nullptr };
	class CCamera_Free*	m_pCameraFree = { nullptr };

private:
	ITEM_NUMBER			m_eItem_Number = { ITEM_NUMBER_END };
	UI_OPERRATION		m_eViewer_State = { STATE_END };
	OPERATION_TYPE		m_eOperType = { OPER_TYPE_END };
	PUZZLE_PROGRESS		m_ePZ_Progress = { PUZZLE_PROGRESS_END };

	_float				m_fDistCamZ = { 0.f };
	_float				m_fDistCamX = { 0.f };
	
	_float				m_fCurSize = { 0.f };
	_float				m_fStartSize = { 0.f };
	_float				m_fEndSize = { 0.f };

	_float				m_fPopupHide_CurTime = { 0.f };
	_float				m_fPopupHide_EndDist = { 0.4f };
	_float				m_fPickupIdle_StartDist = { 0.f };
	_float				m_fPickupHide_StartDist = { 0.f };

	_bool				m_bStop = { true };

	_float				m_fPreRadian = { 0.f };

	_float4x4			m_matMoveCenter = {};

	_int				m_iSelected_Button = { -1 };
	_uint				m_iPuzzle_Progress = 0;
	_uint				m_iProgressStartPoint = 0;


	string				m_LampTags[8];
	string				m_ButtonTags[8];
	string				m_ButtonBoneTags[8];

	_uint				m_iInputAnswer[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

	_uint				m_iCorrectAnswer[8] = {0, 1, 2, 3, 4, 5, 6, 7};

	BUTTON_STATE		m_eButtonStates[8] = { BUTTON_STATE_END ,BUTTON_STATE_END ,BUTTON_STATE_END ,BUTTON_STATE_END ,BUTTON_STATE_END ,BUTTON_STATE_END ,BUTTON_STATE_END ,BUTTON_STATE_END };
	LAMP_STATE			m_eLampStates[8] = { LAMP_STATE_END ,LAMP_STATE_END ,LAMP_STATE_END ,LAMP_STATE_END ,LAMP_STATE_END ,LAMP_STATE_END ,LAMP_STATE_END ,LAMP_STATE_END };

private:
	HRESULT Add_Components();
	HRESULT Load_ItemsModel();
	HRESULT Bind_ShaderResources();
	HRESULT Load_ItemModelTags();

public:
	static CItem_Mesh_Viewer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END