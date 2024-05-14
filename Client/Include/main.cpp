
#include "Client.h"
#include "Core.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "SceneComponent/00.StartScene.h"
#include "SceneComponent\MainScene.h"

#ifdef _DEBUG
#ifdef UNICODE
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console") 
#else
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console") 
#endif
#endif


ENGINE_USING

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	if (!GET_SINGLE(CCore)->Init(hInstance, TEXT("Client"), TEXT("Client"),	1280, 720, IDI_ICON1, IDI_ICON1, true))
	{
		DESTROY_SINGLE(CCore);
		return 0;
	}

	GET_SINGLE(CSceneManager)->AddSceneComponent<CMainScene>();
	
	int	iRet = GET_SINGLE(CCore)->Run();

	DESTROY_SINGLE(CCore);

	return iRet;
}
