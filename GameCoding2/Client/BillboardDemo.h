#pragma once
class BillboardDemo : public IExecute
{
public:
	void Init() override;
	void Update() override;
	void Render() override;

private:
	shared_ptr<Shader> _shader;

};

#include "MonoBehaviour.h"
class BillboardTest : public MonoBehaviour
{
public:
	virtual void Update(); // GameObject를 조작할 수 있는 스크립트를 MonoBehaviour를 상속 받아 만든 다음에 걔를 컴포넌트로 붙여주면 작동을 했었던 아이다.
};

