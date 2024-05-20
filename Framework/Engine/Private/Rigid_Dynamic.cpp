#include "Rigid_Dynamic.h"

CRigid_Dynamic::CRigid_Dynamic(PxRigidDynamic* RigidDynamic)
{
	m_pRigid_Dynamic = RigidDynamic;
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

PxVec3 CRigid_Dynamic::GetPosition()
{
	return m_pRigid_Dynamic->getGlobalPose().p;
}

void CRigid_Dynamic::Update()
{
	if (m_bUpdated)
	{
		m_pRigid_Dynamic->setGlobalPose(PxTransform(m_UpdatePos));
		m_pRigid_Dynamic->setLinearVelocity(PxVec3(0.0f, 0.0f, 0.0f));
		m_pRigid_Dynamic->setAngularVelocity(PxVec3(0.0f, 0.0f, 0.0f));
		m_bUpdated = false;
	}
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

CRigid_Dynamic* CRigid_Dynamic::Create()
{
	return nullptr;
}

void CRigid_Dynamic::Free()
{
}
