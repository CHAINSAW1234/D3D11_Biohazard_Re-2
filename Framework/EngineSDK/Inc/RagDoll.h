#pragma once

#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CRagdoll : public CBase
{
public:
	CRagdoll();
	CRagdoll(const CRagdoll& rhs);
	virtual ~CRagdoll() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	static CRagdoll* Create();
public:
    std::vector<PxRigidDynamic*>                m_rigid_bodies;
    std::vector<_vector>                        m_relative_joint_pos;
    std::vector<_vector>                        m_original_body_rotations;

    std::vector<_vector>                        m_body_pos_relative_to_joint;
    std::vector<_vector>                        m_original_joint_rotations;

    PxRigidDynamic*                             find_recent_body(uint32_t idx, class Skeleton* skeleton, uint32_t& chosen_idx);
    void                                        set_kinematic(bool state);
    XMVECTOR                                    pos_from_transform(const XMMATRIX& m)
    {
        return m.r[3];
    }
    void                                        WakeUp();
public:
	virtual void Free() override;
};

class ENGINE_DLL AnimRagdoll : public CBase
{
public:
    AnimRagdoll(Skeleton* skeleton);
    ~AnimRagdoll();
    struct PoseTransforms* apply(CRagdoll* ragdoll, _matrix model_scale, _matrix model_rotation);

private:
    Skeleton*       m_skeleton;
    PoseTransforms  m_transforms;
};

END