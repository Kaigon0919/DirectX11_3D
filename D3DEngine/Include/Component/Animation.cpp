#include "Animation.h"
#include "../Device.h"
#include "../Resource/FbxLoader.h"
#include "../GameObject.h"
#include "../PathManager.h"

ENGINE_USING

CAnimation::CAnimation() :
	m_pBoneTex(nullptr),
	m_pBoneRV(nullptr),
	m_pDefaultClip(nullptr),
	m_pCurClip(nullptr),
	m_pNextClip(nullptr),
	m_bEnd(false),
	m_fAnimationGlobalTime(0.f),
	m_fClipProgress(0.f),
	m_fChangeTime(0.f),
	m_fChangeLimitTime(0.25f)
{
	m_eComType = CT_ANIMATION;
}

CAnimation::CAnimation(const CAnimation & com)	:
	CComponent(com)
{
	*this = com;

	m_iRefCount = 1;
	m_pDefaultClip = nullptr;
	m_pCurClip = nullptr;
	m_pNextClip = nullptr;
	m_bEnd = false;

	m_pBoneTex = nullptr;
	m_pBoneRV = nullptr;

	for (size_t i = 0; i < com.m_vecBones.size(); ++i)
	{
		++m_vecBones[i]->iRefCount;
	}

	CreateBoneTexture();

	m_vecBoneMatrix.clear();

	m_vecBoneMatrix.resize(com.m_vecBoneMatrix.size());

	for (size_t i = 0; i < m_vecBoneMatrix.size(); ++i)
	{
		m_vecBoneMatrix[i] = new Matrix;
	}

	unordered_map<string, PANIMATIONCLIP>::const_iterator	iter;
	unordered_map<string, PANIMATIONCLIP>::const_iterator	iterEnd = com.m_mapClip.end();

	m_mapClip.clear();

	for (iter = com.m_mapClip.begin(); iter != iterEnd; ++iter)
	{
		PANIMATIONCLIP	pClip = new ANIMATIONCLIP;

		*pClip = *iter->second;

		pClip->vecCallback.clear();

		size_t KeyFrameCount = iter->second->vecKeyFrame.size();

		for (size_t i = 0; i < KeyFrameCount; ++i)
		{
			++(pClip->vecKeyFrame[i]->iRefCount);
		}

		if (com.m_pCurClip->strName == iter->first)
			m_pCurClip = pClip;

		if (com.m_pDefaultClip->strName == iter->first)
			m_pDefaultClip = pClip;

		m_mapClip.insert(make_pair(iter->first, pClip));
	}
}

CAnimation::~CAnimation()
{
	Safe_Delete_Map(m_mapClip);
	Safe_Delete_VecList(m_vecBoneMatrix);

	SAFE_RELEASE(m_pBoneRV);
	SAFE_RELEASE(m_pBoneTex);

	for (size_t i = 0; i < m_vecBones.size(); ++i)
	{
		--m_vecBones[i]->iRefCount;

		if (m_vecBones[i]->iRefCount == 0)
		{
			//Safe_Release_VecList(m_vecBones[i]->SocketList);
			SAFE_DELETE(m_vecBones[i]->matBone);
			SAFE_DELETE(m_vecBones[i]->matOffset);
			SAFE_DELETE(m_vecBones[i]);
		}
	}

	m_vecBones.clear();
}

const list<string>* CAnimation::GetAddClipNameList() const
{
	return &m_AddClipNameList;
}

void CAnimation::AddBone(PBONE pBone)
{
	m_vecBones.push_back(pBone);
}

bool CAnimation::CreateBoneTexture()
{
	SAFE_RELEASE(m_pBoneTex);
	D3D11_TEXTURE2D_DESC	tDesc = {};
	tDesc.ArraySize = 1;
	tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	tDesc.Usage = D3D11_USAGE_DYNAMIC;
	tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	tDesc.Height = 1;
	tDesc.Width = UINT(m_vecBones.size() * 4);
	tDesc.MipLevels = 1;
	tDesc.SampleDesc.Quality = 0;
	tDesc.SampleDesc.Count = 1;

	if (FAILED(_DEVICE->CreateTexture2D(&tDesc, NULL, &m_pBoneTex)))
		return false;

	if (FAILED(_DEVICE->CreateShaderResourceView(m_pBoneTex, NULL, &m_pBoneRV)))
		return false;

	m_vecBlendInfo.resize(m_vecBones.size());
	
	return true;
}

