#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include "Globals.h"
#include <string>

class Serializer;

enum RESOURCE_TYPE
{
	UNDEF_RESOURCE = 0,
	MESH_RESOURCE,
	MATERIAL_RESOURCE,
	SCENE_RESOURCE
};

class Resource
{
public:

	Resource();
	Resource(RESOURCE_TYPE type);
	virtual ~Resource(); //A copy constructor is not required here! 

protected:

	uint			id = 0;
	RESOURCE_TYPE	type = UNDEF_RESOURCE;
	uint			references = 0;
	std::string		original_file;
	std::string		own_file;

public:

	//Get Methods -----------
	uint			GetID()const;
	RESOURCE_TYPE	GetResourceType()const;
	uint			GetReferences()const;

	//Set Methods -----------
	void SetID(uint n_id);
	void SetResourceType(RESOURCE_TYPE n_type);
	void SetOriginalFile(const char* str);
	void SetOwnFile(const char* str);

	//Functionality ---------
	//References Methods
	void AddReference();
	void RestReference();

	//Save/Load Methods
	virtual bool Save(Serializer& array_root)const;
	virtual bool Load(Serializer& data);

};
#endif // !_RESOURCE_H_
