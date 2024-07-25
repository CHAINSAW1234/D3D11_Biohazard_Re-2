#include "Rigid_Dynamic.h"
#include "GameInstance.h"

CRigid_Dynamic::CRigid_Dynamic() : CPhysics_Component()
{

}

CRigid_Dynamic::CRigid_Dynamic(const CRigid_Dynamic& rhs)
{
}

HRESULT CRigid_Dynamic::Initialize_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CRigid_Dynamic::Initialize(void* pArg)
{
	return E_NOTIMPL;
}

void CRigid_Dynamic::SetPosition(PxVec3 Pos)
{
	//m_pRigid_Dynamic->setGlobalPose(PxTransform(Pos));

	m_UpdatePos = Pos;
}

void CRigid_Dynamic::SetPosition(_float4 vPos)
{
	m_pRigid_Dynamic->setGlobalPose(PxTransform(PxVec3(vPos.x, vPos.y, vPos.z)));
}

PxVec3 CRigid_Dynamic::GetPosition()
{
	return m_pRigid_Dynamic->getGlobalPose().p;
}

void CRigid_Dynamic::Update()
{
	/*if (m_bUpdated)
	{
		m_pRigid_Dynamic->setGlobalPose(PxTransform(m_UpdatePos));
		m_pRigid_Dynamic->setLinearVelocity(PxVec3(0.0f, 0.0f, 0.0f));
		m_pRigid_Dynamic->setAngularVelocity(PxVec3(0.0f, 0.0f, 0.0f));
		m_bUpdated = false;
	}*/
}

_float4 CRigid_Dynamic::GetTranslation()
{
	auto Pos = m_pRigid_Dynamic->getGlobalPose();
	_float x = Pos.p.x;
	_float y = Pos.p.y;
	_float z = Pos.p.z;
	_float4 ReturnPos(x, y, z, 1.f);

	return ReturnPos;
}

PxTransform CRigid_Dynamic::GetTransform_Px()
{
	return m_pRigid_Dynamic->getGlobalPose();
}

void CRigid_Dynamic::SetKinematic(_bool boolean)
{
	m_pRigid_Dynamic->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, boolean);

	if (boolean == false)
	{
		m_pRigid_Dynamic->wakeUp();
	}
}

void CRigid_Dynamic::AddForce(_float4 vDir)
{
	PxVec3 randomAxis(static_cast<float>(rand()) / RAND_MAX,
		static_cast<float>(rand()) / RAND_MAX,
		static_cast<float>(rand()) / RAND_MAX);
	randomAxis.normalize();

	float randomAngle = static_cast<float>(rand()) / RAND_MAX * PxPi * 2.0f;

	PxQuat randomRotation(randomAngle, randomAxis);

	PxTransform transform = m_pRigid_Dynamic->getGlobalPose();
	transform.q = randomRotation;

	m_pRigid_Dynamic->setGlobalPose(transform);

	_float fMag = m_pGameInstance->GetRandom_Real(6.f, 7.f);
	vDir = vDir * (fMag);
	PxVec3 pxForce(vDir.x, vDir.y+0.5f, vDir.z);

	PxVec3 pxTorque(m_pGameInstance->GetRandom_Real(-3.f, 3.f), m_pGameInstance->GetRandom_Real(-3.f, 3.f), m_pGameInstance->GetRandom_Real(-3.f, 3.f));

	m_pRigid_Dynamic->addForce(pxForce, PxForceMode::eIMPULSE);
	m_pRigid_Dynamic->addTorque(pxTorque, PxForceMode::eIMPULSE);
}

void CRigid_Dynamic::ClearForce()
{
	m_pRigid_Dynamic->clearForce(PxForceMode::eFORCE);
	m_pRigid_Dynamic->clearForce(PxForceMode::eIMPULSE);
	m_pRigid_Dynamic->clearForce(PxForceMode::eVELOCITY_CHANGE);
	m_pRigid_Dynamic->clearForce(PxForceMode::eACCELERATION);

	m_pRigid_Dynamic->clearTorque(PxForceMode::eFORCE);
	m_pRigid_Dynamic->clearTorque(PxForceMode::eIMPULSE);
	m_pRigid_Dynamic->clearTorque(PxForceMode::eVELOCITY_CHANGE);
	m_pRigid_Dynamic->clearTorque(PxForceMode::eACCELERATION);

	m_pRigid_Dynamic->setLinearVelocity(PxVec3(0.0f, 0.0f, 0.0f));
	m_pRigid_Dynamic->setAngularVelocity(PxVec3(0.0f, 0.0f, 0.0f));
}

void CRigid_Dynamic::Release_Body()
{
	m_pRigid_Dynamic->release();
	m_pRigid_Dynamic = nullptr;
}

void CRigid_Dynamic::ToSleep()
{
	m_pRigid_Dynamic->putToSleep();
}

CRigid_Dynamic* CRigid_Dynamic::Create()
{
	return nullptr;
}

_matrix CRigid_Dynamic::GetWorldMatrix_Rigid_Dynamic(_float3 vScale)
{
	PxTransform globalPose = m_pRigid_Dynamic->getGlobalPose();

	PxVec3 position = globalPose.p;

	PxQuat rotation = globalPose.q;

	_matrix translationMatrix = XMMatrixTranslation(position.x, position.y, position.z);

	_matrix rotationMatrix = XMMatrixRotationQuaternion(XMVectorSet(rotation.x, rotation.y, rotation.z, rotation.w));

	_matrix worldMatrix = rotationMatrix * translationMatrix;

	return worldMatrix;
}

void CRigid_Dynamic::Free()
{
	__super::Free();
}
