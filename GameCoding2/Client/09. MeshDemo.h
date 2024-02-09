#pragma once
#include "IExecute.h"
#include "Geometry.h"

class MeshDemo : public IExecute
{	
public: 
	void Init() override; 
	void Update() override; 	 
	void Render() override; 

	shared_ptr<GameObject> _obj; 
	shared_ptr<GameObject> _camera;
};

