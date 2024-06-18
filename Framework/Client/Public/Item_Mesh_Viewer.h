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
	enum VIEWER_STATE { POP_UP, IDLE, HIDE, STATE_END };

private:
	CItem_Mesh_Viewer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItem_Mesh_Viewer(const CItem_Mesh_Viewer& rhs);
	virtual ~CItem_Mesh_Viewer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	vector<CModel*>		m_vecModelCom;
	CShader*			m_pShaderCom = { nullptr };
	class CCamera_Free*	m_pCameraFree = { nullptr };

private:
	ITEM_NUMBER			m_eItem_Number = { ITEM_NUMBER_END };
	VIEWER_STATE		m_eViewer_State = { STATE_END };

	_float				m_fDistCam = { 0.6f };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CItem_Mesh_Viewer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END