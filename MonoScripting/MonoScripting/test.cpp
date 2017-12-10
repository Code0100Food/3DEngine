#define _CRT_SECURE_NO_WARNINGS

#include "test.h"

#include <direct.h>
#include <string>

#include <mono/jit/jit.h>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/mono-gc.h>

#pragma comment(lib, "mono-2.0-sgen.lib")

//Global variables
MonoDomain*	main_domain = nullptr;
std::string	dll_path;

namespace MonoScripting
{

	const char* MonoScripting::GetDLLPath()
	{
		return dll_path.c_str();
	}

	bool MonoScripting::InitMono()
	{
		char my_path[FILENAME_MAX];
		_getcwd(my_path, FILENAME_MAX);
		dll_path = my_path;

		std::string lib_path = dll_path + "/DATA/Scripting/lib";

		std::string etc_path = dll_path + "/DATA/Scripting/etc";

		mono_set_dirs(lib_path.c_str(), etc_path.c_str());

		mono_config_parse(nullptr);

		const char domain_name[] = "MonoScripting";

		main_domain = mono_jit_init(domain_name);

		return main_domain != nullptr;
	}

	const char* MonoScripting::CompileFile(const char* input_file, const char* output_file, const char* name, MonoObject** returned_obj)
	{
		//Set the compiler path
		std::string compiler_path = dll_path;
		compiler_path += "/DATA/Scripting/compiler.dll";

		//Load the compiler.dll
		MonoAssembly* compile_assembly = nullptr;
		compile_assembly = mono_domain_assembly_open(main_domain, compiler_path.c_str());

		if (!compile_assembly)
		{
			return "[error] Compiler not load";
		}

		//Load the compiler class and method
		MonoImage* image = mono_assembly_get_image(compile_assembly);
		MonoClass* _class = mono_class_from_name(image, "MonoScripting", "Compiler");
		MonoMethod* compile_dll = mono_class_get_method_from_name(_class, "CompileDll", 2);

		//Create the arguments
		void* arguments[2];
		MonoString* cs_path = mono_string_new(main_domain, input_file);
		MonoString* cs_name = mono_string_new(main_domain, output_file);

		arguments[0] = cs_path;
		arguments[1] = cs_name;

		//Instantiate the compiler class
		MonoObject* compiler = mono_object_new(main_domain, _class);

		//Execute compiler
		MonoString* handle = (MonoString*)mono_runtime_invoke(compile_dll, compiler, arguments, NULL);
		const char* returned_message = mono_string_to_utf8(handle);

		if (strlen(returned_message) == 0)
		{
			return nullptr;
		}

		//Load the compiled script assembly
		MonoAssembly* new_script_assembly = nullptr;
		new_script_assembly = mono_domain_assembly_open(main_domain, output_file);

		if (!new_script_assembly)
		{
			return "[error] Script assembly not load";
		}

		

		//Load the compiled script class and method
		MonoImage* compiled_script_image = mono_assembly_get_image(new_script_assembly);
		MonoClass* compiled_script_class = mono_class_from_name(image, "", name);

		*returned_obj = mono_object_new(main_domain, compiled_script_class);

		return returned_message;
	}

	bool MonoScripting::ExecuteMethod(MonoObject* script, const char* name)// unsigned int num_args, ...)
	{
		MonoClass* script_class = mono_object_get_class(script);

		MonoMethod* method_to_execute = nullptr;
		void* iterator = nullptr;

		//Find the method to invoke
		do
		{
			method_to_execute = mono_class_get_methods(script_class, &iterator);

			if (strcmp(mono_method_get_name(method_to_execute), name) == 0)
			{
				MonoMethodSignature* method_info = mono_method_signature(method_to_execute);
				break;			
			}

		} while (method_to_execute);

		//Didn't find the method name return
		if (!method_to_execute)
		{
			return false;
		}

		
		mono_runtime_invoke(method_to_execute, script, NULL, NULL);
		return true;
	}

	bool MonoScripting::CleanUpMono()
	{
		if (main_domain)
		{
			mono_jit_cleanup(main_domain);
			return true;
		}	

		return false;
	}
}


/*int main(int argc, char *argv[])
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

	printf("Error");

	if (!assembler)
	{
		printf("Error");
	}

	MonoImage* image = mono_assembly_get_image(assembler);
	MonoClass* _class = mono_class_from_name(image, "MonoScripting", "Compiler");

	MonoMethod* hellman = mono_class_get_method_from_name(_class, "CompileDll", 2);
	
	int lol = 8;
	int* arguments = new int[lol];

	std::string data_path = my_path;
	data_path += "/DATA/";

	std::string source_file = data_path;
	source_file += "HelloWorld_child.txt";
	std::string result_file = data_path;
	result_file+= "HelloWorld_child.dll";

	MonoString* cs_path = mono_string_new(dom, source_file.c_str());
	MonoString* cs_name = mono_string_new(dom, result_file.c_str());
	MonoString* cs_return = mono_string_empty_wrapper();

	arguments[0] = (int)cs_path;
	arguments[1] = (int)cs_name;

	MonoObject* my_class_instance = mono_object_new(dom, _class);
	MonoObject* exception = NULL;

	char* mega_lol = mono_string_to_utf8(cs_path);

	MonoString* handle = (MonoString*)mono_runtime_invoke(hellman, my_class_instance, (void**)arguments, NULL);
	//bool tmp = *(bool*)mono_object_unbox(handle);
	char* handle_errors = mono_string_to_utf8(handle);
	
	MonoAssembly* hello_world_assembler = nullptr;
	MonoAssembly* hello_world_assembler1 = nullptr;
	hello_world_assembler = mono_domain_assembly_open(dom, result_file.c_str());
	hello_world_assembler1 = mono_domain_assembly_open(dom, result_file.c_str());

	if (!hello_world_assembler)
	{
		printf("Error");
	}

	//Execute a file
	//mono_jit_exec(dom, assembler, argc, argv);

	MonoImage* hello_world_image = mono_assembly_get_image(hello_world_assembler);
	MonoClass* hello_world_class = mono_class_from_name(hello_world_image, "", "HelloWorldchild");

	MonoMethod* hello_world_hellman = mono_class_get_method_from_name(hello_world_class, "mega_lol", 0);

	MonoObject* hello_world_instance = mono_object_new(dom, hello_world_class);
	MonoObject* hello_world_handle = mono_runtime_invoke(hello_world_hellman, hello_world_instance, NULL, NULL);
	
	getchar();
	return 0;
}*/
