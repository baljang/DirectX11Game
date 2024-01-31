#include "pch.h"
#include "01. TriangleDemo.h"

void TriangleDemo::Init()
{
	_shader = make_shared<Shader>(L"01. Triangle.fx");

	{
		_vertices.resize(3); 

		_vertices[0].position = Vec3{ -0.5f, 0.f, 0.f }; 
		_vertices[1].position = Vec3{ 0.f, 0.5f, 0.f }; 
		_vertices[2].position = Vec3{ 0.5f, 0.f, 0.f }; 
	}

	_buffer = make_shared<VertexBuffer>(); 
	_buffer->Create(_vertices); 
}

void TriangleDemo::Update()
{
}

void TriangleDemo::Render()
{
	uint32 stride = _buffer->GetStride(); 
	uint32 offset = _buffer->GetOffset(); 

	// DeviceContext에서 IA단계에서 사용할 VertextBuffer를 묶어주는 함수였다.
	DC->IASetVertexBuffers(0, 1, _buffer->GetComPtr().GetAddressOf(), &stride, &offset);

	// 원래 그렸던 device를 이용한 Draw에서는 technique, pass라는 인자가 없었어. 
	// 이거 만들어준 Shader라는 클래스에 포함이 되어 있는 기능이다. 
	// technique이란 pass를 골라줄 수 있다는 특징이 있다. 
	// _buffer->GetCount()이걸로 넣어줘도 되지만 몇 개 없으니 3으로 넣어준다.
	_shader->Draw(1, 0, 3); 
}
