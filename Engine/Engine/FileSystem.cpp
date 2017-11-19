#include "FileSystem.h"
#include "Globals.h"
#include "imgui/imgui_dock.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleImgui.h"
#include "ImporterManager.h"
#include <fstream>
#include <iostream>
#include "Serializer.h"
#include "ModuleInput.h"
#include "ResourcesManager.h"

//Directory -------------------------------------
Directory::Directory()
{

}

Directory::Directory(const char * _path) : path(_path)
{

}

Directory::~Directory()
{
	path.clear();
	name.clear();
	parent = nullptr;
	uint size = childs.size();
	for (uint k = 0; k < size; k++)
	{
		RELEASE(childs[k]);
	}
	childs.clear();
}

void Directory::SetPath(const char * _path)
{
	path = _path;
}

void Directory::SetName(const char * _name)
{
	name = _name;
}

const char* Directory::GetPath() const
{
	return path.c_str();
}

void Directory::GetDirPath(std::string* str)
{
	*str = path;
	*str += "\\";
}

const char* Directory::GetName() const
{
	return name.c_str();
}

void Directory::AddChild(Directory * new_child)
{
	if (new_child)
	{
		new_child->parent = this;
		childs.push_back(new_child);
	}
}
void Directory::BlitDirectoryChilds(uint index)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_OpenOnDoubleClick;
	if (childs.empty())flags += ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Leaf;
	if(App->fs->GetFocusDir() == this)flags += ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Selected;
	
	char name_str[250];
	sprintf_s(name_str, 250, "%s##%i", name.c_str(), index);
	bool opened = ImGui::TreeNodeEx(name_str, flags);

	//Focus the dir on click
	if (ImGui::IsItemHovered() && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		App->fs->SetFocusDir(this);
	}

	if (opened)
	{
		for (std::vector<Directory*>::const_iterator it = childs.begin(); it != childs.end(); it++)
		{
			index++;
			(*it)->BlitDirectoryChilds(index);
		}
		
		ImGui::TreePop();
	}
}

void Directory::BlitFilesInside() const
{
	//Set String to look inside Parent folder
	char str[150];
	sprintf(str, "%s\\*.*", path.c_str());

	//Will recieve all the files list
	WIN32_FIND_DATA files_list;

	//Will handle the list when changing the looked element
	HANDLE file_handle = FindFirstFileA(LPCSTR(str), &files_list);

	if (file_handle == INVALID_HANDLE_VALUE)
	{
		LOG("Error in path");
	}

	DWORD attribute;

	bool still_elements = true;
	while (still_elements)
	{
		attribute = GetFileAttributes(files_list.cFileName);

		//Search for directories
		if (FILE_ATTRIBUTE_ARCHIVE & files_list.dwFileAttributes)
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Leaf;
			if (strcmp(App->fs->GetFocusFileName(), files_list.cFileName) == 0)flags += ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Selected;
			ImGui::TreeNodeEx(files_list.cFileName, flags);
			
			if (ImGui::IsItemHovered() && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN && strcmp(files_list.cFileName, App->fs->GetFocusFileName()) != 0)
			{
				App->fs->SetFocusFileName(files_list.cFileName);
			}
			
			ImGui::TreePop();
		}

		//Jump to the other element
		if (!FindNextFile(file_handle, &files_list))
		{
			still_elements = false;
		}

	}

	FindClose(file_handle);
}

