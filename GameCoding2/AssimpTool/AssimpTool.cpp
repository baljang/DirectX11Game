#include "pch.h"
#include "AssimpTool.h"
#include "Converter.h"

void AssimpTool::Init()
{

	{
		shared_ptr<Converter> converter = make_shared<Converter>(); 

		// FBX -> Memory
		converter->ReadAssetFile(L"Tank/Tank.fbx");

		// Memory -> CustomData (File)
		// 1차 목표
		converter->ExportMaterialData(L"Tank/Tank"); // xml 안붙이는 이유는 통일이 되어 있기 때문
		// Material이 필요 없으면 이 코드는 주석 처리하면 된다. Material만 하고 싶으면 얘만 놔두면 된다.
		// 읽은 데이터를 추출해서 우리만의 커스텀 데이터 파일로 만들어 주는게 목적이다.
		converter->ExportModelData(L"Tank/Tank");

		// CustomData (File) -> Memory
	}

	{
		shared_ptr<Converter> converter = make_shared<Converter>();

		// FBX -> Memory
		converter->ReadAssetFile(L"Tower/Tower.fbx");

		// Memory -> CustomData (File)
		// 1차 목표
		converter->ExportMaterialData(L"Tower/Tower");
		converter->ExportModelData(L"Tower/Tower");

		// CustomData (File) -> Memory
	}
}

void AssimpTool::Update()
{
}

void AssimpTool::Render()
{
}
