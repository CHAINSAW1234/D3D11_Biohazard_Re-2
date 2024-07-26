#pragma once

#include "Client_Defines.h"
#include "Cut_Scene.h"

BEGIN(Client)

class CCut_Scene_CF120 final : public CCut_Scene
{
public:
	enum class CF120_ACTOR_TYPE { _PL_0000, _PL_5700, _SM42_198, _END };

private:
	CCut_Scene_CF120(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCut_Scene_CF120(const CCut_Scene_CF120& rhs);
	virtual ~CCut_Scene_CF120() = default;

public:
	virtual HRESULT								Initialize_Prototype() override;
	virtual HRESULT								Initialize(void* pArg) override;

public:
	virtual void								Priority_Tick(_float fTimeDelta) override;
	virtual void								Tick(_float fTimeDelta) override;
	virtual void								Late_Tick(_float fTimeDelta) override;

private:
	virtual HRESULT								SetUp_Animation_Layer() override;

public:
	virtual void								Start_CutScene() override;
	virtual void								Finish_CutScene() override;

public:
	void										Finish_Marvine();

private:
	virtual HRESULT								Add_Actors() override;
	virtual HRESULT								Add_Props() override;
	virtual	HRESULT								Add_Camera_Event() override;

public:
	static CCut_Scene_CF120* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual	void Free();
};

END