#define EXPORT_THIS  __declspec(dllexport) 

#include <stdio.h>
#include <string>

typedef struct _MonoDomain MonoDomain;
typedef struct _MonoObject MonoObject;


namespace MonoScripting
{
	EXPORT_THIS const char*		GetDLLPath();

	EXPORT_THIS bool			InitMono();
	EXPORT_THIS const char*		CompileFile(const char* input_file, const char* output_file, const char* name, MonoObject** returned_obj);
	EXPORT_THIS bool			ExecuteMethod(MonoObject* script, const char* name);//, unsigned int num_args = 0, ...);
	EXPORT_THIS bool			CleanUpMono();
}


