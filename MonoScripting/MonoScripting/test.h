#define EXPORT_THIS  __declspec(dllexport) 

#include <stdio.h>
#include <string>

typedef struct _MonoDomain MonoDomain;

namespace MonoScripting
{
	EXPORT_THIS bool			InitMono();
	EXPORT_THIS const char*		CompileFile(const char* input_file, const char* output_file);
	EXPORT_THIS const char*		GetDLLPath();
}


