#pragma once

#include "../Ref.h"

ENGINE_BEGIN

typedef struct ENGINE_DLL _tagVertexBuffer
{
	ID3D11Buffer*	pBuffer;
	void*			pData;
	int				iSize;
	int				iCount;
	D3D11_USAGE		eUsage;
	D3D11_PRIMITIVE_TOPOLOGY	ePrimitive;
}VertexBuffer, *PVertexBuffer;

typedef struct ENGINE_DLL _tagIndexBuffer
{
	ID3D11Buffer*	pBuffer;
	void*			pData;
	int				iSize;
	int				iCount;
	D3D11_USAGE		eUsage;
	DXGI_FORMAT		eFmt;
}IndexBuffer, *PIndexBuffer;

typedef struct ENGINE_DLL _tagMeshContainer
{
	VertexBuffer	tVB;
	vector<PIndexBuffer>	vecIB;
}MeshContainer, *PMeshContainer;

class ENGINE_DLL CMesh	:
	public CRef
{
	friend class CResourcesManager;

private:
	CMesh();
	~CMesh();

private:
	vector<PMeshContainer>	m_vecMeshContainer;
	string	m_strShaderKey;
	string	m_strInputLayoutKey;
	Vector3	m_vView;
	Vector3	m_vMin;
	Vector3	m_vMax;
	Vector3	m_vLength;
	Vector3	m_vCenter;
	float	m_fRadius;
	class CMaterial*	m_pMaterial;
	class CAnimation*	m_pAnimation;

public:
	string GetShaderKey()	const;
	string GetInputLayoutKey()	const;
	size_t GetContainerCount()	const;
	size_t GetSubsetCount(int iContainer = 0)	const;
	Vector3 GetView()	const;
	Vector3 GetCenter()	const;
	Vector3 GetMin()	const;
	Vector3 GetMax()	const;
	float GetRadius()	const;

public:
	void SetView(const Vector3& vView);

public:
	bool CreateMesh(const string& strName, const string& strShaderKey,
		const string& strInputLayoutKey, int iVtxSize, int iVtxCount,
		D3D11_USAGE eVtxUsage, D3D11_PRIMITIVE_TOPOLOGY ePrimitive,
		void* pVtx, int iIdxSize = 0, int iIdxCount = 0,
		D3D11_USAGE eIdxUsage = D3D11_USAGE_DEFAULT, 
		DXGI_FORMAT eFmt = DXGI_FORMAT_UNKNOWN,
		void* pIdx = nullptr);
	bool LoadMesh(const string& strName, const TCHAR* pFileName,
		const Vector3& vView = Vector3::Axis[AXIS_Z], 
		const string& strPathName = MESH_PATH);
	bool LoadMesh(const string& strName, const char* pFileName,
		const Vector3& vView = Vector3::Axis[AXIS_Z],
		const string& strPathName = MESH_PATH);
	bool LoadMeshFromFullPath(const string& strName,
		const TCHAR* pFullPath,
		const Vector3& vView = Vector3::Axis[AXIS_Z]);
	bool LoadMeshFromFullPath(const string& strName,
		const char* pFullPath,
		const Vector3& vView = Vector3::Axis[AXIS_Z]);
	void Render();
	void Render(int iContainer, int iSubset = 0);
	void RenderInstancing(unsigned int iContainer, unsigned int iSubset, PInstancingBuffer pInstancingBuffer, int iInstancingCount);
private:
	bool CreateVertexBuffer(int iSize, int iCount, D3D11_USAGE eUsage,
		D3D11_PRIMITIVE_TOPOLOGY ePrimitive, void* pData);
	bool CreateIndexBuffer(int iSize, int iCount, D3D11_USAGE eUsage,
		DXGI_FORMAT eFmt, void* pData);

private:
	bool ConvertFbx(class CFbxLoader* pLoader,
		const char* pFullPath);

public:
	bool Save(const char* pFileName,
		const string& strPathKey = MESH_PATH);
	bool SaveFromFullPath(const char* pFullPath);
	bool Load(const char* pFileName,
		const string& strPathKey = MESH_PATH);
	bool LoadFromFullPath(const char* pFullPath);
	class CMaterial* CloneMaterial();
	class CAnimation* CloneAnimation();
};

ENGINE_END
