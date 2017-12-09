#define _CRT_SECURE_NO_WARNINGS

#include "test.h"

namespace MonoScripting
{
	bool MonoScripting::InitMono()
	{
		_getcwd(my_path, FILENAME_MAX);

		std::string lib_path = my_path;
		lib_path += "/DATA/lib";

		std::string etc_path = my_path;
		etc_path += "/DATA/etc";

		mono_set_dirs(lib_path.c_str(), etc_path.c_str());

		mono_config_parse(nullptr);

		const char domain_name[] = "MonoScripting";

		main_domain = mono_jit_init(domain_name);

		return main_domain != nullptr;
	}
}


int main(int argc, char *argv[])
{

	char my_path[FILENAME_MAX];
	_getcwd(my_path, FILENAME_MAX);
	
	std::string lib_path = my_path;
	lib_path += "/DATA/lib";

	std::string etc_path = my_path;
	etc_path += "/DATA/etc";

	std::string compiler_path = my_path;
	compiler_path += "/DATA/compiler.dll";

	mono_set_dirs(lib_path.c_str(), etc_path.c_str());
	
	mono_config_parse(nullptr);

	const char domain_name[] = "MonoScripting";

	MonoDomain* dom = mono_jit_init(domain_name);

	MonoAssembly* assembler = nullptr;
	assembler = mono_domain_assembly_open(dom, compiler_path.c_str());

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

	std::string data_path = my_path;
	data_path += "/DATA/";

	std::string source_file = data_path;
	source_file += "HelloWorld.txt";
	std::string result_file = data_path;
	result_file+= "HelloWorld.dll";

	MonoString* cs_path = mono_string_new(dom, source_file.c_str());
	MonoString* cs_name = mono_string_new(dom, result_file.c_str());
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
	hello_world_assembler = mono_domain_assembly_open(dom, result_file.c_str());

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
