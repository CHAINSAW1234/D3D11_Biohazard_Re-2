#include "Animation_Layer.h"
#include "Animation.h"

CAnimation_Layer::CAnimation_Layer()
{
}

HRESULT CAnimation_Layer::Initialize()
{
	return S_OK;
}
HRESULT CAnimation_Layer::Add_Animation(CAnimation* pAnimation)
{
	m_Animations.push_back(pAnimation);

	return S_OK;
}

CAnimation_Layer* CAnimation_Layer::Create()
{
	CAnimation_Layer*			pInstance = { new CAnimation_Layer() };

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CAnimation_Layer"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimation_Layer::Free()
{
	__super::Free();

	for (auto& pAnimation : m_Animations)
	{
		Safe_Release(pAnimation);
		pAnimation = nullptr; 
	}
	m_Animations.clear();
}
