#include "pch.h"
#include "08. NormalDemo.h"
#include "GeometryHelper.h"
#include "Camera.h"
#include "CameraScript.h"

void NormalDemo::Init()
{
	_shader = make_shared<Shader>(L"07. Normal.fx");

	// Object
	_geometry = make_shared<Geometry<VertexTextureNormalData>>();
	//GeometryHelper::CreateCube(_geometry); 
	GeometryHelper::CreateSphere(_geometry); 
	_vertexBuffer = make_shared<VertexBuffer>(); 
	_vertexBuffer->Create(_geometry->GetVertices()); 
	_indexBuffer = make_shared<IndexBuffer>(); 
	_indexBuffer->Create(_geometry->GetIndices()); 

	// Camera
	_camera = make_shared<GameObject>(); 
	_camera->GetOrAddTransform(); 
	_camera->AddComponent(make_shared<Camera>()); 
	_camera->AddComponent(make_shared<CameraScript>()); 

	// Texture
	_texture = RESOURCES->Load<Texture>(L"Veigar", L"..\\Resources\\Textures\\veigar.jpg");
}

void NormalDemo::Update()
{
	_camera->Update(); 
}

void NormalDemo::Render()
{
	// ? 
	_shader->GetMatrix("World")->SetMatrix((float*)&_world);
	_shader->GetMatrix("View")->SetMatrix((float*)&Camera::S_MatView);
	_shader->GetMatrix("Projection")->SetMatrix((float*)&Camera::S_MatProjection);
	_shader->GetSRV("Texture0")->SetResource(_texture->GetComPtr().Get()); 
	_shader->GetVector("LightDir")->SetFloatVector((float*)&_lightDir); 

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
	_shader->DrawIndexed(0,0, _indexBuffer->GetCount(), 0, 0); 
}