void CAnimation::AddClip(ANIMATION_OPTION eOption, _tagFbxAnimationClip * pClip)
{
	PANIMATIONCLIP	pAnimClip = FindClip(pClip->strName);

	if (pAnimClip)
		return;

	pAnimClip = new ANIMATIONCLIP;

	// 인자로 들어온 애니메이션 옵션정보를 설정한다.
	pAnimClip->eOption = eOption;
	pAnimClip->strName = pClip->strName;

	pAnimClip->iChangeFrame = 0;

	// FBXANIMATIONCLIP에 있는 starttime 과 endtime 을 이용하여 keyframe 을 얻어온다.
	pAnimClip->iStartFrame = (int)pClip->tStart.GetFrameCount(pClip->eTimeMode);
	pAnimClip->iEndFrame = (int)pClip->tEnd.GetFrameCount(pClip->eTimeMode);
	pAnimClip->iFrameLength = pAnimClip->iEndFrame - pAnimClip->iStartFrame;

	// 시간 정보를 저장해준다.
	pAnimClip->fStartTime = 0.f;
	pAnimClip->fEndTime = pAnimClip->fPlayTime;
	pAnimClip->fTimeLength = pAnimClip->fPlayTime;

	pAnimClip->fFrameTime = pAnimClip->fPlayTime / pAnimClip->iFrameLength;

	// 키 프레임 수만큼 반복하며 각각의 프레임을 보간할 행렬 정보를 위치, 크기, 회전정보로
	// 뽑아온다.
	for (size_t i = 0; i < pClip->vecBoneKeyFrame.size(); ++i)
	{
		PBONEKEYFRAME	pBoneKeyFrame = new BONEKEYFRAME;

		pBoneKeyFrame->iBoneIndex = pClip->vecBoneKeyFrame[i].iBoneIndex;

		pAnimClip->vecKeyFrame.push_back(pBoneKeyFrame);

		// 아래부터 키프레임 정보를 저장한다.
		pBoneKeyFrame->vecKeyFrame.reserve(pClip->vecBoneKeyFrame[i].vecKeyFrame.size());

		for (size_t j = 0; j < pClip->vecBoneKeyFrame[i].vecKeyFrame.size(); ++j)
		{
			PKEYFRAME	pKeyFrame = new KEYFRAME;

			pKeyFrame->dTime = j * pAnimClip->fFrameTime;

			// 현재 본의 키 프레임에 해당하는 행렬 정보를 얻어온다.
			FbxAMatrix	mat = pClip->vecBoneKeyFrame[i].vecKeyFrame[j].matTransform;

			FbxVector4	vPos, vScale;
			FbxQuaternion	qRot;

			// 행렬로부터 위치, 크기, 회전 정보를 얻어온다.
			vPos = mat.GetT();
			vScale = mat.GetS();
			qRot = mat.GetQ();

			pKeyFrame->vScale = Vector3((float)vScale.mData[0], (float)vScale.mData[1],
				(float)vScale.mData[2]);
			pKeyFrame->vPos = Vector3((float)vPos.mData[0], (float)vPos.mData[1],
				(float)vPos.mData[2]);
			pKeyFrame->vRot = Vector4((float)qRot.mData[0], (float)qRot.mData[1],
				(float)qRot.mData[2], (float)qRot.mData[3]);

			pBoneKeyFrame->vecKeyFrame.push_back(pKeyFrame);
		}
	}

	switch (pClip->eTimeMode)
	{
	case FbxTime::eFrames24:
		pAnimClip->iFrameMode = 24;
		break;
	case FbxTime::eFrames30:
		pAnimClip->iFrameMode = 30;
		break;
	case FbxTime::eFrames60:
		pAnimClip->iFrameMode = 60;
		break;
	}

	if (m_mapClip.empty())
	{
		m_pDefaultClip = pAnimClip;
		m_pCurClip = pAnimClip;
	}

	m_mapClip.insert(make_pair(pAnimClip->strName, pAnimClip));

	m_AddClipNameList.clear();

	m_AddClipNameList.push_back(pAnimClip->strName);
}

void CAnimation::AddClip(const string & strName, ANIMATION_OPTION eOption, 
	int iStartFrame, int iEndFrame, float fPlayTime,
	const vector<PBONEKEYFRAME>& vecFrame)
{
	PANIMATIONCLIP	pAnimClip = FindClip(strName);

	if (pAnimClip)
		return;

	pAnimClip = new ANIMATIONCLIP;

	// 인자로 들어온 애니메이션 옵션정보를 설정한다.
	pAnimClip->eOption = eOption;
	pAnimClip->strName = strName;

	pAnimClip->iChangeFrame = 0;

	int	iLength = iEndFrame - iStartFrame;

	// FBXANIMATIONCLIP에 있는 starttime 과 endtime 을 이용하여 keyframe 을 얻어온다.
	pAnimClip->iStartFrame = 0;
	pAnimClip->iEndFrame = iLength;
	pAnimClip->iFrameLength = iLength;

	// 시간 정보를 저장해준다.
	pAnimClip->fStartTime = 0.f;
	pAnimClip->fEndTime = fPlayTime;
	pAnimClip->fTimeLength = fPlayTime;
	pAnimClip->fPlayTime = fPlayTime;

	pAnimClip->fFrameTime = pAnimClip->fPlayTime / pAnimClip->iFrameLength;

	// 키 프레임 수만큼 반복하며 각각의 프레임을 보간할 행렬 정보를 위치, 크기, 회전정보로
	// 뽑아온다.
	for (size_t i = 0; i < vecFrame.size(); ++i)
	{
		PBONEKEYFRAME	pBoneFrame = new BONEKEYFRAME;
		pAnimClip->vecKeyFrame.push_back(pBoneFrame);

		PBONEKEYFRAME	pClipFrame = vecFrame[i];
		pBoneFrame->iBoneIndex = pClipFrame->iBoneIndex;
		pBoneFrame->iRefCount = 1;

		if (!pClipFrame->vecKeyFrame.empty())
		{
			for (size_t j = iStartFrame; j <= iEndFrame; ++j)
			{
				PKEYFRAME	pFrame = new KEYFRAME;

				pFrame->dTime = (j - iStartFrame) * pAnimClip->fFrameTime;
				pFrame->vPos = pClipFrame->vecKeyFrame[j]->vPos;
				pFrame->vScale = pClipFrame->vecKeyFrame[j]->vScale;
				pFrame->vRot = pClipFrame->vecKeyFrame[j]->vRot;

				pBoneFrame->vecKeyFrame.push_back(pFrame);
			}
		}
	}

	if (m_mapClip.empty())
	{
		m_pDefaultClip = pAnimClip;
		m_pCurClip = pAnimClip;
	}

	m_mapClip.insert(make_pair(pAnimClip->strName, pAnimClip));

	m_AddClipNameList.clear();

	m_AddClipNameList.push_back(pAnimClip->strName);
}

