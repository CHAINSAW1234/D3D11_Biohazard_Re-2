#include "Event_Call_Back.h"
#include "Rigid_Dynamic.h"
#include "Character_Controller.h"
#include "RagDoll_Physics.h"

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

#pragma region 탄피 소리
					if ((filterData0.word0 == (int)COLLISION_CATEGORY::STATIC_MESH) && (filterData1.word0 == (int)COLLISION_CATEGORY::EFFECT))
					{
						auto Rigid_Dynamic = GameInstance->GetRigid_Dynamic(filterData1.word3);

						if (Rigid_Dynamic != nullptr)
						{
							Rigid_Dynamic->SetbPlaySound(true);
						}
					}

					if ((filterData1.word0 == (int)COLLISION_CATEGORY::STATIC_MESH) && (filterData0.word0 == (int)COLLISION_CATEGORY::EFFECT))
					{
						auto Rigid_Dynamic = GameInstance->GetRigid_Dynamic(filterData0.word3);

						if (Rigid_Dynamic != nullptr)
						{
							Rigid_Dynamic->SetbPlaySound(true);
						}
					}
#pragma endregion

#pragma region Ragdoll 소리
					if ((filterData0.word0 == (int)COLLISION_CATEGORY::STATIC_MESH) && (filterData1.word0 == (int)COLLISION_CATEGORY::RAGDOLL))
					{
						auto pRagdoll = GameInstance->Get_Ragdoll(filterData1.word2);

						if (pRagdoll != nullptr)
						{
							COLLIDER_TYPE eType = (COLLIDER_TYPE)(_int)filterData1.word3;
							pRagdoll->PlaySound_Ragdoll(eType);
						}
					}

					if ((filterData1.word0 == (int)COLLISION_CATEGORY::STATIC_MESH) && (filterData0.word0 == (int)COLLISION_CATEGORY::RAGDOLL))
					{
						auto pRagdoll = GameInstance->Get_Ragdoll(filterData0.word2);

						if (pRagdoll != nullptr)
						{
							COLLIDER_TYPE eType = (COLLIDER_TYPE)(_int)filterData0.word3;
							pRagdoll->PlaySound_Ragdoll(eType);
						}
					}
#pragma endregion
				}
			}
		}
	}
}

