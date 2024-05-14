#include "Texture.h"
#include "../PathManager.h"
#include "../Device.h"

ENGINE_USING

CTexture::CTexture()	:
	m_pSRV(nullptr)
{
}

CTexture::~CTexture()
{
	for (size_t i = 0; i < m_vecFullPath.size(); ++i)
	{
		SAFE_DELETE_ARRAY(m_vecFullPath[i]);
	}

	m_vecFullPath.clear();

	Safe_Delete_VecList(m_vecImage);
	SAFE_RELEASE(m_pSRV);
}

const vector<TCHAR*>* CTexture::GetFullPath() const
{
	return &m_vecFullPath;
}

bool CTexture::LoadTexture(const string & strName, const TCHAR * pFileName,
	const string & strPathKey)
{
	const TCHAR* pPath = GET_SINGLE(CPathManager)->FindPath(strPathKey);

	TCHAR	strPath[MAX_PATH] = {};

	if (pPath)
		lstrcpy(strPath, pPath);
	lstrcat(strPath, pFileName);

	return LoadTextureFromFullPath(strName, strPath);
}

bool CTexture::LoadTextureFromFullPath(const string & strName, 
	const TCHAR * pFullPath)
{
	SetTag(strName.c_str());

	ScratchImage*	pImage = new ScratchImage;

	TCHAR	strExt[_MAX_EXT] = {};

	SplitPath(pFullPath, nullptr, 0, nullptr, 0, nullptr, 0, strExt, _MAX_EXT);

	char	_strExt[_MAX_EXT] = {};

	WideCharToMultiByte(CP_ACP, 0, strExt, -1, _strExt, lstrlen(strExt), 0, 0);

	_strupr_s(_strExt);

	if (strcmp(_strExt, ".DDS") == 0)
	{
		if (FAILED(LoadFromDDSFile(pFullPath, DDS_FLAGS_NONE, nullptr, *pImage)))
		{
			pImage->Release();
			delete pImage;
			pImage = nullptr;
			return false;
		}
	}

	else if (strcmp(_strExt, ".TGA") == 0)
	{
		if (FAILED(LoadFromTGAFile(pFullPath, nullptr, *pImage)))
		{
			pImage->Release();
			delete pImage;
			pImage = nullptr;
			return false;
		}
	}

	else
	{
		if (FAILED(LoadFromWICFile(pFullPath, WIC_FLAGS_NONE, nullptr, *pImage)))
		{
			pImage->Release();
			delete pImage;
			pImage = nullptr;
			return false;
		}
	}

	TCHAR*	pSavePath = new TCHAR[MAX_PATH];

	memset(pSavePath, 0, sizeof(TCHAR) * MAX_PATH);

	lstrcpy(pSavePath, pFullPath);

	m_vecFullPath.push_back(pSavePath);

	m_vecImage.push_back(pImage);

	return CreateShaderResourceView();
}

bool CTexture::LoadTexture(const string & strName,
	const vector<TCHAR*>& vecFileName, const string & strPathKey)
{
	const TCHAR* pPath = GET_SINGLE(CPathManager)->FindPath(strPathKey);

	vector<TCHAR*>	vecFullPath;
	vecFullPath.reserve(vecFileName.size());

	for (size_t i = 0; i < vecFileName.size(); ++i)
	{
		TCHAR*	pFullPath = new TCHAR[MAX_PATH];
		memset(pFullPath, 0, sizeof(TCHAR) * MAX_PATH);

		if (pPath)
			lstrcpy(pFullPath, pPath);
		lstrcat(pFullPath, vecFileName[i]);

		vecFullPath.push_back(pFullPath);
	}

	if (!LoadTextureFromFullPath(strName, vecFullPath))
	{
		Safe_Delete_Array_VecList(vecFullPath);
		return false;
	}

	Safe_Delete_Array_VecList(vecFullPath);

	return true;
}

bool CTexture::LoadTextureFromFullPath(const string & strName,
	const vector<TCHAR*>& vecFullPath)
{
	SetTag(strName.c_str());

	for (size_t i = 0; i < vecFullPath.size(); ++i)
	{
		ScratchImage*	pImage = new ScratchImage;

		TCHAR	strExt[_MAX_EXT] = {};

		SplitPath(vecFullPath[i], nullptr, 0, nullptr, 0, nullptr, 0, strExt, _MAX_EXT);

		char	_strExt[_MAX_EXT] = {};

		WideCharToMultiByte(CP_ACP, 0, strExt, -1, _strExt, lstrlen(strExt), 0, 0);

		_strupr_s(_strExt);

		if (strcmp(_strExt, ".DDS") == 0)
		{
			if (FAILED(LoadFromDDSFile(vecFullPath[i], DDS_FLAGS_NONE, nullptr, *pImage)))
				return false;
		}

		else if (strcmp(_strExt, ".TGA") == 0)
		{
			if (FAILED(LoadFromTGAFile(vecFullPath[i], nullptr, *pImage)))
				return false;
		}

		else
		{
			if (FAILED(LoadFromWICFile(vecFullPath[i], WIC_FLAGS_NONE, nullptr, *pImage)))
				return false;
		}

		TCHAR*	pSavePath = new TCHAR[MAX_PATH];

		memset(pSavePath, 0, sizeof(TCHAR) * MAX_PATH);

		lstrcpy(pSavePath, vecFullPath[i]);

		m_vecFullPath.push_back(pSavePath);

		m_vecImage.push_back(pImage);
	}

	return CreateShaderResourceViewArray();
}

