#include "Mesh.h"
#include "../Device.h"
#include "../PathManager.h"
#include "FbxLoader.h"
#include "../Component/Material.h"
#include "../Component/Animation.h"

ENGINE_USING

CMesh::CMesh()	:
	m_pMaterial(nullptr),
	m_pAnimation(nullptr)
{
	m_vView = Vector3::Axis[AXIS_Z];
}

CMesh::~CMesh()
{
	for (size_t i = 0; i < m_vecMeshContainer.size(); ++i)
	{
		for (size_t j = 0; j < m_vecMeshContainer[i]->vecIB.size(); ++j)
		{
			SAFE_DELETE_ARRAY(m_vecMeshContainer[i]->vecIB[j]->pData);
			SAFE_RELEASE(m_vecMeshContainer[i]->vecIB[j]->pBuffer);
			SAFE_DELETE(m_vecMeshContainer[i]->vecIB[j]);
		}

		SAFE_DELETE_ARRAY(m_vecMeshContainer[i]->tVB.pData);
		SAFE_RELEASE(m_vecMeshContainer[i]->tVB.pBuffer);
		SAFE_DELETE(m_vecMeshContainer[i]);
	}

	m_vecMeshContainer.clear();

	SAFE_RELEASE(m_pAnimation);
	SAFE_RELEASE(m_pMaterial);
}

string CMesh::GetShaderKey() const
{
	return m_strShaderKey;
}

string CMesh::GetInputLayoutKey() const
{
	return m_strInputLayoutKey;
}

size_t CMesh::GetContainerCount() const
{
	return m_vecMeshContainer.size();
}

size_t CMesh::GetSubsetCount(int iContainer) const
{
	return m_vecMeshContainer[iContainer]->vecIB.size();
}

Vector3 CMesh::GetView() const
{
	return m_vView;
}

Vector3 CMesh::GetCenter() const
{
	return m_vCenter;
}

Vector3 CMesh::GetMin() const
{
	return m_vMin;
}

Vector3 CMesh::GetMax() const
{
	return m_vMax;
}

float CMesh::GetRadius() const
{
	return m_fRadius;
}

void CMesh::SetView(const Vector3 & vView)
{
	m_vView = vView;
}

bool CMesh::CreateMesh(const string & strName, const string& strShaderKey, const string& strInputLayoutKey,
	int iVtxSize, int iVtxCount, D3D11_USAGE eVtxUsage, D3D11_PRIMITIVE_TOPOLOGY ePrimitive, void * pVtx,
	int iIdxSize, int iIdxCount, D3D11_USAGE eIdxUsage,	DXGI_FORMAT eFmt, void * pIdx)
{
	m_strTag = strName;
	m_strShaderKey = strShaderKey;
	m_strInputLayoutKey = strInputLayoutKey;

	PMeshContainer	pContainer = new MeshContainer;
	m_vecMeshContainer.push_back(pContainer);

	if (!CreateVertexBuffer(iVtxSize, iVtxCount, eVtxUsage, ePrimitive, pVtx))
		return false;

	if (pIdx != nullptr)
	{
		if (!CreateIndexBuffer(iIdxSize, iIdxCount, eIdxUsage, eFmt, pIdx))
			return false;
	}

	return true;
}

bool CMesh::LoadMesh(const string & strName, const TCHAR * pFileName,  const Vector3& vView, const string & strPathName)
{
	const TCHAR*	pPath = GET_SINGLE(CPathManager)->FindPath(strPathName);

	TCHAR	strFullPath[MAX_PATH] = {};

	if (pPath)
		lstrcpy(strFullPath, pPath);

	lstrcat(strFullPath, pFileName);

	return LoadMeshFromFullPath(strName, strFullPath);
}

bool CMesh::LoadMesh(const string & strName, const char * pFileName,
	const Vector3& vView,
	const string & strPathName)
{
	const char*	pPath = GET_SINGLE(CPathManager)->FindPathMultibyte(strPathName);

	char	strFullPath[MAX_PATH] = {};

	if (pPath)
		strcpy_s(strFullPath, pPath);

	strcat_s(strFullPath, pFileName);

	return LoadMeshFromFullPath(strName, strFullPath);
}

