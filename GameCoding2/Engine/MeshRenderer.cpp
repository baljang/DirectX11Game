#include "pch.h"
#include "MeshRenderer.h"
#include "Camera.h"
#include "Game.h"
#include "Mesh.h"
#include "Shader.h"
#include "Material.h"
#include "Light.h"

MeshRenderer::MeshRenderer() : Super(ComponentType::MeshRenderer)
{

}

MeshRenderer::~MeshRenderer()
{

}

//void MeshRenderer::Update()
//{
//	if (_mesh == nullptr || _texture == nullptr || _shader == nullptr)
//		return;
//
//	_shader->GetSRV("Texture0")->SetResource(_texture->GetComPtr().Get());
//
//	auto world = GetTransform()->GetWorldMatrix();
//	RENDER->PushTransformData(TransformDesc{ world }); 
//	//_shader->GetMatrix("World")->SetMatrix((float*)&world);
//	
//	uint32 stride = _mesh->GetVertexBuffer()->GetStride();
//	uint32 offset = _mesh->GetVertexBuffer()->GetOffset();
//
//	DC->IASetVertexBuffers(0, 1, _mesh->GetVertexBuffer()->GetComPtr().GetAddressOf(), &stride, &offset);
//	DC->IASetIndexBuffer(_mesh->GetIndexBuffer()->GetComPtr().Get(), DXGI_FORMAT_R32_UINT, 0);
//
//	_shader->DrawIndexed(0, 0, _mesh->GetIndexBuffer()->GetCount(), 0, 0);
//}

//void MeshRenderer::Update()
//{
//	if (_mesh == nullptr || _texture == nullptr || _shader == nullptr)
//		return;
//
//	_shader->GetSRV("DiffuseMap")->SetResource(_texture->GetComPtr().Get());
//
//	auto world = GetTransform()->GetWorldMatrix();
//	RENDER->PushTransformData(TransformDesc{ world }); 
//	//_shader->GetMatrix("World")->SetMatrix((float*)&world);
//	
//	uint32 stride = _mesh->GetVertexBuffer()->GetStride();
//	uint32 offset = _mesh->GetVertexBuffer()->GetOffset();
//
//	DC->IASetVertexBuffers(0, 1, _mesh->GetVertexBuffer()->GetComPtr().GetAddressOf(), &stride, &offset);
//	DC->IASetIndexBuffer(_mesh->GetIndexBuffer()->GetComPtr().Get(), DXGI_FORMAT_R32_UINT, 0);
//
//	_shader->DrawIndexed(0, 0, _mesh->GetIndexBuffer()->GetCount(), 0, 0);
//}

void MeshRenderer::RenderInstancing(shared_ptr<class InstancingBuffer>& buffer)
{
	if (_mesh == nullptr || _material == nullptr)
		return; 

	auto shader = _material->GetShader(); 
	if (shader == nullptr)
		return; 

	// GlobalData
	shader->PushGlobalData(Camera::S_MatView, Camera::S_MatProjection); 

	// Light
	auto lightObj = SCENE->GetCurrentScene()->GetLight(); 
	if(lightObj)
		shader->PushLightData(lightObj->GetLight()->GetLightDesc());

	// Light
	_material->Update(); 

	_mesh->GetVertexBuffer()->PushData(); 
	_mesh->GetIndexBuffer()->PushData(); 

	buffer->PushData(); // instancing buffer에서 pushData를 하면 world이긴 한데 나의 world가 아니라 10000개라면 10000개 모두의 world가 들어가 있다. 그걸 밀어 넣는거 

	shader->DrawIndexedInstanced(0, _pass, _mesh->GetIndexBuffer()->GetCount(), buffer->GetCount());  
}

InstanceID MeshRenderer::GetInstanceID()
{
	return make_pair((uint64)_mesh.get(), (uint64)_material.get()); // get()으로 포인터를 얻어올 수 있다. 주소값 2개로 짝을 이뤄 아이디를 발급해줬다.
	// 극단적인 예외로 메모리 풀링을 하는데 머테리얼을 사용하다가 머테리얼을 삭제했고, 다시 발급을 했는데 같은 주소를 차지하는 확률을 생각하면 이방식이 맞진 않지만 여기선 이렇게 한다. 
	// mesh랑 material은 리소스로 계속 들고 있을 거라 괜찮지만 서버를 만들 때는 극악의 확률도 생각해서 만들어야 한다. 
}
