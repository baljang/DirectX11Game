#pragma once
#include "Component.h"
class Button :  public Component
{
	using Super = Component; 

public: 
	Button(); 
	virtual ~Button(); 

	bool Picked(POINT screenPos); 

	void Create(Vec2 screenPos, Vec2 size, shared_ptr<class Material> material); 

	void AddOnClickedEvent(std::function<void(void)> func); 
	void InvokeOnClicked(); // 버튼이 눌렸으니까 등록된 함수가 있으면 호출하라. C#으로 치면 delegate

private: 
	std::function<void(void)> _onClicked; 
	RECT _rect; 

};

