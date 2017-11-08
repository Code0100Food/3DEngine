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

void Resource::AddReference()
{
	references += 1;
}

void Resource::RestReference()
{
	if(references > 0)references -= 1;
}

bool Resource::Save(Serializer & array_root) const
{
	return true;
}

bool Resource::Load(Serializer & data)
{
	return true;
}
