#include "Component.h"

#include "Serializer.h"

// Constructors =================================
Component::Component(COMPONENT_TYPE _type) :type(_type)
{

}

Component::Component(const Component & cpy) : type(cpy.type), parent(cpy.parent), actived(cpy.actived)
{

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

bool Component::Start()
{
	return true;
}

// Game Loop ====================================
bool Component::Update()
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
	//Insert actived
	ret = comp_data.InsertBool("actived", actived);

	//Save the built data in the components array
	ret = array_root.InsertArrayElement(comp_data);

	return ret;
}

bool Component::Load(const JSON_Object * root)
{
	return true;
}

COMPONENT_TYPE Component::StrToComponentType(const char * str) const
{
	if (strcmp(str, "transformation") == 0)	return COMP_TRANSFORMATION;
	if (strcmp(str, "mesh") == 0)			return COMP_MESH;
	if (strcmp(str, "mesh_renderer") == 0)	return COMP_MESH_RENDERER;
	if (strcmp(str, "material") == 0)		return COMP_MATERIAL;
	if (strcmp(str, "camera") == 0)			return COMP_CAMERA;
	return COMP_NONE;
}

const char * Component::ComponentTypeToStr(COMPONENT_TYPE type) const
{
	switch (type)
	{
	case COMP_TRANSFORMATION:	return "transformation";
	case COMP_MESH:				return "mesh";
	case COMP_MESH_RENDERER:	return "mesh_renderer";
	case COMP_MATERIAL:			return "material";
	case COMP_CAMERA:			return "camera";
	}
	return "none";
}
