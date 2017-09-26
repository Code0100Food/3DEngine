#include "FileSystem.h"

// Constructors =================================
FileSystem::FileSystem(bool start_enabled) :Module(start_enabled)
{
	id = M_FILE_SYSTEM;
	name = "FileSystem";
}

// Destructors ==================================
FileSystem::~FileSystem()
{

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