void CAnimation::AddClip(const TCHAR * pFullPath)
{
	char	strFullPath[MAX_PATH] = {};

#ifdef UNICODE
	WideCharToMultiByte(CP_UTF8, 0, pFullPath, -1, strFullPath, lstrlen(pFullPath),
		0, 0);
#else
	strcpy_s(strFullPath, pFullPath);
#endif // UNICODE

	AddClipFromMultibyte(strFullPath);
}

void CAnimation::AddClipFromMultibyte(const char * pFullPath)
{
	char	strExt[_MAX_EXT] = {};

	_splitpath_s(pFullPath, 0, 0, 0, 0, 0, 0, strExt, _MAX_EXT);

	_strupr_s(strExt);

	if (strcmp(strExt, ".FBX") == 0)
	{
		LoadFbxAnimation(pFullPath);
	}

	else
	{
		LoadFromFullPath(pFullPath);
	}
}

PANIMATIONCLIP CAnimation::FindClip(const string & strName)
{
	unordered_map<string, PANIMATIONCLIP>::iterator	iter = m_mapClip.find(strName);

	if (iter == m_mapClip.end())
		return nullptr;

	return iter->second;
}

bool CAnimation::IsAnimationEnd() const
{
	return m_bEnd;
}

float CAnimation::GetAniPercent() const
{
	float	fAnimationTime = m_fAnimationGlobalTime + m_pCurClip->fStartTime;
	return fAnimationTime / m_pCurClip->fEndTime;
}

PANIMATIONCLIP CAnimation::GetCurrentClip() const
{
	return m_pCurClip;
}

const unordered_map<string, PANIMATIONCLIP>* CAnimation::GetClips() const
{
	return &m_mapClip;
}

void CAnimation::GetCurrentkeyFrame(vector<PBONEKEYFRAME>& vecKeyFrame)
{
	for (size_t i = 0; i < m_pCurClip->vecKeyFrame.size(); ++i)
	{
		PBONEKEYFRAME	pBoneFrame = new BONEKEYFRAME;
		vecKeyFrame.push_back(pBoneFrame);

		PBONEKEYFRAME	pClipFrame = m_pCurClip->vecKeyFrame[i];
		pBoneFrame->iBoneIndex = pClipFrame->iBoneIndex;
		pBoneFrame->iRefCount = 1;

		for (size_t j = 0; j < pClipFrame->vecKeyFrame.size(); ++j)
		{
			PKEYFRAME	pFrame = new KEYFRAME;

			pFrame->dTime = pClipFrame->vecKeyFrame[j]->dTime;
			pFrame->vPos = pClipFrame->vecKeyFrame[j]->vPos;
			pFrame->vScale = pClipFrame->vecKeyFrame[j]->vScale;
			pFrame->vRot = pClipFrame->vecKeyFrame[j]->vRot;

			pBoneFrame->vecKeyFrame.push_back(pFrame);
		}
	}
}

void CAnimation::ChangeClipKey(const string & strOrigin, const string & strChange)
{
	unordered_map<string, PANIMATIONCLIP>::iterator	iter = m_mapClip.find(strOrigin);

	if (iter == m_mapClip.end())
		return;

	PANIMATIONCLIP	pClip = iter->second;

	pClip->strName = strChange;

	m_mapClip.erase(iter);

	m_mapClip.insert(make_pair(strChange, pClip));
}

PBONE CAnimation::FindBone(const string & strBoneName)
{
	for (size_t i = 0; i < m_vecBones.size(); ++i)
	{
		if (m_vecBones[i]->strName == strBoneName)
			return m_vecBones[i];
	}

	return NULL;
}

int CAnimation::FindBoneIndex(const string & strBoneName)
{
	for (size_t i = 0; i < m_vecBones.size(); ++i)
	{
		if (m_vecBones[i]->strName == strBoneName)
			return (int)i;
	}

	return -1;
}

int CAnimation::GetCurBoneTransform(const string& strBoneName, __out Vector3 & vPos, __out  Vector4 & vRot, __out  Vector3 & vScale)
{
	int iBoneIdx = FindBoneIndex(strBoneName);
	if (iBoneIdx == -1)
	{
		vPos = Vector3::Zero;
		vRot = Vector4::Zero;
		vScale = Vector3::One;
		return -1;
	}
	vPos = m_vecBlendInfo[iBoneIdx].vPos;
	vRot = m_vecBlendInfo[iBoneIdx].vRot;
	vScale = m_vecBlendInfo[iBoneIdx].vScale;

	return 0;
}

