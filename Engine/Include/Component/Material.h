#pragma once
#include "Component.h"

ENGINE_BEGIN

typedef struct ENGINE_DLL _tagTextureSet
{
	class CTexture*	pTexture;
	int				iTexRegister;

	_tagTextureSet() :
		pTexture(nullptr),
		iTexRegister(0)
	{
	}
}TextureSet, *PTextureSet;

typedef struct ENGINE_DLL _tagMaterialSubset
{
	Material	tMtrl;
	unordered_map<string, PTextureSet>	mapTextureSet;

	_tagMaterialSubset()
	{
	}
}MaterialSubset, *PMaterialSubset;

typedef struct ENGINE_DLL _tagMaterialContainer
{
	vector<PMaterialSubset>	vecSubset;
}MaterialContainer, *PMaterialContainer;

class ENGINE_DLL CMaterial :
	public CComponent
{
	friend class CGameObject;
	friend class CMesh;

protected:
	CMaterial();
	CMaterial(const CMaterial& com);
	virtual ~CMaterial();

private:
	vector<PMaterialContainer>	m_vecContainer;

public:
	void SetColor(const Vector4& vDif, const Vector4& vAmb, const Vector4& vSpc,
		const Vector4& vEmv, float fSpcPower, UINT iContainer = 0,
		UINT iSubset = 0);
	void SetEmissiveColor(float fEmv);
	void SetEmissiveColor(int iContainer, int iSubset, float fEmv);
	void AddTextureSet(int iContainer, int iSubset,
		int iTexReg, const string& strTexName,
		const TCHAR* pFileName = nullptr,
		const string& strPathName = TEXTURE_PATH);
	void AddTextureSet(int iContainer, int iSubset,
		int iTexReg, class CTexture* pTexture);
	void AddTextureSetFromFullPath(int iContainer, int iSubset,
		int iTexReg, const string& strTexName,
		const TCHAR* pFullPath = nullptr);
	void AddTextureSetArray(int iContainer, int iSubset,
		int iTexReg, const string& strTexName,
		const vector<TCHAR*>* vecFileName = nullptr,
		const string& strPathName = TEXTURE_PATH);
	void AddTextureSetArrayFromFullPath(int iContainer, int iSubset,
		int iTexReg, const string& strTexName,
		const vector<TCHAR*>* vecFullPath = nullptr);
	void DeleteTextureSet(int iContainer, int iSubset,
		const string& strName);
	void DeleteTextureSet(int iContainer, int iSubset);
	void ChangeTextureSet(int iContainer, int iSubset,
		int iTexReg, const string& strName, const string& strChangeName,
		const TCHAR* pFileName = nullptr,
		const string& strPathName = TEXTURE_PATH);
	void ChangeTextureSet(int iContainer, int iSubset,
		int iTexReg, const string& strName, class CTexture* pTexture);
	void ChangeTextureSetFromFullPath(int iContainer, int iSubset,
		int iTexReg, const string& strName, const string& strChangeName,
		const TCHAR* pFullPath = nullptr);
	void ChangeTextureSetArray(int iContainer, int iSubset,
		int iTexReg, const string& strName, const string& strChangeName,
		const vector<TCHAR*>* vecFileName = nullptr,
		const string& strPathName = TEXTURE_PATH);
	void ChangeTextureSetArrayFromFullPath(int iContainer, int iSubset,
		int iTexReg, const string& strName, const string& strChangeName,
		const vector<TCHAR*>* vecFullPath = nullptr);
	void BumpTextureEnable(int iContainer, int iSubset);
	void SpecularTextureEnable(int iContainer, int iSubset);
	void Skinning(int iContainer, int iSubset);

private:
	PTextureSet FindTextureSet(int iContainer, int iSubset,
		const string& strName);

public:
	virtual void Start();
	virtual bool Init();
	virtual int Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual int Collision(float fTime);
	virtual int PrevRender(float fTime);
	virtual int Render(float fTime);
	virtual CMaterial* Clone()	const;
	
public:
	void SetShader(int iContainer, int iSubset);
	virtual void Save(FILE* pFile);
	virtual void Load(FILE* pFile);
	virtual void SaveFromPath(const char* pFileName, const string& strPathKey = DATA_PATH);
	virtual void LoadFromPath(const char* pFileName, const string& strPathKey = DATA_PATH);

private:
	void SaveTextureSet(FILE* pFile, PTextureSet pTexture);
	void LoadTextureSet(FILE* pFile, PTextureSet* ppTexture);
};

ENGINE_END
