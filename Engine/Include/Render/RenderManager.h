#pragma once

#include "../GameObject.h"

ENGINE_BEGIN

#define INSTANCING_BUFFER_SIZE	1024
#define INSTANCING_COUNT		5

typedef struct ENGINE_DLL _tagRenderList
{
	CGameObject**	pObjList;
	int		iSize;
	int		iCapacity;

	_tagRenderList() 
	{
		iSize = 0;
		iCapacity = 100;
		pObjList = new CGameObject*[iCapacity];
	}

	~_tagRenderList()
	{
		SAFE_DELETE_ARRAY(pObjList);
	}
}RenderList, *PRenderList;

typedef struct ENGINE_DLL _tagMRT
{
	vector<class CRenderTarget*>	vecTarget;
	vector<ID3D11RenderTargetView*>	vecOldTarget;
	ID3D11DepthStencilView*			pDepth;
	ID3D11DepthStencilView*			pOldDepth;

	_tagMRT() :
		pDepth(nullptr),
		pOldDepth(nullptr)
	{
	}
}MRT, *PMRT;

class ENGINE_DLL CRenderManager
{
private:
	unordered_map<string, class CRenderState*>	m_mapRenderState;
	unordered_map<string, class CRenderTarget*>	m_mapRenderTarget;
	unordered_map<string, PMRT>					m_mapMRT;
	RenderList			m_tLightList;
	RenderList			m_tRenderGroup[RG_END];
	RENDER_MODE			m_eMode;
	RenderCBuffer		m_tRenderCBuffer;
	class CRenderState*	m_pDepthDisable;
	class CRenderState*	m_pAccBlend;
	class CRenderState*	m_pAlphaBlend;
	class CRenderState*	m_pCullNone;
	class CShader*		m_pLightAccDirShader;
	class CShader*		m_pLightAccPointShader;
	class CShader*		m_pLightAccSpotShader;
	class CShader*		m_pLightBlendShader;
	class CShader*		m_pLightBlendRenderShader;
	class CMesh*		m_pLightPointVolume;
	ID3D11InputLayout*	m_pLightPointLayout;
private:
	unordered_map<unsigned __int64, PInstancingGeometry>	m_mapInstancingGemoetry;
	list<PInstancingGeometry>	m_InstancingList[RG_END];
	PInstancingBuffer			m_pStaticInstancing;
	PInstancingBuffer			m_pAnimFrameInstancing;
	PInstancingBuffer			m_pAnimInstancing;
	PInstancingBuffer			m_pColliderInstancing;
	PInstancingBuffer			m_pLightInstancing;
	class CShader*				m_pStaticInstancingShader;
	class CShader*				m_pAnimFrameInstancingShader;
	class CShader*				m_pAnimInstancingShader;
	ID3D11InputLayout*			m_pStaticInstancingLayout;
	ID3D11InputLayout*			m_pAnimFrameInstancingLayout;
	ID3D11InputLayout*			m_pAnimInstancingLayout;

private:
	PInstancingGeometry FindInstancingGeometry(unsigned __int64 iKey);
	PInstancingBuffer CreateInstancingBuffer(int iSize,	int iCount = INSTANCING_BUFFER_SIZE);
	void ResizeInstancingBuffer(PInstancingBuffer pBuffer, int iCount);
	void AddInstancingData(PInstancingBuffer pBuffer, int iPos, void* pData);
	void CopyInstancingData(PInstancingBuffer pBuffer, int iCount);
public:
	class CLight* GetFirstLight()	const;

public:
	bool Init();
	void AddRenderObject(CGameObject* pObj);

public:
	bool CreateRasterizerState(const string& strName, D3D11_FILL_MODE eFill = D3D11_FILL_SOLID,	D3D11_CULL_MODE eCull = D3D11_CULL_BACK);
	bool CreateDepthState(const string& strName, BOOL bEnable = TRUE, D3D11_DEPTH_WRITE_MASK eWriteMask = D3D11_DEPTH_WRITE_MASK_ALL, D3D11_COMPARISON_FUNC eDepthFunc = D3D11_COMPARISON_LESS);
	bool AddTargetBlendDesc(const string& strName, BOOL bEnable = FALSE, D3D11_BLEND eSrcBlend = D3D11_BLEND_ONE, D3D11_BLEND eDestBlend = D3D11_BLEND_ZERO, D3D11_BLEND_OP eOp = D3D11_BLEND_OP_ADD,
		D3D11_BLEND eSrcBlendAlpha = D3D11_BLEND_ONE, D3D11_BLEND eDestBlendAlpha = D3D11_BLEND_ZERO, D3D11_BLEND_OP eAlphaOp = D3D11_BLEND_OP_ADD,	UINT iWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL);
	bool CreateBlendState(const string& strName, BOOL bAlphaToCoverage = FALSE,	BOOL bIndependentBlend = FALSE);

	bool CreateRenderTarget(const string& strName, UINT iWidth, UINT iHeight, DXGI_FORMAT eFmt, float fClearColor[4], int iSampleCount = 1, DXGI_FORMAT eDepthFmt = DXGI_FORMAT_UNKNOWN);
	bool OnDebugRenderTarget(const string& strName, const Vector3& vPos, const Vector3& vScale);

	bool AddMRT(const string& strMRTName, const string& strTarget);
	bool AddDepth(const string& strMRTName, const string& strTarget);
	void ClearMRT(const string& strMRTName);
	void SetMRT(const string& strMRTName);
	void ResetMRT(const string& strMRTName);

public:
	class CRenderState* FindRenderState(const string& strName);
	class CRenderTarget* FindRenderTarget(const string& strName);
	PMRT FindMRT(const string& strName);

public:
	void ComputeInstancing();
	void Render(float fTime);

private:
	void RenderForward(float fTime);
	void RenderDeferred(float fTime);
	void RenderGBuffer(float fTime);
	void RenderLightAcc(float fTime);
	void RenderLightDir(float fTime, class CLight* pLight);
	void RenderLightPoint(float fTime, class CLight* pLight);
	void RenderLightSpot(float fTime, class CLight* pLight);
	void RenderLightBlend(float fTime);
	void RenderLightBlendRender(float fTime);
	
	DECLARE_SINGLE(CRenderManager)
};

ENGINE_END
