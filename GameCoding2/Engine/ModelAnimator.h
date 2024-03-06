#pragma once
#include "Component.h"

class Model;

struct AnimTransform
{	
	// [ ][ ][ ][ ][ ][ ][ ] ... 250개
	using TransformArrayType = array<Matrix, MAX_MODEL_TRANSFORMS>; // 뼈가 최대 250개
	// [ ][ ][ ][ ][ ][ ][ ] ... 500 개
	array<TransformArrayType, MAX_MODEL_KEYFRAMES> transforms; // 500프레임에 프레임 당 250개의 뼈대가 있는 거
};

class ModelAnimator : public Component
{
	using Super = Component;

public:
	ModelAnimator(shared_ptr<Shader> shader);
	~ModelAnimator();

	virtual void Update() override;

	void SetModel(shared_ptr<Model> model);
	void SetPass(uint8 pass) { _pass = pass; }

private:
	void CreateTexture(); 
	void CreateAnimationTransform(uint32 index); 

private:
	vector<AnimTransform> _animTransforms; // 애니메이션 갯수만큼 2차 배열이 늘어난다.
	ComPtr<ID3D11Texture2D> _texture; 
	ComPtr<ID3D11ShaderResourceView> _srv; 


private:
	shared_ptr<Shader>	_shader;
	uint8				_pass = 0;
	shared_ptr<Model>	_model;
};
