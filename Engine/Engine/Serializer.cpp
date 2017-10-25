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
	const JSON_Value *data_file = nullptr;
	ret = App->fs->SaveJSONFile(data_file, str);
	if (!ret)
	{
		LOG("[error] Error Generating Scene JSON file!");
		return false;
	}
	else LOG("Scene JSON file generated!");


	return ret;// = root->Save();
}

// Shortcuts ====================================
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
