#pragma once
#include"Engine.h"

ENGINE_BEGIN

template<class T>
class ENGINE_DLL CHeap
{
private:
	T* m_pArray;
	int m_iSize;
	int m_iCapacity;
	function<bool(const T&, const T&)> m_CmpFunc;

public:
	CHeap() : m_iSize(), m_iCapacity(100)
	{
		m_pArray = new T[m_iCapacity];
		SetSortFunc(CHeap<T>::Sort);
	}
	~CHeap()
	{
		SAFE_DELETE_ARRAY(m_pArray);
	}
public:
	void SetSortFunc(bool(*pFunc)(const T&, const T&))
	{
		m_CmpFunc = bind(pFunc, placeholders::_1, placeholders::_2);
	}

	template<class ClassType>
	void SetSortFunc(ClassType* pObj, bool(ClassType::*pFunc)(const T&, const T&))
	{
		m_CmpFunc = bind(pFunc, pObj, placeholders::_1, placeholders::_2);
	}
private:
	void Resize()
	{
		if (m_iCapacity == m_iSize)
		{
			m_iCapacity *= 2;
			T* pList = new T[m_iCapacity];
			memset(pList, 0, sizeof(T)*m_iCapacity);

			memcpy(pList, m_pArray, sizeof(T) * m_iSize);

			SAFE_DELETE_ARRAY(m_pArray);
			m_pArray = pList;
		}
	}

public:
	void Resize(int iCapacity)
	{
		m_iCapacity = iCapacity;

		T* pList = new T[m_iCapacity];
		memset(pList, 0, sizeof(T)* m_iCapacity);

		SAFE_DELETE_ARRAY(m_pArray);
		m_pArray = pList;
	}

	void Insert(const T& data)
	{
		Resize();

		m_pArray[m_iSize] = data;
		++m_iSize;

		_Insert(m_iSize - 1);
	}
	bool Pop(T& data)
	{
		if (m_iSize == 0)
			return false;

		data = m_pArray[0];

		m_pArray[0] = m_pArray[m_iSize - 1];
		--m_iSize;

		_Pop(0);

		return true;
	}

	void Clear()
	{
		m_iSize = 0;
	}

	bool empty()	const
	{
		return m_iSize == 0;
	}

	void Sort()
	{
		int	iIndex = m_iSize / 2 - 1;

		while (iIndex >= 0)
		{
			int	iBuildCount = 1;
			int	iBuildIndex = iIndex + 1;

			while (iBuildIndex /= 2)
			{
				iBuildCount *= 2;
			}

			--iBuildCount;

			iBuildCount = iIndex - iBuildCount;

			for (int i = 0; i <= iBuildCount; ++i)
			{
				_Pop(iIndex - i);
			}

			iIndex = iIndex - iBuildCount - 1;
		}
	}
	private:
		void _Insert(int iIndex)
		{
			if (iIndex == 0)
				return;

			int	iParent = (iIndex - 1) / 2;

			if (m_CmpFunc(m_pArray[iParent], m_pArray[iIndex]))
			{
				T	data = m_pArray[iParent];
				m_pArray[iParent] = m_pArray[iIndex];
				m_pArray[iIndex] = data;

				_Insert(iParent);
			}
		}

		void _Pop(int iIndex)
		{
			// 왼쪽 자식의 인덱스를 구한다.
			int	iLeftChild = iIndex * 2 + 1;

			if (iLeftChild >= m_iSize)
				return;

			int	iRightChild = iLeftChild + 1;
			int	iChildIndex = iLeftChild;

			if (iRightChild < m_iSize)
			{
				if (m_CmpFunc(m_pArray[iLeftChild], m_pArray[iRightChild]))
					iChildIndex = iRightChild;
			}

			if (m_CmpFunc(m_pArray[iIndex], m_pArray[iChildIndex]))
			{
				T	data = m_pArray[iChildIndex];
				m_pArray[iChildIndex] = m_pArray[iIndex];
				m_pArray[iIndex] = data;

				_Pop(iChildIndex);
			}
		}

private:
	static bool Sort(const T& src, const T& dest)
	{
		return src > dest;
	}
};

ENGINE_END