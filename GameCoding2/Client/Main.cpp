#include "pch.h"
#include "Main.h"
#include "Engine/Game.h"
#include "10. GlobalTestDemo.h"
#include "11. DepthStencilDemo.h"
#include "12. AmbientDemo.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	GameDesc desc; 
	desc.appName = L"GameCoding";
	desc.hInstance = hInstance; 
	desc.vsync = false;
	desc.hWnd = NULL; 
	desc.width = 800; 
	desc.height = 600; 
	desc.clearColor = Color(0.f, 0.f, 0.f, 0.f);
	desc.app = make_shared<AmbientDemo>(); // 실행 단위

	GAME->Run(desc); 

	return 0; 
}