#include "Material.h"
#include "../Resource/ResourcesManager.h"
#include "../Resource/Texture.h"
#include "../Resource/Sampler.h"
#include "../Render/ShaderManager.h"
#include "../PathManager.h"

ENGINE_USING

CMaterial::CMaterial()
{
	m_eComType = CT_MATERIAL;
}

CMaterial::CMaterial(const CMaterial & com)	:
	CComponent(com)
{
	m_vecContainer.clear();
	for (size_t i = 0; i < com.m_vecContainer.size(); ++i)
	{
		PMaterialContainer	pContainer = new MaterialContainer;

		for (size_t j = 0; j < com.m_vecContainer[i]->vecSubset.size(); ++j)
		{
			PMaterialSubset	pSubset = new MaterialSubset;

			pSubset->tMtrl = com.m_vecContainer[i]->vecSubset[j]->tMtrl;

			unordered_map<string, PTextureSet>::const_iterator	iter;
			unordered_map<string, PTextureSet>::const_iterator	iterEnd = 
				com.m_vecContainer[i]->vecSubset[j]->mapTextureSet.end();

			for (iter = com.m_vecContainer[i]->vecSubset[j]->mapTextureSet.begin();
				iter != iterEnd; ++iter)
			{
				PTextureSet	pTextureSet = new TextureSet;
				*pTextureSet = *iter->second;
				if (pTextureSet->pTexture)
					pTextureSet->pTexture->AddRef();

				pSubset->mapTextureSet.insert(make_pair(iter->first, pTextureSet));
			}

			pContainer->vecSubset.push_back(pSubset);
		}

		m_vecContainer.push_back(pContainer);
	}
}

CMaterial::~CMaterial()
{
	for (size_t i = 0; i < m_vecContainer.size(); ++i)
	{
		PMaterialContainer	pContainer = m_vecContainer[i];

		for (size_t j = 0; j < pContainer->vecSubset.size(); ++j)
		{
			unordered_map<string, PTextureSet>::iterator	iter;
			unordered_map<string, PTextureSet>::iterator	iterEnd =
				pContainer->vecSubset[j]->mapTextureSet.end();

			for (iter = pContainer->vecSubset[j]->mapTextureSet.begin();
				iter != iterEnd; ++iter)
			{
				SAFE_RELEASE(iter->second->pTexture);
				SAFE_DELETE(iter->second);
			}
			SAFE_DELETE(pContainer->vecSubset[j]);
		}

		SAFE_DELETE(pContainer);
	}
}

void CMaterial::SetColor(const Vector4 & vDif, const Vector4 & vAmb, 
	const Vector4 & vSpc, const Vector4 & vEmv, float fSpcPower, UINT iContainer,
	UINT iSubset)
{
	if (iContainer >= m_vecContainer.size())
	{
		PMaterialContainer	pContainer = new MaterialContainer;
		m_vecContainer.push_back(pContainer);
	}

	if (iSubset >= m_vecContainer[iContainer]->vecSubset.size())
	{
		PMaterialSubset	pSubset = new MaterialSubset;
		m_vecContainer[iContainer]->vecSubset.push_back(pSubset);
	}

	PMaterialContainer	pContainer = m_vecContainer[iContainer];
	PMaterialSubset	pSubset = pContainer->vecSubset[iSubset];

	memcpy(&pSubset->tMtrl.vDif, &vDif, sizeof(Vector4));
	memcpy(&pSubset->tMtrl.vAmb, &vAmb, sizeof(Vector4));
	memcpy(&pSubset->tMtrl.vSpc, &vSpc, sizeof(Vector4));
	memcpy(&pSubset->tMtrl.vEmv, &vEmv, sizeof(Vector4));
	pSubset->tMtrl.vSpc.w = fSpcPower;
}

void CMaterial::SetEmissiveColor(float fEmv)
{
	for (size_t i = 0; i < m_vecContainer.size(); ++i)
	{
		for (size_t j = 0; j < m_vecContainer[i]->vecSubset.size(); ++j)
		{
			m_vecContainer[i]->vecSubset[j]->tMtrl.vEmv.x = fEmv;
			m_vecContainer[i]->vecSubset[j]->tMtrl.vEmv.y = fEmv;
			m_vecContainer[i]->vecSubset[j]->tMtrl.vEmv.z = fEmv;
			m_vecContainer[i]->vecSubset[j]->tMtrl.vEmv.w = 1.f;
		}
	}
}