bool CMesh::LoadMeshFromFullPath(const string & strName,
	const TCHAR * pFullPath,
	const Vector3& vView)
{
	char	strFullPath[MAX_PATH] = {};

	WideCharToMultiByte(CP_UTF8, 0, pFullPath, -1,
		strFullPath, lstrlen(pFullPath), 0, 0);

	return LoadMeshFromFullPath(strName, strFullPath);
}

bool CMesh::LoadMeshFromFullPath(const string & strName,
	const char * pFullPath,
	const Vector3& vView)
{
	SetTag(strName);
	m_vView = vView;

	char	strExt[_MAX_EXT] = {};

	_splitpath_s(pFullPath, nullptr, 0, nullptr, 0, nullptr, 0, strExt, _MAX_EXT);

	_strupr_s(strExt);

	if (strcmp(strExt, ".FBX") == 0)
	{
		CFbxLoader	loader;

		loader.LoadFbx(pFullPath);

		return ConvertFbx(&loader, pFullPath);
	}

	return LoadFromFullPath(pFullPath);
}

void CMesh::Render()
{
	for (size_t i = 0; i < m_vecMeshContainer.size(); ++i)
	{
		PMeshContainer	pContainer = m_vecMeshContainer[i];
		
		UINT	iStride = pContainer->tVB.iSize;
		UINT	iOffset = 0;

		_CONTEXT->IASetPrimitiveTopology(pContainer->tVB.ePrimitive);
		_CONTEXT->IASetVertexBuffers(0, 1, &pContainer->tVB.pBuffer,
			&iStride, &iOffset);

		if (pContainer->vecIB.empty())
		{
			_CONTEXT->Draw(pContainer->tVB.iCount, 0);
		}

		else
		{
			for (size_t j = 0; j < pContainer->vecIB.size(); ++j)
			{
				PIndexBuffer	pSubset = pContainer->vecIB[j];

				_CONTEXT->IASetIndexBuffer(pSubset->pBuffer, pSubset->eFmt, 0);
				_CONTEXT->DrawIndexed(pSubset->iCount, 0, 0);
			}
		}
	}
}

void CMesh::Render(int iContainer, int iSubset)
{
	PMeshContainer	pContainer = m_vecMeshContainer[iContainer];

	UINT	iStride = pContainer->tVB.iSize;
	UINT	iOffset = 0;

	_CONTEXT->IASetPrimitiveTopology(pContainer->tVB.ePrimitive);
	_CONTEXT->IASetVertexBuffers(0, 1, &pContainer->tVB.pBuffer,
		&iStride, &iOffset);

	if (pContainer->vecIB.empty())
	{
		_CONTEXT->Draw(pContainer->tVB.iCount, 0);
	}

	else
	{
		PIndexBuffer	pSubset = pContainer->vecIB[iSubset];

		_CONTEXT->IASetIndexBuffer(pSubset->pBuffer, pSubset->eFmt, 0);
		_CONTEXT->DrawIndexed(pSubset->iCount, 0, 0);
	}
}

void CMesh::RenderInstancing(unsigned int iContainer, unsigned int iSubset, PInstancingBuffer pInstancingBuffer, int iInstancingCount)
{
	PMeshContainer pContainer = m_vecMeshContainer[iContainer];

	UINT iStride[2] = { (UINT)pContainer->tVB.iSize, (UINT)pInstancingBuffer->iSize };
	UINT iOffset[2] = { 0,0 };

	ID3D11Buffer* pBuffer[2] = { pContainer->tVB.pBuffer, pInstancingBuffer->pBuffer };
	_CONTEXT->IASetPrimitiveTopology(pContainer->tVB.ePrimitive);
	_CONTEXT->IASetVertexBuffers(0, 2, pBuffer, iStride, iOffset);

	if (pContainer->vecIB.empty())
		_CONTEXT->DrawInstanced(pContainer->tVB.iCount, iInstancingCount, 0, 0);
	else
	{
		PIndexBuffer pSubset = pContainer->vecIB[iSubset];

		_CONTEXT->IASetIndexBuffer(pSubset->pBuffer, pSubset->eFmt, 0);
		_CONTEXT->DrawIndexedInstanced(pSubset->iCount, iInstancingCount, 0, 0, 0);
	}
}

