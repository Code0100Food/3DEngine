#ifndef _SCENE_H_
#define _SCENE_H_

#include "Module.h"
#include "GameObject.h"

class ModuleScene : public Module
{
public:

	ModuleScene(const char* _name, MODULE_ID _id, bool _config_menu, bool _enabled = true);
	~ModuleScene();

public:

	bool			Start()final;
	update_status	Update(float dt)final;
	bool			CleanUp()final;

private:

	GameObject*		root_gameobject = nullptr;
	bool			hierarchy_win_state = false;
	bool			inspector_state = false;
	GameObject*		selected_gameobject = nullptr;

public:

	//Set Methods -----------
	void	SetSelectedGameObject(const GameObject* target);

	//Functionality ---------
	GameObject* CreateGameObject();
	bool		RemoveGameObject(GameObject* target, const GameObject* parent, bool search_in = true);

	void		BlitHierarchy();
	bool		GetHierarchyWinState()const;
	void		SwapHierarchyWinState();

	void		BlitInspector();
	bool		GetInspectorState()const;
	void		EnableInspector();
	void		DisableInspector();

};
#endif
