// EditorForm.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Editor.h"
#include "EditorForm.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "Scene/Layer.h"
#include "Resource/ResourcesManager.h"
#include "Component/Transform.h"
#include "Component/Arm.h"

// CEditorForm

IMPLEMENT_DYNCREATE(CEditorForm, CFormView)

CEditorForm::CEditorForm()
	: CFormView(IDD_DIALOG_FORM)
	, m_strName(_T(""))
	, m_strClipName(_T(""))
	, m_iStartFrame(0)
	, m_iEndFrame(0)
	, m_fPlayTime(0)
	, m_bKeyFrameCopy(FALSE)
	, m_pObj(nullptr)
	, m_pAnimation(nullptr)
	, m_pRenderer(nullptr)
{

}

CEditorForm::~CEditorForm()
{
	Safe_Delete_VecList(m_vecKeyFrame);
	SAFE_RELEASE(m_pObj);
	SAFE_RELEASE(m_pAnimation);
	SAFE_RELEASE(m_pRenderer);
}

void CEditorForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_MESHNAME, m_strName);
	DDX_Text(pDX, IDC_EDIT_CLIPNAME, m_strClipName);
	DDX_Text(pDX, IDC_EDIT_STARTFRAME, m_iStartFrame);
	DDX_Text(pDX, IDC_EDIT_ENDFRAME, m_iEndFrame);
	DDX_Text(pDX, IDC_EDIT_PLAYTIME, m_fPlayTime);
	DDX_Control(pDX, IDC_COMBO_OPTION, m_OptionCombo);
	DDX_Control(pDX, IDC_LIST_CLIP, m_ClipList);
	DDX_Check(pDX, IDC_CHECK_FRAMECOPY, m_bKeyFrameCopy);
}

BEGIN_MESSAGE_MAP(CEditorForm, CFormView)
	ON_BN_CLICKED(IDC_BUTTON_ADDCLIP, &CEditorForm::OnBnClickedButtonAddclip)
	ON_BN_CLICKED(IDC_BUTTON_MODIFYCLIP, &CEditorForm::OnBnClickedButtonModifyclip)
	ON_BN_CLICKED(IDC_BUTTON_DELETECLIP, &CEditorForm::OnBnClickedButtonDeleteclip)
	ON_BN_CLICKED(IDC_BUTTON_MESHLOAD, &CEditorForm::OnBnClickedButtonMeshload)
	ON_BN_CLICKED(IDC_BUTTON_MESHSAVE, &CEditorForm::OnBnClickedButtonMeshsave)
	ON_BN_CLICKED(IDC_BUTTON_FBXLOAD, &CEditorForm::OnBnClickedButtonFbxload)
	ON_BN_CLICKED(IDC_BUTTON_CLIPLOAD, &CEditorForm::OnBnClickedButtonClipload)
	ON_BN_CLICKED(IDC_BUTTON_CLIPSAVE, &CEditorForm::OnBnClickedButtonClipsave)
	ON_LBN_SELCHANGE(IDC_LIST_CLIP, &CEditorForm::OnLbnSelchangeListClip)
	ON_BN_CLICKED(IDC_CHECK_FRAMECOPY, &CEditorForm::OnBnClickedCheckFramecopy)
END_MESSAGE_MAP()


// CEditorForm 진단입니다.

#ifdef _DEBUG
void CEditorForm::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CEditorForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CEditorForm 메시지 처리기입니다.

// 클립 추가
void CEditorForm::OnBnClickedButtonAddclip()
{
	if (!m_pAnimation)
		return;

	UpdateData(TRUE);

	char	strName[256] = {};

	WideCharToMultiByte(CP_ACP, 0, m_strClipName.GetString(), -1,
		strName, m_strClipName.GetLength(), 0, 0);

	//  클립 이름만 수정한다.
	if (!m_vecKeyFrame.empty())
	{
		m_pAnimation->AddClip(strName, (ANIMATION_OPTION)m_OptionCombo.GetCurSel(),
			m_iStartFrame, m_iEndFrame, m_fPlayTime,
			m_vecKeyFrame);

		m_ClipList.AddString(m_strClipName);
	}
}

