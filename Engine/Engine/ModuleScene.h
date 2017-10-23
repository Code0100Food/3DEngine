#ifndef _SCENE_H_
#define _SCENE_H_

#include "Module.h"
#include "GameObject.h"
#include "Octree.h"
#include "Primitive_.h"

class ModuleScene : public Module
{
public:

	ModuleScene(const char* _name, MODULE_ID _id, bool _config_menu, bool _enabled = true);
	~ModuleScene();

public:

	bool			Start()final;
	bool			SceneUpdate(float dt);
	bool			CleanUp()final;

private:

	GameObject*				root_gameobject = nullptr;
	GameObject*				selected_gameobject = nullptr;

	bool					hierarchy_win_state = true;
	bool					inspector_state = true;

	Octree<GameObject*>		octree;

public:

	//Set Methods -----------
	void		SetSelectedGameObject(const GameObject* target);

	//Get Methods -----------
	GameObject*	GetSelectedGameObject()const;

	//Functionality ---------
	GameObject* CreateGameObject();
	bool		RemoveGameObject(GameObject* target, const GameObject* parent, bool search_in = true);
	GameObject* CreatePrimitive(PRIMITIVE_TYPE type, uint divisions = 2);
	
	void		BlitHierarchy();
	bool		GetHierarchyWinState()const;
	void		SwapHierarchyWinState();

	void		BlitInspector();
	bool		GetInspectorState()const;
	void		EnableInspector();
	void		DisableInspector();

	bool		IsRoot(const GameObject* target)const;
	GameObject* GetRoot()const;

	void		PushGameObjectInOctree(GameObject* target, bool childs = true);
	void		ReFillOctree();
	void		CleanOctree();
};
#endif