uint Directory::ImportAllFilesInside()
{
	uint updates = 0;

	//Set String to look inside Parent folder
	char str[150];
	sprintf(str, "%s\\*.*", path.c_str());

	//Will recieve all the files list
	WIN32_FIND_DATA files_list;

	//Will handle the list when changing the looked element
	HANDLE file_handle = FindFirstFileA(LPCSTR(str), &files_list);

	if (file_handle == INVALID_HANDLE_VALUE)
	{
		LOG("Error in path");
	}

	DWORD attribute;

	bool still_elements = true;
	while (still_elements)
	{
		attribute = GetFileAttributes(files_list.cFileName);

		//Search for directories
		if (FILE_ATTRIBUTE_ARCHIVE & files_list.dwFileAttributes)
		{
			//Need the full path
			char full_path[200];
			sprintf(full_path, "%s\\%s", path.c_str(), files_list.cFileName);
			if (App->importer->ImportFile(full_path))
			{
				updates++;
			}
		}

		//Jump to the other element
		if (!FindNextFile(file_handle, &files_list))
		{
			still_elements = false;
		}

	}

	FindClose(file_handle);
	
	//Iterate all the sub folders
	uint size = childs.size();
	for (uint k = 0; k < size; k++)
	{
		updates += childs[k]->ImportAllFilesInside();
	}

	return updates;
}
bool Directory::FindFile(const char * file_name, bool search_in_childs) const
{
	bool ret = false;

	//Set String to look inside Parent folder
	char str[150];
	sprintf(str, "%s\\*.*", path.c_str());

	//Will recieve all the files list
	WIN32_FIND_DATA files_list;

	//Will handle the list when changing the looked element
	HANDLE file_handle = FindFirstFileA(LPCSTR(str), &files_list);

	if (file_handle == INVALID_HANDLE_VALUE)
	{
		LOG("Error in path");
	}

	DWORD attribute;

	bool still_elements = true;
	while (still_elements)
	{
		attribute = GetFileAttributes(files_list.cFileName);

		//Search for directories
		if (strcmp(files_list.cFileName,file_name) == 0)
		{
			ret = true;
			break;
		}

		//Jump to the other element
		if (!FindNextFile(file_handle, &files_list))
		{
			still_elements = false;
		}

	}

	FindClose(file_handle);

	if (search_in_childs && !ret)
	{
		//Iterate all the sub folders
		uint size = childs.size();
		for (uint k = 0; k < size; k++)
		{
			ret = childs[k]->FindFile(file_name, search_in_childs);
			if (ret)break;
		}
	}

	return ret;
}

bool Directory::FindFile(const char * file_name, std::string * new_path, bool search_in_childs)
{
	bool ret = false;

	//Set String to look inside Parent folder
	char str[150];
	sprintf(str, "%s\\*.*", path.c_str());

	//Will recieve all the files list
	WIN32_FIND_DATA files_list;

	//Will handle the list when changing the looked element
	HANDLE file_handle = FindFirstFileA(LPCSTR(str), &files_list);

	if (file_handle == INVALID_HANDLE_VALUE)
	{
		LOG("Error in path");
	}

	DWORD attribute;

	bool still_elements = true;
	while (still_elements)
	{
		attribute = GetFileAttributes(files_list.cFileName);

		//Search for directories
		if (strcmp(files_list.cFileName, file_name) == 0)
		{
			*new_path = path;
			*new_path += "\\";
			*new_path += files_list.cFileName;

			ret = true;
			break;
		}

		//Jump to the other element
		if (!FindNextFile(file_handle, &files_list))
		{
			still_elements = false;
		}

	}

	FindClose(file_handle);

	if (search_in_childs && !ret)
	{
		//Iterate all the sub folders
		uint size = childs.size();
		for (uint k = 0; k < size; k++)
		{
			ret = childs[k]->FindFile(file_name, new_path, search_in_childs);
			if (ret)break;
		}
	}

	return ret;
}
//-----------------------------------------------

// Constructors =================================
FileSystem::FileSystem(const char* _name, MODULE_ID _id, bool _config_menu, bool _enabled) :Module(_name, _id, _config_menu, _enabled)
{

}

// Destructors ==================================
FileSystem::~FileSystem()
{

}

bool FileSystem::Start()
{
	user_root_dir = CreateDir("Assets", false);
	focus_dir = user_root_dir;

	LoadDirs(user_root_dir);

	//Starts library folder
	engine_root_dir = CreateDir("Library", true);
	CreateDir("Meshes", true, engine_root_dir);
	CreateDir("Materials", true, engine_root_dir);
	CreateDir("Scenes", true, engine_root_dir);
	metas_dir = CreateDir("Metas", true, engine_root_dir);

	file_system_dock = new DockContext();

	return true;
}

bool FileSystem::CleanUp()
{
	if (user_root_dir != nullptr)RELEASE(user_root_dir);
	if (engine_root_dir != nullptr)RELEASE(engine_root_dir);
	focus_dir = nullptr;

	RELEASE(file_system_dock);
	return true;
}