Matrix CAnimation::GetBoneMatrix(const string & strBoneName)
{
	int	iBone = FindBoneIndex(strBoneName);

	if (iBone == -1)
		return Matrix();

	return *m_vecBoneMatrix[iBone];
}

bool CAnimation::ChangeClip(const string & strClip)
{
	if (m_pCurClip->strName == strClip)
	{
		m_pNextClip = nullptr;
		return false;
	}
	m_pNextClip = FindClip(strClip);

	if (!m_pNextClip)
		return false;

	return true;
}

ID3D11ShaderResourceView ** CAnimation::GetBoneTexture()
{
	return &m_pBoneRV;
}

bool CAnimation::Save(const TCHAR * pFileName, const string & strPathKey)
{
	char	strFileName[MAX_PATH] = {};

#ifdef UNICODE
	WideCharToMultiByte(CP_UTF8, 0, pFileName, -1, strFileName, lstrlen(pFileName),
		NULL, NULL);
#else
	strcpy_s(strFileName, pFileName);
#endif // UNICODE

	return Save(strFileName, strPathKey);
}

bool CAnimation::Save(const char * pFileName, const string & strPathKey)
{
	const char*	pPath = GET_SINGLE(CPathManager)->FindPathMultibyte(strPathKey);

	string	strFullPath;

	if (pPath)
		strFullPath = pPath;

	strFullPath += pFileName;

	return SaveFromFullPath(strFullPath.c_str());
}

bool CAnimation::SaveFromFullPath(const TCHAR * pFullPath)
{
	char	strFileName[MAX_PATH] = {};

#ifdef UNICODE
	WideCharToMultiByte(CP_UTF8, 0, pFullPath, -1, strFileName, lstrlen(pFullPath),
		NULL, NULL);
#else
	strcpy_s(strFileName, pFileName);
#endif // UNICODE

	return SaveFromFullPath(strFileName);
}

bool CAnimation::SaveFromFullPath(const char * pFullPath)
{
	FILE*	pFile = NULL;

	fopen_s(&pFile, pFullPath, "wb");

	if (!pFile)
		return false;

	fwrite(&m_fChangeLimitTime, sizeof(float), 1, pFile);

	// 애니메이션 클립정보를 저장한다.
	size_t iCount = m_mapClip.size();

	fwrite(&iCount, sizeof(size_t), 1, pFile);

	size_t iLength = m_pDefaultClip->strName.length();
	fwrite(&iLength, sizeof(size_t), 1, pFile);
	fwrite(m_pDefaultClip->strName.c_str(), sizeof(char),
		iLength, pFile);

	iLength = m_pCurClip->strName.length();
	fwrite(&iLength, sizeof(size_t), 1, pFile);
	fwrite(m_pCurClip->strName.c_str(), sizeof(char), iLength, pFile);

	unordered_map<string, PANIMATIONCLIP>::iterator	iter;
	unordered_map<string, PANIMATIONCLIP>::iterator	iterEnd = m_mapClip.end();

	for (iter = m_mapClip.begin(); iter != iterEnd; ++iter)
	{
		PANIMATIONCLIP	pClip = iter->second;

		// 애니메이션 클립 키를 저장한다.
		iLength = pClip->strName.length();
		fwrite(&iLength, sizeof(size_t), 1, pFile);
		fwrite(pClip->strName.c_str(), sizeof(char), iLength, pFile);

		fwrite(&pClip->eOption, sizeof(ANIMATION_OPTION), 1, pFile);

		fwrite(&pClip->fStartTime, sizeof(float), 1, pFile);
		fwrite(&pClip->fEndTime, sizeof(float), 1, pFile);
		fwrite(&pClip->fTimeLength, sizeof(float), 1, pFile);
		fwrite(&pClip->fFrameTime, sizeof(float), 1, pFile);

		fwrite(&pClip->iFrameMode, sizeof(int), 1, pFile);
		fwrite(&pClip->iStartFrame, sizeof(int), 1, pFile);
		fwrite(&pClip->iEndFrame, sizeof(int), 1, pFile);
		fwrite(&pClip->iFrameLength, sizeof(int), 1, pFile);
		fwrite(&pClip->fPlayTime, sizeof(float), 1, pFile);

		size_t	iCount = pClip->vecKeyFrame.size();

		fwrite(&iCount, sizeof(size_t), 1, pFile);

		for (size_t i = 0; i < iCount; ++i)
		{
			fwrite(&pClip->vecKeyFrame[i]->iBoneIndex, sizeof(int), 1,
				pFile);

			size_t	iFrameCount = pClip->vecKeyFrame[i]->vecKeyFrame.size();

			fwrite(&iFrameCount, sizeof(size_t), 1, pFile);

			for (size_t j = 0; j < iFrameCount; ++j)
			{
				fwrite(&pClip->vecKeyFrame[i]->vecKeyFrame[j]->dTime, sizeof(double), 1, pFile);
				fwrite(&pClip->vecKeyFrame[i]->vecKeyFrame[j]->vPos, sizeof(Vector3), 1, pFile);
				fwrite(&pClip->vecKeyFrame[i]->vecKeyFrame[j]->vScale, sizeof(Vector3), 1, pFile);
				fwrite(&pClip->vecKeyFrame[i]->vecKeyFrame[j]->vRot, sizeof(Vector4), 1, pFile);
			}
		}
	}

	fclose(pFile);

	return true;
}

