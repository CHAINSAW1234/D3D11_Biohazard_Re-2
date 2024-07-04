#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CPartObject;
class CCollider;
class CBone;
END


BEGIN(Client)

class CPart_InteractProps abstract : public CPartObject
{
public:
	typedef struct tagPart_InteractProps_Desc : public CPartObject::PARTOBJECT_DESC
	{
		const _bool* pRender;
		const _ubyte*	pState;
		_float3*				pRootTranslation = { nullptr };
		wstring				strModelComponentName = { TEXT("") };
		_int				iPropType = {0};
	}PART_INTERACTPROPS_DESC;
	enum Part_INTERACTPROPS_COL
	{
		Part_INTERACTPROPS_COL_AABB,
		Part_INTERACTPROPS_COL_SPHERE,
		Part_INTERACTPROPS_COL_OBB,
		Part_INTERACTPROPS_COL_END
	};

protected:
	CPart_InteractProps(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPart_InteractProps(const CPart_InteractProps& rhs);
	virtual ~CPart_InteractProps() = default;

public:
	virtual HRESULT									Initialize_Prototype() override;
	virtual HRESULT									Initialize(void* pArg) override;
	virtual void									Tick(_float fTimeDelta) override;
	virtual void									Late_Tick(_float fTimeDelta) override;
	virtual HRESULT									Render() override;


	virtual HRESULT									Render_LightDepth_Dir()override;
	virtual HRESULT									Render_LightDepth_Point() override;
	virtual HRESULT									Render_LightDepth_Spot()override;

public:
	void Set_PlayerSetting(class CPlayer* pPlayer, _bool* pPlayerInteract, CTransform* pPlayerTransform)
	{
		m_pPlayer = pPlayer;
		m_pPlayerInteract = pPlayerInteract;
		m_pPlayerTransform = pPlayerTransform;
	}
	void Set_CameraSetting(class CCamera_Free* pCamera, CTransform* pCameraTransform)
	{
		m_pCamera = pCamera;
		m_pCameraTransform = pCameraTransform;
	}
	virtual _float4									Get_Pos(_int iArg = 0) { return XMVectorSetW( m_WorldMatrix.Translation(),1.f); }
	virtual _int									Get_PartObject_Props_ItemIndex() { return -1; };
protected:
	_int											m_iPropType = { 0 };
	_bool											m_bCol = { false };
	_bool*											m_pRender;
	const _ubyte*									m_pState;
	_float4											m_vRotation = {};

	//player
	class CPlayer*									m_pPlayer = { nullptr };
	_bool*											m_pPlayerInteract = {nullptr};
	CTransform*										m_pPlayerTransform = {nullptr};

	//camera
	class CCamera_Free*								m_pCamera= { nullptr };
	CTransform*										m_pCameraTransform = {nullptr};


	CModel*											m_pModelCom = { nullptr };
	CShader*										m_pShaderCom = { nullptr };
	wstring											m_strModelComponentName = { TEXT("") };
	CCollider*										m_pColliderCom[Part_INTERACTPROPS_COL_END] = { nullptr,nullptr,nullptr };

	string											m_strMeshTag = {};

	class CPxCollider*								m_pPx_Collider = { nullptr };
	vector<CBone*>									m_vecRotationBone;

protected:
	void											Check_Col_Sphere_Player();
	HRESULT											Add_Components();
	virtual HRESULT									Add_PartObjects();
	virtual HRESULT									Initialize_PartObjects();
	virtual void									Get_SpecialBone_Rotation();
protected:
	HRESULT											Bind_ShaderResources();

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END