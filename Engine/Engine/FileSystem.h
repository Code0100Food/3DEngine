#ifndef _FILE_SYSTEM_H_
#define _FILE_SYSTEM_H_

#include "Module.h"

class Directory
{
private:
	std::string path;
	std::string name;

public:
	Directory() {};
	Directory(const char* _path) : path(_path) {};
	~Directory() { path.clear(); name.clear(); };

	void		SetPath(const char* _path)	{ path = _path; };
	void		SetName(const char* _name)	{ name = _name; };
	const char* GetPath() const				{ return path.c_str(); };

	void		AddChild(Directory* new_child);

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

public:

	bool				Start();

	const JSON_Value*	LoadJSONFile(const char* str);
	bool				SaveJSONFile(const JSON_Value* data, const char* name);
	JSON_Object*		AccessObject(const JSON_Value* config_data, uint str_num, ...);

	Directory*			CreateDir(const char* name, bool hidden, Directory* parent = nullptr);
	int					LoadFile(const char* path, char** buffer_to_fill);

	void				BlitDirsUI();
private:

	Directory* user_root_dir = nullptr;
	Directory* engine_root_dir = nullptr;
	Directory* focus_dir = nullptr;

private:

	//Creates dirs for all folders in root node aka Assets
	void LoadDirs(Directory* parent);

};

#endif