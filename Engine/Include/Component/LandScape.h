#pragma once
#include "Component.h"

ENGINE_BEGIN
typedef struct ENGINE_DLL _tagSplatName
{
	vector<TCHAR*>	vecDifName;
	vector<TCHAR*>	vecNrmName;
	vector<TCHAR*>	vecSpcName;
	vector<TCHAR*>	vecAlphaName;
	vector<string>	vecPathName;

	~_tagSplatName()
	{
		Safe_Delete_Array_VecList(vecDifName);
		Safe_Delete_Array_VecList(vecNrmName);
		Safe_Delete_Array_VecList(vecSpcName);
		Safe_Delete_Array_VecList(vecAlphaName);
		vecPathName.clear();
	}
}SplatName, *PSplatName;


class ENGINE_DLL CLandScape :
	public CComponent
{
	friend class CGameObject;

protected:
	CLandScape();
	CLandScape(const CLandScape& com);
	virtual ~CLandScape();

private:
	vector<Vertex3D>	m_vecVtx;
	vector<Vector3>		m_vecFaceNormal;
	vector<int>			m_vecIdx;
	int					m_iNumX;
	int					m_iNumZ;
	LandScaleCBuffer	m_tCBuffer;
	class CMaterial*	m_pMaterial;
	SplatName			m_tSplatName;
	class CTexture*		m_pSplatDifTex;
	class CTexture*		m_pSplatNrmTex;
	class CTexture*		m_pSplatSpcTex;
	class CTexture*		m_pSplatAlphaTex;

public:
	bool CreateLandScape(const string& strName, const char* pFileName,
		const string& strPathName = TEXTURE_PATH);

	void SetDetailLevel(float fLevelX, float fLevelZ);
	void ChangeBaseTexture(const TCHAR* pDiffuseName, const TCHAR* pNormalName, const TCHAR* pSpecularName, const string& strPathName = TEXTURE_PATH);
	void AddSplatTexture(const TCHAR* pDiffuseName, const TCHAR* pNormalName, const TCHAR* pSpecularName, const TCHAR* pAlphaName, const string& strPathName = TEXTURE_PATH);
	void CreateSplatTexture();
public:
	virtual void Start();
	virtual bool Init();
	virtual int Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual int Collision(float fTime);
	virtual int PrevRender(float fTime);
	virtual int Render(float fTime);
	virtual CLandScape* Clone()	const;

private:
	void ComputeNormal();
	void ComputeTangent();
};

ENGINE_END