bool CMesh::CreateVertexBuffer(int iSize, int iCount, D3D11_USAGE eUsage, 
	D3D11_PRIMITIVE_TOPOLOGY ePrimitive, void * pData)
{
	PMeshContainer	pContainer = m_vecMeshContainer.back();

	D3D11_BUFFER_DESC	tDesc = {};

	tDesc.ByteWidth = iSize * iCount;
	tDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	tDesc.Usage = eUsage;

	switch (eUsage)
	{
	case D3D11_USAGE_DYNAMIC:
		tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		break;
	case D3D11_USAGE_STAGING:
		tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
		break;
	}

	pContainer->tVB.iSize = iSize;
	pContainer->tVB.iCount = iCount;
	pContainer->tVB.eUsage = eUsage;
	pContainer->tVB.ePrimitive = ePrimitive;
	pContainer->tVB.pData = new char[iSize * iCount];

	memcpy(pContainer->tVB.pData, pData, iSize * iCount);

	D3D11_SUBRESOURCE_DATA	tData = {};
	tData.pSysMem = pContainer->tVB.pData;

	if (FAILED(_DEVICE->CreateBuffer(&tDesc, &tData, &pContainer->tVB.pBuffer)))
		return false;

	char*	pVertices = (char*)pData;
	Vector3	vPos;
	memcpy(&vPos, pVertices, sizeof(Vector3));

	m_vMin = vPos;
	m_vMax = vPos;

	for (int i = 1; i < iCount; ++i)
	{
		memcpy(&vPos, pVertices + iSize * i, sizeof(Vector3));
		if (m_vMin.x > vPos.x)
			m_vMin.x = vPos.x;

		if (m_vMin.y > vPos.y)
			m_vMin.y = vPos.y;

		if (m_vMin.z > vPos.z)
			m_vMin.z = vPos.z;

		if (m_vMax.x < vPos.x)
			m_vMax.x = vPos.x;

		if (m_vMax.y < vPos.y)
			m_vMax.y = vPos.y;

		if (m_vMax.z < vPos.z)
			m_vMax.z = vPos.z;
	}

	m_vCenter = (m_vMin + m_vMax) / 2.f;
	m_vLength = m_vMax - m_vMin;
	m_fRadius = m_vLength.Length() / 2.f;

	return true;
}

bool CMesh::CreateIndexBuffer(int iSize, int iCount, D3D11_USAGE eUsage,
	DXGI_FORMAT eFmt, void * pData)
{
	PMeshContainer	pContainer = m_vecMeshContainer.back();
	PIndexBuffer	pIB = new IndexBuffer;

	pContainer->vecIB.push_back(pIB);

	D3D11_BUFFER_DESC	tDesc = {};

	tDesc.ByteWidth = iSize * iCount;
	tDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	tDesc.Usage = eUsage;

	switch (eUsage)
	{
	case D3D11_USAGE_DYNAMIC:
		tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		break;
	case D3D11_USAGE_STAGING:
		tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
		break;
	}

	pIB->iSize = iSize;
	pIB->iCount = iCount;
	pIB->eUsage = eUsage;
	pIB->eFmt = eFmt;
	pIB->pData = new char[iSize * iCount];

	memcpy(pIB->pData, pData, iSize * iCount);

	D3D11_SUBRESOURCE_DATA	tData = {};
	tData.pSysMem = pIB->pData;

	if (FAILED(_DEVICE->CreateBuffer(&tDesc, &tData, &pIB->pBuffer)))
		return false;

	return true;
}

