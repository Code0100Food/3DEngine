#ifndef _SCENE_H_
#define _SCENE_H_

#include "Module.h"
#include "GameObject.h"
#include "Octree.h"
#include "Primitive_.h"

enum SCENE_UPDATE_STATE
{
	UNDEF_SCENE_STATE = 0,
	EDIT_SCENE_STATE,
	PLAY_SCENE_STATE,
	PAUSE_SCENE_STATE,
	NEXT_SCENE_STATE
};

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

	SCENE_UPDATE_STATE		scene_update_state = EDIT_SCENE_STATE;

	GameObject*				root_gameobject = nullptr;
	GameObject*				selected_gameobject = nullptr;

	uint					play_tex_id = 0, stop_tex_id = 0, next_tex_id = 0;
	bool					hierarchy_win_state = true;
	bool					inspector_state = true;

	Octree<GameObject*>			octree;
	std::vector<GameObject*>	static_objects;

	//Vectors used during serialization
	std::vector<std::pair<GameObject*, uint>>	objects_links;
	std::vector<std::pair<Component*, uint>>	components_links;

public:

	//Set Methods -----------
	void		SetSelectedGameObject(const GameObject* target);

	//Get Methods -----------
	GameObject*	GetSelectedGameObject()const;

	//Functionality ---------
	GameObject* CreateGameObject();
	bool		RemoveGameObject(GameObject* target, const GameObject* parent, bool search_in = true);
	GameObject* FindGameObject(uint id)const;
	GameObject* CreatePrimitive(PRIMITIVE_TYPE type, uint divisions = 2);
	
	void		BlitConfigInfo();
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
	void		CollectOctreeCandidates(math::Frustum& frustum, std::queue<GameObject*>& queue);
	void		CleanOctree();

	void		InsertStaticObject(const GameObject* target);
	void		RemoveStaticObject(const GameObject* target);
	void		HideStaticObjects();

	void		SerializeScene()const;
	bool		LoadSerializedScene(const char* path);
	bool		InitializeScene();
	void		CleanScene();

	void		PlayGame();
	void		PauseGame();
	void		NextGameFrame();

};
#endif
