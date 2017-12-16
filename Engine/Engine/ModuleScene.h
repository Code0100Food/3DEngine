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
	bool					frame_passed = false;

	GameObject*					root_gameobject = nullptr;
	GameObject*					selected_gameobject = nullptr;
	std::vector<GameObject*>	objects_to_delete;

	uint						play_tex_id = 0, stop_tex_id = 0, next_tex_id = 0;
	bool						hierarchy_win_state = true;
	bool						inspector_state = true;
	bool						show_components_window = false;
	bool						show_gameobject_hierarchy_window = false;

	Octree<GameObject*>			octree;
	std::vector<GameObject*>	static_objects;

	//Vectors used during serialization
	std::vector<std::pair<GameObject*, uint>>	objects_links;
	std::vector<std::pair<Component*, uint>>	components_links;
	std::vector<GameObject*>					loaded_objs;
	std::vector<Component*>						loaded_cmps;
	
	std::vector<Component*>						found_cmps;

public:

	//Set Methods -----------
	void		SetSelectedGameObject(const GameObject* target);

	//Get Methods -----------
	GameObject*			GetSelectedGameObject()const;
	SCENE_UPDATE_STATE	GetSceneState() const;

	//Functionality ---------
	//GameObject methods
	GameObject*					CreateGameObject(bool link = true);
	bool						ReleaseGameObject(GameObject* target, const GameObject* parent, bool search_in = true);
	void						SendGameObjectToRemoveVec(const GameObject* target);
	GameObject*					FindGameObject(uint id)const;
	std::vector<Component*>*	FindComponentsByType(COMPONENT_TYPE cmp_type);
	GameObject*					CreatePrimitive(PRIMITIVE_TYPE type);
	
	//UI Methods
	void		BlitConfigInfo();
	void		BlitHierarchy();
	void		HierarchyWindowOptions();
	bool		GetHierarchyWinState()const;
	void		SwapHierarchyWinState();

	void		BlitInspector();
	bool		GetInspectorState()const;
	void		EnableInspector();
	void		DisableInspector();

	void		BlitComponentsWindow(GameObject* target);
	bool		GetComponentsWinState()const;
	void		SetComponentsWindowState(bool val);

	void		BlitGameObjectHierarchyWindow(GameObject* target);
	bool		GetGameObjectHierarchyWindowState()const;
	void		SetGameObjectHierarchyWindowState(bool val);

	//Hierarchy methods
	bool		IsRoot(const GameObject* target)const;
	GameObject* GetRoot()const;

	//Octree methods
	void		PushGameObjectInOctree(GameObject* target, bool childs = true);
	void		ReFillOctree();
	void		CollectOctreeCandidates(math::Frustum& frustum, std::queue<GameObject*>& queue);
	void		CleanOctree();

	//StaticObjects methods
	void		InsertStaticObject(const GameObject* target);
	void		RemoveStaticObject(const GameObject* target);
	void		HideStaticObjects();

	//Serialize methods
	bool		SerializeScene(const GameObject* root, Serializer& serializer)const;
	void		SerializeAndSaveCurrentScene();
	GameObject*	LoadSerializedScene(const char* path);
	bool		InitializeScene();
	void		CleanScene();

	//Play/Pause methods
	void		PlayGame();
	void		PauseGame();
	void		NextGameFrame();

};
#endif
