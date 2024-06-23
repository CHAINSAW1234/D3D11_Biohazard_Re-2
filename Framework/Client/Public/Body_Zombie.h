#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

/* For. Bone Tag */
#define ROOT_BONE_TAG					"root"

/* For. Bone_Layer Tag */
#define BONE_LAYER_DEFAULT_TAG			TEXT("Default")
#define BONE_LAYER_LOWER_TAG			TEXT("Lower")
#define BONE_LAYER_UPPER_TAG			TEXT("Upper")

BEGIN(Client)

class CBody_Zombie final : public CPartObject
{
public:
#include "Body_Zombie_Enums.h"

public:
	typedef struct tagBodyMonsterDesc : public CPartObject::PARTOBJECT_DESC
	{
		_float3*			pRootTranslation = { nullptr };
	}BODY_MONSTER_DESC;

private:
	CBody_Zombie(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Zombie(const CBody_Zombie& rhs);
	virtual ~CBody_Zombie() = default;

public:
	virtual HRESULT						Initialize_Prototype() override;
	virtual HRESULT						Initialize(void* pArg) override;
	virtual void						Priority_Tick(_float fTimeDelta) override;
	virtual void						Tick(_float fTimeDelta) override;
	virtual void						Late_Tick(_float fTimeDelta) override;

	virtual HRESULT						Render() override;
	HRESULT								Render_LightDepth_Dir()override;
	HRESULT								Render_LightDepth_Point() override;
	HRESULT								Render_LightDepth_Spot()override;

private:
	HRESULT								Initialize_Model();
	HRESULT								Add_Animations();

public:
	MOTION_TYPE							Get_Current_MotionType() { return m_eCurrentMotionType; }
	MOTION_TYPE							Get_Pre_MotionType() { return m_ePreMotionType; }

private:
	void								Update_Current_MotionType();

public:	/* For.Check Anim Type */
	_bool								Is_Start_Anim(_uint iAnimIndex);
	_bool								Is_Loop_Anim(_uint iAnimIndex);
	_bool								Is_Move_Anim(_uint iAnimIndex);
	_bool								Is_Turn_Anim(_uint iAnimIndex);
	class CModel*						GetModel()
	{
		return m_pModelCom;
	}

public:
	virtual void						SetRagdoll(_int iId, _float4 vForce, COLLIDER_TYPE eType) override;
	virtual void						SetCulling(_bool boolean) override;
private:
	CModel*								m_pModelCom = { nullptr };
	CShader*							m_pShaderCom = { nullptr };
	CCollider*							m_pColliderCom = { nullptr };

	_float3*							m_pRootTranslation = { nullptr };

	_float4x4							m_RotationMatrix;

	class CRagdoll_Physics*				m_pRagdoll = { nullptr };

private:		/* For Anim_Controll */
	MOTION_TYPE							m_ePreMotionType = { MOTION_END };
	MOTION_TYPE							m_eCurrentMotionType = { MOTION_END };

	set<_uint>							m_StartAnimIndices;
	set<_uint>							m_LoopAnimIndices;

	set<_uint>							m_MoveAnimIndices;
	set<_uint>							m_TurnAnimIndices;

private:
	HRESULT								Add_Components();
	HRESULT								Bind_ShaderResources();

public:
	static CBody_Zombie* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END