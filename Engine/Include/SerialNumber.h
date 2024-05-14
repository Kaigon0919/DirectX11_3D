#pragma once

#include "Engine.h"

ENGINE_BEGIN

class ENGINE_DLL CSerialNumber
{
public:
	CSerialNumber();
	~CSerialNumber();

private:
	unsigned int	m_iNumber;
	unsigned int*	m_pValidNumber;
	unsigned int	m_iSize; // ��ȯ�� �ø���ѹ� �迭�� ũ��.
	unsigned int	m_iCapacity;
	unsigned int	m_iMin;
	unsigned int	m_iMax;

public:
	void SetMinMax(unsigned int iMin, unsigned int iMax);
	unsigned int GetSerialNumber();
	void AddValidNumber(unsigned int iNumber);
};

ENGINE_END