void CMaterial::SetEmissiveColor(int iContainer, int iSubset, float fEmv)
{
	m_vecContainer[iContainer]->vecSubset[iSubset]->tMtrl.vEmv.x = fEmv;
	m_vecContainer[iContainer]->vecSubset[iSubset]->tMtrl.vEmv.y = fEmv;
	m_vecContainer[iContainer]->vecSubset[iSubset]->tMtrl.vEmv.z = fEmv;
	m_vecContainer[iContainer]->vecSubset[iSubset]->tMtrl.vEmv.w = 1.f;
}

void CMaterial::AddTextureSet(int iContainer, int iSubset,
	int iTexReg, const string & strTexName, 
	const TCHAR * pFileName, const string & strPathName)
{
	if (iContainer >= m_vecContainer.size())
	{
		PMaterialContainer	pContainer = new MaterialContainer;
		m_vecContainer.push_back(pContainer);
	}

	if (iSubset >= m_vecContainer[iContainer]->vecSubset.size())
	{
		PMaterialSubset	pSubset = new MaterialSubset;
		m_vecContainer[iContainer]->vecSubset.push_back(pSubset);
	}

	if (FindTextureSet(iContainer, iSubset, strTexName))
		return;

	PMaterialContainer	pContainer = m_vecContainer[iContainer];
	PMaterialSubset	pSubset = pContainer->vecSubset[iSubset];

	PTextureSet	pTextureSet = new TextureSet;

	if (pFileName)
		GET_SINGLE(CResourcesManager)->LoadTexture(strTexName, pFileName, strPathName);
	pTextureSet->pTexture = GET_SINGLE(CResourcesManager)->FindTexture(strTexName);

	pTextureSet->iTexRegister = iTexReg;

	pSubset->mapTextureSet.insert(make_pair(strTexName, pTextureSet));

	if (pSubset->mapTextureSet.size() == 1)
	{
		m_iSerialNumber = pTextureSet->pTexture->GetSerialNumber();
	}
}

void CMaterial::AddTextureSet(int iContainer, int iSubset,
	int iTexReg, CTexture * pTexture)
{
	if (iContainer >= m_vecContainer.size())
	{
		PMaterialContainer	pContainer = new MaterialContainer;
		m_vecContainer.push_back(pContainer);
	}

	if (iSubset >= m_vecContainer[iContainer]->vecSubset.size())
	{
		PMaterialSubset	pSubset = new MaterialSubset;
		m_vecContainer[iContainer]->vecSubset.push_back(pSubset);
	}

	if (FindTextureSet(iContainer, iSubset, pTexture->GetTag()))
		return;

	PMaterialContainer	pContainer = m_vecContainer[iContainer];
	PMaterialSubset	pSubset = pContainer->vecSubset[iSubset];

	PTextureSet	pTextureSet = new TextureSet;

	pTexture->AddRef();
	pTextureSet->pTexture = pTexture;

	pTextureSet->iTexRegister = iTexReg;

	pSubset->mapTextureSet.insert(make_pair(pTexture->GetTag(), pTextureSet));

	if (pSubset->mapTextureSet.size() == 1)
	{
		m_iSerialNumber = pTextureSet->pTexture->GetSerialNumber();
	}
}

void CMaterial::AddTextureSetFromFullPath(int iContainer, int iSubset, 
	int iTexReg, const string & strTexName, const TCHAR * pFullPath)
{
	if (iContainer >= m_vecContainer.size())
	{
		PMaterialContainer	pContainer = new MaterialContainer;
		m_vecContainer.push_back(pContainer);
	}

	if (iSubset >= m_vecContainer[iContainer]->vecSubset.size())
	{
		PMaterialSubset	pSubset = new MaterialSubset;
		m_vecContainer[iContainer]->vecSubset.push_back(pSubset);
	}

	if (FindTextureSet(iContainer, iSubset, strTexName))
		return;

	PMaterialContainer	pContainer = m_vecContainer[iContainer];
	PMaterialSubset	pSubset = pContainer->vecSubset[iSubset];

	PTextureSet	pTextureSet = new TextureSet;

	if (pFullPath)
		GET_SINGLE(CResourcesManager)->LoadTextureFromFullPath(strTexName, pFullPath);
	pTextureSet->pTexture = GET_SINGLE(CResourcesManager)->FindTexture(strTexName);

	pTextureSet->iTexRegister = iTexReg;

	pSubset->mapTextureSet.insert(make_pair(strTexName, pTextureSet));

	if (pSubset->mapTextureSet.size() == 1)
	{
		m_iSerialNumber = pTextureSet->pTexture->GetSerialNumber();
	}
}

