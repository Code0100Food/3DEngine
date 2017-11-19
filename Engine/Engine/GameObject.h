#ifndef _GAME_OBJECT_H_
#define _GAME_OBJECT_H_

#include <vector>
#include <string>

#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentPrimitiveMesh.h"
#include "ComponentCubeMesh.h"
#include "ComponentSphereMesh.h"
#include "ComponentCylinderMesh.h"
#include "ComponentMeshRenderer.h"
#include "ComponentMaterial.h"
#include "ComponentCamera.h"

class Serializer;
class ResourceMaterial;
class ResourceMesh;

class GameObject
{
public:

	GameObject();
	GameObject(const GameObject& cpy);
	~GameObject();

public:

	bool	Start();
	bool	Update(float dt);

private:

	uint						id = 0;
	bool						actived = true;
	bool						hide = false;
	bool						static_ = false;
	std::string					name = "Unnamed";
	GameObject*					parent = nullptr;
	std::vector<Component*>		components;
	std::vector<GameObject*>	childs;
	
	bool						draw_selected_bounding_box = false;
	bool						draw_bounding_box = false;
	math::AABB					bounding_box;
	math::AABB					original_bounding_box;
	std::vector<math::float3>	childs_aabb_points;

public:

	//Set Methods -----------
	void SetActiveState(bool act);
	void SetHideState(bool new_hide_state);
	void SetStatic(bool st);
	void SetDrawSelectedBoundingBoxState(bool val);
	void SetName(const char* str);
	void SetParent(GameObject* target);
	void SetMaterial(ResourceMaterial* mat);
	void SetMesh(ResourceMesh* mesh);

	//Get Methods -----------
	bool				GetActive()const;
	bool				GetHide()const;
	bool				GetStatic()const;
	float				GetBoundingBoxDiagonalSize()const;
	uint				GetID()const;
	GameObject*			GetParent() const;
	bool				IsRoot() const;
	bool				IsSelectedObject() const;
	
	//Functionality ---------
	//Components Methods 
	Component*		CreateComponent(COMPONENT_TYPE c_type);
	bool			RemoveComponent(Component* cmp);
	bool			FindComponent(Component* cmp)const;
	Component*		FindComponent(COMPONENT_TYPE type)const;
	Component*		FindComponent(uint id)const;
	ComponentMesh*	FindMeshComponent()const;
	Component*		CloneComponent(const Component* target)const;
	void			CloneGameObject();

	//Childs Methods 
	void							AddChild(const GameObject* child);
	bool							RemoveChild(GameObject* child, bool search_in = false);
	bool							PopChild(GameObject* child, bool search_in = false);
	GameObject*						FindChild(uint id)const;
	std::vector<GameObject*>*		GetChilds();
	const std::vector<GameObject*>*	GetChildsConst()const;
	void							RecalculateID();

	//UI Methods
	void		BlitGameObjectHierarchy(uint index);
	void		BlitGameObjectInspector();
	void		HandleRightClickInput();

	void		BlitRemove();
	void		BlitClone();
	void		BlitAddEmpty();
	void		Blit3DObject();
	void		BlitPrefabs();

	//Bounding Box Methods
	std::pair<math::float3, math::float3>	AdjustBoundingBox(bool all_childs = true);
	void									DrawBoundingBox();
	math::AABB*								GetBoundingBox();
	math::AABB								GetTransformedBoundingBox();
	math::AABB								GetInheritTransformedBoundingBox();
	void									UpdateBoundingBox();

	//Save/Load Methods
	bool Save(Serializer& array_root)const;
	bool Load(Serializer& data, std::vector<std::pair<GameObject*, uint>>& links, std::vector<std::pair<Component*, uint>>& components_links, std::vector<Component*>& loaded_cmps);

};
#endif // !_GAME_OBJECT_H_