// 클립 수정
void CEditorForm::OnBnClickedButtonModifyclip()
{
	if (!m_pAnimation)
		return;

	int	iListIndex = m_ClipList.GetCurSel();

	if (iListIndex == -1)
		return;

	CString	strListName;
	m_ClipList.GetText(iListIndex, strListName);

	char	strOriginName[256] = {};

	WideCharToMultiByte(CP_ACP, 0, strListName.GetString(), -1,
		strOriginName, strListName.GetLength(), 0, 0);

	UpdateData(TRUE);

	char	strName[256] = {};

	WideCharToMultiByte(CP_ACP, 0, m_strClipName.GetString(), -1,
		strName, m_strClipName.GetLength(), 0, 0);

	m_ClipList.DeleteString(iListIndex);
	m_ClipList.AddString(m_strClipName);

	//  클립 이름만 수정한다.
	if (m_vecKeyFrame.empty())
	{
		m_pAnimation->ChangeClipKey(strOriginName, strName);
	}

	else
	{
		m_pAnimation->ModifyClip(strOriginName, strName, 
			(ANIMATION_OPTION)m_OptionCombo.GetCurSel(),
			m_iStartFrame, m_iEndFrame, m_fPlayTime, m_vecKeyFrame);
	}
}

// 클립 삭제
void CEditorForm::OnBnClickedButtonDeleteclip()
{
	if (!m_pAnimation)
		return;

	else if (m_ClipList.GetCount() == 1)
		return;

	int	iListIndex = m_ClipList.GetCurSel();

	if (iListIndex == -1)
		return;

	CString	strListName;
	m_ClipList.GetText(iListIndex, strListName);

	char	strOriginName[256] = {};

	WideCharToMultiByte(CP_ACP, 0, strListName.GetString(), -1,
		strOriginName, strListName.GetLength(), 0, 0);

	char	strName[256] = {};

	m_ClipList.DeleteString(iListIndex);

	m_pAnimation->DeleteClip(strOriginName);

	PANIMATIONCLIP	pClip = m_pAnimation->GetCurrentClip();

	m_strClipName = CA2CT(pClip->strName.c_str());
	m_iStartFrame = pClip->iStartFrame;
	m_iEndFrame = pClip->iEndFrame;
	m_fPlayTime = pClip->fPlayTime;
	m_OptionCombo.SetCurSel(pClip->eOption);

	UpdateData(FALSE);
}

// 메쉬 로딩
void CEditorForm::OnBnClickedButtonMeshload()
{
	const TCHAR* pFilter = TEXT("Mesh File(*.msh)|*.msh|모든파일(*.*)|*.*||");
	CFileDialog	dlg(TRUE, TEXT(".msh"), nullptr, OFN_HIDEREADONLY,
		pFilter);

	if (dlg.DoModal() == IDOK)
	{
		CString	strPath = dlg.GetPathName();

		if (!m_pObj)
		{
			CScene*	pScene = GET_SINGLE(CSceneManager)->GetScene();
			CLayer*	pLayer = pScene->FindLayer("Default");

			m_pObj = CGameObject::CreateObject("Obj", pLayer);

			CTransform*	pTransform = m_pObj->GetTransform();

			pTransform->SetWorldScale(0.05f, 0.05f, 0.05f);

			SAFE_RELEASE(pTransform);

			CGameObject*	pMainCameraObj = pScene->GetMainCameraObj();

			CArm*	pCameraArm = pMainCameraObj->AddComponent<CArm>("CameraArm");

			pCameraArm->MouseEnable(true);

			pCameraArm->SetTarget(m_pObj);
			
			SAFE_RELEASE(pCameraArm);
			SAFE_RELEASE(pMainCameraObj);
		}

		if (!m_pRenderer)
		{
			m_pRenderer = m_pObj->AddComponent<CRenderer>("Renderer");

			m_pRenderer->SetMeshFromFullPath("EditMesh", strPath.GetString());
		}

		else
		{
			GET_SINGLE(CResourcesManager)->DeleteMesh("EditMesh");
			m_pRenderer->SetMeshFromFullPath("EditMesh", strPath.GetString());
		}

		m_strName = TEXT("EditMesh");

		UpdateData(FALSE);
	}
}