void CMaterial::AddTextureSetArray(int iContainer, int iSubset,
	int iTexReg, const string & strTexName, 
	const vector<TCHAR*>* vecFileName, const string & strPathName)
{
	if (iContainer >= m_vecContainer.size())
	{
		PMaterialContainer	pContainer = new MaterialContainer;
		m_vecContainer.push_back(pContainer);
	}

	if (iSubset >= m_vecContainer[iContainer]->vecSubset.size())
	{
		PMaterialSubset	pSubset = new MaterialSubset;
		m_vecContainer[iContainer]->vecSubset.push_back(pSubset);
	}

	if (FindTextureSet(iContainer, iSubset, strTexName))
		return;

	PMaterialContainer	pContainer = m_vecContainer[iContainer];
	PMaterialSubset	pSubset = pContainer->vecSubset[iSubset];

	PTextureSet	pTextureSet = new TextureSet;

	if (vecFileName)
		GET_SINGLE(CResourcesManager)->LoadTexture(strTexName, *vecFileName, strPathName);
	pTextureSet->pTexture = GET_SINGLE(CResourcesManager)->FindTexture(strTexName);

	pTextureSet->iTexRegister = iTexReg;

	pSubset->mapTextureSet.insert(make_pair(strTexName, pTextureSet));

	if (pSubset->mapTextureSet.size() == 1)
	{
		m_iSerialNumber = pTextureSet->pTexture->GetSerialNumber();
	}
}

void CMaterial::AddTextureSetArrayFromFullPath(int iContainer, int iSubset,
	int iTexReg, const string & strTexName, 
	const vector<TCHAR*>* vecFullPath)
{
	if (iContainer >= m_vecContainer.size())
	{
		PMaterialContainer	pContainer = new MaterialContainer;
		m_vecContainer.push_back(pContainer);
	}

	if (iSubset >= m_vecContainer[iContainer]->vecSubset.size())
	{
		PMaterialSubset	pSubset = new MaterialSubset;
		m_vecContainer[iContainer]->vecSubset.push_back(pSubset);
	}

	if (FindTextureSet(iContainer, iSubset, strTexName))
		return;

	PMaterialContainer	pContainer = m_vecContainer[iContainer];
	PMaterialSubset	pSubset = pContainer->vecSubset[iSubset];

	PTextureSet	pTextureSet = new TextureSet;

	if (vecFullPath)
		GET_SINGLE(CResourcesManager)->LoadTextureFromFullPath(strTexName, *vecFullPath);
	pTextureSet->pTexture = GET_SINGLE(CResourcesManager)->FindTexture(strTexName);

	pTextureSet->iTexRegister = iTexReg;

	pSubset->mapTextureSet.insert(make_pair(strTexName, pTextureSet));

	if (pSubset->mapTextureSet.size() == 1)
	{
		m_iSerialNumber = pTextureSet->pTexture->GetSerialNumber();
	}
}

void CMaterial::DeleteTextureSet(int iContainer, int iSubset,
	const string & strName)
{
	if (iContainer >= m_vecContainer.size())
		return;

	else if (iSubset >= m_vecContainer[iContainer]->vecSubset.size())
		return;

	PMaterialContainer	pContainer = m_vecContainer[iContainer];
	PMaterialSubset	pSubset = pContainer->vecSubset[iSubset];

	unordered_map<string, PTextureSet>::iterator	iter = pSubset->mapTextureSet.find(strName);

	if (iter == pSubset->mapTextureSet.end())
		return;

	SAFE_RELEASE(iter->second->pTexture);
	SAFE_DELETE(iter->second);

	pSubset->mapTextureSet.erase(iter);
}

