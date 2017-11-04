#include "Component.h"

#include "Application.h"
#include "Serializer.h"

// Constructors =================================
Component::Component(COMPONENT_TYPE _type) :type(_type)
{
	//Generate id
	id = App->randomizer->Int();
}

Component::Component(const Component & cpy) : type(cpy.type), parent(cpy.parent), actived(cpy.actived)
{
	//Generate id
	id = App->randomizer->Int();
}

// Destructors ==================================
Component::~Component()
{
	parent = nullptr;
}

// Operators ====================================
bool Component::operator==(const Component & target)
{
	return bool(parent == target.parent &&type == target.type && actived == target.actived);
}

// Game Loop ====================================
bool Component::Start()
{
	return true;
}

bool Component::Update(float dt)
{
	return true;
}

// Set Methods ==================================
void Component::SetActiveState(bool act)
{
	actived = act;
}

void Component::SetParent(const GameObject * target)
{
	parent = (GameObject*)target;
}

COMPONENT_TYPE Component::GetType() const
{
	return type;
}

bool Component::GetActive() const
{
	return actived;
}

uint Component::GetID() const
{
	return id;
}

// Functionality ================================
void Component::BlitComponentInspector()
{
	ImGui::Text("Undef Component");
	ImGui::Checkbox("Active", &actived);
}

bool Component::Save(Serializer & array_root) const
{
	bool ret = false;

	//Serializer where all the data of the component is built
	Serializer comp_data;

	//Insert Component Type
	ret = comp_data.InsertString("type", ComponentTypeToStr(type));
	//Insert component id
	ret = comp_data.InsertInt("id", id);
	//Insert actived
	ret = comp_data.InsertBool("actived", actived);

	//Save the built data in the components array
	ret = array_root.InsertArrayElement(comp_data);

	return ret;
}

bool Component::Load(Serializer & data, std::vector<std::pair<Component*, uint>>& links)
{
	bool ret = true;

	//Get component id
	id = data.GetInt("id");
	//Get actived
	actived = data.GetBool("actived");

	return ret;
}

void Component::LinkComponent(const Component * target)
{

}

COMPONENT_TYPE StrToComponentType(const char * str)
{
	if (strcmp(str, "transformation") == 0)	return COMP_TRANSFORMATION;
	if (strcmp(str, "mesh") == 0)			return COMP_MESH;
	if (strcmp(str, "primitive_mesh") == 0)	return COMP_PRIMITIVE_MESH;
	if (strcmp(str, "cube_mesh") == 0)		return COMP_CUBE_MESH;
	if (strcmp(str, "sphere_mesh") == 0)	return COMP_SPHERE_MESH;
	if (strcmp(str, "cylinder_mesh") == 0)	return COMP_CYLINDER_MESH;
	if (strcmp(str, "frustum_mesh") == 0)	return COMP_FRUSTUM_MESH;
	if (strcmp(str, "mesh_renderer") == 0)	return COMP_MESH_RENDERER;
	if (strcmp(str, "material") == 0)		return COMP_MATERIAL;
	if (strcmp(str, "camera") == 0)			return COMP_CAMERA;
	return COMP_NONE;
}

const char * ComponentTypeToStr(COMPONENT_TYPE type)
{
	switch (type)
	{
	case COMP_TRANSFORMATION:	return "transformation";
	case COMP_MESH:				return "mesh";
	case COMP_PRIMITIVE_MESH:	return "primitive_mesh";
	case COMP_CUBE_MESH:		return "cube_mesh";
	case COMP_SPHERE_MESH:		return "sphere_mesh";
	case COMP_CYLINDER_MESH:	return "cylinder_mesh";
	case COMP_FRUSTUM_MESH:		return "frustum_mesh";
	case COMP_MESH_RENDERER:	return "mesh_renderer";
	case COMP_MATERIAL:			return "material";
	case COMP_CAMERA:			return "camera";
	}
	return "none";
}
