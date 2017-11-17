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
	uint		ImportAllFilesInside();
	bool		FindFile(const char* file_name)const;

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
	Directory* metas_dir = nullptr;

	Directory* focus_dir = nullptr;

	mutable std::string usable_str_a;
	mutable std::string usable_str_b;
	mutable std::string usable_str_c;

public:

	std::string data_folder = "DATA";

public:

	//Get Methods -----------
	Directory* GetUserRootDir()const;
	Directory* GetMetasDir()const;

	//Functionality ---------
	//Load/Save
	const JSON_Value*	LoadJSONFile(const char* str);
	bool				SaveJSONFile(const JSON_Value* data, const char* name);
	
	//File folder creation
	bool				CreateAFile(const char* name);
	Directory*			CreateDir(const char* name, bool hidden, Directory* parent = nullptr);
	int					LoadFile(const char* path, char** buffer_to_fill);
	void				SaveFile(const char* file, const char* buffer, unsigned int size, const char* library_folder);
	int					CloneFile(const char* file, Directory* folder, std::string* n_path = nullptr);
	
	//Paths methods
	void				GetUnformatedFileNameFromPath(const char* path, std::string* name);
	void				GetFileNameFromPath(const char* path, std::string* name);
	void				GetFileFormatFromPath(const char* path, std::string* format);
	void				GetFolderFromPath(const char* path, std::string* folder);
	Directory*			GetAssetsFolder()const;
	void				ChangeFileFormat(const char* path,const char* new_format, std::string* new_path);
	bool				IsInAssets(const char* path)const;
	
	//UI
	void				BlitFileSystemInfo();

private:

	//Creates dirs for all folders in root node aka Assets
	void LoadDirs(Directory* parent);

	//Dock context for directories window
	DockContext*	file_system_dock = nullptr;
};

#endif