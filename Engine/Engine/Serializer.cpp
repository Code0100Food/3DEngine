#include "Serializer.h"
#include "GameObject.h"
#include "Application.h"
#include "FileSystem.h"

// Constructors =================================
Serializer::Serializer()
{

}

// Destructors ==================================
Serializer::~Serializer()
{

}

// Functionality ================================
bool Serializer::SerializeScene(const GameObject * root) const
{
	bool ret = false;

	//Save config json file
	char str[50];
	sprintf(str, "%sscene.json", SETTINGS_FOLDER);
	JSON_Value* data_file = App->fs->CreateJSONFile(str);
	if (data_file == NULL)
	{
		LOG("[error] Error Generating Scene JSON file!");
		return false;
	}
	else LOG("Scene JSON file generated!");

	//Generate the file root
	

	return ret;// = root->Save();
}

// Shortcuts ====================================
JSON_Object * Serializer::AccessObject(const JSON_Value * config_data, uint str_num, ...)
{
	//Open the file root
	const JSON_Object *root_object = json_value_get_object(config_data);

	va_list str_list;
	va_start(str_list, str_num);

	const char* str = va_arg(str_list, char*);

	JSON_Object* app_object = nullptr;
	for (uint k = 0; k < str_num; k++)
	{
		app_object = json_object_dotget_object(root_object, str);
		str = va_arg(str_list, char*);
	}

	//delete str;

	va_end(str_list);

	return app_object;
}

void Serializer::LoadFloatArray(const JSON_Object * root, const char * var_name, float * values, uint size) const
{

	JSON_Array* _array = json_object_get_array(root, var_name);
	for (uint k = 0; k < size; k++)
	{
		values[0] = json_array_get_number(_array, k);
	}

}

void Serializer::SaveFloatArray(JSON_Object * root, const char * var_name, float * values, uint size)
{
	json_array_t* _array = json_object_get_array(root, var_name);

	for (uint k = 0; k < size; k++)
	{
		json_array_replace_number(_array, k, values[k]);
	}
}