void CTexture::VSSetShader(int iRegister)
{
	_CONTEXT->VSSetShaderResources(iRegister, 1, &m_pSRV);
}

void CTexture::PSSetShader(int iRegister)
{
	_CONTEXT->PSSetShaderResources(iRegister, 1, &m_pSRV);
}

bool CTexture::CreateShaderResourceView()
{
	if (FAILED(DirectX::CreateShaderResourceView(_DEVICE, m_vecImage[0]->GetImages(),
		m_vecImage[0]->GetImageCount(), m_vecImage[0]->GetMetadata(),
		&m_pSRV)))
		return false;

	return true;
}

bool CTexture::CreateShaderResourceViewArray()
{
	vector<ID3D11Texture2D*>	vecTexture;

	for (size_t i = 0; i < m_vecImage.size(); ++i)
	{
		ID3D11Texture2D*	pTexture = NULL;

		if (FAILED(DirectX::CreateTextureEx(_DEVICE, m_vecImage[i]->GetImages(),
			m_vecImage[i]->GetImageCount(),
			m_vecImage[i]->GetMetadata(),
			D3D11_USAGE_STAGING, 0, 
			D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ, 0, FALSE,
			(ID3D11Resource**)&pTexture)))
		{
			assert(false);
			return false;
		}

		vecTexture.push_back(pTexture);
	}

	D3D11_TEXTURE2D_DESC	tTexDesc = {};
	vecTexture[0]->GetDesc(&tTexDesc);

	// Array Texture를 만들기 위한 Desc 구조체를 생성한다.
	D3D11_TEXTURE2D_DESC	tArrayDesc = {};
	tArrayDesc.Width = tTexDesc.Width;
	tArrayDesc.Height = tTexDesc.Height;
	tArrayDesc.MipLevels = tTexDesc.MipLevels;
	tArrayDesc.ArraySize = (UINT)vecTexture.size();
	tArrayDesc.Format = tTexDesc.Format;
	tArrayDesc.SampleDesc.Count = 1;
	tArrayDesc.SampleDesc.Quality = 0;
	tArrayDesc.Usage = D3D11_USAGE_DEFAULT;
	tArrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	// 위에서 설정한 정보를 이용해서 비어있는 Array Texture를 생성한다.
	ID3D11Texture2D*	pArrayTexture = NULL;
	if (FAILED(_DEVICE->CreateTexture2D(&tArrayDesc, NULL, &pArrayTexture)))
	{
		assert(false);
		return false;
	}

	// Array Texture의 픽셀정보를 불러온 텍스쳐의 픽셀정보로 채워준다.
	for (UINT i = 0; i < vecTexture.size(); ++i)
	{
		// 각각의 텍스쳐들을 밉맵 수준만큼 반복한다.
		for (UINT mip = 0; mip < tTexDesc.MipLevels; ++mip)
		{
			// 텍스처를 Map을 걸어준다.
			D3D11_MAPPED_SUBRESOURCE	tMap = {};
			_CONTEXT->Map(vecTexture[i], mip, D3D11_MAP_READ, 0, &tMap);

			_CONTEXT->UpdateSubresource(pArrayTexture,
				D3D11CalcSubresource(mip, i, tTexDesc.MipLevels),
				NULL, tMap.pData, tMap.RowPitch, tMap.DepthPitch);

			_CONTEXT->Unmap(vecTexture[i], mip);
		}
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC	tViewDesc = {};
	tViewDesc.Format = tArrayDesc.Format;
	tViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	tViewDesc.Texture2DArray.MostDetailedMip = 0;
	tViewDesc.Texture2DArray.MipLevels = tArrayDesc.MipLevels;
	tViewDesc.Texture2DArray.FirstArraySlice = 0;
	tViewDesc.Texture2DArray.ArraySize = (UINT)vecTexture.size();

	if (FAILED(_DEVICE->CreateShaderResourceView(pArrayTexture, &tViewDesc, &m_pSRV)))
	{
		assert(false);
		return false;
	}

	SAFE_RELEASE(pArrayTexture);

	Safe_Release_VecList(vecTexture);

	return true;
}


