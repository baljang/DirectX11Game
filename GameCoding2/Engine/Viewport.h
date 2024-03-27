#pragma once

class Viewport
{
public: 
	Viewport(); 
	Viewport(float width, float height, float x = 0, float y = 0, float minDepth = 0, float maxDepth = 1); 
	~Viewport(); 

	void RSSetViewport(); // 현재 설정된 viewport 값에 따라가지고 RS에서 설정, 원래 Graphics::RenderBegin에서 해줌
	void Set(float width, float height, float x = 0, float y = 0, float minDepth = 0, float maxDepth = 1);

	float GetWidth() { return _vp.Width; } 
	float GetHeight() { return _vp.Height; } 

	// 공식 행렬을 그대로 넣어주면 된다.
	Vec3 Project(const Vec3& pos, const Matrix& W, const Matrix& V, const Matrix& P); // View까지 안가고 World에서 하는게 편할 수도 있어서 버전을 여러개 파주기도 한다.
	// 반대로 2D 좌표에서 시작을 해서 깊이 값을 이용해서 3D로 넘어오는 작업
	Vec3 Unproject(const Vec3& pos, const Matrix& W, const Matrix& V, const Matrix& P); 


private: 
	D3D11_VIEWPORT _vp; 

};