// 메쉬 저장
void CEditorForm::OnBnClickedButtonMeshsave()
{
}

// FBX 로딩
void CEditorForm::OnBnClickedButtonFbxload()
{
	const TCHAR* pFilter = TEXT("FBX File(*.fbx)|*.fbx|모든파일(*.*)|*.*||");
	CFileDialog	dlg(TRUE, TEXT(".fbx"), nullptr, OFN_HIDEREADONLY,
		pFilter);

	if (dlg.DoModal() == IDOK)
	{
		CString	strPath = dlg.GetPathName();

		if (!m_pObj)
		{
			CScene*	pScene = GET_SINGLE(CSceneManager)->GetScene();
			CLayer*	pLayer = pScene->FindLayer("Default");

			m_pObj = CGameObject::CreateObject("Obj", pLayer);

			CTransform*	pTransform = m_pObj->GetTransform();

			pTransform->SetWorldScale(0.05f, 0.05f, 0.05f);

			SAFE_RELEASE(pTransform);
		}

		if (!m_pRenderer)
		{
			m_pRenderer = m_pObj->AddComponent<CRenderer>("Renderer");

			m_pRenderer->SetMeshFromFullPath("EditMesh", strPath.GetString());
		}

		else
		{
			GET_SINGLE(CResourcesManager)->DeleteMesh("EditMesh");
			m_pRenderer->SetMeshFromFullPath("EditMesh", strPath.GetString());
		}

		SAFE_RELEASE(m_pAnimation);
		m_pAnimation = m_pObj->FindComponentFromType<CAnimation>(CT_ANIMATION);

		const list<string>*	pClips =
			m_pAnimation->GetAddClipNameList();

		list<string>::const_iterator	iter;
		list<string>::const_iterator	iterEnd = pClips->end();

		for (iter = pClips->begin(); iter != iterEnd; ++iter)
		{
			CString	strClipName = CA2CT(iter->c_str());

			m_ClipList.AddString(strClipName);
		}

		m_ClipList.SetCurSel(0);

		PANIMATIONCLIP	pClip = m_pAnimation->GetCurrentClip();
		
		m_strClipName = CA2CT(pClip->strName.c_str());
		m_iStartFrame = pClip->iStartFrame;
		m_iEndFrame = pClip->iEndFrame;
		m_fPlayTime = pClip->fPlayTime;
		m_OptionCombo.SetCurSel(pClip->eOption);

		UpdateData(FALSE);
	}
}

