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
private:
	enum OPERATION_TYPE{EXAMIN, PICKUPITEM, SECON_PICKUPITEM, OPER_TYPE_END};
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

public:
	void Set_Operation(UI_OPERRATION eOperation, ITEM_NUMBER eCallItemType, _uint iOperateType);
	void Set_ScaleByItemNum(ITEM_NUMBER eCallItemType);
	void Set_Weapon_Accessories(ITEM_NUMBER eCallItemType, _uint iAccessories);
	
private:
	vector<CModel*>		m_vecModelCom;
	vector<wstring>		m_vecModelTag;
	CShader*			m_pShaderCom = { nullptr };
	class CCamera_Free*	m_pCameraFree = { nullptr };

private:
	ITEM_NUMBER			m_eItem_Number = { ITEM_NUMBER_END };
	UI_OPERRATION		m_eViewer_State = { STATE_END };
	OPERATION_TYPE		m_eOperType = { OPER_TYPE_END };


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

	_float4x4			m_matMoveCenter = {};

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