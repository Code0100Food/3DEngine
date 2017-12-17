#ifndef _RESOURCE_SCRIPT_H_
#define _RESOURCE_SCRIPT_H_

#include "Resource.h"
#include <vector>

typedef struct _MonoObject MonoObject;
typedef struct _MonoAssemblyName MonoAssemblyName;

enum FIELD_TYPE
{
	UNDEF_FIELD_TYPE = 0,
	INT8_FIELD,
	INT16_FIELD,
	INT32_FIELD,
	INT64_FIELD,
	UINT8_FIELD,
	UINT16_FIELD,
	UINT32_FIELD,
	UINT64_FIELD,
	FLOAT_FIELD,
	BOOL_FIELD,
	OBJECT_FIELD,
	TRANS_FIELD
};

/// ScriptField -------------------------------------------
class ScriptField
{
public:

	ScriptField();
	ScriptField(const ScriptField& cpy);
	~ScriptField();

public:

	std::string	name = "undef_field";
	void*		data = nullptr;
	uint		data_size = 0;
	FIELD_TYPE	type = UNDEF_FIELD_TYPE;
	bool		setted = false;

public:

	void CloneData();

};
/// -------------------------------------------------------

/// ResourceScript ----------------------------------------
class ResourceScript : public Resource
{
public:

	ResourceScript();
	ResourceScript(const ResourceScript& cpy);
	~ResourceScript();

private:

	//Script text buffer
	char* buffer = nullptr;

	//Script assembly
	MonoAssemblyName* assembly = nullptr;

	//Script fields
	std::vector<ScriptField> fields;

public:

	//Set Methods -----------
	void SetBuffer(const char* bf);
	void SetAssembly(const MonoAssemblyName* asm_);

	//Get Methods -----------
	char*						GetBuffer()const;
	MonoAssemblyName*			GetAssembly()const;
	std::vector<ScriptField>*	GetFields();

	//Functionality ---------
	//Scene
	void ReloadRelatedComponents();
	//Fields
	void ClearFields();
	void AddField(const char* name, FIELD_TYPE type, void* value, uint size);
	//UI
	void BlitUI()const;
	void BlitComplexUI();

};
/// -------------------------------------------------------
#endif // !_RESOURCE_SCRIPT_H_
