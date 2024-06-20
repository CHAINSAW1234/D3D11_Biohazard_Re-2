#include "Animation_Library.h"
#include "Animation.h"

CAnimation_Library::CAnimation_Library()
{
}

HRESULT CAnimation_Library::Initialize()
{
	return S_OK;
}

HRESULT CAnimation_Library::Add_Prototype_Animation(const wstring& strAnimLayerTag, const wstring& strPrototypeTag)
{
	return S_OK;
}

const vector<class CAnimation*>& CAnimation_Library::Get_Animation_PrototypeLayer(const wstring& strAnimLayerTag)
{

}

CAnimation* CAnimation_Library::Get_Animation_Prototype(const wstring& strAnimLayerTag, _uint iIndex)
{
	vector<CAnimation*>			AnimLayer = { Find_AnimLayer(strAnimLayerTag) };
	_uint						iNumAnims = { static_cast<_uint>(AnimLayer.size()) };

	CAnimation*					pAnim = { nullptr };

	if (iNumAnims > iIndex)
	{
		pAnim = AnimLayer[iIndex];
	}

	return pAnim;
}

_uint CAnimation_Library::Get_NumAnim_Prototypes(const wstring& strAnimLayerTag)
{
	_uint			iNumAnims = { static_cast<_uint>(Find_AnimLayer(strAnimLayerTag).size()) };
	return iNumAnims;
}

vector<CAnimation*>& CAnimation_Library::Find_AnimLayer(const wstring& strAnimLayerTag)
{
	_uint			iNumAnims = { 0 };
	map<wstring, vector<CAnimation*>>::iterator			iter = { m_AnimLayers.find(strAnimLayerTag) };
	if (iter != m_AnimLayers.end())
	{
		return m_AnimLayers[strAnimLayerTag];
	}

	return vector<CAnimation*>();
}

CAnimation_Library* CAnimation_Library::Create()
{
	CAnimation_Library*			pInstance = { new CAnimation_Library() };

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CAnimation_Library"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimation_Library::Free()
{
	__super::Free();

	for (auto& Pair : m_AnimLayers)
	{
		for (auto& pAnimation : Pair.second)
		{
			Safe_Release(pAnimation);
			pAnimation = nullptr;
		}
		Pair.second.clear();
	}
	m_AnimLayers.clear();
}
