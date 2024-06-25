#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

/* For. Bone Tag */
#define ROOT_BONE_TAG					"root"

/* For. Bone_Layer Tag */
#define BONE_LAYER_DEFAULT_TAG				TEXT("Default")
#define BONE_LAYER_LOWER_TAG				TEXT("Lower")
#define BONE_LAYER_UPPER_TAG				TEXT("Upper")

#define BONE_LAYER_L_LEG_TWIST_TAG			TEXT("L_Leg_Twistter")
#define BONE_LAYER_R_LEG_TWIST_TAG			TEXT("R_Leg_Twistter")
#define BONE_LAYER_L_ARM_TWIST_TAG			TEXT("L_Arm_Twistter")
#define BONE_LAYER_R_ARM_TWIST_TAG			TEXT("R_Arm_Twistter")
#define BONE_LAYER_L_SHOULDER_TWIST_TAG		TEXT("L_Shoulder_Twistter")
#define BONE_LAYER_R_SHOULDER_TWIST_TAG		TEXT("R_Shoulder_Twistter")
#define BONE_LAYER_BODY_TWIST_TAG			TEXT("Body_Twistter")
#define BONE_LAYER_Head_TWIST_TAG			TEXT("Head_Twistter")

BEGIN(Client)

#include "Body_Zombie_Enums.h"

class CBody_Zombie final : public CPartObject
{
public:

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
	virtual void						Add_RenderGroup() override;
private:
	HRESULT								Initialize_Model();
	HRESULT								Add_Animations();
	HRESULT								Register_Animation_Branches_AnimType();
	HRESULT								Register_Animation_Branches_AnimGroup();
	//	ÇÇ°Ý½Ã Èçµé¸± »Àµé
	HRESULT								Register_BoneLayer_Additional_TwisterBones();

private:		/* For. Register BoneLayer */
	HRESULT								Register_BoneLayer_Childs_NonInclude_Joint(const wstring& strBoneLayerTag, const string& strTopParentTag, const string& strEndEffectorTag);

public:
	void								Set_MotionType(MOTION_TYPE eType) { m_eCurrentMotionType = eType; }

	MOTION_TYPE							Get_Current_MotionType() { return m_eCurrentMotionType; }
	MOTION_TYPE							Get_Pre_MotionType() { return m_ePreMotionType; }

private:
	void								Update_Current_MotionType();

public:	/* For.Check Anim Type */
	class CModel*						GetModel()
	{
		return m_pModelCom;
	}

public: /* For. Anim Branch */
	ZOMBIE_BODY_ANIM_TYPE				Get_Current_AnimType(PLAYING_INDEX eIndex);
	ZOMBIE_BODY_ANIM_GROUP				Get_Current_AnimGroup(PLAYING_INDEX eIndex);

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
	MOTION_TYPE							m_ePreMotionType = { MOTION_TYPE::MOTION_END };
	MOTION_TYPE							m_eCurrentMotionType = { MOTION_TYPE::MOTION_END };

private:		/* For Anim_Branch_AnimType */
	vector<unordered_set<wstring>>		m_AnimTypeAnimLayerTags;

private:		/* For Anim_Branch_AnimGroup */
	vector<unordered_set<wstring>>		m_GroupAnimLayerTags;

private:
	HRESULT								Add_Components();
	HRESULT								Bind_ShaderResources();

public:
	static CBody_Zombie* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END