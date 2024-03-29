﻿#include "pch.h"
#include "Main.h"
#include "Engine/Game.h"
#include "SceneDemo.h"
#include "GroupDemo.h"
#include "TextureBufferDemo.h"
#include "StructuredBufferDemo.h"
#include "ViewportDemo.h"
#include "CollisionDemo.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	GameDesc desc; 
	desc.appName = L"GameCoding";
	desc.hInstance = hInstance; 
	desc.vsync = false;
	desc.hWnd = NULL; 
	desc.width = 800; 
	desc.height = 600; 
	desc.clearColor = Color(0.5f, 0.5f, 0.5f, 0.5f);
	desc.app = make_shared<CollisionDemo>(); // 실행 단위

	GAME->Run(desc); 

	return 0; 
}