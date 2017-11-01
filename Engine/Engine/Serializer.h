#ifndef _SERIALIZER_H_

#include "Parson/parson.h"
#include "Globals.h"

class Serializer
{
public:

	Serializer(const char* name = nullptr);
	Serializer(const JSON_Object* child);
	~Serializer();

private:

	JSON_Value*		root = nullptr;
	JSON_Object*	current_node = nullptr;
	JSON_Array*		current_array = nullptr;

public:

	//Functionality -------------------
	
	//Insert Methods --------
	Serializer	InsertChild(const char* name);
	Serializer	InsertArray(const char* name);
	bool		InsertArrayElement(const Serializer& data);
	bool		InsertArrayInt(int value);
	bool		InsertArrayFloat(float value);
	bool		InsertString(const char* var_name, const char* value);
	bool		InsertInt(const char* var_name, int value);
	bool		InsertFloat(const char* var_name, float value);
	bool		InsertBool(const char* var_name, bool value);

	//Get Methods -----------
	Serializer GetChild(const char* name)const;

	//Save/Load Methods -----
	uint Save(char** buffer);

	//Shortcuts -------------
	JSON_Object*	AccessObject(const JSON_Value* config_data, uint str_num, ...);
};

#endif // !_SERIALIZER_H_