void CMaterial::DeleteTextureSet(int iContainer, int iSubset)
{
	if (iContainer >= m_vecContainer.size())
		return;

	else if (iSubset >= m_vecContainer[iContainer]->vecSubset.size())
		return;

	PMaterialContainer	pContainer = m_vecContainer[iContainer];
	PMaterialSubset	pSubset = pContainer->vecSubset[iSubset];

	unordered_map<string, PTextureSet>::iterator	iter;
	unordered_map<string, PTextureSet>::iterator	iterEnd = pSubset->mapTextureSet.end();

	for (iter = pSubset->mapTextureSet.begin(); iter != iterEnd; ++iter)
	{
		SAFE_RELEASE(iter->second->pTexture);
		SAFE_DELETE(iter->second);
	}

	pSubset->mapTextureSet.clear();
}

void CMaterial::ChangeTextureSet(int iContainer, int iSubset, 
	int iTexReg, const string & strName, 
	const string & strChangeName, const TCHAR * pFileName, 
	const string & strPathName)
{
	if (iContainer >= m_vecContainer.size())
	{
		PMaterialContainer	pContainer = new MaterialContainer;
		m_vecContainer.push_back(pContainer);
	}

	if (iSubset >= m_vecContainer[iContainer]->vecSubset.size())
	{
		PMaterialSubset	pSubset = new MaterialSubset;
		m_vecContainer[iContainer]->vecSubset.push_back(pSubset);
	}

	PMaterialContainer	pContainer = m_vecContainer[iContainer];
	PMaterialSubset	pSubset = pContainer->vecSubset[iSubset];

	PTextureSet	pOriginSet = FindTextureSet(iContainer, iSubset,
		strName);

	if (pOriginSet)
	{
		string	strName = pOriginSet->pTexture->GetTag();
		SAFE_RELEASE(pOriginSet->pTexture);
		GET_SINGLE(CResourcesManager)->DeleteTexture(strName);
	}

	else
	{
		pOriginSet = new TextureSet;
	}
	pOriginSet->iTexRegister = iTexReg;

	if(pFileName)
		GET_SINGLE(CResourcesManager)->LoadTexture(strChangeName, pFileName, strPathName);
	pOriginSet->pTexture = GET_SINGLE(CResourcesManager)->FindTexture(strChangeName);

	// 키를 교체한다.
	pSubset->mapTextureSet.erase(strName);
	pSubset->mapTextureSet.insert(make_pair(strChangeName, pOriginSet));
	if (pSubset->mapTextureSet.size() == 1)
	{
		m_iSerialNumber = pOriginSet->pTexture->GetSerialNumber();
	}
}

void CMaterial::ChangeTextureSet(int iContainer, int iSubset, 
	int iTexReg, const string & strName, CTexture * pTexture)
{
	if (iContainer >= m_vecContainer.size())
	{
		PMaterialContainer	pContainer = new MaterialContainer;
		m_vecContainer.push_back(pContainer);
	}

	if (iSubset >= m_vecContainer[iContainer]->vecSubset.size())
	{
		PMaterialSubset	pSubset = new MaterialSubset;
		m_vecContainer[iContainer]->vecSubset.push_back(pSubset);
	}

	PMaterialContainer	pContainer = m_vecContainer[iContainer];
	PMaterialSubset	pSubset = pContainer->vecSubset[iSubset];

	PTextureSet	pOriginSet = FindTextureSet(iContainer, iSubset,
		strName);

	if (pOriginSet)
	{
		string	strName = pOriginSet->pTexture->GetTag();
		SAFE_RELEASE(pOriginSet->pTexture);
		GET_SINGLE(CResourcesManager)->DeleteTexture(strName);
	}

	else
	{
		pOriginSet = new TextureSet;
	}

	pOriginSet->iTexRegister = iTexReg;

	pTexture->AddRef();
	pOriginSet->pTexture = pTexture;

	// 키를 교체한다.
	pSubset->mapTextureSet.erase(strName);
	pSubset->mapTextureSet.insert(make_pair(pTexture->GetTag(), pOriginSet));
	if (pSubset->mapTextureSet.size() == 1)
	{
		m_iSerialNumber = pOriginSet->pTexture->GetSerialNumber();
	}
}