bool CAnimation::Load(const TCHAR * pFileName, const string & strPathKey)
{
	char	strFileName[MAX_PATH] = {};

#ifdef UNICODE
	WideCharToMultiByte(CP_UTF8, 0, pFileName, -1, strFileName, lstrlen(pFileName),
		NULL, NULL);
#else
	strcpy_s(strFileName, pFileName);
#endif // UNICODE

	return Load(strFileName, strPathKey);
}

bool CAnimation::Load(const char * pFileName, const string & strPathKey)
{
	const char*	pPath = GET_SINGLE(CPathManager)->FindPathMultibyte(strPathKey);

	string	strFullPath;

	if (pPath)
		strFullPath = pPath;

	strFullPath += pFileName;

	return LoadFromFullPath(strFullPath.c_str());
}

bool CAnimation::LoadFromFullPath(const TCHAR * pFullPath)
{
	char	strFileName[MAX_PATH] = {};

#ifdef UNICODE
	WideCharToMultiByte(CP_UTF8, 0, pFullPath, -1, strFileName, lstrlen(pFullPath),
		NULL, NULL);
#else
	strcpy_s(strFileName, pFileName);
#endif // UNICODE

	return LoadFromFullPath(strFileName);
}

bool CAnimation::LoadFromFullPath(const char * pFullPath)
{
	FILE*	pFile = NULL;

	fopen_s(&pFile, pFullPath, "rb");

	if (!pFile)
		return false;

	fread(&m_fChangeLimitTime, sizeof(float), 1, pFile);

	// 애니메이션 클립정보를 저장한다.
	size_t iCount = 0, iLength = 0;
	fread(&iCount, sizeof(size_t), 1, pFile);

	char	strDefaultClip[256] = {};
	fread(&iLength, sizeof(size_t), 1, pFile);
	fread(strDefaultClip, sizeof(char),
		iLength, pFile);

	char	strCurClip[256] = {};
	fread(&iLength, sizeof(size_t), 1, pFile);
	fread(strCurClip, sizeof(char), iLength, pFile);

	m_AddClipNameList.clear();

	for (int l = 0; l < iCount; ++l)
	{
		PANIMATIONCLIP	pClip = new ANIMATIONCLIP;

		// 애니메이션 클립 키를 저장한다.
		char	strClipName[256] = {};
		fread(&iLength, sizeof(size_t), 1, pFile);
		fread(strClipName, sizeof(char), iLength, pFile);

		m_mapClip.insert(make_pair(strClipName, pClip));

		pClip->strName = strClipName;
		pClip->iChangeFrame = 0;

		m_AddClipNameList.push_back(strClipName);

		fread(&pClip->eOption, sizeof(ANIMATION_OPTION), 1, pFile);

		fread(&pClip->fStartTime, sizeof(float), 1, pFile);
		fread(&pClip->fEndTime, sizeof(float), 1, pFile);
		fread(&pClip->fTimeLength, sizeof(float), 1, pFile);
		fread(&pClip->fFrameTime, sizeof(float), 1, pFile);

		fread(&pClip->iFrameMode, sizeof(int), 1, pFile);
		fread(&pClip->iStartFrame, sizeof(int), 1, pFile);
		fread(&pClip->iEndFrame, sizeof(int), 1, pFile);
		fread(&pClip->iFrameLength, sizeof(int), 1, pFile);
		fread(&pClip->fPlayTime, sizeof(float), 1, pFile);

		size_t	iFrameCount = 0;

		fread(&iFrameCount, sizeof(size_t), 1, pFile);

		for (size_t i = 0; i < iFrameCount; ++i)
		{
			PBONEKEYFRAME	pBoneKeyFrame = new BONEKEYFRAME;
			pClip->vecKeyFrame.push_back(pBoneKeyFrame);

			fread(&pBoneKeyFrame->iBoneIndex, sizeof(int), 1,
				pFile);

			size_t	iBoneFrameCount = 0;

			fread(&iBoneFrameCount, sizeof(size_t), 1, pFile);

			for (size_t j = 0; j < iBoneFrameCount; ++j)
			{
				PKEYFRAME	pKeyFrame = new KEYFRAME;
				pBoneKeyFrame->vecKeyFrame.push_back(pKeyFrame);

				fread(&pKeyFrame->dTime, sizeof(double), 1, pFile);
				fread(&pKeyFrame->vPos, sizeof(Vector3), 1, pFile);
				fread(&pKeyFrame->vScale, sizeof(Vector3), 1, pFile);
				fread(&pKeyFrame->vRot, sizeof(Vector4), 1, pFile);
			}
		}
	}

	m_pCurClip = FindClip(strCurClip);
	m_pDefaultClip = FindClip(strDefaultClip);

	fclose(pFile);

	return true;
}

bool CAnimation::SaveBone(const TCHAR * pFileName, const string & strPathKey)
{
	char	strFileName[MAX_PATH] = {};

#ifdef UNICODE
	WideCharToMultiByte(CP_UTF8, 0, pFileName, -1, strFileName, lstrlen(pFileName),
		NULL, NULL);
#else
	strcpy_s(strFileName, pFileName);
#endif // UNICODE

	return SaveBone(strFileName, strPathKey);
}

