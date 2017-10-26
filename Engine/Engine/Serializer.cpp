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

	//Generatethe json file
	char str[50];
	sprintf(str, "%sscene.json", SETTINGS_FOLDER);
	
	//JSON_Value *schema = json_parse_string("{\"name\":\"\"}");
	//JSON_Value *data_file = json_parse_file("user_data.json");
	//JSON_Value* data_file = json_parse_file(str);

	/*if (data_file == NULL)
	{
		LOG("[error] Error Generating Scene JSON file!");
		return false;
	}
	else LOG("Scene JSON file generated!");*/

	JSON_Value *schema = json_parse_string("{\"GameObjects\":\"\"}");
	JSON_Value *user_data = json_parse_file(str);
	
	user_data = json_value_init_object();
	JSON_Object *root_object = json_value_get_object(user_data);
	char *serialized_string = NULL;
	json_object_set_string(root_object, "name", "John Smith");
	json_object_set_number(root_object, "age", 25);
	json_object_dotset_string(root_object, "address.city", "Cupertino");
	json_object_dotset_value(root_object, "contact.emails", json_parse_string("[\"email@example.com\",\"email2@example.com\"]"));
	serialized_string = json_serialize_to_string_pretty(user_data);
	puts(serialized_string);


	//user_data = json_value_init_object();
	
	json_object_set_string(json_object(user_data), "name", serialized_string);
	json_serialize_to_file(user_data, str);
	
	json_free_serialized_string(serialized_string);
	json_value_free(user_data);

	//name = json_object_get_string(json_object(user_data), "name");
	//printf("Hello, %s.", name);
	json_value_free(schema);
	json_value_free(user_data);

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
