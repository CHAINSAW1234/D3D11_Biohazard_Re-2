#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CActor abstract : public CGameObject
{
public:
	typedef struct tagActorDesc : GAMEOBJECT_DESC
	{
		_uint					iBasePartIndex = { 0 };
		_uint					iNumParts = { 0 };
	}ACTOR_DESC;

protected:
	CActor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CActor(const CActor& rhs);
	virtual ~CActor() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Priority_Tick(_float fTimeDelta) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

protected:
	void					Priority_Tick_PartObjects(_float fTimeDelta);
	void					Tick_PartObjects(_float fTimeDelta);
	void					Late_Tick_PartObjects(_float fTimeDelta);

public:
	HRESULT					Set_Animation(_uint iPartType, const wstring& strAnimLayerTag, _uint iAnimIndex);
	void					Set_Next_Animation_Sequence();
	void					Set_Loop(_uint iPartType, _bool isLoop);

	void					Reset_Animations();

	inline _uint			Get_NumParts() { return m_iNumParts; }

	_bool					Is_Finished_Animation_Part(_uint iPartType);
	_bool					Is_Finished_Animation_All_Part();

	_bool					Is_Finished_All_Animation_All();

public:
	class CActor_PartObject* Get_PartObject(_uint iPartType);

public:
	HRESULT					Initialize_PartObjects();

public:
	inline _uint			Get_BasePartIndex() { return m_iBasePartIndex; }

protected:
	vector<class CActor_PartObject*>				m_PartObjects;
	_float3											m_vRootTranslation = {};
	_uint											m_iBasePartIndex = {};
	_uint											m_iNumParts = {};

	_uint											m_iCurSeqLev = { 0 };

protected:
	HRESULT					Add_Components();
	virtual HRESULT			Add_PartObjects() = 0;

public:
	virtual void Free() override;
};

END