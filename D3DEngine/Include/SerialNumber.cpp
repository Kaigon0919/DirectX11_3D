#include "SerialNumber.h"

ENGINE_USING

CSerialNumber::CSerialNumber() : m_iSize(), m_iNumber(1), m_iCapacity(100), m_iMax(), m_iMin()
{
	m_pValidNumber = new unsigned int[m_iCapacity];
}

CSerialNumber::~CSerialNumber()
{
	SAFE_DELETE_ARRAY(m_pValidNumber);
}

void CSerialNumber::SetMinMax(unsigned int iMin, unsigned int iMax)
{
	m_iMin = iMin;
	m_iMax = iMax;
}

unsigned int CSerialNumber::GetSerialNumber()
{
	if (m_iSize == 0)
	{
		unsigned int iNumber = m_iNumber + m_iMin;
		++m_iNumber;
		return iNumber;
	}

	--m_iSize;
	return m_pValidNumber[m_iSize];
}

void CSerialNumber::AddValidNumber(unsigned int iNumber)
{
	m_pValidNumber[m_iSize] = iNumber;
	++m_iSize;
}