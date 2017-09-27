#ifndef _FILE_SYSTEM_H_
#define _FILE_SYSTEM_H_

#include "Module.h"

class FileSystem : public Module
{
public:

	FileSystem(const char* _name, MODULE_ID _id, bool _config_menu, bool _enabled = true);
	~FileSystem();

public:

	std::string data_folder = "DATA";

public:

	const JSON_Value*	LoadJSONFile(const char* str);
	bool				SaveJSONFile(const JSON_Value* data, const char* name);
	JSON_Object*		AccessObject(const JSON_Value* config_data, uint str_num, ...);

};

#endif