bool CMesh::ConvertFbx(CFbxLoader * pLoader,
	const char* pFullPath)
{
	const vector<PFBXMESHCONTAINER>*	pvecContainer = pLoader->GetMeshContainers();
	const vector<vector<PFBXMATERIAL>>*	pvecMaterials = pLoader->GetMaterials();

	vector<PFBXMESHCONTAINER>::const_iterator	iter;
	vector<PFBXMESHCONTAINER>::const_iterator	iterEnd = pvecContainer->end();

	vector<bool>	vecEmptyIndex;
	bool			bAnimation = false;

	for (iter = pvecContainer->begin(); iter != iterEnd; ++iter)
	{
		PMeshContainer	pContainer = new MeshContainer;

		m_strInputLayoutKey = VERTEX3D_LAYOUT;

		m_vecMeshContainer.push_back(pContainer);

		int	iVtxSize = 0;

		// 범프가 있을 경우
		if ((*iter)->bBump)
		{
			if ((*iter)->bAnimation)
			{
				bAnimation = true;
				//m_strShaderKey = STANDARD_BUMP_ANIM_SHADER;
			}

			else
			{
				m_strShaderKey = STANDARD_3D_SHADER;
			}
		}

		// 범프가 없을 경우
		else
		{
			if ((*iter)->bAnimation)
			{
				bAnimation = true;
				//m_strShaderKey = STANDARD_TEX_NORMAL_ANIM_SHADER;
			}

			else
			{
				m_strShaderKey = STANDARD_TEX_NORMAL_SHADER;
			}
		}

		m_strShaderKey = STANDARD_3D_SHADER;

		vector<Vertex3D>	vecVtx;
		iVtxSize = sizeof(Vertex3D);

		for (size_t i = 0; i < (*iter)->vecPos.size(); ++i)
		{
			Vertex3D	tVtx = {};

			tVtx.vPos = (*iter)->vecPos[i];
			tVtx.vNormal = (*iter)->vecNormal[i];
			tVtx.vUV = (*iter)->vecUV[i];

			if (!(*iter)->vecTangent.empty())
				tVtx.vTangent = (*iter)->vecTangent[i];

			if (!(*iter)->vecBinormal.empty())
				tVtx.vBinormal = (*iter)->vecBinormal[i];

			if (!(*iter)->vecBlendWeight.empty())
			{
				tVtx.vBlendWeights = (*iter)->vecBlendWeight[i];
				tVtx.vBlendIndices = (*iter)->vecBlendIndex[i];
			}

			vecVtx.push_back(tVtx);
		}

		pContainer->tVB.ePrimitive = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		if (!CreateVertexBuffer(iVtxSize, (int)vecVtx.size(),
			D3D11_USAGE_DEFAULT, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			&vecVtx[0]))
			return false;

		// 인덱스버퍼 생성
		for (size_t i = 0; i < (*iter)->vecIndices.size(); ++i)
		{
			if ((*iter)->vecIndices[i].empty())
			{
				vecEmptyIndex.push_back(false);
				continue;
			}

			vecEmptyIndex.push_back(true);

			if (!CreateIndexBuffer(4, (int)(*iter)->vecIndices[i].size(),
				D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32_UINT,
				&(*iter)->vecIndices[i][0]))
				return false;
		}
	}

	// 재질 정보를 읽어온다.
	const vector<vector<PFBXMATERIAL>>*	pMaterials = pLoader->GetMaterials();

	vector<vector<PFBXMATERIAL>>::const_iterator	iterM;
	vector<vector<PFBXMATERIAL>>::const_iterator	iterMEnd = pMaterials->end();

	if (!pMaterials->empty())
	{
		// 실제 사용할 재질 클래스를 생성한다.
		m_pMaterial = new CMaterial;

		if (!m_pMaterial->Init())
		{
			SAFE_RELEASE(m_pMaterial);
			return NULL;
		}
	}

	int	iContainer = 0;
	for (iterM = pMaterials->begin(); iterM != iterMEnd; ++iterM, ++iContainer)
	{
		for (int i = 0; i < (*iterM).size(); ++i)
		{
			// 인덱스 버퍼가 비어있을 경우에는 재질을 추가하지 않는다.
			if (!vecEmptyIndex[i])
				continue;

			// 재질 정보를 얻어온다.
			PFBXMATERIAL	pMtrl = (*iterM)[i];

			m_pMaterial->SetColor(pMtrl->vDif, pMtrl->vAmb,
				pMtrl->vSpc, pMtrl->vEmv, pMtrl->fShininess, iContainer, (UINT)i);

			if (bAnimation)
				m_pMaterial->Skinning(iContainer, i);

			// 이름을 불러온다.
			char	strName[MAX_PATH] = {};
			_splitpath_s(pMtrl->strDifTex.c_str(), NULL, 0, NULL, 0,
				strName, MAX_PATH, NULL, 0);

			TCHAR	strPath[MAX_PATH] = {};

#ifdef UNICODE
			MultiByteToWideChar(CP_ACP, 0, pMtrl->strDifTex.c_str(),
				-1, strPath, (int)pMtrl->strDifTex.length());
#else
			strcpy_s(strPath, pMtrl->strDifTex.c_str());
#endif // UNICODE

			m_pMaterial->AddTextureSetFromFullPath(iContainer, i, 0, strName,
				strPath);

			if (!pMtrl->strBumpTex.empty())
			{
				memset(strName, 0, MAX_PATH);
				_splitpath_s(pMtrl->strBumpTex.c_str(), NULL, 0, NULL, 0,
					strName, MAX_PATH, NULL, 0);

				memset(strPath, 0, sizeof(wchar_t) * MAX_PATH);

#ifdef UNICODE
				MultiByteToWideChar(CP_ACP, 0, pMtrl->strBumpTex.c_str(),
					-1, strPath, (int)pMtrl->strBumpTex.length());
#else
				strcpy_s(strPath, pMtrl->strBumpTex.c_str());
#endif // UNICODE

				m_pMaterial->AddTextureSetFromFullPath(iContainer, i,
					1, strName,
					strPath);

				m_pMaterial->BumpTextureEnable(iContainer, i);
			}

			if (!pMtrl->strSpcTex.empty())
			{
				memset(strName, 0, MAX_PATH);
				_splitpath_s(pMtrl->strSpcTex.c_str(), NULL, 0, NULL, 0,
					strName, MAX_PATH, NULL, 0);

				memset(strPath, 0, sizeof(wchar_t) * MAX_PATH);

#ifdef UNICODE
				MultiByteToWideChar(CP_ACP, 0, pMtrl->strSpcTex.c_str(),
					-1, strPath, (int)pMtrl->strSpcTex.length());
#else
				strcpy_s(strPath, pMtrl->strSpcTex.c_str());
#endif // UNICODE

				m_pMaterial->AddTextureSetFromFullPath(iContainer, i,
					2, strName,
					strPath);

				m_pMaterial->SpecularTextureEnable(iContainer, i);
			}
		}
	}

	// 텍스쳐가 저장된 폴더명을 키로 변경한다.
	//char	strFullName[MAX_PATH] = {};
	//iterM = pMaterials->begin();
	//strcpy_s(strFullName, (*iterM)[0]->strDifTex.c_str());

	//int	iLength = strlen(strFullName);
	//for (int i = iLength - 1; i >= 0; --i)
	//{
	//	if (strFullName[i] == '\\' || strFullName[i] == '/')
	//	{
	//		memset(strFullName + (i + 1), 0, sizeof(char) * (iLength - (i + 1)));
	//		strFullName[i] = '\\';
	//		//strFullName[i] = 0;
	//		break;
	//	}
	//}

	//char	strChange[MAX_PATH] = {};
	//strcpy_s(strChange, strFullName);
	//iLength = strlen(strChange);
	//for (int i = iLength - 2; i >= 0; --i)
	//{
	//	if (strChange[i] == '\\' || strChange[i] == '/')
	//	{
	//		memset(strChange + (i + 1), 0, sizeof(char) * (iLength - (i + 1)));
	//		break;
	//	}
	//}

	//strcat_s(strChange, m_strTag.c_str());
	//strcat_s(strChange, "\\");

	//MoveFileA(strFullName, strChange);

	// Mesh\\ 까지의 경로를 제거한다.
	/*iLength = strlen(strChange);
	for (int i = iLength - 2; i >= 0; --i)
	{
	char	cText[5] = {};
	memcpy(cText, &strChange[i - 4], 4);
	_strupr_s(cText);

	if (strcmp(cText, "MESH") == 0)
	{
	memset(strChange, 0, sizeof(char) * (i + 1));
	memcpy(strChange, &strChange[i + 1], sizeof(char) * (iLength - (i + 1)));
	memset(strChange + (i + 1), 0, sizeof(char) * (iLength - (i + 1)));
	break;
	}
	}*/

	/*for (size_t i = 0; i < m_vecMeshContainer.size(); ++i)
	{
	PMESHCONTAINER	pContainer = m_vecMeshContainer[i];

	for (size_t j = 0; j < pContainer->vecMaterial.size(); ++j)
	{
	pContainer->vecMaterial[j]->SetTexturePathKey(MESH_PATH);
	pContainer->vecMaterial[j]->ChangeTexturePath(strChange);
	}
	}*/

	m_vLength = m_vMax - m_vMin;

	m_vCenter = (m_vMax + m_vMin) / 2.f;
	m_fRadius = m_vLength.Length() / 2.f; 
	
	char	strFullPath[MAX_PATH] = {};
	strcpy_s(strFullPath, pFullPath);
	int	iPathLength = (int)strlen(strFullPath);
	memcpy(&strFullPath[iPathLength - 3], "msh", 3);

	SaveFromFullPath(strFullPath);

	// 애니메이션 처리
	const vector<PFBXBONE>*	pvecBone = pLoader->GetBones();

	if (pvecBone->empty())
		return true;

	SAFE_RELEASE(m_pAnimation);

	m_pAnimation = new CAnimation;

	if (!m_pAnimation->Init())
	{
		SAFE_RELEASE(m_pAnimation);
		return false;
	}

	//// 본 수만큼 반복한다.
	vector<PFBXBONE>::const_iterator	iterB;
	vector<PFBXBONE>::const_iterator	iterBEnd = pvecBone->end();

	for (iterB = pvecBone->begin(); iterB != iterBEnd; ++iterB)
	{
		PBONE	pBone = new BONE;

		pBone->strName = (*iterB)->strName;
		pBone->iDepth = (*iterB)->iDepth;
		pBone->iParentIndex = (*iterB)->iParentIndex;

		float	fMat[4][4];

		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				fMat[i][j] = (float)(*iterB)->matOffset.mData[i].mData[j];
			}
		}

		pBone->matOffset = new Matrix;
		*pBone->matOffset = fMat;

		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				fMat[i][j] = (float)(*iterB)->matBone.mData[i].mData[j];
			}
		}

		pBone->matBone = new Matrix;
		*pBone->matBone = fMat;

		m_pAnimation->AddBone(pBone);
	}

	m_pAnimation->CreateBoneTexture();

	// 애니메이션 클립을 추가한다.
	const vector<PFBXANIMATIONCLIP>* pvecClip = pLoader->GetClips();

	// 클립을 읽어온다.
	vector<PFBXANIMATIONCLIP>::const_iterator	iterC;
	vector<PFBXANIMATIONCLIP>::const_iterator	iterCEnd = pvecClip->end();

	for (iterC = pvecClip->begin(); iterC != iterCEnd; ++iterC)
	{
		m_pAnimation->AddClip(AO_LOOP, *iterC);
	}

	if (m_pAnimation)
	{
		memcpy(&strFullPath[iPathLength - 3], "bne", 3);
		m_pAnimation->SaveBoneFromFullPath(strFullPath);

		memcpy(&strFullPath[iPathLength - 3], "anm", 3);
		m_pAnimation->SaveFromFullPath(strFullPath);
	}

	return true;
}

