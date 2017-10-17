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

	void		SetPath(const char* _path);
	void		SetName(const char* _name);
	const char* GetPath() const;
	const char* GetName() const;
	
	void		AddChild(Directory* new_child);
	void		BlitDirectoryChilds();

private:

	Directory*				parent = nullptr;
	std::vector<Directory*> childs;

};

class FileSystem : public Module
{
public:

	FileSystem(const char* _name, MODULE_ID _id, bool _config_menu, bool _enabled = true);
	~FileSystem();

public:

	std::string data_folder = "DATA";

private:

	Directory* user_root_dir = nullptr;
	Directory* engine_root_dir = nullptr;
	Directory* focus_dir = nullptr;

public:

	bool	Start();
	bool	CleanUp();

public:

	const JSON_Value*	LoadJSONFile(const char* str);
	bool				SaveJSONFile(const JSON_Value* data, const char* name);
	JSON_Object*		AccessObject(const JSON_Value* config_data, uint str_num, ...);

	Directory*			CreateDir(const char* name, bool hidden, Directory* parent = nullptr);
	int					LoadFile(const char* path, char** buffer_to_fill);
	void				SaveFile(const char* file, const char* buffer, unsigned int size, const char* library_folder);
	void				GetFileNameFromPath(const char* path, std::string* name)const;
	void				GetFileFormatFromPath(const char* path, std::string* format)const;
	void				GetFolderFromPath(const char* path, std::string* folder)const;
	void				ChangeFileFormat(const char* path,const char* new_format, std::string* new_path)const;

	void				BlitFileSystemInfo();

private:

	//Creates dirs for all folders in root node aka Assets
	void LoadDirs(Directory* parent);

	//Dock context for directories window
	DockContext*	file_system_dock = nullptr;
};

#endif