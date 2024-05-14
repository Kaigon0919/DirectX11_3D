#pragma once

#include "Engine.h"

ENGINE_BEGIN

class ENGINE_DLL CRef
{
protected:
	CRef();
	virtual ~CRef() = 0;

protected:
	int		m_iRefCount;
	string	m_strTag;
	bool	m_bActive;
	bool	m_bEnable;
	unsigned int	m_iSerialNumber;
public:
	unsigned int GetSerialNumber()	const;
	void SetSerialNumber(unsigned int iNumber);
	void Release();
	void AddRef();
	void SetTag(const string& strTag);
	string GetTag()	const;
	bool IsEnable()	const;
	bool IsActive()	const;
	void Enable(bool bEnable);
	void Active(bool bActive);
};

ENGINE_END
