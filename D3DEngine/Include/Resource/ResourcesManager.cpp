#include "ResourcesManager.h"
#include "Mesh.h"
#include "Texture.h"
#include "Sampler.h"
#include "../SerialNumber.h"
ENGINE_USING

DEFINITION_SINGLE(CResourcesManager)

CResourcesManager::CResourcesManager()
{
	m_pSerialNumber = new CSerialNumber;
	m_pTexSerialNumber = new CSerialNumber;
}

CResourcesManager::~CResourcesManager()
{
	SAFE_DELETE(m_pTexSerialNumber);
	SAFE_DELETE(m_pSerialNumber);
	Safe_Release_Map(m_mapSampler);
	Safe_Release_Map(m_mapTexture);
	Safe_Release_Map(m_mapMesh);
}

bool CResourcesManager::Init()
{
	// 구정보를 만든다.
	CreateSphere(1.f, 5);

	// 피라미드를 만들기 위한 5개의 위치정보를 만든다.
	Vector3	vPyramidPos[5]	=
	{
		Vector3(0.f, 0.5f, 0.f),
		Vector3(-0.5f, -0.5f, 0.5f),
		Vector3(0.5f, -0.5f, 0.5f),
		Vector3(-0.5f, -0.5f, -0.5f),
		Vector3(0.5f, -0.5f, -0.5f)
	};

	Vector3	vPyramidFaceNormal[4];
	Vector3	vEdge[2];

	// 왼쪽 삼각형
	vEdge[0] = vPyramidPos[3] - vPyramidPos[0];
	vEdge[1] = vPyramidPos[1] - vPyramidPos[0];
	vEdge[0].Normalize();
	vEdge[1].Normalize();
	vPyramidFaceNormal[0] = vEdge[0].Cross(vEdge[1]);
	vPyramidFaceNormal[0].Normalize();

	// 바깥쪽 삼각형
	vEdge[0] = vPyramidPos[1] - vPyramidPos[0];
	vEdge[1] = vPyramidPos[2] - vPyramidPos[0];
	vEdge[0].Normalize();
	vEdge[1].Normalize();
	vPyramidFaceNormal[1] = vEdge[0].Cross(vEdge[1]);
	vPyramidFaceNormal[1].Normalize();

	// 오른쪽 삼각형
	vEdge[0] = vPyramidPos[2] - vPyramidPos[0];
	vEdge[1] = vPyramidPos[4] - vPyramidPos[0];
	vEdge[0].Normalize();
	vEdge[1].Normalize();
	vPyramidFaceNormal[2] = vEdge[0].Cross(vEdge[1]);
	vPyramidFaceNormal[2].Normalize();

	// 안쪽 삼각형
	vEdge[0] = vPyramidPos[4] - vPyramidPos[0];
	vEdge[1] = vPyramidPos[3] - vPyramidPos[0];
	vEdge[0].Normalize();
	vEdge[1].Normalize();
	vPyramidFaceNormal[3] = vEdge[0].Cross(vEdge[1]);
	vPyramidFaceNormal[3].Normalize();

	Vector3	vPyramidNormal[4];

	vPyramidNormal[0] = vPyramidFaceNormal[0] + vPyramidFaceNormal[1];
	vPyramidNormal[0].Normalize();

	vPyramidNormal[1] = vPyramidFaceNormal[2] + vPyramidFaceNormal[1];
	vPyramidNormal[1].Normalize();

	vPyramidNormal[2] = vPyramidFaceNormal[0] + vPyramidFaceNormal[3];
	vPyramidNormal[2].Normalize();

	vPyramidNormal[3] = vPyramidFaceNormal[2] + vPyramidFaceNormal[3];
	vPyramidNormal[3].Normalize();

	VertexNormalColor	tPyramid[9]	=
	{
		VertexNormalColor(vPyramidPos[0], Vector3(0.f, 1.f, 0.f), Vector4::Green),
		VertexNormalColor(vPyramidPos[1], vPyramidNormal[0], Vector4::Red),
		VertexNormalColor(vPyramidPos[2], vPyramidNormal[1], Vector4::Blue),
		VertexNormalColor(vPyramidPos[3], vPyramidNormal[2], Vector4::Yellow),
		VertexNormalColor(vPyramidPos[4], vPyramidNormal[3], Vector4::Magenta),
		VertexNormalColor(vPyramidPos[1], Vector3(0.f, -1.f, 0.f), Vector4::Red),
		VertexNormalColor(vPyramidPos[2], Vector3(0.f, -1.f, 0.f), Vector4::Blue),
		VertexNormalColor(vPyramidPos[3], Vector3(0.f, -1.f, 0.f), Vector4::Yellow),
		VertexNormalColor(vPyramidPos[4], Vector3(0.f, -1.f, 0.f), Vector4::Magenta)
	};

	int	iIdx[18] = { 0, 3, 1, 0, 1, 2, 0, 2, 4, 0, 4, 3, 7, 8, 6, 7, 6, 5 };

	CreateMesh("Pyramid", STANDARD_NORMAL_COLOR_SHADER,
		POS_NORMAL_COLOR_LAYOUT, Vector3::Axis[AXIS_Y], 
		sizeof(VertexNormalColor), 9, D3D11_USAGE_DEFAULT,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, tPyramid,
		4, 18, D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32_UINT, iIdx);

	VertexUV	tRectangle[4]	=
	{
		VertexUV(Vector3(0.f, 1.f, 0.f), Vector2(0.f, 0.f)),
		VertexUV(Vector3(1.f, 1.f, 0.f), Vector2(1.f, 0.f)),
		VertexUV(Vector3(0.f, 0.f, 0.f), Vector2(0.f, 1.f)),
		VertexUV(Vector3(1.f, 0.f, 0.f), Vector2(1.f, 1.f)),
	};

	int	iRectangleIdx[6] = { 0, 1, 3, 0, 3, 2 };

	CreateMesh("Rectangle", DEBUG_SHADER,
		POS_UV_LAYOUT, Vector3::Axis[AXIS_Y],
		sizeof(VertexUV), 4, D3D11_USAGE_DEFAULT,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, tRectangle,
		4, 6, D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32_UINT, iRectangleIdx);

	// 육면체 충돌체 메쉬
	Vector3	vColliderBox[8] =
	{
		Vector3(-1.f, 1.f, -1.f),
		Vector3(1.f, 1.f, -1.f),
		Vector3(-1.f, -1.f, -1.f),
		Vector3(1.f, -1.f, -1.f),
		Vector3(-1.f, 1.f, 1.f),
		Vector3(1.f, 1.f, 1.f),
		Vector3(-1.f, -1.f, 1.f),
		Vector3(1.f, -1.f, 1.f)
	};

	int	iBoxIdx[24] = { 0, 1, 0, 2, 1, 3, 2, 3, 4, 0, 5, 1, 6, 2, 7, 3, 4, 5, 5, 7, 6, 7, 4, 6 };

	CreateMesh("ColliderBox", COLLIDER_SHADER,
		POS_LAYOUT, Vector3::Axis[AXIS_Y],
		sizeof(Vector3), 8, D3D11_USAGE_DEFAULT,
		D3D11_PRIMITIVE_TOPOLOGY_LINELIST, vColliderBox,
		4, 24, D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32_UINT, iBoxIdx);

	Vector3	vParticleVtx = {};

	CreateMesh("Particle", PARTICLE_SHADER,
		POS_LAYOUT, Vector3::Axis[AXIS_Y],
		sizeof(Vector3), 1, D3D11_USAGE_DEFAULT,
		D3D11_PRIMITIVE_TOPOLOGY_POINTLIST, &vParticleVtx);

	CreateSpherePos("Sky", "SkyShader", POS_LAYOUT);

	CreateSampler(SAMPLER_LINEAR);
	CreateSampler(SAMPLER_POINT, D3D11_FILTER_MIN_MAG_MIP_POINT);

	CSampler*	pSampler = FindSampler(SAMPLER_LINEAR);

	pSampler->VSSetShader(0);
	pSampler->PSSetShader(0);

	SAFE_RELEASE(pSampler);
	
	pSampler = FindSampler(SAMPLER_POINT);

	pSampler->VSSetShader(1);
	pSampler->PSSetShader(1);

	SAFE_RELEASE(pSampler);

	return true;
}

