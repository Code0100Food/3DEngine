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

Serializer Serializer::InsertArray(const char * name)
{
	JSON_Value* va = json_value_init_array();
	current_array = json_value_get_array(va);

	if (json_object_set_value(current_node, name, va) == JSONSuccess)
	{
		Serializer ret(current_node);
		ret.current_array = this->current_array;
		return ret;
	}
	else return Serializer();
}

bool Serializer::InsertArrayElement(const Serializer & data)
{
	if (current_array == nullptr)return false;

	return json_array_append_value(current_array, json_value_deep_copy(data.root)) == JSONSuccess;	
}

bool Serializer::InsertString(const char * var_name, const char * value)
{
	return json_object_set_string(current_node, var_name, value) == JSONSuccess;
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
