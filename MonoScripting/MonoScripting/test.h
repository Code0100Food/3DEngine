#define EXPORT_THIS  __declspec(dllexport) 

#include <direct.h>
#include <string>

#include <mono/jit/jit.h>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/mono-gc.h>

#pragma comment(lib, "mono-2.0-sgen.lib")

namespace MonoScripting
{
	MonoDomain* main_domain = nullptr;
	char		my_path[FILENAME_MAX];

	EXPORT_THIS bool InitMono();

}


