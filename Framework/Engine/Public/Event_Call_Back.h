#include "Engine_Defines.h"
#include "GameInstance.h"
//#include "Rigid_Dynamic.h"

class QueryFilterCallback : public PxQueryFilterCallback
{
public:
	QueryFilterCallback() {}
	virtual PxQueryHitType::Enum preFilter(const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags)
	{
		PxFilterData filterData0 = shape->getSimulationFilterData();

		if ((filterData0.word0 & COLLISION_CATEGORY::COLLIDER) || (filterData0.word0 & COLLISION_CATEGORY::RAGDOLL))
		{
			return PxQueryHitType::eNONE; // �浹 ����(Rigid Dynamic�� �з���)
		}
		else
		{
			return PxQueryHitType::eBLOCK; // �浹 ó��
		}
	}
	
	virtual PxQueryHitType::Enum PxQueryFilterCallback::postFilter(const PxFilterData& filterData, const PxQueryHit& hit, const PxShape* shape, const PxRigidActor* actor)
	{
		// �߰����� ��ó�� ������ �ʿ��ϴٸ� �ۼ�
		return PxQueryHitType::eNONE; // �浹 ����
	}
};

class QueryFilterCallback_Ray : public PxQueryFilterCallback
{
public:
	QueryFilterCallback_Ray() {}
	virtual PxQueryHitType::Enum preFilter(const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags)
	{
		PxFilterData filterData0 = shape->getSimulationFilterData();

		if ((filterData0.word0 & COLLISION_CATEGORY::CCT))
		{
			return PxQueryHitType::eNONE;
		}

		return PxQueryHitType::eBLOCK; // �浹 ó��
	}

	virtual PxQueryHitType::Enum PxQueryFilterCallback::postFilter(const PxFilterData& filterData, const PxQueryHit& hit, const PxShape* shape, const PxRigidActor* actor)
	{
		// �߰����� ��ó�� ������ �ʿ��ϴٸ� �ۼ�
		return PxQueryHitType::eNONE; // �浹 ����
	}
};

static physx::PxFilterFlags MegamotionFilterShader(
	physx::PxFilterObjectAttributes attributes0,
	physx::PxFilterData filterData0,
	physx::PxFilterObjectAttributes attributes1,
	physx::PxFilterData filterData1,
	physx::PxPairFlags& pairFlags,
	const void* constantBlock,
	physx::PxU32 constantBlockSize)
{
	pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;

	if ((filterData0.word0 & COLLISION_CATEGORY::STATIC_MESH) && (filterData1.word0 & COLLISION_CATEGORY::EFFECT))
	{
		auto GameInstance = CGameInstance::Get_Instance();

		//auto Rigid_Dynamic = GameInstance->GetRigid_Dynamic(filterData0.word3);
	}

	if ((filterData0.word1 & COLLISION_CATEGORY::STATIC_MESH) && (filterData1.word0 & COLLISION_CATEGORY::EFFECT))
	{
		auto GameInstance = CGameInstance::Get_Instance();

		//auto Rigid_Dynamic = GameInstance->GetRigid_Dynamic(filterData1.word3);
	}


#pragma region Default
	//�浹 ���� �ڵ�
	if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
	{
		pairFlags = physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;
		return physx::PxFilterFlag::eNOTIFY;

		// �浹�� ���������� ������ ���� ����
		pairFlags = PxPairFlag::eCONTACT_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}
	else
	{
		// �浹�� ���������� ������ ���� ����
		pairFlags = PxPairFlag::eCONTACT_DEFAULT;
		return PxFilterFlag::eDEFAULT;

		pairFlags = physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;
		return PxFilterFlag::eDEFAULT;
	}
#pragma endregion
}

class CEventCallBack : public PxSimulationEventCallback {
public:
	virtual void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) override;
	// �ٸ� �̺�Ʈ �ݹ� �Լ����� ������� �����Ƿ� �� �������� ���ܵӴϴ�.
	virtual void onTrigger(PxTriggerPair*, PxU32) override {}
	virtual void onConstraintBreak(PxConstraintInfo*, PxU32) override {}
	virtual void onWake(PxActor**, PxU32) override {}
	virtual void onSleep(PxActor**, PxU32) override {}
	virtual void onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count) override {}
};

class CFilterCallBack : public PxSimulationFilterCallback {
public:
    virtual PxFilterFlags pairFound(
        PxU64 pairID,
        PxFilterObjectAttributes attributes0, PxFilterData filterData0,
        const PxActor* a0, const PxShape* s0,
        PxFilterObjectAttributes attributes1, PxFilterData filterData1,
        const PxActor* a1, const PxShape* s1,
        PxPairFlags& pairFlags) override {

		pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;

#pragma region CCT �浹 ����
		//if ((filterData0.word0 & COLLISION_CATEGORY::CCT_NO_COLLISION) && (filterData1.word0 & COLLISION_CATEGORY::CCT_NO_COLLISION))
		//{
		//	pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;
		//	return physx::PxFilterFlag::eNOTIFY;
		//}
#pragma endregion

#pragma region Default
		if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
		{
			pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;
			return physx::PxFilterFlag::eNOTIFY;
		}
#pragma endregion

		return physx::PxFilterFlag::eDEFAULT;

		//�浹 ����
		if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
		{
			pairFlags = physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;
			return PxFilterFlag::eDEFAULT;

			// �浹�� ���������� ������ ���� ����
			pairFlags = PxPairFlag::eCONTACT_DEFAULT;
			return physx::PxFilterFlag::eNOTIFY;
		}
		else
		{
			// �浹�� ���������� ������ ���� ����
			pairFlags = PxPairFlag::eCONTACT_DEFAULT;
			return PxFilterFlag::eDEFAULT;

			pairFlags = physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;
			return PxFilterFlag::eDEFAULT;
		}
    }

    virtual void pairLost(
        PxU64 pairID,
        PxFilterObjectAttributes attributes0, PxFilterData filterData0,
        PxFilterObjectAttributes attributes1, PxFilterData filterData1,
        bool objectRemoved) override {
        // �ʿ��� ��� ����
    }

    virtual bool statusChange(
        PxU64& pairID, PxPairFlags& pairFlags, PxFilterFlags& filterFlags) override {
        // �ʿ��� ��� ����
        return false;
    }
};
