#include "..\Public\PartObject.h"

CPartObject::CPartObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject{ pDevice, pContext }
{
}

CPartObject::CPartObject(const CPartObject & rhs)
	: CGameObject{ rhs }
{
}

HRESULT CPartObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPartObject::Initialize(void * pArg)
{
	if (nullptr != pArg)
	{
		PARTOBJECT_DESC*		pPartObjectDesc = (PARTOBJECT_DESC*)pArg;

		m_pParentsTransform = pPartObjectDesc->pParentsTransform;
		Safe_AddRef(m_pParentsTransform);
		if (nullptr == m_pParentsTransform)
		{
			MSG_BOX(TEXT("부모 트랜스폼 없는 파트오브젝튼 없다"));
			return E_FAIL;
		}
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CPartObject::Priority_Tick(_float fTimeDelta)
{
}

void CPartObject::Tick(_float fTimeDelta)
{
	Update_WorldMatrix();
}

void CPartObject::Late_Tick(_float fTimeDelta)
{
}

HRESULT CPartObject::Render()
{
	return S_OK;
}

void CPartObject::Update_WorldMatrix()
{
	_matrix			ParentsWorldMatrix = { m_pParentsTransform->Get_WorldMatrix() };
	_matrix			WorldTransform = { m_pTransformCom->Get_WorldMatrix() };
	_matrix			WorldMatrix = { WorldTransform * ParentsWorldMatrix };

	XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
}

void CPartObject::Free()
{
	__super::Free();

	Safe_Release(m_pParentsTransform);
}
