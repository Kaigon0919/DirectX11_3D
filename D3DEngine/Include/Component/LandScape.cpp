#include "LandScape.h"
#include "../PathManager.h"
#include "../Resource/Mesh.h"
#include "../Resource/ResourcesManager.h"
#include "../Resource/Texture.h"
#include "Material.h"
#include "Renderer.h"
#include "../GameObject.h"
#include "../Render/ShaderManager.h"
#include "../Navigation/NavigationManager.h"
#include "../Navigation/NavigationMesh.h"

ENGINE_USING

CLandScape::CLandScape()	:
	m_iNumX(0),
	m_iNumZ(0)
{
	m_eComType = CT_LANDSCAPE;
	SetTag("LandScape");
	SetDetailLevel(30.f, 30.f);
	m_tCBuffer.iSplatCount = 0;
	m_pMaterial = nullptr;
	m_pSplatDifTex = nullptr;
	m_pSplatNrmTex = nullptr;
	m_pSplatSpcTex = nullptr;
	m_pSplatAlphaTex = nullptr;
}

CLandScape::CLandScape(const CLandScape & com)	:
	CComponent(com)
{
	m_iNumX = com.m_iNumX;
	m_iNumZ = com.m_iNumZ;

	m_tCBuffer = com.m_tCBuffer;
	m_pMaterial = nullptr;
	m_pSplatDifTex = com.m_pSplatDifTex;
	m_pSplatNrmTex = com.m_pSplatNrmTex;
	m_pSplatSpcTex = com.m_pSplatSpcTex;
	m_pSplatAlphaTex = com.m_pSplatAlphaTex;

	if (m_pSplatDifTex)
		m_pSplatDifTex->AddRef();

	if (m_pSplatNrmTex)
		m_pSplatNrmTex->AddRef();

	if (m_pSplatSpcTex)
		m_pSplatSpcTex->AddRef();

	if (m_pSplatAlphaTex)
		m_pSplatAlphaTex->AddRef();
}

CLandScape::~CLandScape()
{
	SAFE_RELEASE(m_pMaterial);
	SAFE_RELEASE(m_pSplatDifTex);
	SAFE_RELEASE(m_pSplatNrmTex);
	SAFE_RELEASE(m_pSplatSpcTex);
	SAFE_RELEASE(m_pSplatAlphaTex);
}

