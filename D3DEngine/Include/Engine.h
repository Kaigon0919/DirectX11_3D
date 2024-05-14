
#pragma once

#include <Windows.h>
#include <list>
#include <vector>
#include <unordered_map>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dwrite.h>
#include <d2d1.h>
#include <dinput.h>
#include <crtdbg.h>
#include <string>
#include <functional>
#include <stack>

using namespace std;

#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")
#pragma comment(lib, "d2d1")
#pragma comment(lib, "dwrite")
#pragma comment(lib, "dinput8")
#pragma comment(lib, "dxguid")

#include "Types.h"

// Path Key
#define	ROOT_PATH	"RootPath"
#define	SHADER_PATH	"ShaderPath"
#define	TEXTURE_PATH	"TexturePath"
#define	MESH_PATH		"MeshPath"
#define	DATA_PATH		"DataPath"
#define	SOUND_PATH		"SoundPath"

// Shader Key
#define	STANDARD_COLOR_SHADER	"StandardColorShader"
#define	STANDARD_NORMAL_COLOR_SHADER	"StandardNormalColorShader"
#define	STANDARD_TEX_NORMAL_SHADER	"StandardTexNormalShader"
#define	STANDARD_BUMP_SHADER		"StandardBumpShader"
#define	STANDARD_3D_SHADER		"Standard3DShader"
#define	STANDARD_3D_INSTANCING_SHADER		"Standard3DInstancingShader"
#define	COLLIDER_SHADER		"ColliderShader"
#define	PARTICLE_SHADER		"ParticleShader"
#define	LANDSCAPE_SHADER	"LandScapeShader"

#define	DEBUG_SHADER		"DebugShader"

#define	LIGHTACC_DIR_SHADER	"LightAccDirShader"

// InputLayout Key
#define	POS_COLOR_LAYOUT	"PosColorLayout"
#define	POS_NORMAL_COLOR_LAYOUT	"PosNormalColorLayout"
#define	POS_LAYOUT			"PosLayout"
#define	POS_UV_LAYOUT		"PosUVLayout"
#define	VERTEX3D_LAYOUT		"Vertex3DLayout"
#define	VERTEX3D_STATIC_INSTANCING_LAYOUT		"Vertex3DStaticInstancingLayout"

// Sampler Key
#define	SAMPLER_LINEAR		"SamplerLinear"
#define	SAMPLER_POINT		"SamplerPoint"

template <typename T>
void Safe_Delete_VecList(T& p)
{
	T::iterator	iter;
	T::iterator	iterEnd = p.end();

	for (iter = p.begin(); iter != iterEnd; ++iter)
	{
		SAFE_DELETE((*iter));
	}

	p.clear();
}

template <typename T>
void Safe_Delete_Array_VecList(T& p)
{
	T::iterator	iter;
	T::iterator	iterEnd = p.end();

	for (iter = p.begin(); iter != iterEnd; ++iter)
	{
		SAFE_DELETE_ARRAY((*iter));
	}

	p.clear();
}

template <typename T>
void Safe_Release_VecList(T& p)
{
	T::iterator	iter;
	T::iterator	iterEnd = p.end();

	for (iter = p.begin(); iter != iterEnd; ++iter)
	{
		SAFE_RELEASE((*iter));
	}

	p.clear();
}

template <typename T>
void Safe_Delete_Map(T& p)
{
	T::iterator	iter;
	T::iterator	iterEnd = p.end();

	for (iter = p.begin(); iter != iterEnd; ++iter)
	{
		SAFE_DELETE(iter->second);
	}

	p.clear();
}

template <typename T>
void Safe_Delete_Array_Map(T& p)
{
	T::iterator	iter;
	T::iterator	iterEnd = p.end();

	for (iter = p.begin(); iter != iterEnd; ++iter)
	{
		SAFE_DELETE_ARRAY(iter->second);
	}

	p.clear();
}

template <typename T>
void Safe_Release_Map(T& p)
{
	T::iterator	iter;
	T::iterator	iterEnd = p.end();

	for (iter = p.begin(); iter != iterEnd; ++iter)
	{
		SAFE_RELEASE(iter->second);
	}

	p.clear();
}