bool CResourcesManager::CreateMesh(const string & strName, const string& strShaderKey,
	const string& strInputLayoutKey, const Vector3& vView, int iVtxSize, int iVtxCount,
	D3D11_USAGE eVtxUsage, D3D11_PRIMITIVE_TOPOLOGY ePrimitive, void * pVtx,
	int iIdxSize, int iIdxCount, D3D11_USAGE eIdxUsage, DXGI_FORMAT eFmt, void * pIdx)
{
	CMesh*	pMesh = FindMesh(strName);

	if (pMesh)
	{
		SAFE_RELEASE(pMesh);
		return false;
	}

	pMesh = new CMesh;

	if (!pMesh->CreateMesh(strName, strShaderKey, strInputLayoutKey,
		iVtxSize, iVtxCount, eVtxUsage, ePrimitive,
		pVtx, iIdxSize, iIdxCount, eIdxUsage, eFmt, pIdx))
	{
		SAFE_RELEASE(pMesh);
		return false;
	}

	pMesh->SetView(vView);
	pMesh->SetSerialNumber(m_pSerialNumber->GetSerialNumber());

	m_mapMesh.insert(make_pair(strName, pMesh));

	return true;
}

bool CResourcesManager::CreateSpherePos(const string & strName, 
	const string& strShaderKey,
	const string& strInputLayoutKey)
{
	CMesh*	pMesh = FindMesh(strName);

	if (pMesh)
	{
		SAFE_RELEASE(pMesh);
		return false;
	}

	pMesh = new CMesh;

	vector<Vector3>	vecPos;
	vecPos.resize(m_vecSphereVtx.size());

	for (size_t i = 0; i < m_vecSphereVtx.size(); ++i)
	{
		vecPos[i] = m_vecSphereVtx[i].vPos;
	}

	if (!pMesh->CreateMesh(strName, strShaderKey, strInputLayoutKey,
		12, (int)vecPos.size(), D3D11_USAGE_DEFAULT, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		&vecPos[0], 4, (int)m_vecSphereIdx.size(), D3D11_USAGE_DEFAULT, 
		DXGI_FORMAT_R32_UINT, &m_vecSphereIdx[0]))
	{
		SAFE_RELEASE(pMesh);
		return false;
	}

	pMesh->SetView(Vector3(0.f, 0.f, 1.f));
	pMesh->SetSerialNumber(m_pSerialNumber->GetSerialNumber());
	m_mapMesh.insert(make_pair(strName, pMesh));

	return true;
}