// Set Methods ==================================
void FileSystem::SetFocusDir(const Directory* dir)
{
	focus_dir = (Directory*)dir;
}

void FileSystem::SetFocusFileName(const char * str)
{
	focus_file_name = str;
	focus_dir->FindFile(str, &usable_str_a, false);
	App->res_manager->SetInspectedResources(usable_str_a.c_str());
}

// Get Methods ==================================
Directory * FileSystem::GetUserRootDir() const
{
	return user_root_dir;
}

Directory * FileSystem::GetMetasDir() const
{
	return metas_dir;
}

Directory * FileSystem::GetFocusDir() const
{
	return focus_dir;
}

const char * FileSystem::GetFocusFileName() const
{
	return focus_file_name.c_str();
}

// Functionality ================================
const JSON_Value * FileSystem::LoadJSONFile(const char * str)
{
	return json_parse_file(str);
}

bool FileSystem::SaveJSONFile(const JSON_Value * data, const char * name)
{
	return json_serialize_to_file(data, name);
}

bool FileSystem::CreateAFile(const char * name)
{
	bool ret = true;
	//Generate a new file
	FILE* fp = fopen(name, "w+");

	//If error close it and return null
	if (fp == NULL)
	{
		LOG("[error] Error Generating %s file!",name);
		ret = false;
	}
	else LOG("%s file Created!", name);

	//Close the generated file
	fclose(fp);

	return ret;
}

Directory* FileSystem::CreateDir(const char* name, bool hidden, Directory* parent)
{

	Directory* ret = nullptr;

	//Get the current Dir
	char my_path[MAX_PATH + 1];
	GetCurrentDirectory(MAX_PATH, my_path);

	//Create the New Directory path
	if (parent)
	{
		usable_str_a = parent->GetPath();
	}
	else
	{
		usable_str_a = my_path;
	}

	usable_str_a += "\\";
	usable_str_a += name;

	//Create the Dir
	if (CreateDirectory(usable_str_a.c_str(), NULL))
	{
		if (hidden)
		{
			SetFileAttributes(usable_str_a.c_str(), FILE_ATTRIBUTE_HIDDEN);
		}
		ret = new Directory(usable_str_a.c_str());
		ret->SetName(name);
		if (parent)
		{
			parent->AddChild(ret);
		}
		LOG("Created New Directory: %s, path: %s", name, usable_str_a.c_str());
	}
	else
	{
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			ret = new Directory(usable_str_a.c_str());
			ret->SetName(name);
			if (parent)
			{
				parent->AddChild(ret);
			}
			LOG("Directory (%s) already exists", usable_str_a.c_str());
		}

		if (GetLastError() == ERROR_PATH_NOT_FOUND)
			LOG("Path (%s) not found", usable_str_a.c_str());
	}

	return ret;
}

int FileSystem::LoadFile(const char * path, char ** buffer_to_fill)
{
	std::ifstream f_read(path, std::ifstream::binary);
	int size = 0;
	
	if (f_read.good())
	{
		if (f_read.is_open())
		{
			f_read.seekg(0, f_read.end);
			std::streamsize lenght = f_read.tellg();
			f_read.seekg(0, f_read.beg);

			*buffer_to_fill = new char[lenght];

			f_read.read(*buffer_to_fill, lenght);

			if (f_read)
			{
				LOG("All chars readed");
				size = lenght;
			}
			else
			{
				LOG("[error] Only %i could be read", f_read.gcount());
			}

			f_read.close();
		}
	}

	return size;
}

void FileSystem::SaveFile(const char * file, const char* buffer, unsigned int size, const char* library_folder)
{
	usable_str_c = library_folder;
	usable_str_c += file;

	std::ofstream outfile(usable_str_c.c_str(), std::ofstream::binary);

	if (outfile.good())
	{
		// write to outfile
		outfile.write(buffer, size);
		outfile.close();
	}
	else
	{
		LOG("[error] Couldn't open file to write")
	}
}