// 클립 로딩
void CEditorForm::OnBnClickedButtonClipload()
{
	const TCHAR* pFilter = TEXT("Animation File(*.anm)|*.anm|모든파일(*.*)|*.*||");
	CFileDialog	dlg(TRUE, TEXT(".anm"), nullptr, OFN_HIDEREADONLY,
		pFilter);

	if (dlg.DoModal() == IDOK)
	{
		CString	strPath = dlg.GetPathName();

		if (!m_pObj)
		{
			CScene*	pScene = GET_SINGLE(CSceneManager)->GetScene();
			CLayer*	pLayer = pScene->FindLayer("Default");

			m_pObj = CGameObject::CreateObject("Obj", pLayer);

			CTransform*	pTransform = m_pObj->GetTransform();

			pTransform->SetWorldScale(0.05f, 0.05f, 0.05f);

			SAFE_RELEASE(pTransform);
		}

		SAFE_RELEASE(m_pAnimation);
		m_pObj->EraseComponentFromType(CT_ANIMATION);
		m_pAnimation = m_pObj->AddComponent<CAnimation>("Animation");

		TCHAR	strName[MAX_PATH] = {};
		
		lstrcpy(strName, strPath.GetString());

		TCHAR	strBoneName[MAX_PATH] = {};

		lstrcpy(strBoneName, strName);

		int	iLength = strPath.GetLength();

		strBoneName[iLength - 1] = 'e';
		strBoneName[iLength - 2] = 'n';
		strBoneName[iLength - 3] = 'b';

		m_pAnimation->LoadBoneFromFullPath(strBoneName);

		m_pAnimation->AddClip(strName);

		const list<string>*	pClips =
			m_pAnimation->GetAddClipNameList();

		list<string>::const_iterator	iter;
		list<string>::const_iterator	iterEnd = pClips->end();

		for (iter = pClips->begin(); iter != iterEnd; ++iter)
		{
			CString	strClipName = CA2CT((*iter).c_str());

			m_ClipList.AddString(strClipName);
		}

		m_ClipList.SetCurSel(0);

		PANIMATIONCLIP	pClip = m_pAnimation->GetCurrentClip();

		m_strClipName = CA2CT(pClip->strName.c_str());
		m_iStartFrame = pClip->iStartFrame;
		m_iEndFrame = pClip->iEndFrame;
		m_fPlayTime = pClip->fPlayTime;
		m_OptionCombo.SetCurSel(pClip->eOption);

		UpdateData(FALSE);
	}
}

// 클립 저장
void CEditorForm::OnBnClickedButtonClipsave()
{
	if (!m_pAnimation)
		return;

	const TCHAR* pFilter = TEXT("Animation File(*.anm)|*.anm|모든파일(*.*)|*.*||");
	CFileDialog	dlg(FALSE, TEXT(".anm"), nullptr, OFN_OVERWRITEPROMPT,
		pFilter);

	if (dlg.DoModal() == IDOK)
	{
		CString	strPath = dlg.GetPathName();

		TCHAR	strName[MAX_PATH] = {};

		lstrcpy(strName, strPath.GetString());

		TCHAR	strBoneName[MAX_PATH] = {};

		lstrcpy(strBoneName, strName);

		int	iLength = strPath.GetLength();

		strBoneName[iLength - 1] = 'e';
		strBoneName[iLength - 2] = 'n';
		strBoneName[iLength - 3] = 'b';

		m_pAnimation->SaveBoneFromFullPath(strBoneName);

		m_pAnimation->SaveFromFullPath(strName);
	}
}

// 클립리스트 선택변경
void CEditorForm::OnLbnSelchangeListClip()
{
	if (!m_pAnimation)
		return;

	int	iIndex = m_ClipList.GetCurSel();

	if (iIndex == -1)
		return;

	CString	strClipName;
	m_ClipList.GetText(iIndex, strClipName);

	char	strName[256] = {};

	WideCharToMultiByte(CP_ACP, 0, strClipName.GetString(), -1,
		strName, strClipName.GetLength(), 0, 0);

	PANIMATIONCLIP	pClip = m_pAnimation->FindClip(strName);

	if (!pClip)
		return;

	m_strClipName = CA2CT(pClip->strName.c_str());
	m_iStartFrame = pClip->iStartFrame;
	m_iEndFrame = pClip->iEndFrame;
	m_fPlayTime = pClip->fPlayTime;

	m_OptionCombo.SetCurSel(pClip->eOption);

	UpdateData(FALSE);

	m_pAnimation->ChangeClip(pClip->strName);
}

// 체크박스 클릭시
void CEditorForm::OnBnClickedCheckFramecopy()
{
	if (!m_pAnimation)
	{
		m_bKeyFrameCopy = FALSE;
		UpdateData(FALSE);
		return;
	}

	UpdateData(TRUE);

	if (m_bKeyFrameCopy)
	{
		m_pAnimation->GetCurrentkeyFrame(m_vecKeyFrame);
	}

	else
	{
		Safe_Delete_VecList(m_vecKeyFrame);
	}
}