bool CLandScape::CreateLandScape(const string & strName, 
	const char * pFileName, const string & strPathName)
{
	char	strExt[_MAX_EXT] = {};

	_splitpath_s(pFileName, 0, 0, 0, 0, 0, 0, strExt, _MAX_EXT);

	_strupr_s(strExt);

	char	strFullPath[MAX_PATH] = {};

	const char* pPath = GET_SINGLE(CPathManager)->FindPathMultibyte(strPathName);

	if (pPath)
		strcpy_s(strFullPath, pPath);
	strcat_s(strFullPath, pFileName);

	FILE*	pFile = nullptr;

	fopen_s(&pFile, strFullPath, "rb");

	if (!pFile)
		return false;


	if (strcmp(strExt, ".BMP") == 0)
	{
		BITMAPFILEHEADER	fh;
		BITMAPINFOHEADER	ih;

		fread(&fh, sizeof(fh), 1, pFile);
		fread(&ih, sizeof(ih), 1, pFile);

		int	iPixelLength = fh.bfSize - fh.bfOffBits;

		m_iNumX = ih.biWidth;
		m_iNumZ = ih.biHeight;

		// 전체 픽셀의 크기만큼 동적할당한다.
		unsigned char* pPixel = new unsigned char[iPixelLength];

		// 전체 픽셀 정보를 읽어온다.
		fread(pPixel, 1, iPixelLength, pFile);

		int	iPixelByte = 0;

		switch (ih.biBitCount)
		{
		case 8:
			iPixelByte = 1;
			break;
		case 24:
			iPixelByte = 3;
			break;
		case 32:
			iPixelByte = 4;
			break;
		}
		unsigned char* pLine = new unsigned char[m_iNumX * iPixelByte];

		for (int i = 0; i < m_iNumZ / 2; ++i)
		{
			int	iSrcIdx = i * m_iNumX * iPixelByte;
			int	iDestIdx = (m_iNumZ - 1 - i) * m_iNumX * iPixelByte;

			// 라인별로 Swap한다.
			memcpy(pLine, &pPixel[iSrcIdx], m_iNumX * iPixelByte);
			memcpy(&pPixel[iSrcIdx],&pPixel[iDestIdx],	m_iNumX * iPixelByte);
			memcpy(&pPixel[iDestIdx], pLine, m_iNumX * iPixelByte);
		}

		SAFE_DELETE_ARRAY(pLine);


		m_vecVtx.reserve(m_iNumX * m_iNumZ);

		for (int i = 0; i < m_iNumZ; ++i)
		{
			for (int j = 0; j < m_iNumX; ++j)
			{
				Vertex3D	tVtx = {};
				int	idx = i * m_iNumX * iPixelByte + j * iPixelByte;

				float	fY = pPixel[idx] / 20.f;

				tVtx.vPos = Vector3((float)j, fY, (float)(m_iNumZ - i - 1));
				tVtx.vNormal = Vector3(0.f, 0.f, 0.f);
				tVtx.vUV = Vector2((float)j / (m_iNumX - 1.f), (float)i / (m_iNumZ - 1.f));
				tVtx.vTangent = Vector3(1.f, 0.f, 0.f);
				tVtx.vBinormal = Vector3(0.f, 0.f, -1.f);

				m_vecVtx.push_back(tVtx);
			}
		}

		m_vecIdx.reserve((m_iNumX - 1) * (m_iNumZ - 1) * 2);
		m_vecFaceNormal.resize((m_iNumX - 1) * (m_iNumZ - 1) * 2);

		// 내비메쉬 생성
		CNavigationMesh*	pNavMesh = GET_SINGLE(CNavigationManager)->CreateNavMesh(m_pScene, m_strTag);

		int	iTriIndex = 0;
		Vector3	vEdge1, vEdge2;
		Vector3	vCellPos[3];

		for (int i = 0; i < m_iNumZ - 1; ++i)
		{
			for (int j = 0; j < m_iNumX - 1; ++j)
			{
				int	idx = i * m_iNumX + j;

				// 우상단 삼각형 추가
				m_vecIdx.push_back(idx);
				m_vecIdx.push_back(idx + 1);
				m_vecIdx.push_back(idx + m_iNumX + 1);

				vEdge1 = m_vecVtx[idx + 1].vPos - m_vecVtx[idx].vPos;
				vEdge2 = m_vecVtx[idx + m_iNumX + 1].vPos - m_vecVtx[idx].vPos;

				vEdge1.Normalize();
				vEdge2.Normalize();

				m_vecFaceNormal[iTriIndex] = vEdge1.Cross(vEdge2);
				m_vecFaceNormal[iTriIndex].Normalize();
				++iTriIndex;

				// 좌하단 삼각형 추가
				m_vecIdx.push_back(idx);
				m_vecIdx.push_back(idx + m_iNumX + 1);
				m_vecIdx.push_back(idx + m_iNumX);

				vEdge1 = m_vecVtx[idx + m_iNumX + 1].vPos - m_vecVtx[idx].vPos;
				vEdge2 = m_vecVtx[idx + m_iNumX].vPos - m_vecVtx[idx].vPos;

				vEdge1.Normalize();
				vEdge2.Normalize();

				m_vecFaceNormal[iTriIndex] = vEdge1.Cross(vEdge2);
				m_vecFaceNormal[iTriIndex].Normalize();
				++iTriIndex;

				// 내비메쉬 좌상단 삼각형 추가
				vCellPos[0] = m_vecVtx[idx].vPos;
				vCellPos[1] = m_vecVtx[idx + 1].vPos;
				vCellPos[2] = m_vecVtx[idx + m_iNumX + 1].vPos;
				pNavMesh->AddCell(vCellPos);
				// 내비메쉬 좌하단 삼각형 추가
				vCellPos[0] = m_vecVtx[idx].vPos;
				vCellPos[1] = m_vecVtx[idx + m_iNumX + 1].vPos;
				vCellPos[2] = m_vecVtx[idx + m_iNumX].vPos;
				pNavMesh->AddCell(vCellPos);
			}
		}

		SAFE_DELETE_ARRAY(pPixel);

		fclose(pFile);

		ComputeNormal();
		ComputeTangent();

		pNavMesh->CreateGridMapAdj(m_iNumX - 1);

		// 메쉬를 만든다.
		GET_SINGLE(CResourcesManager)->CreateMesh(strName, LANDSCAPE_SHADER, 
			VERTEX3D_LAYOUT, Vector3(0.f, 1.f, 0.f), sizeof(Vertex3D), (int)m_vecVtx.size(), D3D11_USAGE_DEFAULT, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, &m_vecVtx[0],
			4, (int)m_vecIdx.size(), D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32_UINT, &m_vecIdx[0]);

		CRenderer*	pRenderer = FindComponentFromType<CRenderer>(CT_RENDERER);

		if (!pRenderer)
			pRenderer = AddComponent<CRenderer>("LandScapeRenderer");

		pRenderer->SetMesh(strName);

		SAFE_RELEASE(pRenderer);

		m_pMaterial = FindComponentFromType<CMaterial>(CT_MATERIAL);

		if (!m_pMaterial)
			m_pMaterial = AddComponent<CMaterial>("LandScapeMaterial");

		m_pMaterial->AddTextureSet(0, 0, 0, "LandScapeDiffuse"	,	TEXT("LandScape/BD_Terrain_Cave_01.dds"));
		m_pMaterial->AddTextureSet(0, 0, 1, "LandScapeNormal"	,	TEXT("LandScape/BD_Terrain_Cave_01_NRM.bmp"));
		m_pMaterial->AddTextureSet(0, 0, 2, "LandScapeSpecular"	,	TEXT("LandScape/BD_Terrain_Cave_01_SPC.bmp"));

		m_pMaterial->BumpTextureEnable(0, 0);
		m_pMaterial->SpecularTextureEnable(0, 0);


		return true;
	}



	fclose(pFile);

	return true;
}

