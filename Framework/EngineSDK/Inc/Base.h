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

	FORCEINLINE _float4	Axis_Rotate_Vector(_float4 Vector, _float4 Axis, _float4 Point, _float RotateAmount)
	{
		_float W = Vector.w;

		_vector Vec = XMLoadFloat4(&Vector);
		_vector Pos = XMLoadFloat4(&Point);
		_vector AxisVec = XMLoadFloat4(&Axis);

		Vec -= Pos;
		_matrix RotationMatrix = XMMatrixRotationAxis(AxisVec, RotateAmount);

		Vec = XMVector4Transform(Vec, RotationMatrix);
		Vec += Pos;

		_float4 Result;
		XMStoreFloat4(&Result, Vec);
		Result.w = W;

		return Result;
	}

	FORCEINLINE _float4 Sub_Float4(_float4 Src, _float4 Dst)
	{
		_float4 Result;
		Result.x = Src.x - Dst.x;
		Result.y = Src.y - Dst.y;
		Result.z = Src.z - Dst.z;
		Result.w = Src.w;

		return Result;
	}

	FORCEINLINE _float4 Float4_Normalize(_float4 vFloat4)
	{
		_vector Result = XMLoadFloat4(&vFloat4);
		Result = XMVector4Normalize(Result);

		_float4 ReturnFloat4;
		XMStoreFloat4(&ReturnFloat4,Result);

		return ReturnFloat4;
	}
public:
	virtual void Free();
};

END