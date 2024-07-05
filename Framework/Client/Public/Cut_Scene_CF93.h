#pragma once

#include "Client_Defines.h"
#include "Cut_Scene.h"

BEGIN(Client)

class CCut_Scene_CF93 final : public CCut_Scene
{
public:
	enum class CF93_ACTOR_TYPE{ _PL_7800, _END };

private:
	CCut_Scene_CF93(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCut_Scene_CF93(const CCut_Scene_CF93& rhs);
	virtual ~CCut_Scene_CF93() = default;

public:
	virtual HRESULT								Initialize_Prototype() override;
	virtual HRESULT								Initialize(void* pArg) override;

private:
	virtual HRESULT								Add_Actors() override;

public:
	static CCut_Scene_CF93* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual	void Free();
};

END