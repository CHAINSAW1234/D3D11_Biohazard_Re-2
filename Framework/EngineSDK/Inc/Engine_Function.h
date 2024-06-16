#pragma once

namespace Engine
{
	template<typename T>
	void Safe_Delete(T& pPoint)
	{
		if (nullptr != pPoint)
		{
			delete pPoint;
			pPoint = nullptr;
		}	
	}

	template<typename T>
	void Safe_Delete_Array(T& pPoint)
	{
		if (nullptr != pPoint)
		{
			delete[] pPoint;
			pPoint = nullptr;
		}
	}


	template<typename T>
	unsigned int Safe_AddRef(T& pInstance)
	{
		unsigned int iRefCnt = 0;

		if (nullptr != pInstance)		
			iRefCnt = pInstance->AddRef();

		return iRefCnt;
	}

	template<typename T>
	unsigned int Safe_Release(T& pInstance)
	{
		unsigned int iRefCnt = 0;

		if (nullptr != pInstance)
		{
			iRefCnt = pInstance->Release();

			if(0 == iRefCnt)
				pInstance = nullptr;
		}

		return iRefCnt;
	}

	inline _float Cal_Degree_From_Directions_Between_Min180_To_180(_float4 vDir1, _float4 vDir2) {
		// 1. 정규화 한 두 벡터를 가지고 각도를 계산 하기
		_float fRadian = XMVector3AngleBetweenNormals(vDir1, vDir2).m128_f32[0];

		// 외적으로 방향 찾기
		_float  vCross = XMVector3Cross(vDir1, vDir2).m128_f32[1];

		// 각도를 -180 ~ 180도 사이로 세팅
		_float fDegree = XMConvertToDegrees(fRadian * vCross / abs(vCross)); // 0 ~ 180 사이의 값 
		// 각도가 0보다 크면 왼쪽이다
		if (isnan(fDegree))
		{
			if (XMVector3Equal(vDir1, XMVectorScale(vDir2, -1.f)))
				return 180.f;
			else
				return 0.f;
		}

		return fDegree;
	}

}

