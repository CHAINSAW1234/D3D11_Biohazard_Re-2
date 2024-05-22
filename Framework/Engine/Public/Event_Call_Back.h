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
			return PxQueryHitType::eNONE; // 충돌 무시(Rigid Dynamic이 밀려남)
		}
		else
		{
			return PxQueryHitType::eBLOCK; // 충돌 처리
		}
	}
	
	virtual PxQueryHitType::Enum PxQueryFilterCallback::postFilter(const PxFilterData& filterData, const PxQueryHit& hit, const PxShape* shape, const PxRigidActor* actor)
	{
		// 추가적인 후처리 로직이 필요하다면 작성
		return PxQueryHitType::eNONE; // 충돌 무시
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


	//충돌 무시 코드
	if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
	{
		pairFlags = physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;
		return physx::PxFilterFlag::eNOTIFY;

		// 충돌은 감지하지만 응답은 하지 않음
		pairFlags = PxPairFlag::eCONTACT_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}
	else
	{
		// 충돌은 감지하지만 응답은 하지 않음
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
				// 충돌한 첫 번째 객체
				const PxRigidActor* actor0 = pairHeader.actors[0]->is<PxRigidActor>();
				// 충돌한 두 번째 객체
				const PxRigidActor* actor1 = pairHeader.actors[1]->is<PxRigidActor>();

				if (actor0 && actor1)
				{
					PxShape* shape0;
					PxShape* shape1;
					// 각 객체에서 첫 번째 Shape을 얻습니다.
					actor0->getShapes(&shape0, 1);
					actor1->getShapes(&shape1, 1);

					if (shape0 && shape1)
					{
						// 각 Shape의 simulation filter data에 접근합니다.
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

	// 다른 이벤트 콜백 함수들은 사용하지 않으므로 빈 구현으로 남겨둡니다.
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

		//충돌 무시
		if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
		{
			pairFlags = physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;
			return PxFilterFlag::eDEFAULT;

			// 충돌은 감지하지만 응답은 하지 않음
			pairFlags = PxPairFlag::eCONTACT_DEFAULT;
			return physx::PxFilterFlag::eNOTIFY;
		}
		else
		{
			// 충돌은 감지하지만 응답은 하지 않음
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
        // 필요한 경우 구현
    }

    virtual bool statusChange(
        PxU64& pairID, PxPairFlags& pairFlags, PxFilterFlags& filterFlags) override {
        // 필요한 경우 구현
        return false;
    }
};
