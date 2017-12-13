#define EXPORT_THIS  __declspec(dllexport) 

#include <stdio.h>
#include <string>
#include <vector>

typedef struct _MonoDomain MonoDomain;
typedef struct _MonoObject MonoObject;


namespace MonoScripting
{
	EXPORT_THIS const char*		GetDLLPath();

	//Mono basics 
	EXPORT_THIS bool							InitMono();
	EXPORT_THIS bool							CleanUpMono();

	//Compile Function
	EXPORT_THIS const char*						CompileFile(const char* input_file, const char* output_file, const char* name, MonoObject** returned_obj);
	//EXPORT_THIS MonoObject*						CreateMonoObject(const char* input_file);

	//Method's Functions
	EXPORT_THIS bool							ExecuteMethod(MonoObject* script, const char* name);

	//Fields [attributes] methods
	EXPORT_THIS std::vector<const char*>		GetFieldsNameAndType(MonoObject* script);
	EXPORT_THIS bool							GetFieldValue(MonoObject* script, const char* field_name, void* output_value);
	EXPORT_THIS bool							SetFieldValue(MonoObject* script, const char* field_name, void* input_value);


}


