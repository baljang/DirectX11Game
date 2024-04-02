#include "pch.h"
#include "Transform.h"

Transform::Transform() : Super(ComponentType::Transform)
{
}

Transform::~Transform()
{
}

void Transform::Awake()
{
}

void Transform::Update()
{
}

Vec3 Transform::ToEulerAngles(Quaternion q)
{
	Vec3 angles;

	// roll (x-axis rotation)
	double sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
	double cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
	angles.x = std::atan2(sinr_cosp, cosr_cosp);

	// pitch (y-axis rotation)
	double sinp = std::sqrt(1 + 2 * (q.w * q.y - q.x * q.z));
	double cosp = std::sqrt(1 - 2 * (q.w * q.y - q.x * q.z));
	angles.y = 2 * std::atan2(sinp, cosp) - 3.14159f / 2;

	// yaw (z-axis rotation)
	double siny_cosp = 2 * (q.w * q.z + q.x * q.y);
	double cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
	angles.z = std::atan2(siny_cosp, cosy_cosp);

	return angles;
}


void Transform::UpdateTransform()
{
	Matrix matScale = Matrix::CreateScale(_localScale);
	Matrix matRotation = Matrix::CreateRotationX(_localRotation.x);
	matRotation *= Matrix::CreateRotationY(_localRotation.y);
	matRotation *= Matrix::CreateRotationZ(_localRotation.z); // x,y,z 축 회전 행렬 곱하는 순서는 상관 없다. 
	Matrix matTranslation = Matrix::CreateTranslation(_localPosition);

	_matLocal = matScale * matRotation * matTranslation; // 아직 부모로까진 못 간 것이다. 
	// 부모가 없는 상황이라면 어떻게 되는걸까?
	if (HasParent())
	{
		_matWorld = _matLocal * _parent->GetWorldMatrix(); 
	}
	else
	{
		_matWorld = _matLocal; // _matLocal이 _matWorld가 된다. 
	}

	Quaternion quat; 
	_matWorld.Decompose(_scale, quat, _position);
	_rotation = ToEulerAngles(quat); 

	
	// Children
	for (const shared_ptr<Transform>& child : _children)
		child->UpdateTransform(); 
}

void Transform::SetScale(const Vec3& worldScale)
{
	if (HasParent())
	{
		Vec3 parentScale = _parent->GetScale(); 
		Vec3 scale = worldScale; 
		scale.x /= parentScale.x; 
		scale.y /= parentScale.y; 
		scale.z /= parentScale.z; 		
		SetLocalScale(scale); 
	}
	else
	{
		SetLocalScale(worldScale); 
	}
}

void Transform::SetRotation(const Vec3& worldRotation)
{
	if (HasParent())
	{
		Matrix inverseMatrix = _parent->GetWorldMatrix().Invert(); // world에서 부모의 좌표계로 가는 행렬

		Vec3 rotation;
		rotation.TransformNormal(worldRotation, inverseMatrix);

		SetLocalRotation(rotation);
	}
	else
	{
		SetLocalRotation(worldRotation);
	}
}

void Transform::SetPosition(const Vec3& worldPosition)
{
	if (HasParent())
	{
	    Matrix worldToParentLocalMatrix = _parent->GetWorldMatrix().Invert(); // world에서 부모의 좌표계로 가는 행렬
	
		Vec3 position; 
		position.Transform(worldPosition, worldToParentLocalMatrix); 

		SetLocalPosition(position); 
	}
	else
	{
		SetLocalPosition(worldPosition); 
	}
}
