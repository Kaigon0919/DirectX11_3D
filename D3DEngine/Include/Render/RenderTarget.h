#pragma once

#include "../Engine.h"

ENGINE_BEGIN

class ENGINE_DLL CRenderTarget
{
	friend class CRenderManager;

private:
	CRenderTarget();
	~CRenderTarget();

private:
	string						m_strName;
	float						m_fClearColor[4];
	ID3D11RenderTargetView*		m_pTargetView;
	ID3D11Texture2D*			m_pTargetTex;
	ID3D11ShaderResourceView*	m_pTargetSRV;
	ID3D11DepthStencilView*		m_pDepthView;
	ID3D11Texture2D*			m_pDepthTex;

private:
	ID3D11RenderTargetView*	m_pOldTargetView;
	ID3D11DepthStencilView*	m_pOldDepthView;
	
public:
	ID3D11RenderTargetView* GetRenderTargetView()	const;
	ID3D11DepthStencilView* GetDepthStencilView()	const;

private:
	Vector3			m_vPos;
	Vector3			m_vScale;
	bool			m_bDebug;
	Matrix*			m_pProj;
	class CMesh*	m_pMesh;
	class CShader*	m_pShader;
	ID3D11InputLayout*	m_pLayout;

public:
	bool CreateRenderTarget(const string& strName,
		UINT iWidth, UINT iHeight, DXGI_FORMAT eFmt,
		float fClearColor[4], int iSampleCount = 1,
		DXGI_FORMAT eDepthFmt = DXGI_FORMAT_UNKNOWN);
	void OnDebug(const Vector3& vPos, const Vector3& vScale,
		bool bDebug = true);
	void ClearTarget();
	void SetTarget();
	void ResetTarget();
	void SetShader(int iRegister);
	void ResetShader(int iRegister);
	void Render();
};

ENGINE_END
