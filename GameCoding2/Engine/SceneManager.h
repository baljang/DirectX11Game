#pragma once
#include "Scene.h"

class SceneManager
{
	DECLARE_SINGLE(SceneManager);
	 
public: 
	void Update(); 

	template<typename T>
	void ChangeScene(shared_ptr<T> scene)
	{
		_currentScene = scene; 
		scene->Start(); 	// 분기별로 실행되는 함수들 넣어준다. 
	}

	shared_ptr<Scene> GetCurrentScene() { return _currentScene;  }

private: 
	shared_ptr<Scene> _currentScene = make_shared<Scene>(); 

};

