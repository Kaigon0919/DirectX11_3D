#pragma once
#include "afxwin.h"

#include "GameObject.h"
#include "Component/Animation.h"
#include "Component/Renderer.h"

ENGINE_USING

// CEditorForm 폼 뷰입니다.

class CEditorForm : public CFormView
{
	DECLARE_DYNCREATE(CEditorForm)

protected:
	CEditorForm();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CEditorForm();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_FORM };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

private:
	class CGameObject*	m_pObj;
	class CAnimation*	m_pAnimation;
	class CRenderer*	m_pRenderer;
	vector<PBONEKEYFRAME>	m_vecKeyFrame;

	DECLARE_MESSAGE_MAP()
public:
	CString m_strName;
	CString m_strClipName;
	UINT m_iStartFrame;
	UINT m_iEndFrame;
	float m_fPlayTime;
	CComboBox m_OptionCombo;
	CListBox m_ClipList;
	afx_msg void OnBnClickedButtonAddclip();
	afx_msg void OnBnClickedButtonModifyclip();
	afx_msg void OnBnClickedButtonDeleteclip();
	afx_msg void OnBnClickedButtonMeshload();
	afx_msg void OnBnClickedButtonMeshsave();
	afx_msg void OnBnClickedButtonFbxload();
	afx_msg void OnBnClickedButtonClipload();
	afx_msg void OnBnClickedButtonClipsave();
	afx_msg void OnLbnSelchangeListClip();
	afx_msg void OnBnClickedCheckFramecopy();
	BOOL m_bKeyFrameCopy;
};


