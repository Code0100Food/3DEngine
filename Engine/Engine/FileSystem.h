#ifndef _FILE_SYSTEM_H_
#define _FILE_SYSTEM_H_

#include "Module.h"

class FileSystem : public Module
{
public:

	FileSystem(bool start_enabled = true);
	~FileSystem();

public:

	const JSON_Value*	LoadJSONFile(const char* str);
	bool				SaveJSONFile(const JSON_Value* data, const char* name);
	JSON_Object*		AccessObject(const JSON_Value* config_data, uint str_num, ...);

};

#endif