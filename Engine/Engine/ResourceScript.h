#ifndef _RESOURCE_SCRIPT_H_
#define _RESOURCE_SCRIPT_H_

#include "Resource.h"
#include <vector>

typedef struct _MonoObject MonoObject;
typedef struct _MonoAssemblyName MonoAssemblyName;

enum FIELD_TYPE
{
	UNDEF_FIELD_TYPE = 0,
	CHAR_FIELD,
	STRING_FIELD,
	INT8_FIELD,
	INT16_FIELD,
	INT32_FIELD,
	INT64_FIELD,
	UINT8_FIELD,
	UINT16_FIELD,
	UINT32_FIELD,
	UINT64_FIELD,
	CLASS_FIELD
};

/// ScriptField -------------------------------------------
class ScriptField
{
public:

	ScriptField();
	~ScriptField();

public:

	std::string	name = "undef_field";
	void*		data = nullptr;
	FIELD_TYPE	type = UNDEF_FIELD_TYPE;

};
/// -------------------------------------------------------

/// ResourceScript ----------------------------------------
class ResourceScript : public Resource
{
public:

	ResourceScript();
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
	std::vector<ScriptField>*	GetFields();

	//Functionality ---------
	//Fields
	void ClearFields();
	void AddField(const char* name, FIELD_TYPE type);
	//UI
	void BlitUI()const;
	void BlitComplexUI();

};
/// -------------------------------------------------------
#endif // !_RESOURCE_SCRIPT_H_
