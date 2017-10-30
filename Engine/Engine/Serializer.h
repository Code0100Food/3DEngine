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

public:

	//Functionality -------------------
	
	//Insert Methods --------
	Serializer InsertChild(const char* name);

	//Get Methods -----------
	Serializer GetChild(const char* name)const;

	//Save/Load Methods -----
	uint Save(char** buffer);

	//Shortcuts -------------
	JSON_Object*	AccessObject(const JSON_Value* config_data, uint str_num, ...);

	void			LoadFloatArray(const JSON_Object* root, const char* var_name, float * values, uint size)const;
	void			SaveFloatArray(JSON_Object* root, const char* var_name, float* values, uint size);
};

#endif // !_SERIALIZER_H_
