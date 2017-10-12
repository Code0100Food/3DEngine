#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include "Globals.h"

class GameObject;

enum COMPONENT_TYPE
{
	COMP_NONE = 0,
	COMP_TRANSFORMATION,
	COMP_MESH,
	COMP_MATERIAL
};

class Component
{
public:
	
	Component();
	Component(const Component& cpy);
	~Component();

public:

	bool operator ==(const Component& target);

public:

	virtual bool Update();

private:

	bool			actived = true;
	COMPONENT_TYPE	type = COMP_NONE;
	GameObject*		parent = nullptr;

public:

	//Set Methods -----------
	void SetActiveState(bool act);
	void SetParent(const GameObject* target);

};
#endif // !_COMPONENT_H_
