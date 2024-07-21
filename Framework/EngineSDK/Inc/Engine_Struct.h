#pragma once

namespace Engine
{
	typedef struct tagEngineDesc
	{
		HWND			hWnd;
		bool			isWindowed;
		unsigned int	iWinSizeX, iWinSizeY;
	}ENGINE_DESC;

	typedef struct tagLightDesc
	{
		bool bRender = { true };	// 객체에 대한 렌더 : 빛을 끄고 킨다 처리
		bool bShadow = { false };	// 이 라이트를 그림자 연산에 사용할지 여부 처리

		enum TYPE { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_SPOT, TYPE_END };
		TYPE		eType;

		vector<_int> BelongNumVec;
		_int      BelongNum[50];

		float		fRange;
		float 		fCutOff;
		float 		fOutCutOff;
		_float4		vDirection;
		_float4		vPosition;

		_float4		vDiffuse;
		_float4		vAmbient;
		_float4		vSpecular;

		//	_float4X4	ViewMatrix[6];
		//	_float4X4	ProjMatrix;

		_float4x4	ViewMatrix[6];
		_float4x4	ProjMatrix;

	}LIGHT_DESC;


	typedef struct tagMeshMaterial
	{
		class CTexture* MaterialTextures[AI_TEXTURE_TYPE_MAX];
	}MESH_MATERIAL;

	typedef struct tagMaterialDesc
	{
		_float		fMetalic = { 0.f };
		_float		fRoughness = { 0.f };
		_float		fSpecular = { 0.f };
		_float		fEmissive = { 0.f };
	}MATERIAL_DESC;

	typedef struct tagKeyFrame
	{
		_float3		vScale;
		_float4		vRotation;
		_float3		vTranslation;
		float		fTime;
	}KEYFRAME;