void CMaterial::ChangeTextureSetFromFullPath(int iContainer,
	int iSubset, int iTexReg, const string & strName, 
	const string & strChangeName, const TCHAR * pFullPath)
{
	if (iContainer >= m_vecContainer.size())
	{
		PMaterialContainer	pContainer = new MaterialContainer;
		m_vecContainer.push_back(pContainer);
	}

	if (iSubset >= m_vecContainer[iContainer]->vecSubset.size())
	{
		PMaterialSubset	pSubset = new MaterialSubset;
		m_vecContainer[iContainer]->vecSubset.push_back(pSubset);
	}

	PMaterialContainer	pContainer = m_vecContainer[iContainer];
	PMaterialSubset	pSubset = pContainer->vecSubset[iSubset];

	PTextureSet	pOriginSet = FindTextureSet(iContainer, iSubset,
		strName);

	if (pOriginSet)
	{
		string	strName = pOriginSet->pTexture->GetTag();
		SAFE_RELEASE(pOriginSet->pTexture);
		GET_SINGLE(CResourcesManager)->DeleteTexture(strName);
	}

	else
	{
		pOriginSet = new TextureSet;
	}
	pOriginSet->iTexRegister = iTexReg;

	if (pFullPath)
		GET_SINGLE(CResourcesManager)->LoadTextureFromFullPath(strChangeName, pFullPath);
	pOriginSet->pTexture = GET_SINGLE(CResourcesManager)->FindTexture(strChangeName);

	// 키를 교체한다.
	pSubset->mapTextureSet.erase(strName);
	pSubset->mapTextureSet.insert(make_pair(strChangeName, pOriginSet));

	if (pSubset->mapTextureSet.size() == 1)
	{
		m_iSerialNumber = pOriginSet->pTexture->GetSerialNumber();
	}
}

void CMaterial::ChangeTextureSetArray(int iContainer, int iSubset, 
	int iTexReg, const string & strName, 
	const string & strChangeName,
	const vector<TCHAR*>* vecFileName, 
	const string & strPathName)
{
	if (iContainer >= m_vecContainer.size())
	{
		PMaterialContainer	pContainer = new MaterialContainer;
		m_vecContainer.push_back(pContainer);
	}

	if (iSubset >= m_vecContainer[iContainer]->vecSubset.size())
	{
		PMaterialSubset	pSubset = new MaterialSubset;
		m_vecContainer[iContainer]->vecSubset.push_back(pSubset);
	}

	PMaterialContainer	pContainer = m_vecContainer[iContainer];
	PMaterialSubset	pSubset = pContainer->vecSubset[iSubset];

	PTextureSet	pOriginSet = FindTextureSet(iContainer, iSubset,
		strName);

	if (pOriginSet)
	{
		string	strName = pOriginSet->pTexture->GetTag();
		SAFE_RELEASE(pOriginSet->pTexture);
		GET_SINGLE(CResourcesManager)->DeleteTexture(strName);
	}

	else
	{
		pOriginSet = new TextureSet;
	}
	pOriginSet->iTexRegister = iTexReg;

	if (vecFileName)
		GET_SINGLE(CResourcesManager)->LoadTexture(strChangeName, *vecFileName, strPathName);
	pOriginSet->pTexture = GET_SINGLE(CResourcesManager)->FindTexture(strChangeName);

	// 키를 교체한다.
	pSubset->mapTextureSet.erase(strName);
	pSubset->mapTextureSet.insert(make_pair(strChangeName, pOriginSet));

	if (pSubset->mapTextureSet.size() == 1)
	{
		m_iSerialNumber = pOriginSet->pTexture->GetSerialNumber();
	}
}

void CMaterial::ChangeTextureSetArrayFromFullPath(int iContainer,
	int iSubset, int iTexReg, const string & strName, 
	const string & strChangeName, 
	const vector<TCHAR*>* vecFullPath)
{
	if (iContainer >= m_vecContainer.size())
	{
		PMaterialContainer	pContainer = new MaterialContainer;
		m_vecContainer.push_back(pContainer);
	}

	if (iSubset >= m_vecContainer[iContainer]->vecSubset.size())
	{
		PMaterialSubset	pSubset = new MaterialSubset;
		m_vecContainer[iContainer]->vecSubset.push_back(pSubset);
	}

	PMaterialContainer	pContainer = m_vecContainer[iContainer];
	PMaterialSubset	pSubset = pContainer->vecSubset[iSubset];

	PTextureSet	pOriginSet = FindTextureSet(iContainer, iSubset,
		strName);

	if (pOriginSet)
	{
		string	strName = pOriginSet->pTexture->GetTag();
		SAFE_RELEASE(pOriginSet->pTexture);
		GET_SINGLE(CResourcesManager)->DeleteTexture(strName);
	}

	else
	{
		pOriginSet = new TextureSet;
	}
	pOriginSet->iTexRegister = iTexReg;

	if (vecFullPath)
		GET_SINGLE(CResourcesManager)->LoadTextureFromFullPath(strChangeName, *vecFullPath);
	pOriginSet->pTexture = GET_SINGLE(CResourcesManager)->FindTexture(strChangeName);

	// 키를 교체한다.
	pSubset->mapTextureSet.erase(strName);
	pSubset->mapTextureSet.insert(make_pair(strChangeName, pOriginSet));

	if (pSubset->mapTextureSet.size() == 1)
	{
		m_iSerialNumber = pOriginSet->pTexture->GetSerialNumber();
	}
}

