#pragma once

#include "Base.h"

BEGIN(Engine)

class CIK_Solver final : public CBase
{
public:
	typedef struct tagIK_Info
	{
		_int								iEndEffectorIndex = { -1 };
		_int								iIKRootBoneIndex = { -1 };
		vector<_uint>						JointIndices;
		_float3								vIKEndTargetPosition = {};
		list<_uint>							IKIncludedIndices;

		_uint								iNumIteration = { 0 };

		_float								fBlend = { 0.f };

		_float4								vTargetJointStartTranslation;
		_float4								vEndEffectorResultPosition;

		vector<_uint>						JointTypes;

		vector<_float>						BoneOrientationLimits;
		vector<_float4>						BoneOrientations;
		vector<_float4>						BoneThetas;
		vector<_float>						TargetDistancesToChild;
		vector<_float>						TargetDistancesToParrent;
		vector<_float4>						BoneTranslationsResult;
		vector<_float4>						BoneTranslationsOrigin;
	}IK_INFO;

public:
	enum JOINT_TYPE { JOINT_HINGE, JOINT_BALL, JOINT_END };
	enum CONIC_SECTION { SECTION_1, SECTION_2, SECTION_3, SECTION_4, SECTION_END };

private:
	CIK_Solver();
	virtual ~CIK_Solver() = default;

public:
	HRESULT									Initialize(void* pArg);

public:		/* For.IK Public*/
	void									Add_IK(vector<class CBone*>& Bones, list<_uint> IkIncludedIndices, _uint iIKRootBoneIndex, _uint iEndEffectorIndex, string strTargetJointTag, string strEndEffectorTag, const wstring& strIKTag, _uint iNumIteration, _float fBlend);
	void									Release_IK(const wstring& strIKTag);
	void									Set_TargetPosition_IK(const wstring& strIKTag, _fvector vTargetPosition);
	void									Set_NumIteration_IK(const wstring& strIKTag, _uint iNumIteration);
	void									Set_Blend_IK(wstring strIKTag, _float fBlend);
	vector<_float4>							Get_ResultTranslation_IK(const wstring& strIKTag);
	vector<_float4>							Get_OriginTranslation_IK(const wstring& strIKTag);
	vector<_float4x4>						Get_JointCombinedMatrices_IK(const wstring& strIKTag, vector<CBone*>& Bones);

	void									Play_IK(vector<CBone*>& Bones, class CTransform* pTransform);

private:	/* For.IK Private */
	void									Apply_IK(vector<CBone*>& Bones, class CTransform* pTransform, IK_INFO& IkInfo);
	void									Update_Distances_Translations_IK(vector<CBone*>& Bones, IK_INFO& IkInfo);
	void									Solve_IK(IK_INFO& IkInfo);
	void									Solve_IK_Forward(IK_INFO& IkInfo);
	void									Solve_IK_Backward(IK_INFO& IkInfo);
	void									Solve_For_Distance_IK(IK_INFO& IkInfo, _int iSrcJointIndex, _int iDstJointIndex);
	void									Solve_For_Orientation_IK(IK_INFO& IkInfo, _int iOuterJointIndex, _int iInnerJointIndex);
	void 									Rotational_Constranit(IK_INFO& IkInfo, _int iOuterJointIndex, _int iMyJointIndex);
	void									Update_Forward_Reaching_IK(IK_INFO& IkInfo);
	void									Update_Backward_Reaching_IK(IK_INFO& IkInfo);
	void									Update_TransformMatrices_BoneChain(vector<CBone*>& Bones, IK_INFO& IkInfo);
	void									Update_CombinedMatrices_BoneChain(vector<CBone*>& Bones, IK_INFO& IkInfo);

private:	/* Utillity */
	_vector									Compute_Quaternion_From_TwoDirection(_fvector vSrcDirection, _fvector vDstDirection);

private:	/* For.IK_Constraint */
	CONIC_SECTION							Find_ConicSection(_float fTheta);
	_bool									Is_InBound(CONIC_SECTION eSection, _fvector vQ, _float2 vTarget);
	_float2									Find_Nearest_Point_Constraints(_float fMajorAxisLength, _float fMinorAxisLength, CONIC_SECTION eSection, _float2 vTargetPosition);
	_float2									Find_Initial_Point_Constraints(_float fMajorAxisLength, _float fMinorAxisLength, CONIC_SECTION eSection, _float2 vTargetPosition);
	_float2									Find_Next_Point_Constraints(_float2 vCurrentPoint, _float fMajorAxisLength, _float fMinorAxisLength, _float2 vTargetPosition);
	_float2									Compute_Delta_Constratins(_float2 vCurrentPoint, _float fMajorAxisLength, _float fMinorAxisLength, _float2 vTargetPosition);
	_float4x4								Compute_QMatrix(_float2 vCurrentPoint, _float fMajorAxisLength, _float fMinorAxisLength, _float2 vTargetPosition);

private:	/* For.Inverse_Kinematic */
	map<wstring, IK_INFO>					m_IKInfos;

public:
	static CIK_Solver* Create(void* pArg);
	virtual void Free() override;
};

END