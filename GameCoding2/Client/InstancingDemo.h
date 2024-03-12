#pragma once
class InstancingDemo : public IExecute
{
public:
	void Init() override;
	void Update() override;
	void Render() override;

private:
	shared_ptr<Shader> _shader;
	shared_ptr<GameObject> _camera;
	vector<shared_ptr<GameObject>> _objs;

private:
	// INSTANCING
	shared_ptr<Mesh> _mesh; // 원본 메시
	shared_ptr<Material> _material; 

	vector<Matrix> _worlds; // 물체들의 온갖 transform 정보, 월드 변환행렬을 들고 있는 거  
	shared_ptr<VertexBuffer> _instanceBuffer; 
};