void CLandScape::SetDetailLevel(float fLevelX, float fLevelZ)
{
	m_tCBuffer.fDetailLevelX = fLevelX;
	m_tCBuffer.fDetailLevelZ = fLevelZ;
}

void CLandScape::ChangeBaseTexture(const TCHAR * pDiffuseName, const TCHAR * pNormalName, const TCHAR * pSpecularName, const string & strPathName)
{
	m_pMaterial->ChangeTextureSet(0, 0, 0, "LandScapeDiffuse", "LandScapeDiffuse", pDiffuseName, strPathName);
	m_pMaterial->ChangeTextureSet(0, 0, 1, "LandScapeNormal", "LandScapeNormal", pNormalName, strPathName);
	m_pMaterial->ChangeTextureSet(0, 0, 2, "LandScapeSpecular", "LandScapeSpecular", pSpecularName, strPathName);
}

void CLandScape::AddSplatTexture(const TCHAR * pDiffuseName, const TCHAR * pNormalName, const TCHAR * pSpecularName, const TCHAR * pAlphaName, const string & strPathName)
{
	TCHAR*	pName = new TCHAR[MAX_PATH];
	memset(pName, 0, sizeof(TCHAR) * MAX_PATH);

	lstrcpy(pName, pDiffuseName);

	m_tSplatName.vecDifName.push_back(pName);

	pName = new TCHAR[MAX_PATH];
	memset(pName, 0, sizeof(TCHAR) * MAX_PATH);

	lstrcpy(pName, pNormalName);

	m_tSplatName.vecNrmName.push_back(pName);

	pName = new TCHAR[MAX_PATH];
	memset(pName, 0, sizeof(TCHAR) * MAX_PATH);

	lstrcpy(pName, pSpecularName);

	m_tSplatName.vecSpcName.push_back(pName);

	pName = new TCHAR[MAX_PATH];
	memset(pName, 0, sizeof(TCHAR) * MAX_PATH);

	lstrcpy(pName, pAlphaName);

	m_tSplatName.vecAlphaName.push_back(pName);

	m_tSplatName.vecPathName.push_back(strPathName);
}

