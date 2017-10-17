#include "FileSystem.h"
#include "Globals.h"
#include "imgui/imgui_dock.h"
#include "Application.h"
#include "ModuleWindow.h"
#include <fstream>
#include <iostream>

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
void Directory::BlitDirectoryChilds()
{
	bool opened = ImGui::TreeNode(name.c_str());

	if (opened)
	{
		for (std::vector<Directory*>::const_iterator it = childs.begin(); it != childs.end(); it++)
		{
			(*it)->BlitDirectoryChilds();
		}
		
		ImGui::TreePop();
	}
}

void Directory::BlitFilesInside() const
{
	//Set String to look inside Parent folder
	string look_path = path;
	look_path += "\\*.*";

	//Will recieve all the files list
	WIN32_FIND_DATA files_list;

	//Will handle the list when changing the looked element
	HANDLE file_handle = FindFirstFileA(LPCSTR(look_path.c_str()), &files_list);

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
			ImGui::Text(files_list.cFileName);
		}

		//Jump to the other element
		if (!FindNextFile(file_handle, &files_list))
		{
			still_elements = false;
		}

	}

	FindClose(file_handle);
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

// Functionality ================================
const JSON_Value * FileSystem::LoadJSONFile(const char * str)
{
	return json_parse_file(str);
}

bool FileSystem::SaveJSONFile(const JSON_Value * data, const char * name)
{
	return json_serialize_to_file(data, name);
}

JSON_Object* FileSystem::AccessObject(const JSON_Value * config_data, uint str_num, ...)
{
	//Open the file root
	const JSON_Object *root_object = json_value_get_object(config_data);

	va_list str_list;
	va_start(str_list, str_num);

	const char* str = va_arg(str_list, char*);
	
	JSON_Object* app_object = nullptr;
	for(uint k = 0; k < str_num; k++)
	{
		app_object = json_object_dotget_object(root_object, str);
		str = va_arg(str_list, char*);
	}
	
	//delete str;

	va_end(str_list);
	
	return app_object;
}

Directory* FileSystem::CreateDir(const char* name, bool hidden, Directory* parent)
{

	Directory* ret = nullptr;

	//Get the current Dir
	char my_path[MAX_PATH + 1];
	GetCurrentDirectory(MAX_PATH, my_path);

	//Create the New Directory path
	std::string new_dir;

	if (parent)
	{
		new_dir = parent->GetPath();
	}
	else
	{
		new_dir = my_path;
	}

	new_dir += "\\";
	new_dir += name;

	//Create the Dir
	if (CreateDirectory(new_dir.c_str(), NULL))
	{
		if (hidden)
		{
			SetFileAttributes(new_dir.c_str(), FILE_ATTRIBUTE_HIDDEN);
		}
		ret = new Directory(new_dir.c_str());
		ret->SetName(name);
		if (parent)
		{
			parent->AddChild(ret);
		}
		LOG("Created New Directory: %s, path: %s", name, new_dir.c_str());
	}
	else
	{
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			ret = new Directory(new_dir.c_str());
			ret->SetName(name);
			if (parent)
			{
				parent->AddChild(ret);
			}
			LOG("Directory (%s) already exists", new_dir.c_str());
		}

		if (GetLastError() == ERROR_PATH_NOT_FOUND)
			LOG("Path (%s) not found", new_dir.c_str());
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
	string full_path = library_folder;
	full_path += file;

	std::ofstream outfile(full_path.c_str(), std::ofstream::binary);

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

void FileSystem::CloneFile(const char * file, Directory * folder)
{
	char* buffer = nullptr;

	//Load the file
	LoadFile(file, &buffer);

	//Get file name
	std::string f_str;
	GetFileNameFromPath(file, &f_str);
	std::string d_str;
	folder->GetDirPath(&d_str);

	//Save the file
	SaveFile(f_str.c_str(), buffer, sizeof(buffer), d_str.c_str());
}

void FileSystem::GetFileNameFromPath(const char * path, std::string* name)const
{
	std::string str = path;
	size_t pos_separator = str.find_last_of("\\/");
	*name = str.substr(pos_separator + 1);
}

void FileSystem::GetFileFormatFromPath(const char * path, std::string* format)const
{
	std::string str = path;
	size_t pos_separator = str.find_last_of(".");
	*format = str.substr(pos_separator + 1).c_str();
}

void FileSystem::GetFolderFromPath(const char * path, std::string * folder) const
{
	std::string str = path;
	size_t pos_separator = str.find_last_of("\\/");
	*folder = str.substr(0, pos_separator + 1);
}

Directory * FileSystem::GetAssetsFolder() const
{
	return user_root_dir;
}

void FileSystem::ChangeFileFormat(const char * path, const char* new_format, std::string* new_path) const
{
	std::string str = path;
	size_t pos_separator = str.find_last_of(".");
	std::string unformated_str = str.substr(0,pos_separator + 1);
	unformated_str += new_format;
	*new_path = unformated_str;
}

void FileSystem::BlitFileSystemInfo()
{
	file_system_dock->BeginWorkspace("File system info");
	
	//Blit directories
	file_system_dock->BeginDock("Directories", 0, 0);
	user_root_dir->BlitDirectoryChilds();
	file_system_dock->EndDock();

	//Blit data inside the selected directory
	file_system_dock->BeginDock("Files", 0, 0);
	focus_dir->BlitFilesInside();
	file_system_dock->EndDock();

	file_system_dock->EndWorkspace();
}

void FileSystem::LoadDirs(Directory* parent)
{
	//Set String to look inside Parent folder
	string look_path = parent->GetPath();
	look_path += "\\*.*";

	//Will recieve all de files list
	WIN32_FIND_DATA files_list;

	//Will handle the list when changing the looked element
	HANDLE file_handle = FindFirstFileA(LPCSTR(look_path.c_str()), &files_list);
	
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