bool CMesh::Save(const char * pFileName, const string & strPathKey)
{
	char	strFullPath[MAX_PATH] = {};
	const char* pPath = GET_SINGLE(CPathManager)->FindPathMultibyte(strPathKey);

	if (pPath)
		strcpy_s(strFullPath, pPath);
	strcat_s(strFullPath, pFileName);

	return SaveFromFullPath(strFullPath);
}

bool CMesh::SaveFromFullPath(const char * pFullPath)
{
	FILE*	pFile = nullptr;

	fopen_s(&pFile, pFullPath, "wb");

	if (!pFile)
		return false;

	int	iLength = (int)m_strTag.length();

	// Tag 길이를 저장한다.
	fwrite(&iLength, 4, 1, pFile);
	fwrite(m_strTag.c_str(), 1, iLength, pFile);

	// ShaderName 길이를 저장한다.
	iLength = (int)m_strShaderKey.length();
	fwrite(&iLength, 4, 1, pFile);
	fwrite(m_strShaderKey.c_str(), 1, iLength, pFile);

	// 입력레이아웃 이름 길이를 저장한다.
	iLength = (int)m_strInputLayoutKey.length();
	fwrite(&iLength, 4, 1, pFile);
	fwrite(m_strInputLayoutKey.c_str(), 1, iLength, pFile);

	fwrite(&m_vCenter, sizeof(Vector3), 1, pFile);
	fwrite(&m_fRadius, sizeof(float), 1, pFile);
	fwrite(&m_vMin, sizeof(Vector3), 1, pFile);
	fwrite(&m_vMax, sizeof(Vector3), 1, pFile);
	fwrite(&m_vLength, sizeof(Vector3), 1, pFile);

	size_t	iContainer = m_vecMeshContainer.size();

	fwrite(&iContainer, sizeof(size_t), 1, pFile);

	for (size_t i = 0; i < iContainer; ++i)
	{
		PMeshContainer	pContainer = m_vecMeshContainer[i];

		fwrite(&pContainer->tVB.ePrimitive, sizeof(D3D11_PRIMITIVE_TOPOLOGY),
			1, pFile);

		fwrite(&pContainer->tVB.iSize, sizeof(int), 1, pFile);
		fwrite(&pContainer->tVB.iCount, sizeof(int), 1, pFile);
		fwrite(&pContainer->tVB.eUsage, sizeof(D3D11_USAGE), 1, pFile);
		fwrite(pContainer->tVB.pData, pContainer->tVB.iSize,
			pContainer->tVB.iCount, pFile);

		size_t	iIdxCount = pContainer->vecIB.size();

		fwrite(&iIdxCount, sizeof(size_t), 1, pFile);

		for (size_t j = 0; j < iIdxCount; ++j)
		{
			fwrite(&pContainer->vecIB[j]->eFmt, sizeof(DXGI_FORMAT),
				1, pFile);
			fwrite(&pContainer->vecIB[j]->iSize, sizeof(int), 1, pFile);
			fwrite(&pContainer->vecIB[j]->iCount, sizeof(int), 1, pFile);
			fwrite(&pContainer->vecIB[j]->eUsage, sizeof(D3D11_USAGE), 1, pFile);
			fwrite(pContainer->vecIB[j]->pData, pContainer->vecIB[j]->iSize,
				pContainer->vecIB[j]->iCount, pFile);
		}
	}

	bool	bMaterial = false;

	if (m_pMaterial)
	{
		bMaterial = true;

		fwrite(&bMaterial, sizeof(bool), 1, pFile);

		m_pMaterial->Save(pFile);
	}

	else
		fwrite(&bMaterial, sizeof(bool), 1, pFile);

	fclose(pFile);

	return true;
}

