#pragma once

#include "../Engine.h"

ENGINE_BEGIN

class ENGINE_DLL CResourcesManager
{
private:
	unordered_map<string, class CMesh*>	m_mapMesh;
	unordered_map<string, class CTexture*>	m_mapTexture;
	unordered_map<string, class CSampler*>	m_mapSampler;
	// 구 정점정보를 저장할 버퍼
	vector<Vertex3D>		m_vecSphereVtx;
	vector<UINT>			m_vecSphereIdx;

	class CSerialNumber*	m_pSerialNumber;
	class CSerialNumber*	m_pTexSerialNumber;

public:
	bool Init();

	bool CreateMesh(const string& strName, const string& strShaderKey,
		const string& strInputLayoutKey, const Vector3& vView, int iVtxSize, int iVtxCount,
		D3D11_USAGE eVtxUsage, D3D11_PRIMITIVE_TOPOLOGY ePrimitive,
		void* pVtx, int iIdxSize = 0, int iIdxCount = 0,
		D3D11_USAGE eIdxUsage = D3D11_USAGE_DEFAULT,
		DXGI_FORMAT eFmt = DXGI_FORMAT_UNKNOWN,
		void* pIdx = nullptr);
	bool CreateSpherePos(const string& strName, const string& strShaderKey,	const string& strInputLayoutKey);
	bool LoadMesh(const string& strName, const TCHAR* pFileName, const Vector3& vView = Vector3::Axis[AXIS_Z], const string& strPathName = MESH_PATH);
	bool LoadMesh(const string& strName, const char* pFileName, const Vector3& vView = Vector3::Axis[AXIS_Z],  const string& strPathName = MESH_PATH);
	bool LoadMeshFromFullPath(const string& strName, const TCHAR* pFullPath, const Vector3& vView = Vector3::Axis[AXIS_Z]);
	bool LoadMeshFromFullPath(const string& strName, const char* pFullPath,	const Vector3& vView = Vector3::Axis[AXIS_Z]);
	bool DeleteMesh(const string& strName);

	class CMesh* FindMesh(const string& strName);

	bool LoadTexture(const string& strName, const TCHAR* pFileName, const string& strPathKey);
	bool LoadTextureFromFullPath(const string& strName, const TCHAR* pFullPath);
	bool LoadTexture(const string& strName, const vector<TCHAR*>& vecFileName, const string& strPathKey);
	bool LoadTextureFromFullPath(const string& strName, const vector<TCHAR*>& vecFullPath);
	bool DeleteTexture(const string& strName);
	class CTexture* FindTexture(const string& strName);

	bool CreateSampler(const string& strName, D3D11_FILTER eFilter = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		D3D11_TEXTURE_ADDRESS_MODE eAddrU = D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_MODE eAddrV = D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_MODE eAddrW = D3D11_TEXTURE_ADDRESS_WRAP);
	class CSampler* FindSampler(const string& strName);

private:
	bool CreateSphere(float fRadius, unsigned int iSubDivision);
	void Subdivide();
	float AngleFromXY(float x, float y);

	DECLARE_SINGLE(CResourcesManager)
};

ENGINE_END