bool CResourcesManager::LoadMesh(const string & strName, 
	const TCHAR * pFileName,
	const Vector3& vView, const string & strPathName)
{
	CMesh*	pMesh = FindMesh(strName);

	if (pMesh)
	{
		SAFE_RELEASE(pMesh);
		return false;
	}

	pMesh = new CMesh;

	if (!pMesh->LoadMesh(strName, pFileName, vView, strPathName))
	{
		SAFE_RELEASE(pMesh);
		return false;
	}

	//pMesh->SetView(vView);
	pMesh->SetSerialNumber(m_pSerialNumber->GetSerialNumber());
	m_mapMesh.insert(make_pair(strName, pMesh));

	return true;
}

bool CResourcesManager::LoadMesh(const string & strName, 
	const char * pFileName,
	const Vector3& vView, const string & strPathName)
{
	CMesh*	pMesh = FindMesh(strName);

	if (pMesh)
	{
		SAFE_RELEASE(pMesh);
		return false;
	}

	pMesh = new CMesh;

	if (!pMesh->LoadMesh(strName, pFileName, vView, strPathName))
	{
		SAFE_RELEASE(pMesh);
		return false;
	}

	//pMesh->SetView(vView);
	pMesh->SetSerialNumber(m_pSerialNumber->GetSerialNumber());
	m_mapMesh.insert(make_pair(strName, pMesh));

	return true;
}

bool CResourcesManager::LoadMeshFromFullPath(
	const string & strName, const TCHAR * pFullPath,
	const Vector3& vView)
{
	CMesh*	pMesh = FindMesh(strName);

	if (pMesh)
	{
		SAFE_RELEASE(pMesh);
		return false;
	}

	pMesh = new CMesh;

	if (!pMesh->LoadMeshFromFullPath(strName, pFullPath,
		vView))
	{
		SAFE_RELEASE(pMesh);
		return false;
	}

	//pMesh->SetView(vView);
	pMesh->SetSerialNumber(m_pSerialNumber->GetSerialNumber());
	m_mapMesh.insert(make_pair(strName, pMesh));

	return true;
}

