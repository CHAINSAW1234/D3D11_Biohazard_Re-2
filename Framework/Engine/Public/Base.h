#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CBase abstract
{
protected:
	CBase();
	virtual ~CBase() = default;

public:
	unsigned int AddRef();

	unsigned int Release();
private:

	unsigned int		m_iRefCnt = { 0 };

	//편의성 함수
public:
public:
	FORCEINLINE XMFLOAT4 Convert_Float3_To_Float4_Dir(XMFLOAT3 Vec)
	{
		XMFLOAT4 Vec4;
		Vec4.x = Vec.x;
		Vec4.y = Vec.y;
		Vec4.z = Vec.z;
		Vec4.w = 0;

		return Vec4;
	}

	FORCEINLINE XMFLOAT4 Convert_Float3_To_Float4_Coord(XMFLOAT3 Vec)
	{
		XMFLOAT4 Vec4;
		Vec4.x = Vec.x;
		Vec4.y = Vec.y;
		Vec4.z = Vec.z;
		Vec4.w = 1;

		return Vec4;
	}

	FORCEINLINE XMFLOAT3 Convert_Float4_To_Float3(XMFLOAT4 Vec)
	{
		XMFLOAT3 Vec3;
		Vec3.x = Vec.x;
		Vec3.y = Vec.y;
		Vec3.z = Vec.z;

		return Vec3;
	}

	FORCEINLINE XMFLOAT3 Convert_Vector_To_Float3(FXMVECTOR Vec)
	{
		XMFLOAT3 ReturnVec;
		XMStoreFloat3(&ReturnVec, Vec);

		return ReturnVec;
	}

	FORCEINLINE XMVECTOR Convert_Float3_To_Vector_Coord(XMFLOAT3 FVec)
	{
		XMVECTOR ReturnVec = XMLoadFloat3(&FVec);
		ReturnVec = XMVectorSetW(ReturnVec, 1);
		return ReturnVec;
	}

	FORCEINLINE XMVECTOR Convert_Float3_To_Vector_Dir(XMFLOAT3 FVec)
	{
		XMVECTOR ReturnVec = XMLoadFloat3(&FVec);
		ReturnVec = XMVectorSetW(ReturnVec, 0);
		return ReturnVec;
	}

	FORCEINLINE _float4	 Add_Float4_Dir(_float4 F1, _float4 F2)
	{
		_float4 Result;
		Result.x = F1.x + F2.x;
		Result.y = F1.y + F2.y;
		Result.z = F1.z + F2.z;
		Result.w = 0.f;

		return Result;
	}

	FORCEINLINE _float4	 Add_Float4_Coord(_float4 F1, _float4 F2)
	{
		_float4 Result;
		Result.x = F1.x + F2.x;
		Result.y = F1.y + F2.y;
		Result.z = F1.z + F2.z;
		Result.w = 1.f;

		return Result;
	}

	FORCEINLINE _float3	 Add_Float3(_float3 F1, _float3 F2)
	{
		_float3 Result;
		Result.x = F1.x + F2.x;
		Result.y = F1.y + F2.y;
		Result.z = F1.z + F2.z;

		return Result;
	}

	FORCEINLINE _float4	 Add_Float4_With_Float3_Coord(_float4 F1, _float3 F2)
	{
		_float4 Result;
		Result.x = F1.x + F2.x;
		Result.y = F1.y + F2.y;
		Result.z = F1.z + F2.z;
		Result.w = 1.f;

		return Result;
	}

	FORCEINLINE _float4	 Add_Float4_With_Float3_Dir(_float4 F1, _float3 F2)
	{
		_float4 Result;
		Result.x = F1.x + F2.x;
		Result.y = F1.y + F2.y;
		Result.z = F1.z + F2.z;
		Result.w = 0.f;

		return Result;
	}
public:
	virtual void Free();
};

END