int FileSystem::CloneFile(const char * file, Directory * folder, std::string* n_path)
{
	//Get file path
	GetFolderFromPath(file, &usable_str_a);
	//Get folder path
	folder->GetDirPath(&usable_str_b);
	//Check if the file already exists in the target directory
	if (strcmp(usable_str_a.c_str(), usable_str_b.c_str()) == 0)return 0;

	char* buffer = nullptr;
	
	//Load the file
	uint size = LoadFile(file, &buffer);

	if (buffer == NULL)return -1;

	//Get file name
	usable_str_a.clear();
	GetFileNameFromPath(file, &usable_str_a);


	//Save the file
	SaveFile(usable_str_a.c_str(), buffer, size, usable_str_b.c_str());

	if (n_path != nullptr)
	{
		*n_path = usable_str_b;
		*n_path += usable_str_a;
	}

	RELEASE_ARRAY(buffer);

	return 1;
}

void FileSystem::GetUnformatedFileNameFromPath(const char * path, std::string * name)
{
	usable_str_a = (char*)path;
	size_t pos_separator = usable_str_a.find_last_of("\\/");
	*name = usable_str_a.substr(pos_separator + 1);
	pos_separator = name->find_last_of(".");
	usable_str_b = name->substr(0, pos_separator);
	*name = usable_str_b;
}

void FileSystem::GetFileNameFromPath(const char * path, std::string* name)
{
	usable_str_a = path;
	size_t pos_separator = usable_str_a.find_last_of("\\/");
	*name = usable_str_a.substr(pos_separator + 1);
}

void FileSystem::GetFileFormatFromPath(const char * path, std::string* format)
{
	usable_str_a = path;
	size_t pos_separator = usable_str_a.find_last_of(".");
	*format = usable_str_a.substr(pos_separator + 1).c_str();
}

void FileSystem::GetFolderFromPath(const char * path, std::string * folder) 
{
	usable_str_a = path;
	size_t pos_separator = usable_str_a.find_last_of("\\/");
	*folder = usable_str_a.substr(0, pos_separator + 1);
}

Directory * FileSystem::GetAssetsFolder() const
{
	return user_root_dir;
}

void FileSystem::ChangeFileFormat(const char * path, const char* new_format, std::string* new_path)
{
	usable_str_a = path;
	size_t pos_separator = usable_str_a.find_last_of(".");
	std::string unformated_str = usable_str_a.substr(0,pos_separator + 1);
	unformated_str += new_format;
	*new_path = unformated_str;
}

bool FileSystem::IsInAssets(const char * path) const
{
	const char* user_dir = user_root_dir->GetPath();
	uint size = strlen(user_dir);
	
	if (strlen(path) < size)return false;
	
	char cmp_part[150];
	memcpy(cmp_part, path, size);
	cmp_part[size] = '\0';

	return strcmp(cmp_part, user_dir) == 0;
}

void FileSystem::BlitFileSystemInfo()
{
	//Blit directories
	App->imgui->GetWorkspace()->BeginDock("Directories", 0, 0);
	user_root_dir->BlitDirectoryChilds(0);
	App->imgui->GetWorkspace()->EndDock();

	//Blit data inside the selected directory
	App->imgui->GetWorkspace()->BeginDock("Files", 0, 0);
	focus_dir->BlitFilesInside();
	App->imgui->GetWorkspace()->EndDock();
}

void FileSystem::LoadDirs(Directory* parent)
{
	//Set String to look inside Parent folder
	char str[150];
	sprintf(str, "%s\\*.*", parent->GetPath());

	//Will recieve all de files list
	WIN32_FIND_DATA files_list;

	//Will handle the list when changing the looked element
	HANDLE file_handle = FindFirstFileA(LPCSTR(str), &files_list);
	
	if (file_handle == INVALID_HANDLE_VALUE)
	{
		LOG("Error in path");
	}

	DWORD attribute;

	bool still_elements = true;
	while (still_elements)
	{
		attribute = GetFileAttributes(files_list.cFileName);

		//Search for directories
		if (FILE_ATTRIBUTE_DIRECTORY & files_list.dwFileAttributes)
		{
			string dir_name = files_list.cFileName;

			if (dir_name != "." && dir_name != "..")
			{
				//If dir added to the file system
				LoadDirs(CreateDir(dir_name.c_str(), false, parent));
			}
		}

		//Jump to the other element
		if (!FindNextFile(file_handle, &files_list))
		{
			still_elements = false;
		}

	}
	
	FindClose(file_handle);
}
