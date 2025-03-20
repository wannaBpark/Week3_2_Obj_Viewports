#pragma comment(lib, "user32")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")

#include "Core/Engine.h"


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd)
{
	// 사용 안하는 파라미터들
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nShowCmd);

	UEngine& Engine = UEngine::Get();
	Engine.Initialize(hInstance, L"Jungle Engine", L"JungleWindow", 1400, 800);

	Engine.Run();

	Engine.Shutdown();

    return 0;
}
