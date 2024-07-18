#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CProp_Controller abstract : public CGameObject
{
public:
	typedef struct tagPropControllDesc
	{
		wstring					strAnimLayerTag = { TEXT("") };
	}PROP_CONTROLL_DESC;
protected:
	CProp_Controller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CProp_Controller(const CProp_Controller& rhs);
	virtual ~CProp_Controller() = default;


public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Priority_Tick(_float fTimeDelta) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	virtual void			Late_Initialize();

public:
	void					Start() override;

public:
	virtual HRESULT			Set_Animation(_uint iAnimIndex);
	virtual void			Set_Next_Animation_Sequence();
	virtual void			Set_Loop(_bool isLoop);

	virtual void			Reset_Animations();

	_bool					Is_Finished_Animation();
	_bool					Is_Finished_Animation_All();

	HRESULT					Sort_Animation_Seq();

public:
	CGameObject*			Get_PropObject() { return m_pPropObject; }

protected:
	CGameObject*			m_pPropObject = { nullptr };
	CModel*					m_pBodyModel = { nullptr };
	_int					m_iCallerType = { -1 };

	_uint					m_iCurSeqLev = { 0 };

	wstring					m_strAnimLayerTag = { TEXT("") };
	unordered_map<wstring, vector<string>>				m_Animations_Seq;

public:
	virtual void			Free() override;
};

END