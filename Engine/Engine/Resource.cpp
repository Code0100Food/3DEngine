#include "Resource.h"

#include "Application.h"
#include "Serializer.h"
#include "FileSystem.h"
#include "ImporterManager.h"
#include "ResourcesManager.h"

#include <experimental/filesystem>

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

bool Resource::GetConstInMemory() const
{
	return const_in_memory;
}

uint Resource::GetLastEditionTime() const
{
	return last_edition_time;
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

void Resource::SetLastEditionTime(uint time)
{
	last_edition_time = time;
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

bool Resource::Save()
{
	bool ret = false;

	//Save the meta file
	Serializer meta_file;
	
	//Save all the standard resource data
	meta_file.InsertInt("id", id);
	meta_file.InsertString("res_type", ResourceTypeToStr(type));
	meta_file.InsertString("original_file", original_file.c_str());
	meta_file.InsertString("own_file", own_file.c_str());
	//Modification time
	last_edition_time = App->res_manager->GetLastEditionTime(original_file.c_str());
	meta_file.InsertInt("last_edition_time", last_edition_time);
	
	//Save the generated meta file
	char* buffer = nullptr;
	uint size = meta_file.Save(&buffer);
	if (buffer != nullptr)
	{
		char meta_name[200];
		sprintf(meta_name, "%s.meta", own_file.c_str());
		App->fs->SaveFile(meta_name, buffer, size - 1, LIBRARY_META_FOLDER);

		RELEASE_ARRAY(buffer);
		ret = true;
	}
	
	return ret;
}

bool Resource::Load(Serializer & data)
{
	//Load all the standard resource data
	id = data.GetInt("id");
	original_file = data.GetString("original_file");
	own_file = data.GetString("own_file");
	last_edition_time = data.GetInt("last_edition_time");

	return true;
}

void Resource::BlitUI() const
{

}

void Resource::BlitComplexUI()
{
	ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "%s", ResourceTypeToStr(type));
	ImGui::Text("Original File:");
	ImGui::Text("%s", original_file.c_str());
	ImGui::Text("Own File:");
	ImGui::Text("%s", own_file.c_str());
	ImGui::Text("References: %i", references);
	ImGui::Text("Last edition time: %i", last_edition_time);

	ImGui::Separator();
}

void Resource::LoadInMemory()
{

}

void Resource::UnloadInMemory()
{

}

void Resource::ReImport()
{
	App->importer->ReImportResource(this);
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
