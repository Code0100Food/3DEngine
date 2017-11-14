#include "Resource.h"

#include "Application.h"

// Constructors =================================
Resource::Resource()
{
	//Generate a random id for the resource
	id = App->randomizer->Int();
}

Resource::Resource(RESOURCE_TYPE type) :type(type)
{
	//Generate a random id for the resource
	id = App->randomizer->Int();
}

// Destructors ==================================
Resource::~Resource()
{

}

uint Resource::GetID() const
{
	return id;
}

RESOURCE_TYPE Resource::GetResourceType() const
{
	return type;
}

uint Resource::GetReferences() const
{
	return references;
}

const char * Resource::GetOriginalFile() const
{
	return original_file.c_str();
}

const char * Resource::GetOwnFile() const
{
	return own_file.c_str();
}

void Resource::SetID(uint n_id)
{
	id = n_id;
}

void Resource::SetResourceType(RESOURCE_TYPE n_type)
{
	type = n_type;
}

void Resource::SetOriginalFile(const char * str)
{
	original_file = str;
}

void Resource::SetOwnFile(const char * str)
{
	own_file = str;
}

void Resource::SetConstInMemory()
{
	const_in_memory = true;
}

void Resource::AddReference()
{
	references += 1;
	if (references == 1 && !const_in_memory)LoadInMemory();
}

void Resource::RestReference()
{
	if (references > 0)
	{
		references -= 1;
		if (references == 0 && !const_in_memory)UnloadInMemory();
	}
}

bool Resource::Save(Serializer & file_root) const
{
	return true;
}

bool Resource::Load(Serializer & data)
{
	return true;
}

void Resource::BlitUI() const
{

}

void Resource::LoadInMemory()
{

}

void Resource::UnloadInMemory()
{

}

RESOURCE_TYPE StrToResourceType(const char * str)
{
	if (strcmp(str, "mesh_resource") == 0)		return MESH_RESOURCE;
	if (strcmp(str, "material_resource") == 0)	return MATERIAL_RESOURCE;
	if (strcmp(str, "scene_resource") == 0)		return SCENE_RESOURCE;
	return RESOURCE_TYPE::UNDEF_RESOURCE;
}

const char * ResourceTypeToStr(RESOURCE_TYPE type)
{
	switch (type)
	{
	case MESH_RESOURCE:		return "mesh_resource";
	case MATERIAL_RESOURCE:	return "material_resource";
	case SCENE_RESOURCE:	return "scene_resource";
	}
	return "undef_resource";
}
