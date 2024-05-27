#include "..\Public\Base.h"



CBase::CBase()
	: m_iRefCnt{0}
{
}

unsigned int CBase::AddRef()
{
	return ++m_iRefCnt;
}

unsigned int CBase::Release()
{
	if (m_iRefCnt > 1000000000)
	{
		MSG_BOX(TEXT("OverRelease 고치고 작업해라"));
	}

	if (0 == this->m_iRefCnt)
	{
		Free();

		delete this;

		return 0;
	}
	else
	{
		
		return this->m_iRefCnt--;
	}
}


void CBase::Free()
{

}