bool CResourcesManager::LoadMeshFromFullPath(
	const string & strName, const char * pFullPath,
	const Vector3& vView)
{
	CMesh*	pMesh = FindMesh(strName);

	if (pMesh)
	{
		SAFE_RELEASE(pMesh);
		return false;
	}

	pMesh = new CMesh;

	if (!pMesh->LoadMeshFromFullPath(strName, pFullPath,
		vView))
	{
		SAFE_RELEASE(pMesh);
		return false;
	}

	//pMesh->SetView(vView);
	pMesh->SetSerialNumber(m_pSerialNumber->GetSerialNumber());
	m_mapMesh.insert(make_pair(strName, pMesh));

	return true;
}

bool CResourcesManager::DeleteMesh(const string & strName)
{
	unordered_map<string, CMesh*>::iterator	iter = m_mapMesh.find(strName);

	if (iter == m_mapMesh.end())
		return false;

	m_pSerialNumber->AddValidNumber(iter->second->GetSerialNumber());
	SAFE_RELEASE(iter->second);

	m_mapMesh.erase(iter);

	return true;
}

CMesh * CResourcesManager::FindMesh(const string & strName)
{
	unordered_map<string, CMesh*>::iterator	iter = m_mapMesh.find(strName);

	if (iter == m_mapMesh.end())
		return nullptr;

	iter->second->AddRef();

	return iter->second;
}

bool CResourcesManager::LoadTexture(const string & strName, const TCHAR * pFileName, 
	const string & strPathKey)
{
	CTexture*	pTexture = FindTexture(strName);

	if (pTexture)
	{
		SAFE_RELEASE(pTexture);
		return false;
	}

	pTexture = new CTexture;

	if (!pTexture->LoadTexture(strName, pFileName, strPathKey))
	{
		SAFE_RELEASE(pTexture);
		return false;
	}
	pTexture->SetSerialNumber(m_pTexSerialNumber->GetSerialNumber());
	m_mapTexture.insert(make_pair(strName, pTexture));

	return true;
}

bool CResourcesManager::LoadTextureFromFullPath(const string & strName, const TCHAR * pFullPath)
{
	CTexture*	pTexture = FindTexture(strName);

	if (pTexture)
	{
		SAFE_RELEASE(pTexture);
		return false;
	}

	pTexture = new CTexture;

	if (!pTexture->LoadTextureFromFullPath(strName, 
		pFullPath))
	{
		SAFE_RELEASE(pTexture);
		return false;
	}
	pTexture->SetSerialNumber(m_pTexSerialNumber->GetSerialNumber());
	m_mapTexture.insert(make_pair(strName, pTexture));

	return true;
}

bool CResourcesManager::LoadTexture(const string & strName,
	const vector<TCHAR*>& vecFileName, const string & strPathKey)
{
	CTexture*	pTexture = FindTexture(strName);

	if (pTexture)
	{
		SAFE_RELEASE(pTexture);
		return false;
	}

	pTexture = new CTexture;

	if (!pTexture->LoadTexture(strName, vecFileName, strPathKey))
	{
		SAFE_RELEASE(pTexture);
		return false;
	}
	pTexture->SetSerialNumber(m_pTexSerialNumber->GetSerialNumber());
	m_mapTexture.insert(make_pair(strName, pTexture));

	return true;
}

bool CResourcesManager::LoadTextureFromFullPath(
	const string & strName, const vector<TCHAR*>& vecFullPath)
{
	CTexture*	pTexture = FindTexture(strName);

	if (pTexture)
	{
		SAFE_RELEASE(pTexture);
		return false;
	}

	pTexture = new CTexture;

	if (!pTexture->LoadTextureFromFullPath(strName,
		vecFullPath))
	{
		SAFE_RELEASE(pTexture);
		return false;
	}
	pTexture->SetSerialNumber(m_pTexSerialNumber->GetSerialNumber());
	m_mapTexture.insert(make_pair(strName, pTexture));

	return true;
}

