#include "ScriptImporter.h"

#include "Application.h"
#include "FileSystem.h"
#include "ResourcesManager.h"

#include "Globals.h"

ScriptImporter::ScriptImporter()
{

}

uint ScriptImporter::Import(const char * path)
{
	uint ret = 0;

	//Script buffer
	char* buffer = nullptr;
	int	  lenght = App->fs->LoadFile(path, &buffer);

	if (buffer && lenght)
	{
		ResourceScript* resource = (ResourceScript*)App->res_manager->CreateResource(RESOURCE_TYPE::SCRIPT_RESOURCE);
		ret = resource->GetID();

		//Set resource data
		App->fs->GetFileNameFromPath(path, &usable_str);
		resource->SetOwnFile(usable_str.c_str());
		resource->SetOriginalFile(path);
		App->fs->SaveFile(usable_str.c_str(), (char*)buffer, lenght, LIBRARY_SCRIPTS_FOLDER);
		resource->SetBuffer(buffer);
		usable_str.clear();

		//Generate a meta file to link the generated resource with the file data
		resource->Save();
	}
	else
	{
		LOG("Cannot load script from path: %s", path);
	}

	return ret;
}

bool ScriptImporter::ReImport(ResourceScript * to_reload)
{
	bool ret = false;

	//Script buffer
	char* buffer = nullptr;
	int	  lenght = App->fs->LoadFile(to_reload->GetOriginalFile(), &buffer);

	if (buffer && lenght)
	{
		to_reload->SetBuffer(buffer);

		to_reload->Save();

		ret = true;
	}

	return ret;
}
