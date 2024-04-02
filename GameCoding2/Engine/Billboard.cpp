#include "pch.h"
#include "Billboard.h"
#include "Material.h"
#include "Camera.h"

Billboard::Billboard() : Super(ComponentType::Billboard)
{
	int32 vertexCount = MAX_BILLBOARD_COUNT * 4; // 4각형 이니까 
	int32 indexCount = MAX_BILLBOARD_COUNT * 6; // 4각형은 3각형 2개이기 때문에 

	_vertices.resize(vertexCount); 
	_vertexBuffer = make_shared<VertexBuffer>(); 
	_vertexBuffer->Create(_vertices, 0, true); // 갱신할 수 있게 CPU write를 true로 켜줬다.

	_indices.resize(indexCount); 

	for (int32 i = 0; i < MAX_BILLBOARD_COUNT; i++)
	{
		_indices[i * 6 + 0] = i * 4 + 0;
		_indices[i * 6 + 1] = i * 4 + 1;
		_indices[i * 6 + 2] = i * 4 + 2;
		_indices[i * 6 + 3] = i * 4 + 2;
		_indices[i * 6 + 4] = i * 4 + 1;
		_indices[i * 6 + 5] = i * 4 + 3;
	}

	_indexBuffer = make_shared<IndexBuffer>(); 
	_indexBuffer->Create(_indices); 
}

Billboard::~Billboard()
{
}

void Billboard::Update()
{
	if (_drawCount != _prevCount)
	{
		_prevCount = _drawCount;

		D3D11_MAPPED_SUBRESOURCE subResource;
		DC->Map(_vertexBuffer->GetComPtr().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
		{
			memcpy(subResource.pData, _vertices.data(), sizeof(VertexBillboard) * _vertices.size());
		}
		DC->Unmap(_vertexBuffer->GetComPtr().Get(), 0);
	}

	auto shader = _material->GetShader();

	// Transform
	auto world = GetTransform()->GetWorldMatrix();
	shader->PushTransformData(TransformDesc{ world });

	// GlobalData
	shader->PushGlobalData(Camera::S_MatView, Camera::S_MatProjection);

	// Light
	_material->Update();

	// IA
	_vertexBuffer->PushData();
	_indexBuffer->PushData();

	// 인스턴싱을 하지 않는 이유는 빌보드를 포함하고 있는 객체를 늘리는게 아니라 빌보드하는 컴포넌트 안에서 동일하게 그려지는 애들을 다 묶어서 
	// 정점을 한번에 그리기 위해서 DrawIndexed를 한 번만 해주면 된다. 
	shader->DrawIndexed(0, _pass, _drawCount * 6);
}

void Billboard::Add(Vec3 position, Vec2 scale)
{
	// 같은 위치의 정점을 넣었기 때문에 아무것도 안그려질 것이다. 
	// 정점 갯수만 맞춰서 건내줄테니 shader에서 알아서 좌표 계산을 해서 위치를 배정하라는 말이다.
	// 이렇게 해야 풀 같은 거 각기 다른 크기의 다른 위치로 만들 수 있기 때문에 작업을 해주려고 하는 것이다.
	// CPU에서 모든 애들을 만들어서 넘기는게 아니라 언제든지 쉐이더 쪽에서 작업을 해주도록 유도를 해준다.
	_vertices[_drawCount * 4 + 0].position = position;
	_vertices[_drawCount * 4 + 1].position = position;
	_vertices[_drawCount * 4 + 2].position = position;
	_vertices[_drawCount * 4 + 3].position = position;

	_vertices[_drawCount * 4 + 0].uv = Vec2(0, 1);
	_vertices[_drawCount * 4 + 1].uv = Vec2(0, 0);
	_vertices[_drawCount * 4 + 2].uv = Vec2(1, 1);
	_vertices[_drawCount * 4 + 3].uv = Vec2(1, 0);

	_vertices[_drawCount * 4 + 0].scale = scale;
	_vertices[_drawCount * 4 + 1].scale = scale;
	_vertices[_drawCount * 4 + 2].scale = scale;
	_vertices[_drawCount * 4 + 3].scale = scale;

	_drawCount++;
}