void CLandScape::CreateSplatTexture()
{
	if (m_tSplatName.vecDifName.empty())
		return;

	SAFE_RELEASE(m_pSplatDifTex);
	SAFE_RELEASE(m_pSplatNrmTex);
	SAFE_RELEASE(m_pSplatSpcTex);
	SAFE_RELEASE(m_pSplatAlphaTex);

	m_pMaterial->ChangeTextureSetArray(0, 0, 10, "SplatDiffuse", "SplatDiffuse", &m_tSplatName.vecDifName, m_tSplatName.vecPathName[0]);
	m_pMaterial->ChangeTextureSetArray(0, 0, 11, "SplatNormal", "SplatNormal", &m_tSplatName.vecNrmName, m_tSplatName.vecPathName[0]);
	m_pMaterial->ChangeTextureSetArray(0, 0, 12, "SplatSpecular", "SplatSpecular", &m_tSplatName.vecSpcName, m_tSplatName.vecPathName[0]);
	m_pMaterial->ChangeTextureSetArray(0, 0, 13, "SplatAlpha", "SplatAlpha", &m_tSplatName.vecAlphaName, m_tSplatName.vecPathName[0]);

	m_tCBuffer.iSplatCount = (int)m_tSplatName.vecDifName.size();
}

void CLandScape::Start()
{
	SAFE_RELEASE(m_pMaterial);
	m_pMaterial = FindComponentFromType<CMaterial>(CT_MATERIAL);

	m_pObject->EraseComponentFromTag("PickSphere");
}

bool CLandScape::Init()
{
	return true;
}

int CLandScape::Input(float fTime)
{
	return 0;
}

int CLandScape::Update(float fTime)
{
	return 0;
}

int CLandScape::LateUpdate(float fTime)
{
	return 0;
}

int CLandScape::Collision(float fTime)
{
	return 0;
}

int CLandScape::PrevRender(float fTime)
{
	GET_SINGLE(CShaderManager)->UpdateCBuffer("LandScape", &m_tCBuffer);
	return 0;
}

int CLandScape::Render(float fTime)
{
	return 0;
}

CLandScape * CLandScape::Clone() const
{
	return new CLandScape(*this);
}

void CLandScape::ComputeNormal()
{
	for (size_t i = 0; i < m_vecFaceNormal.size(); ++i)
	{
		int	idx0 = m_vecIdx[i * 3];
		int	idx1 = m_vecIdx[i * 3 + 1];
		int	idx2 = m_vecIdx[i * 3 + 2];

		m_vecVtx[idx0].vNormal += m_vecFaceNormal[i];
		m_vecVtx[idx1].vNormal += m_vecFaceNormal[i];
		m_vecVtx[idx2].vNormal += m_vecFaceNormal[i];
	}

	for (size_t i = 0; i < m_vecVtx.size(); ++i)
	{
		m_vecVtx[i].vNormal.Normalize();
	}
}

