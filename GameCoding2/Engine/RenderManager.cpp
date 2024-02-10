#include "pch.h"
#include "RenderManager.h"
#include "Camera.h"

void RenderManager::Init(shared_ptr<Shader> shader)
{
	_shader = shader; 

	_globalBuffer = make_shared<ConstantBuffer<GlobalDesc>>(); 
	_globalBuffer->Create(); 
	_globalEffectBuffer = _shader->GetConstantBuffer("GlobalBuffer"); // Global.fx의 GlobalBuffer가 매핑이 된다. 

	_transformBuffer = make_shared<ConstantBuffer<TransformDesc>>();
	_transformBuffer->Create();
	_transformEffectBuffer = _shader->GetConstantBuffer("TransformBuffer");
}

void RenderManager::Update()
{
	PushGlobalData(Camera::S_MatView, Camera::S_MatProjection);
}

void RenderManager::PushGlobalData(const Matrix& view, const Matrix& projection)
{
	_globalDesc.V = view;
	_globalDesc.P = projection;
	_globalDesc.VP = view * projection; 
	_globalBuffer->CopyData(_globalDesc); 
	_globalEffectBuffer->SetConstantBuffer(_globalBuffer->GetComPtr().Get()); // 00. Global.fx의 GlobalBuffer로  밀어 넣는 작업을 해주고 있는 거다. 
}

void RenderManager::PushTransformData(const TransformDesc& desc)
{
	_transformDesc = desc; 
	_transformBuffer->CopyData(_transformDesc); 
	_transformEffectBuffer->SetConstantBuffer(_transformBuffer->GetComPtr().Get()); 
}