bool CResourcesManager::DeleteTexture(const string & strName)
{
	CTexture*	pTexture = FindTexture(strName);

	if (!pTexture)
		return false;

	if (pTexture->m_iRefCount == 1)
	{
		m_pTexSerialNumber->AddValidNumber(pTexture->GetSerialNumber());
		m_mapTexture.erase(strName);
	}

	SAFE_RELEASE(pTexture);

	return true;
}

CTexture * CResourcesManager::FindTexture(const string & strName)
{
	unordered_map<string, CTexture*>::iterator	iter = m_mapTexture.find(strName);
	
	if (iter == m_mapTexture.end())
		return nullptr;

	iter->second->AddRef();

	return iter->second;
}

bool CResourcesManager::CreateSampler(const string & strName,
	D3D11_FILTER eFilter, D3D11_TEXTURE_ADDRESS_MODE eAddrU,
	D3D11_TEXTURE_ADDRESS_MODE eAddrV, D3D11_TEXTURE_ADDRESS_MODE eAddrW)
{
	CSampler*	pSampler = FindSampler(strName);

	if (pSampler)
	{
		SAFE_RELEASE(pSampler);
		return false;
	}

	pSampler = new CSampler;

	if (!pSampler->CreateSampler(strName, eFilter, eAddrU, eAddrV, eAddrW))
	{
		SAFE_RELEASE(pSampler);
		return false;
	}

	m_mapSampler.insert(make_pair(strName, pSampler));

	return true;
}

CSampler * CResourcesManager::FindSampler(const string & strName)
{
	unordered_map<string, CSampler*>::iterator	iter = m_mapSampler.find(strName);

	if (iter == m_mapSampler.end())
		return nullptr;

	iter->second->AddRef();

	return iter->second;
}

bool CResourcesManager::CreateSphere(float fRadius, unsigned int iSubDivision)
{
	// Put a cap on the number of subdivisions.
	iSubDivision = min(iSubDivision, 5u);

	// Approximate a sphere by tessellating an icosahedron.
	const float X = 0.525731f;
	const float Z = 0.850651f;

	Vector3 pos[12] =
	{
		Vector3(-X, 0.0f, Z),  Vector3(X, 0.0f, Z),
		Vector3(-X, 0.0f, -Z), Vector3(X, 0.0f, -Z),
		Vector3(0.0f, Z, X),   Vector3(0.0f, Z, -X),
		Vector3(0.0f, -Z, X),  Vector3(0.0f, -Z, -X),
		Vector3(Z, X, 0.0f),   Vector3(-Z, X, 0.0f),
		Vector3(Z, -X, 0.0f),  Vector3(-Z, -X, 0.0f)
	};

	DWORD k[60] =
	{
		1,4,0,  4,9,0,  4,5,9,  8,5,4,  1,8,4,
		1,10,8, 10,3,8, 8,3,5,  3,2,5,  3,7,2,
		3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
		10,1,6, 11,0,9, 2,11,9, 5,2,9,  11,2,7
	};

	m_vecSphereVtx.resize(12);
	m_vecSphereIdx.resize(60);

	for (UINT i = 0; i < 12; ++i)
		m_vecSphereVtx[i].vPos = pos[i];

	for (UINT i = 0; i < 60; ++i)
		m_vecSphereIdx[i] = k[i];

	for (UINT i = 0; i < iSubDivision; ++i)
		Subdivide();

	// Project vertices onto sphere and scale.
	for (UINT i = 0; i < m_vecSphereVtx.size(); ++i)
	{
		// Project onto unit sphere.
		Vector3	vN = m_vecSphereVtx[i].vPos;
		vN.Normalize();

		// Project onto sphere.
		Vector3 p = vN * fRadius;

		m_vecSphereVtx[i].vPos = p;
		// Normal이 있을 경우 따로 저장한다.
		m_vecSphereVtx[i].vNormal = vN;

		// Derive texture coordinates from spherical coordinates.
		float theta = AngleFromXY(
			m_vecSphereVtx[i].vPos.x,
			m_vecSphereVtx[i].vPos.z);

		float phi = acosf(m_vecSphereVtx[i].vPos.y / fRadius);

		m_vecSphereVtx[i].vUV.x = theta / XM_2PI;
		m_vecSphereVtx[i].vUV.y = phi / XM_PI;

		// Partial derivative of P with respect to theta
		m_vecSphereVtx[i].vTangent.x = -fRadius*sinf(phi)*sinf(theta);
		m_vecSphereVtx[i].vTangent.y = 0.0f;
		m_vecSphereVtx[i].vTangent.z = +fRadius*sinf(phi)*cosf(theta);

		m_vecSphereVtx[i].vBinormal = m_vecSphereVtx[i].vNormal.Cross(m_vecSphereVtx[i].vTangent);

		//XMVECTOR T = XMLoadFloat3(&vecVertices[i].TangentU);
		//XMStoreFloat3(&meshData.Vertices[i].TangentU, XMVector3Normalize(T));
	}

	return true;
}

