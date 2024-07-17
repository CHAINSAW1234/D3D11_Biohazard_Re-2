#include "stdafx.h"
#include "Prop_Controller.h"

#include "Call_Center.h"

CProp_Controller::CProp_Controller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CProp_Controller::CProp_Controller(const CProp_Controller& rhs)
	: CGameObject{ rhs }
	, m_iCallerType{ rhs.m_iCallerType }
{
}

HRESULT CProp_Controller::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CProp_Controller::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	PROP_CONTROLL_DESC*				pDesc = { static_cast<PROP_CONTROLL_DESC*>(pArg) };
	m_strAnimLayerTag = pDesc->strAnimLayerTag;
	if (TEXT("") == m_strAnimLayerTag)
		return E_FAIL;

	return S_OK;
}

void CProp_Controller::Priority_Tick(_float fTimeDelta)
{
}

void CProp_Controller::Tick(_float fTimeDelta)
{
}

void CProp_Controller::Late_Tick(_float fTimeDelta)
{
}

HRESULT CProp_Controller::Render()
{
	return S_OK;
}

void CProp_Controller::Late_Initialize()
{
	if (FAILED(Sort_Animation_Seq()))
		return;	
}

void CProp_Controller::Start()
{
	CGameObject* pGameObject = { CCall_Center::Get_Instance()->Get_Caller(static_cast<CCall_Center::CALLER>(m_iCallerType)) };
	if (nullptr == pGameObject)
		return;

	m_pPropObject = pGameObject;
	Safe_AddRef(m_pPropObject);
}

HRESULT CProp_Controller::Set_Animation(_uint iAnimIndex)
{
	if (nullptr == m_pBodyModel)
		return E_FAIL;

	_uint				iNumAnims = { static_cast<_uint>(m_pBodyModel->Get_Animations(m_strAnimLayerTag).size()) };
	if (iNumAnims <= iAnimIndex)
		return E_FAIL;

	m_pBodyModel->Change_Animation(0, m_strAnimLayerTag, iAnimIndex);
	
#ifdef _DEBUG

	list<string>			AnimationTags = { m_pBodyModel->Get_Animation_Tags(m_strAnimLayerTag) };
	list<string>::iterator			iter = { AnimationTags.begin() };
	for (_uint i = 0; i < iAnimIndex; ++i)
	{
		++iter;
	}
	cout << "ChangeAnim : " << *iter << endl;

#endif

	return S_OK;
}

void CProp_Controller::Set_Next_Animation_Sequence()
{
	if (nullptr == m_pBodyModel)
		return;

	++m_iCurSeqLev;

	unordered_map<wstring, vector<string>>::iterator			iter = { m_Animations_Seq.find(m_strAnimLayerTag) };
	if (iter == m_Animations_Seq.end())
		return;

	_uint				iNumSequence = { static_cast<_uint>(iter->second.size()) };
	if (iNumSequence <= m_iCurSeqLev)
		return;

	string				strAnimTag = { iter->second[m_iCurSeqLev] };

#ifdef _DEBUG

	cout << "ChangeAnim : " << strAnimTag << endl;

#endif

	m_pBodyModel->Change_Animation(0, m_strAnimLayerTag, strAnimTag);
	m_pBodyModel->Set_TrackPosition(0, 0.f, false);
}

void CProp_Controller::Set_Loop(_bool isLoop)
{
	if (nullptr == m_pBodyModel)
		return;

	m_pBodyModel->Set_Loop(0, isLoop);
}

void CProp_Controller::Reset_Animations()
{
	if (nullptr == m_pBodyModel)
		return;

	m_iCurSeqLev = 0;

	m_pBodyModel->Set_TrackPosition(0, 0.f, false);

	string			strAnimTag = { m_Animations_Seq.find(m_strAnimLayerTag)->second[m_iCurSeqLev] };

	m_pBodyModel->Change_Animation(0, m_strAnimLayerTag, strAnimTag);
}

_bool CProp_Controller::Is_Finished_Animation()
{
	if (nullptr == m_pBodyModel)
		return false;

	return m_pBodyModel->isFinished(0);
}

_bool CProp_Controller::Is_Finished_Animation_All()
{
	if (nullptr == m_pBodyModel)
		return false;

	wstring             strCurrentAnimLayerTag = { m_pBodyModel->Get_CurrentAnimLayerTag(0) };
	_int                iAnimIndex = { m_pBodyModel->Get_CurrentAnimIndex(0) };
	_uint               iNumAnims = { static_cast<_uint>(m_pBodyModel->Get_Animations(strCurrentAnimLayerTag).size()) };

	if (strCurrentAnimLayerTag != m_strAnimLayerTag)
		return false;

	//  마지막 애니메이션이 아니라면
	if (iNumAnims - 1 != iAnimIndex)
		return false;

	if (false == m_pBodyModel->isFinished(0))
		return false;

	return true;
}

HRESULT CProp_Controller::Sort_Animation_Seq()
{
	if (nullptr == m_pBodyModel)
		return E_FAIL;

	list<wstring>				AnimLayerTags = { m_pBodyModel->Get_AnimationLayer_Tags() };
	for (auto& strAnimLayerTag : AnimLayerTags)
	{
		if (strAnimLayerTag != m_strAnimLayerTag)
			continue;

		list<string>			AnimationTags = { m_pBodyModel->Get_Animation_Tags(strAnimLayerTag) };
		_uint					iLevel = { 0 };
		vector<string>			SeqAnimTags;
		for (auto& strAnimTag : AnimationTags)
		{
			if (strAnimTag == "")
			{
				SeqAnimTags.push_back("");
				++iLevel;
				continue;
			}
			_uint				iPos = { static_cast<_uint>(strAnimTag.rfind(L'_')) };
			_uint				iSeqLevel = { static_cast<_uint>(stoi(strAnimTag.substr(iPos + 1))) };

			while (iSeqLevel != iLevel)
			{
				SeqAnimTags.push_back("");
				++iLevel;
			}

			SeqAnimTags.push_back(strAnimTag);
			++iLevel;
		}

		m_Animations_Seq.emplace(strAnimLayerTag, SeqAnimTags);
	}

	return S_OK;
}

void CProp_Controller::Free()
{
	__super::Free();

	Safe_Release(m_pBodyModel);
}
