#include "pch.h"
#include "CameraMove.h"
#include "GameObject.h"

void CameraMove::Update()
{
	auto pos = GetTransform()->GetPosition(); 
	pos.x += 0.01f; 
	GetTransform()->SetPosition(pos);
}