void CMaterial::BumpTextureEnable(int iContainer, int iSubset)
{
	if (iContainer >= m_vecContainer.size())
	{
		PMaterialContainer	pContainer = new MaterialContainer;
		m_vecContainer.push_back(pContainer);
	}

	if (iSubset >= m_vecContainer[iContainer]->vecSubset.size())
	{
		PMaterialSubset	pSubset = new MaterialSubset;
		m_vecContainer[iContainer]->vecSubset.push_back(pSubset);
	}

	PMaterialContainer	pContainer = m_vecContainer[iContainer];
	PMaterialSubset	pSubset = pContainer->vecSubset[iSubset];

	pSubset->tMtrl.iNormal = 1;
}

void CMaterial::SpecularTextureEnable(int iContainer, int iSubset)
{
	if (iContainer >= m_vecContainer.size())
	{
		PMaterialContainer	pContainer = new MaterialContainer;
		m_vecContainer.push_back(pContainer);
	}

	if (iSubset >= m_vecContainer[iContainer]->vecSubset.size())
	{
		PMaterialSubset	pSubset = new MaterialSubset;
		m_vecContainer[iContainer]->vecSubset.push_back(pSubset);
	}

	PMaterialContainer	pContainer = m_vecContainer[iContainer];
	PMaterialSubset	pSubset = pContainer->vecSubset[iSubset];

	pSubset->tMtrl.iSpecular = 1;
}

void CMaterial::Skinning(int iContainer, int iSubset)
{
	if (iContainer >= m_vecContainer.size())
	{
		PMaterialContainer	pContainer = new MaterialContainer;
		m_vecContainer.push_back(pContainer);
	}

	if (iSubset >= m_vecContainer[iContainer]->vecSubset.size())
	{
		PMaterialSubset	pSubset = new MaterialSubset;
		m_vecContainer[iContainer]->vecSubset.push_back(pSubset);
	}

	PMaterialContainer	pContainer = m_vecContainer[iContainer];
	PMaterialSubset	pSubset = pContainer->vecSubset[iSubset];

	pSubset->tMtrl.iSkinning = 1;
}

PTextureSet CMaterial::FindTextureSet(int iContainer, int iSubset, const string & strName)
{
	if (iContainer >= m_vecContainer.size())
		return nullptr;

	else if (iSubset >= m_vecContainer[iContainer]->vecSubset.size())
		return nullptr;

	PMaterialContainer	pContainer = m_vecContainer[iContainer];
	PMaterialSubset	pSubset = pContainer->vecSubset[iSubset];

	unordered_map<string, PTextureSet>::iterator	iter = pSubset->mapTextureSet.find(strName);

	if (iter == pSubset->mapTextureSet.end())
		return nullptr;

	return iter->second;
}

void CMaterial::Start()
{
}

bool CMaterial::Init()
{
	return true;
}

int CMaterial::Input(float fTime)
{
	return 0;
}

int CMaterial::Update(float fTime)
{
	return 0;
}

int CMaterial::LateUpdate(float fTime)
{
	return 0;
}

int CMaterial::Collision(float fTime)
{
	return 0;
}

int CMaterial::PrevRender(float fTime)
{
	return 0;
}

int CMaterial::Render(float fTime)
{
	return 0;
}

CMaterial * CMaterial::Clone() const
{
	return new CMaterial(*this);
}

void CMaterial::SetShader(int iContainer, int iSubset)
{
	PMaterialSubset	pSubset = m_vecContainer[iContainer]->vecSubset[iSubset];

	if (!pSubset)
		return;

	GET_SINGLE(CShaderManager)->UpdateCBuffer("Material", &pSubset->tMtrl);

	unordered_map<string, PTextureSet>::iterator	iter;
	unordered_map<string, PTextureSet>::iterator	iterEnd = pSubset->mapTextureSet.end();

	for (iter = pSubset->mapTextureSet.begin(); iter != iterEnd; ++iter)
	{
		iter->second->pTexture->PSSetShader(iter->second->iTexRegister);
	}
}

