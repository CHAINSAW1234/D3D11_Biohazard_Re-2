#include "Engine_Defines.h"
#include "GameInstance.h"
#include "Rigid_Dynamic.h"

class QueryFilterCallback : public PxQueryFilterCallback
{
public:
	QueryFilterCallback() {}
	virtual PxQueryHitType::Enum preFilter(const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags)
	{
		PxFilterData filterData0 = shape->getSimulationFilterData();

		if (filterData0.word0 == 2)
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

	// trigger the contact callback for pairs (A,B) where
	// the filtermask of A contains the ID of B and vice versa.
	/*if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
	{
		pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;
		return physx::PxFilterFlag::eNOTIFY;
	}

	return physx::PxFilterFlag::eDEFAULT;*/


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
}

class CEventCallBack : public PxSimulationEventCallback {
public:
	/*virtual void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) override {
		for (physx::PxU32 i = 0; i < nbPairs; i++)
		{
			const physx::PxContactPair& cp = pairs[i];

			if (cp.events & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
			{
				int a = 0;
			}
		}
	}*/

	virtual void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) override {
		for (physx::PxU32 i = 0; i < nbPairs; i++) {
			const physx::PxContactPair& cp = pairs[i];
			
			if (cp.events & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND) {
				// �浹�� ù ��° ��ü
				const PxRigidActor* actor0 = pairHeader.actors[0]->is<PxRigidActor>();
				// �浹�� �� ��° ��ü
				const PxRigidActor* actor1 = pairHeader.actors[1]->is<PxRigidActor>();

				if (actor0 && actor1)
				{
					PxShape* shape0;
					PxShape* shape1;
					// �� ��ü���� ù ��° Shape�� ����ϴ�.
					actor0->getShapes(&shape0, 1);
					actor1->getShapes(&shape1, 1);

					if (shape0 && shape1)
					{
						// �� Shape�� simulation filter data�� �����մϴ�.
						PxFilterData filterData0 = shape0->getSimulationFilterData();
						PxFilterData filterData1 = shape1->getSimulationFilterData();

						auto GameInstance = CGameInstance::Get_Instance();

						if (filterData0.word0 == 2)
						{
							auto Rigid_Dynamic = GameInstance->GetRigid_Dynamic(filterData0.word3);

							if(Rigid_Dynamic != nullptr)
							{
								auto Pos = Rigid_Dynamic->GetPosition();
								Rigid_Dynamic->SetPosition(physx::PxVec3(0, 5, 0));
								Pos = Rigid_Dynamic->GetPosition();
								Rigid_Dynamic->SetUpdated(true);
							}
						}

						if (filterData1.word0 == 2)
						{
							auto Rigid_Dynamic = GameInstance->GetRigid_Dynamic(filterData1.word3);

							if(Rigid_Dynamic != nullptr)
							{
								auto Pos = Rigid_Dynamic->GetPosition();
								Rigid_Dynamic->SetPosition(physx::PxVec3(0, 5, 0));
								Pos = Rigid_Dynamic->GetPosition();
								Rigid_Dynamic->SetUpdated(true);
							}
						}
					}
				}
			}
		}
	}

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

		if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
		{
			pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;
			return physx::PxFilterFlag::eNOTIFY;
		}

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
