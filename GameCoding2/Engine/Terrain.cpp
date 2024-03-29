#include "pch.h"
#include "Terrain.h"
#include "MeshRenderer.h"
#include "Camera.h"

Terrain::Terrain() : Super(ComponentType::Terrain)
{
}

Terrain::~Terrain()
{
}

void Terrain::Create(int32 sizeX, int32 sizeZ, shared_ptr<Material> material)
{
	_sizeX = sizeX; 
	_sizeZ = sizeZ; 

	auto go = _gameObject.lock();  // weak_ptr는 관리하는 객체가 이미 소멸되었는지 여부를 확인할 수 있도록 하기 위해 lock()을 사용하여 shared_ptr로 변환해야 한다.

	go->GetOrAddTransform(); 

	if (go->GetMeshRenderer() == nullptr)
		go->AddComponent(make_shared<MeshRenderer>()); 

	_mesh = make_shared<Mesh>(); 
	_mesh->CreateGrid(sizeX, sizeZ); 

	go->GetMeshRenderer()->SetMesh(_mesh); 
	go->GetMeshRenderer()->SetPass(0); 
	go->GetMeshRenderer()->SetMaterial(material);
}

bool Terrain::Pick(int32 screenX, int32 screenY, Vec3& pickPos, float& distance)
{
	Matrix W = GetTransform()->GetWorldMatrix(); 
	Matrix V = Camera::S_MatView; 
	Matrix P = Camera::S_MatProjection; 

	Viewport& vp = GRAPHICS->GetViewport(); 

	// 3D로 변환된 좌표
	Vec3 n = vp.Unproject(Vec3(screenX, screenY, 0), W, V, P); //깊이값이 0이면 near
	Vec3 f = vp.Unproject(Vec3(screenX, screenY, 1), W, V, P); //깊이값이 1이면 far

	Vec3 start = n; 
	Vec3 direction = f - n; 
	direction.Normalize(); 

	Ray ray = Ray(start, direction); 

	const auto& vertices = _mesh->GetGeometry()->GetVertices(); 

	for (int32 z = 0; z < _sizeZ; z++)
	{
		for (int32 x = 0; x < _sizeX; x++)
		{
			uint32 index[4]; 
			index[0] = (_sizeX + 1) * z + x; 
			index[1] = (_sizeX + 1) * z + x + 1; 
			index[2] = (_sizeX + 1) * (z + 1) + x; 
			index[3] = (_sizeX + 1) * (z + 1) + x + 1; 

			Vec3 p[4]; 
			for (int32 i = 0; i < 4; i++)
				p[i] = vertices[index[i]].position; 

			//  [0]
			//   |	\
			//  [2] - [1]
			if (ray.Intersects(p[0], p[1], p[2], OUT distance))
			{
				pickPos = ray.position + ray.direction * distance; 
				return true; 
			}

			//  [2] - [3]
			//   	\  |
			//		  [1]
			if (ray.Intersects(p[3], p[1], p[2], OUT distance)) // 순서는 상관 없다
			{
				pickPos = ray.position + ray.direction * distance;
				return true;
			}
		}
	}

	return false; 
}
