#include "pch.h"
#include "07. HeightMapDemo.h"
#include "GeometryHelper.h"
#include "Camera.h"
#include "CameraScript.h"

void HeightMapDemo::Init()
{
	_shader = make_shared<Shader>(L"06. Terrain.fx");

	// Texture
	_heightMap = RESOURCES->Load<Texture>(L"Height", L"..\\Resources\\Textures\\Terrain\\height.png");
	_texture = RESOURCES->Load<Texture>(L"Grass", L"..\\Resources\\Textures\\Terrain\\grass.jpg");

	const int32 width = _heightMap->GetSize().x; 
	const int32 height = _heightMap->GetSize().y; 

	const DirectX::ScratchImage& info = _heightMap->GetInfo(); 
	uint8* pixelBuffer = info.GetPixels(); 

	// Object
	_geometry = make_shared<Geometry<VertexTextureData>>(); 
	GeometryHelper::CreateGrid(_geometry, width, height); 

	// CPU
	{
		vector<VertexTextureData>& v = const_cast<vector<VertexTextureData>&>(_geometry->GetVertices()); 
	
		for (int32 z = 0; z < height; z++)
		{
			for (int32 x = 0; x < width; x++)
			{
				int32 idx = width * z + x; 
				uint8 height = pixelBuffer[idx] / 255.f * 25.f; 
				v[idx].position.y = height; // 높이 보정
			}
		}
	}

	_vertexBuffer = make_shared<VertexBuffer>(); 
	_vertexBuffer->Create(_geometry->GetVertices()); 
	_indexBuffer = make_shared<IndexBuffer>(); 
	_indexBuffer->Create(_geometry->GetIndices()); 

	// Camera
	_camera = make_shared<GameObject>(); 
	_camera->GetOrAddTransform(); 
	_camera->AddComponent(make_shared<Camera>()); 
	_camera->AddComponent(make_shared<CameraScript>()); 

	_camera->GetTransform()->SetPosition(Vec3(0.f, 5.f, 0.f)); 
	_camera->GetTransform()->SetRotation(Vec3(25.f, 0.f, 0.f)); 
}

void HeightMapDemo::Update()
{
	_camera->Update(); 
}

void HeightMapDemo::Render()
{
	// ? 
	_shader->GetMatrix("World")->SetMatrix((float*)&_world);
	_shader->GetMatrix("View")->SetMatrix((float*)&Camera::S_MatView);
	_shader->GetMatrix("Projection")->SetMatrix((float*)&Camera::S_MatProjection);
	_shader->GetSRV("Texture0")->SetResource(_texture->GetComPtr().Get()); 

	uint32 stride = _vertexBuffer->GetStride(); 
	uint32 offset = _vertexBuffer->GetOffset(); 

	// DeviceContext에서 IA단계에서 사용할 VertextBuffer를 묶어주는 함수였다.
	DC->IASetVertexBuffers(0, 1, _vertexBuffer->GetComPtr().GetAddressOf(), &stride, &offset);
	DC->IASetIndexBuffer(_indexBuffer->GetComPtr().Get(), DXGI_FORMAT_R32_UINT, 0);

	// 원래 그렸던 device를 이용한 Draw에서는 technique, pass라는 인자가 없었어. 
	// 이거 만들어준 Shader라는 클래스에 포함이 되어 있는 기능이다. 
	// technique이란 pass를 골라줄 수 있다는 특징이 있다. 
	// _buffer->GetCount()이걸로 넣어줘도 되지만 몇 개 없으니 3으로 넣어준다.
	//_shader->Draw(1, 0, 3); 
	_shader->DrawIndexed(0,1, _indexBuffer->GetCount(), 0, 0); 
}
