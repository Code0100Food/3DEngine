#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include "Globals.h"
#include "imgui/imgui.h"
#include "Parson/parson.h"

class GameObject;
class Serializer;

enum COMPONENT_TYPE
{
	COMP_NONE = 0,
	COMP_TRANSFORMATION,
	COMP_MESH,
	COMP_PRIMITIVE_MESH,
	COMP_CUBE_MESH,
	COMP_SPHERE_MESH,
	COMP_CYLINDER_MESH,
	COMP_FRUSTUM_MESH,
	COMP_MESH_RENDERER,
	COMP_MATERIAL,
	COMP_CAMERA
};

class Component
{
public:
	
	Component(COMPONENT_TYPE _type = COMP_NONE);
	Component(const Component& cpy);
	virtual ~Component();

public:

	bool operator ==(const Component& target);

public:

	virtual bool Start();
	virtual bool Update();

protected:

	bool			actived = true;
	COMPONENT_TYPE	type = COMP_NONE;
	GameObject*		parent = nullptr;
	uint			id = 0;

public:

	//Set Methods -----------
	void SetActiveState(bool act);
	void SetParent(const GameObject* target);

	//Get Methods -----------
	COMPONENT_TYPE	GetType()const;
	bool			GetActive()const;
	uint			GetID()const;

	//Functionality ---------
	//UI Methods
	virtual void BlitComponentInspector();
	
	//Save/Load Methods -----
	virtual bool	Save(Serializer& array_root)const;
	bool			Load(const JSON_Object* root);

	//Enums Methods ---------
	COMPONENT_TYPE	StrToComponentType(const char* str)const;
	const char*		ComponentTypeToStr(COMPONENT_TYPE type)const;
	
};
#endif // !_COMPONENT_H_
