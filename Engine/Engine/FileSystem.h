#ifndef _FILE_SYSTEM_H_
#define _FILE_SYSTEM_H_

#include "Module.h"

class DockContext;

class Directory
{
private:

	std::string path;
	std::string name;

public:

	Directory();
	Directory(const char* _path);
	~Directory();

private:

	Directory*				parent = nullptr;
	std::vector<Directory*> childs;

public:

	//Set Methods -----------
	void		SetPath(const char* _path);
	void		SetName(const char* _name);

	//Get Methods -----------
	const char* GetPath() const;
	void		GetDirPath(std::string* str); //Return the path with final slashes
	const char* GetName() const;
	
	//Functionality ---------
	void		AddChild(Directory* new_child);
	void		BlitDirectoryChilds();
	void		BlitFilesInside()const;

};

class FileSystem : public Module
{
public:

	FileSystem(const char* _name, MODULE_ID _id, bool _config_menu, bool _enabled = true);
	~FileSystem();

public:

	bool	Start();
	bool	CleanUp();

private:

	Directory* user_root_dir = nullptr;
	Directory* engine_root_dir = nullptr;
	Directory* focus_dir = nullptr;

public:

	std::string data_folder = "DATA";

public:

	const JSON_Value*	LoadJSONFile(const char* str);
	bool				SaveJSONFile(const JSON_Value* data, const char* name);
	JSON_Object*		AccessObject(const JSON_Value* config_data, uint str_num, ...);

	Directory*			CreateDir(const char* name, bool hidden, Directory* parent = nullptr);
	int					LoadFile(const char* path, char** buffer_to_fill);
	void				SaveFile(const char* file, const char* buffer, unsigned int size, const char* library_folder);
	void				CloneFile(const char* file, Directory* folder);
	void				GetFileNameFromPath(const char* path, std::string* name)const;
	void				GetFileFormatFromPath(const char* path, std::string* format)const;
	void				GetFolderFromPath(const char* path, std::string* folder)const;
	Directory*			GetAssetsFolder()const;
	void				ChangeFileFormat(const char* path,const char* new_format, std::string* new_path)const;

	void				BlitFileSystemInfo();

private:

	//Creates dirs for all folders in root node aka Assets
	void LoadDirs(Directory* parent);

	//Dock context for directories window
	DockContext*	file_system_dock = nullptr;
};

#endif