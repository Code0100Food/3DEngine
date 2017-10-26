#ifndef _SERIALIZER_H_

#include "Parson/parson.h"
#include "Globals.h"

class GameObject;

class Serializer
{
public:

	Serializer();
	~Serializer();

private:



public:

	//Functionality ---------
	bool SerializeScene(const GameObject* root)const;

	//Shortcuts -------------
	JSON_Object*	AccessObject(const JSON_Value* config_data, uint str_num, ...);

	void			LoadFloatArray(const JSON_Object* root, const char* var_name, float * values, uint size)const;
	void			SaveFloatArray(JSON_Object* root, const char* var_name, float* values, uint size);
};

#endif // !_SERIALIZER_H_
