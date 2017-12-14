#include "ScriptImporter.h"

#include "Application.h"
#include "FileSystem.h"
#include "ResourcesManager.h"
#include "ModuleScripting.h"

#include "Globals.h"
#include <string.h>

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
		App->fs->GetUnformatedFileNameFromPath(path, &usable_str);
		char own_file_name[150];
		sprintf(own_file_name, "%s.dll", usable_str.c_str());
		resource->SetOwnFile(own_file_name);
		resource->SetOriginalFile(path);

		char out_path[250];
		sprintf(out_path, "%s/%s/%s", App->scripting->GetDLLPath(), LIBRARY_SCRIPTS_FOLDER, resource->GetOwnFile());
		
		//Compile the script!
		if (!App->scripting->Compile(resource->GetOriginalFile(), out_path))
		{
			App->scripting->BlitScriptingError();
		}
		else
		{
			LOG("%s Compile Success!", own_file_name);

			//Import the class fields if the dll has been generated correctly!
			MonoAssemblyName* assembly = App->scripting->LoadScriptAssembly(out_path);
			if (assembly == nullptr)App->scripting->BlitScriptingError();
			else
			{
				resource->SetAssembly(assembly);
				usable_str = resource->GetOwnFile();
				App->fs->GetUnformatedFileNameFromPath(resource->GetOwnFile(), &usable_str);
				MonoObject* obj = App->scripting->CreateMonoObject(assembly, usable_str.c_str(), "");
				if (obj == nullptr)App->scripting->BlitScriptingError();
				else
				{
					std::vector<std::pair<const char*, FIELD_TYPE>>* fields = App->scripting->GetFieldsNameAndType(obj);

				}
			}
		}


		usable_str.clear();

		char* real_str = new char[lenght + 1];
		memcpy(real_str, buffer, lenght);
		real_str[lenght] = '\0';
		RELEASE_ARRAY(buffer);

		resource->SetBuffer(real_str);
		
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
		char* real_str = new char[lenght + 1];
		memcpy(real_str, buffer, lenght);
		real_str[lenght] = '\0';
		RELEASE(buffer);

		to_reload->SetBuffer(real_str);
		
		to_reload->Save();

		ret = true;
	}

	return ret;
}

bool ScriptImporter::NewImport(ResourceScript * to_import)
{
	char* buffer = to_import->GetBuffer();

	if (buffer == nullptr)return false;
	uint size = strlen(buffer);
	
	App->fs->SaveFile("", buffer, size, to_import->GetOriginalFile());

	//Set resource data
	App->fs->GetUnformatedFileNameFromPath(to_import->GetOriginalFile(), &usable_str);
	char own_file_name[150];
	sprintf(own_file_name, "%s.dll", usable_str.c_str());
	to_import->SetOwnFile(own_file_name);
	
	char out_path[250];
	sprintf(out_path, "%s/%s/%s", App->scripting->GetDLLPath(), LIBRARY_SCRIPTS_FOLDER, to_import->GetOwnFile());

	if (!App->scripting->Compile(to_import->GetOriginalFile(), out_path))
	{
		App->scripting->BlitScriptingError();
	}
	else
	{
		LOG("%s Compile Success!", own_file_name);
	}

	usable_str.clear();

	to_import->Save();

	return true;
}
