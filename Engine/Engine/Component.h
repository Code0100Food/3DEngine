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
	
	Component(COMPONENT_TYPE _type = COMP_NONE);
	Component(const Component& cpy);
	~Component();

public:

	bool operator ==(const Component& target);

public:

	virtual bool Update();

protected:

	bool			actived = true;
	COMPONENT_TYPE	type = COMP_NONE;
	GameObject*		parent = nullptr;

public:

	//Set Methods -----------
	void SetActiveState(bool act);
	void SetParent(const GameObject* target);

	//Get Methods -----------
	COMPONENT_TYPE GetType()const;

};
#endif // !_COMPONENT_H_
