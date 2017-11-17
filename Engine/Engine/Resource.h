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
	uint			last_edition_time = 0;

	bool			const_in_memory = false;

public:

	//Get Methods -----------
	uint			GetID()const;
	RESOURCE_TYPE	GetResourceType()const;
	uint			GetReferences()const;
	const char*		GetOriginalFile()const;
	const char*		GetOwnFile()const;

	//Set Methods -----------
	void SetID(uint n_id);
	void SetResourceType(RESOURCE_TYPE n_type);
	void SetOriginalFile(const char* str);
	void SetOwnFile(const char* str);
	void SetConstInMemory();

	//Functionality ---------
	//References Methods
	void AddReference();
	void RestReference();

	//Save/Load Methods
	virtual bool	Save();
	virtual bool	Load(Serializer& data);
	bool			CheckEditionTime();//Return true if the file has been modified

	//UI
	virtual void BlitUI()const;

	//Load/Unload in memory
	virtual void LoadInMemory();
	virtual void UnloadInMemory();

};

RESOURCE_TYPE	StrToResourceType(const char* str);
const char*		ResourceTypeToStr(RESOURCE_TYPE type);

#endif // !_RESOURCE_H_
