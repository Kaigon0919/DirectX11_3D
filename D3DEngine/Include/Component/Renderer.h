#pragma once
#include "Component.h"

ENGINE_BEGIN

class ENGINE_DLL CRenderer :
	public CComponent
{
	friend class CGameObject;

private:
	CRenderer();
	CRenderer(const CRenderer& com);
	~CRenderer();

private:
	class CShader*	m_pShader;
	class CMesh*	m_pMesh;
	ID3D11InputLayout*	m_pInputLayout;
	class CMaterial*	m_pMaterial;
	class CRenderState*	m_pRenderState[RS_END];
public:
	class CMesh* GetMesh()const;
public:
	void SetMesh(const string& strKey);
	void SetMesh(const string& strKey, const TCHAR* pFileName, const Vector3& vView = Vector3::Axis[AXIS_Z], const string& strPathKey = MESH_PATH);
	void SetMeshFromFullPath(const string& strKey, const TCHAR* pFileName, const Vector3& vView = Vector3::Axis[AXIS_Z]);
	void SetShader(const string& strKey);
	void SetInputLayout(const string& strKey);
	void SetRenderState(const string& strName);

public:
	virtual void Start();
	virtual bool Init();
	virtual int Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual int Collision(float fTime);
	virtual int PrevRender(float fTime);
	virtual int Render(float fTime);
	virtual CRenderer* Clone()	const;

public:
	void RenderInstancing(PInstancingBuffer pBuffer, class CShader* pShader, ID3D11InputLayout* pLayout, int iInstancingCount, float fTime);
};

ENGINE_END