bool CMesh::Load(const char * pFileName, const string & strPathKey)
{
	char	strFullPath[MAX_PATH] = {};
	const char* pPath = GET_SINGLE(CPathManager)->FindPathMultibyte(strPathKey);

	if (pPath)
		strcpy_s(strFullPath, pPath);
	strcat_s(strFullPath, pFileName);

	return LoadFromFullPath(strFullPath);
}

bool CMesh::LoadFromFullPath(const char * pFullPath)
{
	FILE*	pFile = nullptr;

	fopen_s(&pFile, pFullPath, "rb");

	if (!pFile)
		return false;

	int	iLength = 0;

	// Tag 길이를 저장한다.
	fread(&iLength, 4, 1, pFile);
	char	strTag[256] = {};
	fread(strTag, 1, iLength, pFile);
	m_strTag = strTag;

	// ShaderName 길이를 저장한다.
	iLength = 0;
	fread(&iLength, 4, 1, pFile);
	char	strShaderKey[256] = {};
	fread(strShaderKey, 1, iLength, pFile);
	m_strShaderKey = strShaderKey;

	// 입력레이아웃 이름 길이를 저장한다.
	iLength = 0;
	fread(&iLength, 4, 1, pFile);
	char	strLayoutKey[256] = {};
	fread(strLayoutKey, 1, iLength, pFile);
	m_strInputLayoutKey = strLayoutKey;

	fread(&m_vCenter, sizeof(Vector3), 1, pFile);
	fread(&m_fRadius, sizeof(float), 1, pFile);
	fread(&m_vMin, sizeof(Vector3), 1, pFile);
	fread(&m_vMax, sizeof(Vector3), 1, pFile);
	fread(&m_vLength, sizeof(Vector3), 1, pFile);

	size_t	iContainer = 0;

	fread(&iContainer, sizeof(size_t), 1, pFile);

	for (size_t i = 0; i < iContainer; ++i)
	{
		PMeshContainer	pContainer = new MeshContainer;
		m_vecMeshContainer.push_back(pContainer);

		int	iVtxSize = 0;
		int	iVtxCount = 0;
		D3D11_USAGE	eUsage;

		fread(&pContainer->tVB.ePrimitive, sizeof(D3D11_PRIMITIVE_TOPOLOGY), 1, pFile);
		fread(&iVtxSize, sizeof(int), 1, pFile);
		fread(&iVtxCount, sizeof(int), 1, pFile);
		fread(&eUsage, sizeof(D3D11_USAGE), 1, pFile);
		char*	pData = new char[iVtxSize * iVtxCount];
		fread(pData, iVtxSize, iVtxCount, pFile);

		CreateVertexBuffer(iVtxSize, iVtxCount, eUsage,
			pContainer->tVB.ePrimitive, pData);

		SAFE_DELETE_ARRAY(pData);

		size_t	iIdxCount = 0;

		fread(&iIdxCount, sizeof(size_t), 1, pFile);

		for (size_t j = 0; j < iIdxCount; ++j)
		{
			DXGI_FORMAT	eFmt;
			int	iIdxSize = 0;
			int	iIdxCount = 0;
			fread(&eFmt, sizeof(DXGI_FORMAT), 1, pFile);
			fread(&iIdxSize, sizeof(int), 1, pFile);
			fread(&iIdxCount, sizeof(int), 1, pFile);
			fread(&eUsage, sizeof(D3D11_USAGE), 1, pFile);

			pData = new char[iIdxSize * iIdxCount];

			fread(pData, iIdxSize, iIdxCount, pFile);

			CreateIndexBuffer(iIdxSize, iIdxCount, eUsage, eFmt,
				pData);

			SAFE_DELETE_ARRAY(pData);
		}
	}

	bool	bMaterial = false;
	fread(&bMaterial, sizeof(bool), 1, pFile);

	if (bMaterial)
	{
		SAFE_RELEASE(m_pMaterial);
		bMaterial = true;

		m_pMaterial = new CMaterial;

		m_pMaterial->Init();

		m_pMaterial->Load(pFile);
	}

	fclose(pFile);

	return true;
}

CMaterial * CMesh::CloneMaterial()
{
	if (!m_pMaterial)
		return nullptr;

	return m_pMaterial->Clone();;
}

CAnimation * CMesh::CloneAnimation()
{
	if (!m_pAnimation)
		return nullptr;

	return m_pAnimation->Clone();
}
