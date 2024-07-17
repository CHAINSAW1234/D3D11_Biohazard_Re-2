#pragma once

#include "Client_Defines.h"
#include "Cut_Scene.h"

BEGIN(Client)

class CCut_Scene_CF92 final : public CCut_Scene
{
public:
	enum class CF92_ACTOR_TYPE { _PL_0000, _SM_60_033, _END };

private:
	CCut_Scene_CF92(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCut_Scene_CF92(const CCut_Scene_CF92& rhs);
	virtual ~CCut_Scene_CF92() = default;

public:
	virtual HRESULT								Initialize_Prototype() override;
	virtual HRESULT								Initialize(void* pArg) override;

private:
	virtual HRESULT								SetUp_Animation_Layer() override;

public:
	virtual void								Priority_Tick(_float fTimeDelta) override;
	virtual void								Tick(_float fTimeDelta) override;
	virtual void								Late_Tick(_float fTimeDelta) override;

public:
	virtual void								Start() override;
	virtual void								Finish() override;

private:
	virtual HRESULT								Add_Actors() override;

public:
	static CCut_Scene_CF92* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual	void Free();
};

END