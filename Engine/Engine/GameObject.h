#ifndef _GAME_OBJECT_H_
#define _GAME_OBJECT_H_

#include "Globals.h"
#include <list>
#include <string>

class Component;

class GameObject
{
public:

	GameObject();
	GameObject(const GameObject& cpy);
	~GameObject();

public:

	bool Update();

private:

	bool					actived = true;
	std::string				name = "Unnamed";
	std::list<Component*>	components;

public:

	//Set Methods -----------
	void SetActiveState(bool act);

	//Functionality ---------
	void PushComponent(const Component* cmp);
	void RemoveComponent(const Component* cmp);

};
#endif // !_GAME_OBJECT_H_
