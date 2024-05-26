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

		float		fRange;
		float 		fCutOff;
		float 		fOutCutOff;
		XMFLOAT4	vDirection;
		XMFLOAT4	vPosition;

		XMFLOAT4	vDiffuse;
		XMFLOAT4	vAmbient;
		XMFLOAT4	vSpecular;

		XMFLOAT4X4	ViewMatrix[6];
		XMFLOAT4X4	ProjMatrix;

	}LIGHT_DESC;
	

	typedef struct tagMeshMaterial
	{
		class CTexture*	MaterialTextures[AI_TEXTURE_TYPE_MAX];
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
		XMFLOAT3	vScale;
		XMFLOAT4	vRotation;
		XMFLOAT3	vTranslation;
		float		fTime;
	}KEYFRAME;

	typedef struct ENGINE_DLL tagVtxPos
	{
		XMFLOAT3		vPosition;		

		static const unsigned int	iNumElements = { 1 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[1];
	}VTXPOS;


	typedef struct ENGINE_DLL tagVtxPosTex
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexcoord;

		static const unsigned int	iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[2];
	}VTXPOSTEX;



	typedef struct ENGINE_DLL tagVtxCube
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vTexcoord;

		static const unsigned int	iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[2];
	}VTXCUBE;

	typedef struct ENGINE_DLL tagVtxNorTex
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;

		static const unsigned int	iNumElements = { 3 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[3];
	}VTXNORTEX;

	typedef struct ENGINE_DLL tagVtxMatrix
	{
		XMFLOAT4		vRight;
		XMFLOAT4		vUp;
		XMFLOAT4		vLook;
		XMFLOAT4		vPosition;		
		bool			isLived;

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
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;	

		static const unsigned int	iNumElements = { 4 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[4];
	}VTXMESH;

	typedef struct ENGINE_DLL tagVtxAnimMesh
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;

		/* 이 정점에게 영향을 주는 뼈들의 인덱스 */
		/* 뼈들의 인덱스 : 이 메시에게 영향을 주는 뼈들의 인덱스를 의미한다. */
		XMUINT4			vBlendIndices;
		XMFLOAT4		vBlendWeights;

		static const unsigned int	iNumElements = { 6 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[6];
	}VTXANIMMESH;

	//For Physics_Controller
	enum COLLISION_CATEGORY {
		Category1 = 1 << 0,
		Category2 = 1 << 1,
		Category3 = 1 << 2,
	};
	// 3D음향을 위한 자료형의 모음 구조체
	typedef struct Sound_Desc
	{
		_bool	 bPause = { true };
		_float2 fRange = { 0.f,1.f };
		_float fVolume = { SOUND_DEFAULT };
		FMOD_VECTOR vPos = { FMOD_VECTOR() };
		FMOD_VECTOR vSpeedDir = { FMOD_VECTOR() };
		FMOD_SOUND* pSound = { nullptr };
		FMOD_MODE eMode = { FMOD_DEFAULT };

		Sound_Desc() = default;
		Sound_Desc(const Sound_Desc&) = default;
		Sound_Desc& operator=(const Sound_Desc&) = default;
		Sound_Desc(Sound_Desc&&) = default;
		Sound_Desc& operator=(Sound_Desc&&) = default;

	}SOUND_DESC;

}