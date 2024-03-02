#pragma once
#include "Component.h"

class Model;
class Shader;
class Material;

class ModelRenderer : public Component
{
	using Super = Component;

public:
	ModelRenderer(shared_ptr<Shader> shader);
	virtual ~ModelRenderer();

	virtual void Update() override;

	void SetModel(shared_ptr<Model> model);
	void SetPass(uint8 pass) { _pass = pass; }

private:
	shared_ptr<Shader>	_shader;
	uint8				_pass = 0; // 렌더 패스를 0번으로 할지 1번으로 할지
	shared_ptr<Model>	_model;
};