bool CAnimation::SaveBone(const char * pFileName, const string & strPathKey)
{
	const char*	pPath = GET_SINGLE(CPathManager)->FindPathMultibyte(strPathKey);

	string	strFullPath;

	if (pPath)
		strFullPath = pPath;

	strFullPath += pFileName;

	return SaveBoneFromFullPath(strFullPath.c_str());
}

bool CAnimation::SaveBoneFromFullPath(const TCHAR * pFullPath)
{
	char	strFileName[MAX_PATH] = {};

#ifdef UNICODE
	WideCharToMultiByte(CP_UTF8, 0, pFullPath, -1, strFileName, lstrlen(pFullPath),
		NULL, NULL);
#else
	strcpy_s(strFileName, pFileName);
#endif // UNICODE

	return SaveBoneFromFullPath(strFileName);
}

bool CAnimation::SaveBoneFromFullPath(const char * pFullPath)
{
	FILE*	pFile = NULL;

	fopen_s(&pFile, pFullPath, "wb");

	if (!pFile)
		return false;

	// ===================== 본 정보 저장 =====================
	size_t	iCount = m_vecBones.size();

	fwrite(&iCount, sizeof(size_t), 1, pFile);

	size_t	iLength = 0;

	for (size_t i = 0; i < iCount; ++i)
	{
		iLength = m_vecBones[i]->strName.length();
		fwrite(&iLength, sizeof(size_t), 1, pFile);
		fwrite(m_vecBones[i]->strName.c_str(), sizeof(char), iLength, pFile);

		fwrite(&m_vecBones[i]->iDepth, sizeof(int), 1, pFile);
		fwrite(&m_vecBones[i]->iParentIndex, sizeof(int), 1, pFile);
		fwrite(&m_vecBones[i]->matOffset->m, sizeof(XMMATRIX), 1, pFile);
		fwrite(&m_vecBones[i]->matBone->m, sizeof(XMMATRIX), 1, pFile);
	}

	fclose(pFile);

	return true;
}

bool CAnimation::LoadBone(const TCHAR * pFileName, const string & strPathKey)
{
	char	strFileName[MAX_PATH] = {};

#ifdef UNICODE
	WideCharToMultiByte(CP_UTF8, 0, pFileName, -1, strFileName, lstrlen(pFileName),
		NULL, NULL);
#else
	strcpy_s(strFileName, pFileName);
#endif // UNICODE

	return LoadBone(strFileName, strPathKey);
}

bool CAnimation::LoadBone(const char * pFileName, const string & strPathKey)
{
	const char*	pPath = GET_SINGLE(CPathManager)->FindPathMultibyte(strPathKey);

	string	strFullPath;

	if (pPath)
		strFullPath = pPath;

	strFullPath += pFileName;

	return LoadBoneFromFullPath(strFullPath.c_str());
}

bool CAnimation::LoadBoneFromFullPath(const TCHAR * pFullPath)
{
	char	strFileName[MAX_PATH] = {};

#ifdef UNICODE
	WideCharToMultiByte(CP_UTF8, 0, pFullPath, -1, strFileName, lstrlen(pFullPath),
		NULL, NULL);
#else
	strcpy_s(strFileName, pFileName);
#endif // UNICODE

	return LoadBoneFromFullPath(strFileName);
}

bool CAnimation::LoadBoneFromFullPath(const char * pFullPath)
{
	FILE*	pFile = NULL;

	fopen_s(&pFile, pFullPath, "rb");

	if (!pFile)
		return false;

	// ===================== 본 정보 읽기 =====================
	size_t	iCount = 0;

	fread(&iCount, sizeof(size_t), 1, pFile);

	size_t	iLength = 0;

	for (size_t i = 0; i < iCount; ++i)
	{
		PBONE	pBone = new BONE;
		m_vecBones.push_back(pBone);

		pBone->matBone = new Matrix;
		pBone->matOffset = new Matrix;

		char	strBoneName[256] = {};
		fread(&iLength, sizeof(size_t), 1, pFile);
		fread(strBoneName, sizeof(char), iLength, pFile);
		pBone->strName = strBoneName;

		fread(&pBone->iDepth, sizeof(int), 1, pFile);
		fread(&pBone->iParentIndex, sizeof(int), 1, pFile);
		fread(&pBone->matOffset->m, sizeof(XMMATRIX), 1, pFile);
		fread(&pBone->matBone->m, sizeof(XMMATRIX), 1, pFile);
	}

	fclose(pFile);

	CreateBoneTexture();

	return true;
}

