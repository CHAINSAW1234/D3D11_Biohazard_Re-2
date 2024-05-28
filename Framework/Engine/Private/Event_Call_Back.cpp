#include "Event_Call_Back.h"
#include "Rigid_Dynamic.h"
#include "Character_Controller.h"

void CEventCallBack::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{
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

						if (Rigid_Dynamic != nullptr)
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

						if (Rigid_Dynamic != nullptr)
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