void CMaterial::Save(FILE * pFile)
{
	size_t	iContainer = m_vecContainer.size();
	fwrite(&iContainer, sizeof(size_t), 1, pFile);

	for (size_t i = 0; i < iContainer; ++i)
	{
		size_t iSubset = m_vecContainer[i]->vecSubset.size();
		fwrite(&iSubset, sizeof(size_t), 1, pFile);

		for (size_t j = 0; j < iSubset; ++j)
		{
			PMaterialSubset	pSubset = m_vecContainer[i]->vecSubset[j];

			fwrite(&pSubset->tMtrl, sizeof(Material), 1, pFile);

			size_t iTexCount = pSubset->mapTextureSet.size();

			fwrite(&iTexCount, sizeof(size_t), 1, pFile);
			
			unordered_map<string, PTextureSet>::iterator	iter;
			unordered_map<string, PTextureSet>::iterator	iterEnd = pSubset->mapTextureSet.end();

			for (iter = pSubset->mapTextureSet.begin(); iter != iterEnd; ++iter)
			{
				SaveTextureSet(pFile, iter->second);
			}
		}
	}
}

void CMaterial::Load(FILE * pFile)
{
	m_vecContainer.clear();
	size_t	iContainer = 0;
	fread(&iContainer, sizeof(size_t), 1, pFile);

	for (size_t i = 0; i < iContainer; ++i)
	{
		size_t iSubset = 0;
		fread(&iSubset, sizeof(size_t), 1, pFile);

		PMaterialContainer	pContainer = new MaterialContainer;

		m_vecContainer.push_back(pContainer);

		for (size_t j = 0; j < iSubset; ++j)
		{
			PMaterialSubset	pSubset = new MaterialSubset;
			pContainer->vecSubset.push_back(pSubset);

			fread(&pSubset->tMtrl, sizeof(Material), 1, pFile);

			size_t	iTexCount = 0;
			fread(&iTexCount, sizeof(size_t), 1, pFile);

			for (size_t iTex = 0; iTex < iTexCount; ++iTex)
			{
				PTextureSet	pTexSet = nullptr;
				LoadTextureSet(pFile, &pTexSet);

				pSubset->mapTextureSet.insert(make_pair(pTexSet->pTexture->GetTag(), pTexSet));

				if (pSubset->mapTextureSet.size() == 1)
				{
					m_iSerialNumber = pTexSet->pTexture->GetSerialNumber();
				}
			}
		}
	}
}

void CMaterial::SaveFromPath(const char * pFileName, const string & strPathKey)
{
}

void CMaterial::LoadFromPath(const char * pFileName, const string & strPathKey)
{
}

