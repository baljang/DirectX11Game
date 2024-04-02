#pragma once
#include "Component.h"

struct VertexBillboard
{
	Vec3 position; 
	Vec2 uv; 
	Vec2 scale; 
};

#define MAX_BILLBOARD_COUNT 500

class Billboard : public Component
{
	using Super = Component; 

public: 
	Billboard(); 
	~Billboard(); 
	
	void Update(); 
	void Add(Vec3 position, Vec2 scale); // 풀 위치가 변화하지 않고 고정한다 가정
	
	void SetMaterial(shared_ptr<Material> material) { _material = material; }
	void SetPass(uint8 pass) { _pass = pass; }

private: 
	vector<VertexBillboard> _vertices; 
	vector<uint32> _indices; 
	shared_ptr<VertexBuffer> _vertexBuffer; 
	shared_ptr<IndexBuffer> _indexBuffer; 

	int32 _drawCount = 0;
	int32 _prevCount = 0; 

	shared_ptr<Material> _material; 
	uint8 _pass = 0; 
};

