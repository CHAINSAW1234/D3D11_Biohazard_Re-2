#include "Base.h"

extern int g_MaxTriangles;

extern int g_MaxSubdivisions;

extern int g_EndNodeCount;

extern int g_TotalNodesDrawn;

extern bool g_bLighting;

extern bool g_RenderMode;

extern int g_MaxTriangles = 5000;

extern int g_MaxSubdivisions =4;

extern int g_EndNodeCount = 0;

extern _int Cook_Temp = 0;

enum eOctreeNodes
{
	TOP_LEFT_FRONT,			
	TOP_LEFT_BACK,			
	TOP_RIGHT_BACK,			
	TOP_RIGHT_FRONT,
	BOTTOM_LEFT_FRONT,
	BOTTOM_LEFT_BACK,
	BOTTOM_RIGHT_BACK,
	BOTTOM_RIGHT_FRONT
};

struct tFaceList
{
	vector<bool> pFaceList;

	int totalFaceCount;
};

BEGIN(Engine)

class ENGINE_DLL COctree : public CBase
{

public:

	COctree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,class CGameInstance* pGameInstance,_float4 vTranslation);
	~COctree();

	_float4						GetCenter()
	{
		return m_vCenter;
	}
	int							GetTriangleCount() 
	{
		return m_TriangleCount; 
	}
	float						GetWidth()
	{
		return m_Width;
	}
	bool						IsSubDivided()
	{
		return m_bSubDivided;
	}
	void						GetSceneDimensions(class CModel* pWorld);
	int							GetSceneTriangleCount(CModel* pWorld);
	int							GetDisplayListID() { return m_DisplayListID; }
	void						SetDisplayListID(int displayListID) { m_DisplayListID = displayListID; }
	_float4						GetNewNodeCenter(_float4 vCenter, float width, int nodeID);
	void						CreateNode(CModel* pWorld, int numberOfTriangles, _float4 vCenter, float width);
	void						CreateNewNode(CModel* pWorld, vector<tFaceList> pList, int triangleCount,_float4 vCenter, float width, int nodeID);
	void						AssignTrianglesToNode(CModel* pWorld, int numberOfTriangles);
	void						DrawOctree(COctree* pNode, CModel* pRootWorld,class CShader* pShader);
	void						DrawOctree_Thread(COctree* pNode);
	void						DrawOctree_Thread_Internal(COctree* pNode, vector<class CModel*>* vecModel);
	void						DrawOctree_1();
	void						DrawOctree_2();
	void						DrawOctree_3();
	void						DrawOctree_4();
	void						DrawOctree_5();
	void						DrawOctree_6();
	void						DrawOctree_7();
	void						DrawOctree_8();
	void						Render_Node(CModel* pRootWorld, CShader* pShader);
public:
	bool								m_bSubDivided;
	float								m_Width;
	int									m_TriangleCount;
	_float4								m_vCenter;
	CModel*								m_pWorld;
	vector<int>							m_pObjectList;
	int									m_DisplayListID;
	COctree*							m_pOctreeNodes[8];
	class CGameInstance*				m_pGameInstance = { nullptr };
	_float4								m_vTranslation;
	vector<class CModel*>				m_vecEntryNode;
	vector<vector<class CMesh*>>		m_vecMesh;
private:
	ID3D11Device*						m_pDevice = { nullptr };
	ID3D11DeviceContext*				m_pContext = { nullptr };

public:
	virtual void Free() override;
};

END