	typedef struct ENGINE_DLL tagVtxPos
	{
		_float3		vPosition;

		static const unsigned int	iNumElements = { 1 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[1];
	}VTXPOS;


	typedef struct ENGINE_DLL tagVtxPosTex
	{
		_float3		vPosition;
		_float2		vTexcoord;

		static const unsigned int	iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[2];
	}VTXPOSTEX;



	typedef struct ENGINE_DLL tagVtxCube
	{
		_float3		vPosition;
		_float3		vTexcoord;

		static const unsigned int	iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[2];
	}VTXCUBE;

	typedef struct ENGINE_DLL tagVtxNorTex
	{
		_float3		vPosition;
		_float3		vNormal;
		_float2		vTexcoord;

		static const unsigned int	iNumElements = { 3 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[3];
	}VTXNORTEX;

	typedef struct ENGINE_DLL tagVtxMatrix
	{
		_float4		vRight;
		_float4		vUp;
		_float4		vLook;
		_float4		vPosition;
		bool		isLived;

	}VTXMATRIX;

	typedef struct ENGINE_DLL tagVtxInstance_Rect
	{
		static const unsigned int	iNumElements = { 7 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[7];
	}VTXINSTANCE_RECT;

	typedef struct ENGINE_DLL tagVtxInstance_Point
	{
		static const unsigned int	iNumElements = { 6 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[6];
	}VTXINSTANCE_POINT;

	typedef struct ENGINE_DLL tagVtxMesh
	{
		_float3		vPosition;
		_float3		vNormal;
		_float2		vTexcoord;
		_float3		vTangent;
		_uint		iIndex;

		static const unsigned int	iNumElements = { 5 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[5];
	}VTXMESH;

	typedef struct ENGINE_DLL tagVtxAnimMesh
	{
		_float3		vPosition;
		_float3		vNormal;
		_float2		vTexcoord;
		_float3		vTangent;

		/* 이 정점에게 영향을 주는 뼈들의 인덱스 */
		/* 뼈들의 인덱스 : 이 메시에게 영향을 주는 뼈들의 인덱스를 의미한다. */
		XMUINT4			vBlendIndices;
		_float4		vBlendWeights;
		_uint			iIndex;

		static const unsigned int	iNumElements = { 7 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[7];
	}VTXANIMMESH;

	// 3D음향을 위한 자료형의 모음 구조체
	typedef struct Sound_Desc
	{
		_uint				iChannelIndex = { 0 };
		_bool				isChange = { false };
		_bool				bPause = { true };
		_float2				vRange = { 0.f,1.f };
		_float				fVolume = { SOUND_DEFAULT };
		FMOD_VECTOR			vPos = { FMOD_VECTOR() };
		FMOD_VECTOR			vSpeedDir = { FMOD_VECTOR() };
		FMOD_SOUND*			pSound = { nullptr };
		FMOD_MODE			eMode = { FMOD_DEFAULT };

		Sound_Desc() = default;
		Sound_Desc(const Sound_Desc&) = default;
		Sound_Desc& operator=(const Sound_Desc&) = default;
		Sound_Desc(Sound_Desc&&) = default;
		Sound_Desc& operator=(Sound_Desc&&) = default;

	}SOUND_DESC;

	//For Ragdoll
	struct PoseTransforms
	{
		_matrix transforms[MAX_BONES];
	};

	//For PathFinding Algorithm
	typedef struct
	{
		_uint parent;

		_float f, g, h;
	}CELL;

#pragma region Decal
	struct SubModelInfo
	{
		UINT firstIndex;
		UINT numTris;
		float decalLookupMapWidth;
		float decalLookupMapHeight;
	};

	struct DecalConstData
	{
		SubModelInfo subModelInfo;
		_float4 rayOrigin;
		_float4 rayDir;
		_float4 decalTangent;
		_float4 hitDistances; // x = minHitDistance, y = maxHitDistance - minHitDistance, z = 1.0f / y
		float decalLookupRtWidth;
		float decalLookupRtHeight;
		float decalSizeX;
		float decalSizeY;
		float decalSizeZ;
		UINT decalIndex;
		UINT decalMaterialIndex;
		UINT iMeshIndex;
	};

	struct DecalInfo
	{
		UINT decalHitMask; // bit 0-19: triangleIndex, bit 20-23: subModelIndex, bit 24-31: hitDistance
		UINT decalIndex;
		_float2 decalScale;
		_float2 decalBias;
		_float3 decalNormal;
		_float4x4 decalMatrix;
		_uint	 isHit;
	};

	struct HitResult
	{
		class CGameObject* hitModel;
		float minHitDistance;
		float maxHitDistance;
	};

	struct AddDecalInfo
	{
		_float4 rayOrigin;
		_float4 rayDir;
		_float4 decalTangent;
		_float3 decalSize;
		float	minHitDistance;
		float	maxHitDistance;
		UINT decalMaterialIndex;
	};

	typedef struct
	{
		_float3		vPosition;
		_float3		vNormal;
		_float3		vTangent;

	}SKINNING_OUTPUT;

	typedef struct
	{
		ID3D11UnorderedAccessView* pUav;
		ID3D11ShaderResourceView* pSRV_Vertex_Position;
		ID3D11ShaderResourceView* pSRV_Vertex_Normal;
		ID3D11ShaderResourceView* pSRV_Vertex_Tangent;
		ID3D11ShaderResourceView* pSRV_Vertex_BlendIndices;
		ID3D11ShaderResourceView* pSRV_Vertex_BlendWeights;
		_uint iNumVertex;
	}SKINNING_INPUT;

	typedef struct
	{
		ID3D11UnorderedAccessView* pUav_Info;
		ID3D11UnorderedAccessView* pUav_Skinning;
		ID3D11ShaderResourceView* pSRV_Indices;
		ID3D11Buffer* pCB_Decal;
		_uint iNumTriangle;
	}RAYCASTING_INPUT;

	typedef struct
	{
		ID3D11UnorderedAccessView* pUav_Info;
		ID3D11UnorderedAccessView* pUav_Skinning;
		ID3D11ShaderResourceView* pSRV_Indices;
		ID3D11Buffer* pCB_Decal;
	}CALC_DECAL_INPUT;

	typedef struct
	{
		ID3D11UnorderedAccessView* pUav_Skinning;
		ID3D11UnorderedAccessView* pDecalMap;
		ID3D11ShaderResourceView*  pSRV_Texcoords;
		_float4x4				   Decal_Matrix_Inv;
		_float3					   vExtent;
		_uint iNumVertex;
	}CALC_DECAL_MAP_INPUT;

	typedef struct
	{
		ID3D11ShaderResourceView*  pSRV_Vertices;
		ID3D11UnorderedAccessView* pDecalMap;
		_float4x4				   Decal_Matrix_Inv;
		_float3					   vExtent;
		_uint iNumVertex;
	}CALC_DECAL_MAP_INPUT_STATIC_MODEL;
#pragma endregion


	typedef struct tagImgSize
	{
		_uint      iSizeX = { 0 };
		_uint      iSizeY = { 0 };
	}IMG_SIZE;
}