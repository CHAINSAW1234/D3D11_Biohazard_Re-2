//#include "Base.h"
//
//class SoftBody
//{
//public:
//	SoftBody(physx::PxSoftBody* softBody, physx::PxCudaContextManager* cudaContextManager) :
//		mSoftBody(softBody),
//		mCudaContextManager(cudaContextManager)
//	{
//		mPositionsInvMass = PX_PINNED_HOST_ALLOC_T(physx::PxVec4, cudaContextManager, softBody->getCollisionMesh()->getNbVertices());
//	}
//
//	~SoftBody()
//	{
//	}
//
//	void release()
//	{
//		if (mSoftBody)
//			mSoftBody->release();
//		if (mPositionsInvMass)
//			PX_PINNED_HOST_FREE(mCudaContextManager, mPositionsInvMass);
//	}
//
//	void copyDeformedVerticesFromGPUAsync(CUstream stream)
//	{
//		physx::PxTetrahedronMesh* tetMesh = mSoftBody->getCollisionMesh();
//
//		physx::PxScopedCudaLock _lock(*mCudaContextManager);
//		mCudaContextManager->getCudaContext()->memcpyDtoHAsync(mPositionsInvMass, reinterpret_cast<CUdeviceptr>(mSoftBody->getPositionInvMassBufferD()), tetMesh->getNbVertices() * sizeof(physx::PxVec4), stream);
//	}
//
//	void copyDeformedVerticesFromGPU()
//	{
//		physx::PxTetrahedronMesh* tetMesh = mSoftBody->getCollisionMesh();
//
//		physx::PxScopedCudaLock _lock(*mCudaContextManager);
//		mCudaContextManager->getCudaContext()->memcpyDtoH(mPositionsInvMass, reinterpret_cast<CUdeviceptr>(mSoftBody->getPositionInvMassBufferD()), tetMesh->getNbVertices() * sizeof(physx::PxVec4));
//	}
//
//
//	physx::PxVec4* mPositionsInvMass;
//	physx::PxSoftBody* mSoftBody;
//	physx::PxCudaContextManager* mCudaContextManager;
//};
//
//#endif
