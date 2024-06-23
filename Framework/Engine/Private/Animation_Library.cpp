#include "Animation_Library.h"
#include "GameInstance.h"
#include "Animation.h"
#include "Model_Extractor.h"

CAnimation_Library::CAnimation_Library()
	: m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CAnimation_Library::Initialize()
{
	return S_OK;
}

HRESULT CAnimation_Library::Add_Prototypes_Animation(const wstring& strAnimLayerTag, const string& strDirPath)
{
	_bool			isExistLayer = { Is_Exist_AnimLayer(strAnimLayerTag) };
	if (true == isExistLayer)
	{
		MSG_BOX(TEXT("Already Exist AnimLayer : HRESULT CAnimation_Library::Add_Prototypes_Animation(const wstring& strAnimLayerTag, const string& strDirPath)"));
		return E_FAIL;
	}

	else
	{
		vector<CAnimation*>			AnimationsPrototypes;
		set<string>					AnimFilePathes;
		
		for (const auto& iter : filesystem::directory_iterator(strDirPath))
		{
			if (true == iter.is_directory())
				continue;

			string				strName = { iter.path().stem().string() };
			string				strExtention = { iter.path().extension().string() };
			string				strFullPath = { strDirPath + strName + string(".fbx") };

			if (".bin" != strExtention && ".fbx" != strExtention)
				continue;			

			AnimFilePathes.emplace(strFullPath);
		}

		for (auto& strFilePath : AnimFilePathes)
		{
			CAnimation*			pAnimation = { CAnimation::Create(strFilePath) };
			if (nullptr == pAnimation)
				return E_FAIL;
			
			AnimationsPrototypes.push_back(pAnimation);
		}

		m_AnimLayers.emplace(strAnimLayerTag, AnimationsPrototypes);
	}

	return S_OK;
}

HRESULT CAnimation_Library::Clone_Animation(const wstring& strAnimLayerTag, _uint iAnimIndex, CAnimation** ppAnimation)
{
	if (false == Is_Exist_AnimLayer(strAnimLayerTag))
		return E_FAIL;

	_uint				iNumAnimLayer = { static_cast<_uint>(m_AnimLayers[strAnimLayerTag].size()) };
	if (iNumAnimLayer <= iAnimIndex)
		return E_FAIL;	

	CAnimation*			pAnimationPrototype = { m_AnimLayers[strAnimLayerTag][iAnimIndex] };
	if (nullptr == pAnimationPrototype)
		return E_FAIL;

	CAnimation*			pClonedAnimation = { pAnimationPrototype->Clone() };
	if (nullptr == pClonedAnimation)
		return E_FAIL;

	*ppAnimation = pClonedAnimation;

	return S_OK;
}

const vector<class CAnimation*>& CAnimation_Library::Get_Animation_PrototypeLayer(const wstring& strAnimLayerTag)
{
	return vector<class CAnimation*>();
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

list<wstring> CAnimation_Library::Get_AnimationLayer_Tags()
{
	list<wstring>			LayerTags;

	for (auto& Pair : m_AnimLayers)
	{
		LayerTags.push_back(Pair.first);
	}

	return LayerTags;
}

list<string> CAnimation_Library::Get_Animation_Tags(const wstring& strAnimLayerTag)
{
	list<string>			AnimTags;
	
	vector<CAnimation*>		Animations = { Find_AnimLayer(strAnimLayerTag) };
	for (auto& pAnimation : Animations)
	{
		string strAnimTag = { pAnimation->Get_Name() };
		AnimTags.emplace_back(strAnimTag);
	}

	return AnimTags;
}

vector<CAnimation*>& CAnimation_Library::Find_AnimLayer(const wstring& strAnimLayerTag)
{
	_bool			isExist = { Is_Exist_AnimLayer(strAnimLayerTag) };

	if (true == isExist)
	{
		return m_AnimLayers[strAnimLayerTag];
	}

	return vector<CAnimation*>();
}

_bool CAnimation_Library::Is_Exist_AnimLayer(const wstring& strAnimLayerTag)
{
	_bool			isExist = { false };

	map<wstring, vector<CAnimation*>>::iterator		iter = { m_AnimLayers.find(strAnimLayerTag) };

	if (iter != m_AnimLayers.end())
		isExist = true;

	return isExist;
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

	Safe_Release(m_pGameInstance);
	m_pGameInstance = nullptr;

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