bool CAnimation::ModifyClip(const string & strKey, const string & strChangeKey,
	ANIMATION_OPTION eOption, int iStartFrame, int iEndFrame, float fPlayTime, 
	const vector<PBONEKEYFRAME>& vecFrame)
{
	PANIMATIONCLIP	pClip = FindClip(strKey);

	if (!pClip)
		return false;

	m_mapClip.erase(strKey);

	int	iLength = iEndFrame - iStartFrame;

	pClip->eOption = eOption;
	pClip->strName = strChangeKey;
	pClip->iStartFrame = 0;
	pClip->iEndFrame = iLength;
	pClip->iFrameLength = iLength;
	pClip->fStartTime = 0.f;
	pClip->fEndTime = fPlayTime;
	pClip->fTimeLength = fPlayTime;
	pClip->fPlayTime = fPlayTime;

	pClip->fFrameTime = pClip->fPlayTime / iLength;

	m_mapClip.insert(make_pair(strChangeKey, pClip));

	Safe_Delete_VecList(pClip->vecKeyFrame);

	for (size_t i = 0; i < vecFrame.size(); ++i)
	{
		PBONEKEYFRAME	pBoneFrame = new BONEKEYFRAME;
		pClip->vecKeyFrame.push_back(pBoneFrame);

		PBONEKEYFRAME	pClipFrame = vecFrame[i];
		pBoneFrame->iBoneIndex = pClipFrame->iBoneIndex;
		pBoneFrame->iRefCount = 1;

		if (!pClipFrame->vecKeyFrame.empty())
		{
			for (size_t j = iStartFrame; j <= iEndFrame; ++j)
			{
				PKEYFRAME	pFrame = new KEYFRAME;

				pFrame->dTime = (j - iStartFrame) * pClip->fFrameTime;
				pFrame->vPos = pClipFrame->vecKeyFrame[j]->vPos;
				pFrame->vScale = pClipFrame->vecKeyFrame[j]->vScale;
				pFrame->vRot = pClipFrame->vecKeyFrame[j]->vRot;

				pBoneFrame->vecKeyFrame.push_back(pFrame);
			}
		}
	}

	return true;
}

bool CAnimation::DeleteClip(const string & strKey)
{
	unordered_map<string, PANIMATIONCLIP>::iterator	iter = m_mapClip.find(strKey);

	if (iter == m_mapClip.end())
		return false;

	if (strKey == m_pDefaultClip->strName)
	{
		if (!m_mapClip.empty())
		{
			unordered_map<string, PANIMATIONCLIP>::iterator	iter1 = m_mapClip.begin();

			while (iter1 != m_mapClip.end() && iter1->first == strKey)
			{
				++iter1;
			}

			m_pDefaultClip = FindClip(iter1->first);
		}
	}

	if (strKey == m_pCurClip->strName)
	{
		m_pCurClip = m_pDefaultClip;
	}

	SAFE_DELETE(iter->second);

	m_mapClip.erase(iter);

	return true;
}

bool CAnimation::ReturnDefaultClip()
{
	ChangeClip(m_pDefaultClip->strName);

	return true;
}

void CAnimation::LoadFbxAnimation(const char * pFullPath)
{
	CFbxLoader	loader;

	loader.LoadFbx(pFullPath);

	// 애니메이션 클립을 추가한다.
	const vector<PFBXANIMATIONCLIP>* pvecClip = loader.GetClips();

	// 클립을 읽어온다.
	vector<PFBXANIMATIONCLIP>::const_iterator	iterC;
	vector<PFBXANIMATIONCLIP>::const_iterator	iterCEnd = pvecClip->end();

	m_AddClipNameList.clear();

	for (iterC = pvecClip->begin(); iterC != iterCEnd; ++iterC)
	{
		AddClip(AO_LOOP, *iterC);

		m_AddClipNameList.push_back((*iterC)->strName);
	}
}

void CAnimation::Start()
{
}

bool CAnimation::Init()
{
	return true;
}

int CAnimation::Input(float fTime)
{
	return 0;
}