void CLandScape::ComputeTangent()
{
	// 탄젠트 벡터 구함.
	for (size_t i = 0; i < m_vecFaceNormal.size(); ++i)
	{

		int	idx0 = m_vecIdx[i * 3];
		int	idx1 = m_vecIdx[i * 3 + 1];
		int	idx2 = m_vecIdx[i * 3 + 2];

		// 두 벡터의 x,y,z의 증가량을 저장한다.
		float	fVtx1[3], fVtx2[3];
		fVtx1[0] = m_vecVtx[idx1].vPos.x - m_vecVtx[idx0].vPos.x;
		fVtx1[1] = m_vecVtx[idx1].vPos.y - m_vecVtx[idx0].vPos.y;
		fVtx1[2] = m_vecVtx[idx1].vPos.z - m_vecVtx[idx0].vPos.z;

		fVtx2[0] = m_vecVtx[idx2].vPos.x - m_vecVtx[idx0].vPos.x;
		fVtx2[1] = m_vecVtx[idx2].vPos.y - m_vecVtx[idx0].vPos.y;
		fVtx2[2] = m_vecVtx[idx2].vPos.z - m_vecVtx[idx0].vPos.z;

		// 두 벡터의 UV증가량을 저장한다.
		float	ftu[2], ftv[2];
		ftu[0] = m_vecVtx[idx1].vUV.x - m_vecVtx[idx0].vUV.x;
		ftv[0] = m_vecVtx[idx1].vUV.y - m_vecVtx[idx0].vUV.y;

		ftu[1] = m_vecVtx[idx2].vUV.x - m_vecVtx[idx0].vUV.x;
		ftv[1] = m_vecVtx[idx2].vUV.y - m_vecVtx[idx0].vUV.y;

		// Tangent = deltaPosition / deltaU
		// BiNormal = deltaPosition  / deltaV

		// Vec1 = P1 - P0 , Vec2 = P2 - P0
		// Vec1 = (u1 - u0) * Tanget + (v1 - v0) * BiNormal
		// Vec2 = (u2 - u0) * Tanget + (v2 - v0) * BiNormal

		// ftu[0] = (u1 - u0) , ftv[0] = (v1 - v0) 
		// ftu[1] = (u2 - u0) , ftv[1] = (v2 - v0)  

		// ftu[0] ftv[0]     Tanget	    Q1
		// ftu[1] ftv[1]   * BiNormal = Q2

		// ftu[0] ftv[0]
		// ftu[1] ftv[1]
		// 행렬은 UV로 표기.

		// 왼쪽 행렬 UV에 역행렬을 곱해주는 것으로 Tanget, BiNormal만 남긴다.
		// 
		// T   = UV^-1	Q1
		// B			Q2

		// 역행렬을 구할때 필요한 1.f / ad-bc를 저장한다.
		float	fDen = 1.f / (ftu[0] * ftv[1] - ftu[1] * ftv[0]);

		// ftv[1] -ftv[0]
		// -ftu[1] ftu[0]


		// T.x = fDen * (ftu[0] * Q1.x) - (ftv[0] * Q2.x)
		// T.y = fDen * (ftu[0] * Q1.y) - (ftv[0] * Q2.y)
		// T.z = fDen * (ftu[0] * Q1.z) - (ftv[0] * Q2.z)

		// B.x = fDen * -(ftu[1] * Q1.x) + (ftu[0] * Q2.x)
		// B.y = fDen * -(ftu[1] * Q1.x) + (ftu[0] * Q2.x)
		// B.z = fDen * -(ftu[1] * Q1.x) + (ftu[0] * Q2.x)

		Vector3	vTangent;
		vTangent.x = (ftv[1] * fVtx1[0] - ftv[0] * fVtx2[0]) * fDen;
		vTangent.y = (ftv[1] * fVtx1[1] - ftv[0] * fVtx2[1]) * fDen;
		vTangent.z = (ftv[1] * fVtx1[2] - ftv[0] * fVtx2[2]) * fDen;

		vTangent.Normalize();

		m_vecVtx[idx0].vTangent = vTangent;
		m_vecVtx[idx1].vTangent = vTangent;
		m_vecVtx[idx2].vTangent = vTangent;

		m_vecVtx[idx0].vBinormal = m_vecVtx[idx0].vNormal.Cross(vTangent);
		m_vecVtx[idx1].vBinormal = m_vecVtx[idx1].vNormal.Cross(vTangent);
		m_vecVtx[idx2].vBinormal = m_vecVtx[idx2].vNormal.Cross(vTangent);

		m_vecVtx[idx0].vBinormal.Normalize();
		m_vecVtx[idx1].vBinormal.Normalize();
		m_vecVtx[idx2].vBinormal.Normalize();
	}
}