void CMaterial::SaveTextureSet(FILE * pFile, PTextureSet pTexture)
{
	bool	bTexEnable = false;

	if (pTexture)
	{
		bTexEnable = true;
		fwrite(&bTexEnable, sizeof(bool), 1, pFile);

		string	strTexKey = pTexture->pTexture->GetTag();
		size_t	iKeyLength = strTexKey.length();
		fwrite(&iKeyLength, sizeof(size_t), 1, pFile);
		fwrite(strTexKey.c_str(), 1, iKeyLength, pFile);

		// Texture 경로를 얻어온다.
		const vector<TCHAR*>* pPathList =
			pTexture->pTexture->GetFullPath();

		size_t iPathCount = pPathList->size();
		fwrite(&iPathCount, sizeof(size_t), 1, pFile);

		const char* pRootPath = GET_SINGLE(CPathManager)->FindPathMultibyte(ROOT_PATH);
		char	strRoot[MAX_PATH] = {};

		int	iRootLength = (int)strlen(pRootPath);

		for (int i = iRootLength - 2; i >= 0; --i)
		{
			if (pRootPath[i] == '\\' || pRootPath[i] == '/')
			{
				memcpy(strRoot, &pRootPath[i + 1], sizeof(char) * (iRootLength - (i + 2)));
				break;
			}
		}

		// 거꾸로 뒤집어놓는다.
		iRootLength = (int)strlen(strRoot);

		char	strReverse[MAX_PATH] = {};

		for (int i = 0; i < iRootLength; ++i)
		{
			strReverse[i] = strRoot[iRootLength - (i + 1)];
		}

		memcpy(strRoot, strReverse, MAX_PATH);

		_strupr_s(strRoot);

		vector<TCHAR*>::const_iterator	iter;
		vector<TCHAR*>::const_iterator	iterEnd = pPathList->end();

		for (iter = pPathList->begin(); iter != iterEnd; ++iter)
		{
			int	iPathLength = lstrlen(*iter);
			char	strPath[MAX_PATH] = {};

#ifdef UNICODE
			WideCharToMultiByte(CP_ACP, 0, *iter, -1, strPath, iPathLength, 0, 0);
#else
			strcpy_s(strPath, *iter);
#endif // UNICODE
			_strupr_s(strPath);

			for (int k = iPathLength - 1; k >= 0; --k)
			{
				if (strPath[k] == '\\' || strPath[k] == '/')
				{
					bool	bEnable = true;
					for (int l = 1; l < iRootLength; ++l)
					{
						if (strPath[k - l] != strRoot[l - 1])
						{
							bEnable = false;
							break;
						}
					}

					if (bEnable)
					{
						char	strSavePath[MAX_PATH] = {};
						int	iSaveCount = iPathLength - (k + 1);
						memcpy(strSavePath, &strPath[k + 1], sizeof(char) * iSaveCount);
						fwrite(&iSaveCount, sizeof(int), 1, pFile);
						fwrite(strSavePath, sizeof(char), iSaveCount, pFile);
						break;
					}
				}
			}
		}

		fwrite(&pTexture->iTexRegister, sizeof(int), 1, pFile);
	}

	else
		fwrite(&bTexEnable, sizeof(bool), 1, pFile);
}

void CMaterial::LoadTextureSet(FILE * pFile, PTextureSet * ppTexture)
{
	bool	bTexEnable = false;
	fread(&bTexEnable, sizeof(bool), 1, pFile);

	if (bTexEnable)
	{
		char	strTexKey[256] = {};
		size_t	iKeyLength = 0;
		fread(&iKeyLength, sizeof(size_t), 1, pFile);
		fread(strTexKey, 1, iKeyLength, pFile);

		size_t iPathCount = 0;
		fread(&iPathCount, sizeof(size_t), 1, pFile);

		if (iPathCount == 1)
		{
			char	strPath[MAX_PATH] = {};
			int	iSaveCount = 0;
			fread(&iSaveCount, sizeof(int), 1, pFile);
			fread(strPath, sizeof(char), iSaveCount, pFile);

			TCHAR	strLoadPath[MAX_PATH] = {};

#ifdef UNICODE
			MultiByteToWideChar(CP_ACP, 0, strPath, -1, strLoadPath, (int)strlen(strPath) * 2);
#else
			strcpy_s(strLoadPath, strPath);
#endif // UNICODE

			*ppTexture = new TextureSet;

			GET_SINGLE(CResourcesManager)->LoadTexture(strTexKey, strLoadPath, ROOT_PATH);
			(*ppTexture)->pTexture = GET_SINGLE(CResourcesManager)->FindTexture(strTexKey);
		}

		else
		{
			vector<TCHAR*>	vecPath;
			for (size_t i = 0; i < iPathCount; ++i)
			{
				char	strPath[MAX_PATH] = {};
				int	iSaveCount = 0;
				fread(&iSaveCount, sizeof(int), 1, pFile);
				fread(strPath, sizeof(char), iSaveCount, pFile);

				TCHAR*	strLoadPath = new TCHAR[MAX_PATH];
				memset(strLoadPath, 0, sizeof(TCHAR) * MAX_PATH);

#ifdef UNICODE
				MultiByteToWideChar(CP_ACP, 0, strPath, -1, strLoadPath, (int)strlen(strPath) * 2);
#else
				strcpy_s(strLoadPath, strPath);
#endif // UNICODE
				vecPath.push_back(strLoadPath);
			}
			*ppTexture = new TextureSet;

			GET_SINGLE(CResourcesManager)->LoadTexture(strTexKey, vecPath, ROOT_PATH);
			(*ppTexture)->pTexture = GET_SINGLE(CResourcesManager)->FindTexture(strTexKey);
		}

		fread(&(*ppTexture)->iTexRegister, sizeof(int), 1, pFile);
	}
}
