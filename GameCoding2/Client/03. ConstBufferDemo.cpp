#include "pch.h"
#include "03. ConstBufferDemo.h"
#include "GeometryHelper.h"

void ConstBufferDemo::Init()
{
	_shader = make_shared<Shader>(L"03. ConstBuffer.fx");

	_geometry = make_shared<Geometry<VertexColorData>>(); 
	GeometryHelper::CreateQuad(_geometry, Color(0.f, 1.f, 0.f, 1.f)); 

	_vertexBuffer = make_shared<VertexBuffer>(); 
	_vertexBuffer->Create(_geometry->GetVertices()); 

	_indexBuffer = make_shared<IndexBuffer>(); 
	_indexBuffer->Create(_geometry->GetIndices()); 
}

void ConstBufferDemo::Update()
{
	float dt = TIME->GetDeltaTime();

	if (INPUT->GetButton(KEY_TYPE::A))
	{
		_translation.x -= 3.f * dt;
	}
	else if (INPUT->GetButton(KEY_TYPE::D))
	{
		_translation.x += 3.f * dt;
	}
	else if (INPUT->GetButton(KEY_TYPE::W))
	{
		_translation.y += 3.f * dt;
	}
	else if (INPUT->GetButton(KEY_TYPE::S))
	{
		_translation.y -= 3.f * dt;
	}

	// SRT
// 지금은 Scale, Rotation은 Identity 행렬이라고 하고 
	_world = Matrix::CreateTranslation(_translation);
}

void ConstBufferDemo::Render()
{
	// ? 
	_shader->GetMatrix("World")->SetMatrix((float*)&_world);
	_shader->GetMatrix("View")->SetMatrix((float*)&_view);
	_shader->GetMatrix("Projection")->SetMatrix((float*)&_projection);

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
	_shader->DrawIndexed(0, 0, _indexBuffer->GetCount(), 0, 0); 
}
