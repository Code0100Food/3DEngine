#define _CRT_SECURE_NO_WARNINGS

#include "test.h"

#include <direct.h>
#include <string>

#include <mono/jit/jit.h>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/mono-gc.h>

#pragma comment(lib, "mono-2.0-boehm.lib")
#pragma comment(lib, "mono-2.0-sgen.lib")

/*namespace MonoScripting
{
	bool MonoScripting::InitMono()
	{
		char my_path[FILENAME_MAX];
		_getcwd(my_path, FILENAME_MAX);
	}
}*/


int main(int argc, char *argv[])
{

	char my_path[FILENAME_MAX];
	_getcwd(my_path, FILENAME_MAX);
	printf(my_path);

	mono_set_dirs("C:/Program Files (x86)/Mono/lib", "C:/Program Files (x86)/Mono/etc");

	mono_config_parse(nullptr);

	const char domain_name[] = "MonoScripting";

	MonoDomain* dom = mono_jit_init(domain_name);

	MonoAssembly* assembler = nullptr;
	assembler = mono_domain_assembly_open(dom, "C:/Users/Usuari/Desktop/compiler.dll");

	if (!assembler)
	{
		printf("Error");
	}

	//Execute a file
	//mono_jit_exec(dom, assembler, argc, argv);

	MonoImage* image = mono_assembly_get_image(assembler);
	MonoClass* _class = mono_class_from_name(image, "MonoScripting", "Compiler");

	MonoMethod* hellman = mono_class_get_method_from_name(_class, "CompileDll", 2);

	void* arguments[2];

	MonoString* cs_path = mono_string_new(dom, "C:/Users/Usuari/Desktop/HelloWorld.txt");
	MonoString* cs_name = mono_string_new(dom, "C:/Users/Usuari/Desktop/HelloWorld.dll");
	MonoString* cs_return = mono_string_empty_wrapper();

	arguments[0] = cs_path;
	arguments[1] = cs_name;
	//arguments[2] = cs_return;

	MonoObject* my_class_instance = mono_object_new(dom, _class);
	MonoObject* exception = NULL;

	char* mega_lol = mono_string_to_utf8(cs_path);

	MonoString* handle = (MonoString*)mono_runtime_invoke(hellman, my_class_instance, arguments, NULL);
	//bool tmp = *(bool*)mono_object_unbox(handle);
	char* handle_errors = mono_string_to_utf8(handle);
	
	MonoAssembly* hello_world_assembler = nullptr;
	hello_world_assembler = mono_domain_assembly_open(dom, "C:/Users/Usuari/Desktop/HelloWorld.dll");

	if (!hello_world_assembler)
	{
		printf("Error");
	}

	//Execute a file
	//mono_jit_exec(dom, assembler, argc, argv);

	MonoImage* hello_world_image = mono_assembly_get_image(hello_world_assembler);
	MonoClass* hello_world_class = mono_class_from_name(hello_world_image, "MonoScript", "HelloWorld");

	MonoMethod* hello_world_hellman = mono_class_get_method_from_name(hello_world_class, "whatthehellman", 0);

	MonoObject* hello_world_instance = mono_object_new(dom, hello_world_class);
	MonoObject* hello_world_handle = mono_runtime_invoke(hello_world_hellman, hello_world_instance, NULL, NULL);

	getchar();
	return 0;
}