int CAnimation::Update(float fTime)
{
	if (m_mapClip.empty())
		return 0;

	if (m_vecBoneMatrix.empty())
	{
		m_vecBoneMatrix.resize(m_vecBones.size());

		for (size_t i = 0; i < m_vecBoneMatrix.size(); ++i)
		{
			m_vecBoneMatrix[i] = new Matrix;
		}
	}

	m_bEnd = false;

	// 모션이 변할때
	if (m_pNextClip)
	{
		m_fChangeTime += fTime;

		bool	bChange = false;
		if (m_fChangeTime >= m_fChangeLimitTime)
		{
			m_fChangeTime = m_fChangeLimitTime;
			bChange = true;
		}

		float	fAnimationTime = m_fAnimationGlobalTime + m_pCurClip->fStartTime;

		// 본 수만큼 반복한다.
		for (size_t i = 0; i < m_vecBones.size(); ++i)
		{
			// 키프레임이 없을 경우
			if (m_pCurClip->vecKeyFrame[i]->vecKeyFrame.empty())
			{
				*m_vecBoneMatrix[i] = *m_vecBones[i]->matBone;
				continue;
			}

			//int	iFrameIndex = (int)(fAnimationTime * m_iFrameMode);
			//int	iFrameIndex = m_pCurClip->iChangeFrame;
			int	iNextFrameIndex = m_pNextClip->iStartFrame;

			//const PKEYFRAME pCurKey = m_pCurClip->vecKeyFrame[i]->vecKeyFrame[iFrameIndex];
			const PKEYFRAME pNextKey = m_pNextClip->vecKeyFrame[i]->vecKeyFrame[iNextFrameIndex];

			float	fPercent = m_fChangeTime / m_fChangeLimitTime;

			XMVECTOR	vS = XMVectorLerp(m_vecBlendInfo[i].vScale.Convert(),pNextKey->vScale.Convert(), fPercent);
			XMVECTOR	vT = XMVectorLerp(m_vecBlendInfo[i].vPos.Convert(),	pNextKey->vPos.Convert(), fPercent);
			XMVECTOR	vR = XMQuaternionSlerp(m_vecBlendInfo[i].vRot.Convert(),pNextKey->vRot.Convert(), fPercent);

			XMVECTOR	vZero = XMVectorSet(0.f, 0.f, 0.f, 1.f);

			Matrix	matBone = XMMatrixAffineTransformation(vS, vZero, vR, vT);

			*m_vecBones[i]->matBone = matBone;

			matBone = *m_vecBones[i]->matOffset * matBone;

			*m_vecBoneMatrix[i] = matBone;
		}

		if (bChange)
		{
			m_pCurClip = m_pNextClip;
			m_pNextClip = nullptr;
			m_fAnimationGlobalTime = 0.f;
			m_fChangeTime = 0.f;
		}
	}

	// 기존 모션이 계속 동작될때
	else
	{
		m_fAnimationGlobalTime += fTime;
		m_fClipProgress = m_fAnimationGlobalTime / m_pCurClip->fPlayTime;

		while (m_fAnimationGlobalTime >= m_pCurClip->fPlayTime)
		{
			m_fAnimationGlobalTime -= m_pCurClip->fPlayTime;
			m_bEnd = true;

			/*for (size_t i = 0; i < m_vecChannel[m_iCurChannel].pClip->m_tInfo.vecCallback.size();
			++i)
			{
			m_vecChannel[m_iCurChannel].pClip->m_tInfo.vecCallback[i]->bCall = false;
			}*/
		}

		float	fAnimationTime = m_fAnimationGlobalTime + m_pCurClip->fStartTime;
		int	iStartFrame = m_pCurClip->iStartFrame;
		int	iEndFrame = m_pCurClip->iEndFrame;

		int	iFrameIndex = (int)(fAnimationTime / m_pCurClip->fFrameTime);

		if (m_bEnd)
		{
			switch (m_pCurClip->eOption)
			{
			case AO_LOOP:
				iFrameIndex = iStartFrame;
				break;
			case AO_ONCE_DESTROY:
				m_pObject->Active(false);
				break;
			}
		}

		else
		{
			int	iNextFrameIndex = iFrameIndex + 1;

			m_pCurClip->iChangeFrame = iFrameIndex;

			if (iNextFrameIndex > iEndFrame)
				iNextFrameIndex = iStartFrame;

			// 본 수만큼 반복한다.
			for (size_t i = 0; i < m_vecBones.size(); ++i)
			{
				// 키프레임이 없을 경우
				if (m_pCurClip->vecKeyFrame[i]->vecKeyFrame.empty())
				{
					*m_vecBoneMatrix[i] = *m_vecBones[i]->matBone;
					continue;
				}

				const PKEYFRAME pCurKey = m_pCurClip->vecKeyFrame[i]->vecKeyFrame[iFrameIndex];
				const PKEYFRAME pNextKey = m_pCurClip->vecKeyFrame[i]->vecKeyFrame[iNextFrameIndex];
				
				// 현재 프레임의 시간을 얻어온다.
				double	 dFrameTime = pCurKey->dTime;

				float	fPercent = float((fAnimationTime - dFrameTime) / m_pCurClip->fFrameTime);

				XMVECTOR	vS = XMVectorLerp(pCurKey->vScale.Convert(), pNextKey->vScale.Convert(), fPercent);
				XMVECTOR	vT = XMVectorLerp(pCurKey->vPos.Convert(), pNextKey->vPos.Convert(), fPercent);
				XMVECTOR	vR = XMQuaternionSlerp(pCurKey->vRot.Convert(),	pNextKey->vRot.Convert(), fPercent);

				m_vecBlendInfo[i].vPos = vT;
				m_vecBlendInfo[i].vScale = vS;
				m_vecBlendInfo[i].vRot = vR;

				XMVECTOR	vZero = XMVectorSet(0.f, 0.f, 0.f, 1.f);

				Matrix	matBone = XMMatrixAffineTransformation(vS, vZero, vR, vT);

				*m_vecBones[i]->matBone = matBone;

				matBone = *m_vecBones[i]->matOffset * matBone;

				//*m_vecBones[i]->matBone = matBone;

				*m_vecBoneMatrix[i] = matBone;
			}
		}
	}

	if (!m_bEnd)
	{
		D3D11_MAPPED_SUBRESOURCE	tMap = {};
		_CONTEXT->Map(m_pBoneTex, 0, D3D11_MAP_WRITE_DISCARD, 0, &tMap);

		PMatrix	pMatrix = (PMatrix)tMap.pData;

		for (size_t i = 0; i < m_vecBoneMatrix.size(); ++i)
		{
			pMatrix[i] = *m_vecBoneMatrix[i];
		}

		_CONTEXT->Unmap(m_pBoneTex, 0);
	}

	return 0;
}

int CAnimation::LateUpdate(float fTime)
{
	return 0;
}

int CAnimation::Collision(float fTime)
{
	return 0;
}

int CAnimation::PrevRender(float fTime)
{
	_CONTEXT->VSSetShaderResources(3, 1, &m_pBoneRV);

	return 0;
}

int CAnimation::Render(float fTime)
{
	return 0;
}

CAnimation * CAnimation::Clone() const
{
	return new CAnimation(*this);
}
