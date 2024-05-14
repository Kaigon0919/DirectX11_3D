
#pragma once

#include "Engine.h"
#include "resource.h"

#ifdef _WIN64
#ifdef _DEBUG
#pragma comment(lib, "D3DEngine64_Debug")
#else
#pragma comment(lib, "D3DEngine64")
#endif // _DEBUG
#else
#ifdef _DEBUG
#pragma comment(lib, "D3DEngine_Debug")
#else
#pragma comment(lib, "D3DEngine")
#endif // _DEBUG
#endif // _WIN64


