#include "Serializer.h"
#include "GameObject.h"
#include "Application.h"
#include "FileSystem.h"

// Constructors =================================
Serializer::Serializer(const char * name)
{
	//Unnamed node case
	if (name == nullptr)
	{
		root = json_value_init_object();
	}
	//Named node case
	else
	{
		root = json_parse_string(name);
		
	}

	current_node = json_value_get_object(root);
}

Serializer::Serializer(const JSON_Object * child) : current_node((JSON_Object*)child)
{

}

// Destructors ==================================
Serializer::~Serializer()
{
	if (root != nullptr)json_value_free(root);
}

// Functionality ================================
Serializer Serializer::InsertChild(const char * name)
{
	json_object_set_value(current_node, name, json_value_init_object());
	return Serializer(json_object_get_object(current_node, name));
}

Serializer Serializer::GetChild(const char * name) const
{
	return Serializer(json_object_get_object(current_node, name));
}

uint Serializer::Save(char ** buffer)
{
	uint len = json_serialization_size(root);
	*buffer = new char[len];
	json_serialize_to_buffer(root, *buffer, len);
	return len;
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
