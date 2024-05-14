#include "Ref.h"

ENGINE_USING

CRef::CRef()	:
	m_iRefCount(1),
	m_bActive(true),
	m_bEnable(true),
	m_iSerialNumber(UINT_MAX)
{
}

CRef::~CRef()
{
}

unsigned int CRef::GetSerialNumber() const
{
	return m_iSerialNumber;
}

void CRef::SetSerialNumber(unsigned int iNumber)
{
	m_iSerialNumber = iNumber;
}

void CRef::Release()
{
	--m_iRefCount;

	if (m_iRefCount == 0)
		delete	this;
}

void CRef::AddRef()
{
	++m_iRefCount;
}

void CRef::SetTag(const string& strTag)
{
	m_strTag = strTag;
}

string CRef::GetTag() const
{
	return m_strTag;
}

bool CRef::IsEnable() const
{
 	return m_bEnable;
}

bool CRef::IsActive() const
{
	return m_bActive;
}

void CRef::Enable(bool bEnable)
{
	m_bEnable = bEnable;
}

void CRef::Active(bool bActive)
{
	m_bActive = bActive;
}
