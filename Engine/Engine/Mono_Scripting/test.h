#define EXPORT_THIS  __declspec(dllexport) 

#include <stdio.h>
#include <string>
#include <vector>

typedef struct _MonoDomain MonoDomain;
typedef struct _MonoObject MonoObject;
typedef struct _MonoString MonoString;
typedef struct _MonoArray MonoArray;
typedef struct _MonoAssemblyName MonoAssemblyName;
typedef struct _MonoAssembly MonoAssembly;
typedef struct _MonoImage MonoImage;
typedef struct _MonoClassField MonoClassField;

namespace MonoScripting
{
	//Functionality
	EXPORT_THIS const char*					GetDLLPath();
	EXPORT_THIS const char*					GetLastError();

	//Mono basics 
	EXPORT_THIS bool						InitMono();
	EXPORT_THIS bool						CleanUpMono();
	EXPORT_THIS MonoDomain*					LoadAppDomain();
	EXPORT_THIS void						UnLoadAppDomain();
	
	//Compile Function
	EXPORT_THIS bool						CompileFile(const char* input_file, const char* output_file);
	EXPORT_THIS MonoAssemblyName*			LoadScriptAssembly(const char* assembly_path);
	EXPORT_THIS bool						UnLoadScriptAssembly(MonoAssemblyName* asm_);
	EXPORT_THIS MonoObject*					CreateMonoObject(MonoAssemblyName* assembly, const char* class_name, const char* name_space = "");
	EXPORT_THIS MonoImage*					LoasdMonoImage(MonoAssembly* asm_);

	//Method's Functions
	EXPORT_THIS bool						ExecuteMethod(MonoObject* script, const char* name);
	EXPORT_THIS void						AddMethodInternalCall(const char* cs_path, const void* method);
	EXPORT_THIS const char*					MonoStringToConstChar(MonoString* string);

	//Fields [attributes] methods
	EXPORT_THIS const char*					GetFieldsNameAndType(MonoObject* script,void** iterator);
	EXPORT_THIS unsigned int				GetFieldValue(MonoObject* script, const char* field_name, void** output_value);
	EXPORT_THIS MonoClassField*				GetFieldValue(MonoImage* image, const char* namespace_, const char* class_name, const char* field_name);
	EXPORT_THIS bool						SetFieldValue(MonoObject* script, const char* field_name, void* input_value);
	EXPORT_THIS void						SetFieldValue(MonoObject* script, MonoClassField* field, void* input_value);
	
}