void CResourcesManager::Subdivide()
{
	// Save a copy of the input geometry.
	vector<Vertex3D>	vecCopyVertex = m_vecSphereVtx;
	vector<UINT>	vecCopyIndex = m_vecSphereIdx;


	m_vecSphereVtx.resize(0);
	m_vecSphereIdx.resize(0);

	//       v1
	//       *
	//      / \
			//     /   \
	//  m0*-----*m1
//   / \   / \
	//  /   \ /   \
	// *-----*-----*
// v0    m2     v2

	UINT numTris = (UINT)vecCopyIndex.size() / 3;
	for (UINT i = 0; i < numTris; ++i)
	{
		Vertex3D v0 = vecCopyVertex[vecCopyIndex[i * 3 + 0]];
		Vertex3D v1 = vecCopyVertex[vecCopyIndex[i * 3 + 1]];
		Vertex3D v2 = vecCopyVertex[vecCopyIndex[i * 3 + 2]];

		//
		// Generate the midpoints.
		//

		Vertex3D m0, m1, m2;

		// For subdivision, we just care about the position component.  We derive the other
		// vertex components in CreateGeosphere.

		m0.vPos = Vector3(
			0.5f*(v0.vPos.x + v1.vPos.x),
			0.5f*(v0.vPos.y + v1.vPos.y),
			0.5f*(v0.vPos.z + v1.vPos.z));

		m1.vPos = Vector3(
			0.5f*(v1.vPos.x + v2.vPos.x),
			0.5f*(v1.vPos.y + v2.vPos.y),
			0.5f*(v1.vPos.z + v2.vPos.z));

		m2.vPos = Vector3(
			0.5f*(v0.vPos.x + v2.vPos.x),
			0.5f*(v0.vPos.y + v2.vPos.y),
			0.5f*(v0.vPos.z + v2.vPos.z));

		//
		// Add new geometry.
		//

		m_vecSphereVtx.push_back(v0); // 0
		m_vecSphereVtx.push_back(v1); // 1
		m_vecSphereVtx.push_back(v2); // 2
		m_vecSphereVtx.push_back(m0); // 3
		m_vecSphereVtx.push_back(m1); // 4
		m_vecSphereVtx.push_back(m2); // 5

		m_vecSphereIdx.push_back(i * 6 + 0);
		m_vecSphereIdx.push_back(i * 6 + 3);
		m_vecSphereIdx.push_back(i * 6 + 5);

		m_vecSphereIdx.push_back(i * 6 + 3);
		m_vecSphereIdx.push_back(i * 6 + 4);
		m_vecSphereIdx.push_back(i * 6 + 5);

		m_vecSphereIdx.push_back(i * 6 + 5);
		m_vecSphereIdx.push_back(i * 6 + 4);
		m_vecSphereIdx.push_back(i * 6 + 2);

		m_vecSphereIdx.push_back(i * 6 + 3);
		m_vecSphereIdx.push_back(i * 6 + 1);
		m_vecSphereIdx.push_back(i * 6 + 4);
	}
}

float CResourcesManager::AngleFromXY(float x, float y)
{
	float theta = 0.0f;

	// Quadrant I or IV
	if (x >= 0.0f)
	{
		// If x = 0, then atanf(y/x) = +pi/2 if y > 0
		//                atanf(y/x) = -pi/2 if y < 0
		theta = atanf(y / x); // in [-pi/2, +pi/2]

		if (theta < 0.0f)
			theta += 2.0f * ENGINE_PI; // in [0, 2*pi).
	}

	// Quadrant II or III
	else
		theta = atanf(y / x) + ENGINE_PI; // in [0, 2*pi).

	return theta;
}
