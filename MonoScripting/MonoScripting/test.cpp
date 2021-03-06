#define _CRT_SECURE_NO_WARNINGS

#include "test.h"

#include <direct.h>

#include <mono/jit/jit.h>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/mono-gc.h>

#pragma comment(lib, "mono-2.0-sgen.lib")

//Global variables
MonoDomain*					main_domain = nullptr;
std::string					dll_path;
std::string					last_error;

namespace MonoScripting
{
	const char* MonoScripting::GetDLLPath()
	{
		return dll_path.c_str();
	}

	const char*  MonoScripting::GetLastError()
	{
		return last_error.c_str();
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

		if (!main_domain)
		{
			last_error = "[error] InitMono: Could not load the domain";
			return false;
		}

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

	MonoDomain * LoadAppDomain()
	{
		MonoDomain* newDomain = mono_domain_create_appdomain("CCubed Child Domain", NULL);
		if (!newDomain) 
		{
			last_error = "Error creating domain";
			return nullptr;
		}
		
		//mono_thread_push_appdomain_ref(newDomain);

		if (!mono_domain_set(newDomain, false)) 
		{
			last_error = "Error setting domain";
			return nullptr;
		}

		return mono_domain_get();
	}

	void UnLoadAppDomain()
	{
		MonoDomain* old_domain = mono_domain_get();
		if (old_domain && old_domain != mono_get_root_domain())
		{
			if (!mono_domain_set(mono_get_root_domain(), false))
			{
				last_error = "Error setting domain";
			}

			//mono_thread_pop_appdomain_ref();
			mono_domain_unload(old_domain);
		}

		//unloading the domain
		mono_gc_collect(mono_gc_max_generation());
	}

	bool MonoScripting::CompileFile(const char* input_file, const char* output_file)
	{
		//Set the compiler path
		std::string compiler_path = dll_path;
		compiler_path += "/DATA/Scripting/compiler.dll";

		//Load the compiler.dll
		MonoAssembly* compile_assembly = nullptr;
		compile_assembly = mono_domain_assembly_open(main_domain, compiler_path.c_str());

		if (!compile_assembly)
		{
			last_error = "[error] Compiler not load";
			return false;
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
			return true;
		}
		else
		{
			char alert[300];
			sprintf(alert, "[error] %s", returned_message);
			last_error = alert;
			return false;
		}
	}

	MonoAssemblyName*  MonoScripting::LoadScriptAssembly(const char* assembly_path)
	{
		MonoAssemblyName* ret = nullptr;

		//Load the compiled script assembly
		MonoAssembly* new_script_assembly = nullptr;
		new_script_assembly = mono_domain_assembly_open(mono_domain_get(), assembly_path);

		if (!new_script_assembly)
		{
			last_error = "[error] LoadScriptAssembly: Could not load Assembly, check if the path is correct";
			return nullptr;
		}
		
		ret = mono_assembly_get_name(new_script_assembly);
		return ret;
	}

	bool MonoScripting::UnLoadScriptAssembly(MonoAssemblyName * asm_)
	{
		if (asm_ == nullptr)
		{
			last_error = "[error] Null Assembly on UnLoad!";
			return false;
		}

		MonoAssembly* tmp = mono_assembly_loaded(asm_);
		mono_assembly_close(tmp);

		return true;
	}

	MonoObject*  MonoScripting::CreateMonoObject(MonoAssemblyName* assembly, const char* class_name, const char* name_space)
	{
		//Returned value
		MonoObject* ret = nullptr;

		//Look if assembly is loaded
		MonoAssembly* tmp = mono_assembly_loaded(assembly);

		if (!tmp)
		{
			last_error = "[error] CreateMonoObject: Assembly not Loaded, call LoadScriptAssembly() to get a correct MonoassemblyName";
			return ret;
		}

		//Load the compiled script class and method
		MonoImage* script_image = mono_assembly_get_image(tmp);
		MonoClass* script_class = mono_class_from_name(script_image, name_space, class_name);

		if (!script_class)
		{
			last_error = "[error] CreateMonoObject: Namespace or Class name not Found";
			return ret;
		}

		ret = mono_object_new(mono_domain_get(), script_class);
		return ret;
	}

	MonoImage * LoadMonoImage(MonoAssemblyName * asm_)
	{
		MonoAssembly* tmp = mono_assembly_loaded(asm_);

		if (!tmp)
		{
			last_error = "[error] LoadMonoImage can't load assembly";
			return nullptr;
		}

		return mono_assembly_get_image(tmp);
	}

	bool MonoScripting::ExecuteMethod(MonoObject* script, const char* method_name)// unsigned int num_args, ...)
	{
		MonoClass* script_class = mono_object_get_class(script);

		MonoMethod* method_to_execute = nullptr;
		void* iterator = nullptr;

		//Find the method to invoke
		do
		{
			method_to_execute = mono_class_get_methods(script_class, &iterator);

			if (strcmp(mono_method_get_name(method_to_execute), method_name) == 0)
			{
				break;
			}

		} while (method_to_execute);

		//Didn't find the method name return
		if (!method_to_execute)
		{
			last_error = "[error] ExecuteMethod: Method not Found check if method_name is correct";
			return false;
		}


		mono_runtime_invoke(method_to_execute, script, NULL, NULL);
		return true;
	}

	void AddMethodInternalCall(const char * cs_path, const void * method)
	{
		mono_add_internal_call(cs_path, method);
	}

	const char* MonoStringToConstChar(MonoString* string)
	{
		return mono_string_to_utf8(string);
	}

	const char* MonoScripting::GetFieldsNameAndType(MonoObject* script, void** iterator)
	{
		//Class
		MonoClass* _class = mono_object_get_class(script);

		if (!_class)
		{
			last_error = "[error] GetFieldsNameAndType: Class didn't Loaded check if MonoObject is correct";
			return NULL;
		}

		MonoClassField* field = nullptr;

		//Loop for all fields, returned value will be Name/Type example -> num_gameobjects/int
		field = mono_class_get_fields(_class, iterator);

		if (field)
		{
			//Get the Name
			const char* name = mono_field_get_name(field);

			//Get Type
			MonoType* type = mono_field_get_type(field);
			const char* type_name = mono_type_get_name(type);

			//Set String and fill vector
			char final_str[250];
			sprintf(final_str, "%s/%s", name, type_name);
			return final_str;
		}

		(*iterator) = nullptr;
		return NULL;


	}

	unsigned int MonoScripting::GetFieldValue(MonoObject* script, const char* field_name, void** output_value)
	{
		//Get class and field
		MonoClass* _class = mono_object_get_class(script);

		if (!_class)
		{
			last_error = "[error] GetFieldValue: Class didn't Loaded check if MonoObject is correct";
			return 0;
		}

		MonoClassField* field = mono_class_get_field_from_name(_class, field_name);

		if (!field)
		{
			last_error = "[error] GetFieldValue: Field didn't Loaded check if the field name is correct";
			return 0;
		}

		//Get the value and store it
		MonoType* type = mono_field_get_type(field);
		unsigned int size = mono_type_stack_size(type, NULL);
		void* data = new char[size];
		mono_field_get_value(script, field, data);
		*output_value = data;

		return size;
	}

	MonoClassField * GetFieldValue(MonoImage * image, const char * namespace_, const char * class_name, const char * field_name)
	{
		MonoClass* new_class = mono_class_from_name(image, namespace_, class_name);

		if (new_class == nullptr)
		{
			last_error = "[error] Error getting class in GetFieldValue!";
			return nullptr;
		}

		MonoClassField* field = mono_class_get_field_from_name(new_class, field_name);
		
		if (field == nullptr)
		{
			last_error = "[error] Error getting field in GetFieldValue!";
			return nullptr;
		}

		return field;
	}

	bool  MonoScripting::SetFieldValue(MonoObject* script, const char* field_name, void* input_value)
	{
		//Get class and field
		MonoClass* _class = mono_object_get_class(script);

		if (!_class)
		{
			last_error = "[error] SetFieldValue: Class didn't Loaded check if MonoObject is correct";
			return false;
		}

		MonoClassField* field = mono_class_get_field_from_name(_class, field_name);

		if (!field)
		{
			last_error = "[error] SetFieldValue: Field didn't Loaded check if the field name is correct";
			return false;
		}

		//Set field
		mono_field_set_value(script, field, input_value);

		return true;
	}
	
	void SetFieldValue(MonoObject * script, MonoClassField * field, void * input_value)
	{
		mono_field_set_value(script, field, input_value);
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

	//Load fiesta engine dll
	MonoDomain* dom = mono_jit_init(domain_name);
	MonoAssembly* assembler = nullptr;
	assembler = mono_domain_assembly_open(dom, "C:/Users/Th_Sola/Documents/GitHub/3DEngine/Engine/Data/DATA/Scripting/FiestaEngineEnviroment.dll");


	std::string data_path = my_path;
	data_path += "/DATA/";

	std::string result_file = data_path;
	result_file+= "HelloWorld.dll";

	MonoAssembly* hello_world_assembler = nullptr;
	hello_world_assembler = mono_domain_assembly_open(dom, result_file.c_str());

	if (!hello_world_assembler)
	{
		printf("Error");
	}

	//Execute a file
	MonoImage* hello_world_image = mono_assembly_get_image(hello_world_assembler);
	MonoClass* hello_world_class = mono_class_from_name(hello_world_image, "", "HelloWorld");
	

	//MonoMethod* hello_world_hellman = mono_class_get_method_from_name(parent, "GetPublicProperties", 0);
	MonoMethod* constructor_method = mono_class_get_method_from_name(hello_world_class, ".ctor", 0);
	MonoMethod* change_int_method = mono_class_get_method_from_name(hello_world_class, "whatthehellman", 0);
	MonoObject* hello_world_instance = mono_object_new(dom, hello_world_class);
	mono_runtime_invoke(constructor_method, hello_world_instance, NULL, NULL);

	//mono_add_internal_call("FiestaEngine.FiestaInput::TestMethod", TestMethod);
	mono_runtime_invoke(change_int_method, hello_world_instance, NULL, NULL);

	getchar();

	//int t = *(int*)args_;

	//void* args[1];
	//args[0] = &value;

	//mono_runtime_invoke(change_int_method, hello_world_instance, args, NULL);

	////*value[0] = 2;

	//mono_field_set_value(hello_world_instance, field, &value);
	//
	//mono_runtime_invoke(change_int_method, hello_world_instance, args, NULL);

	//int e = *(int*)mono_object_unbox(hello_world_handle);


	//MonoArray* array_ = (MonoArray*)mono_object_unbox(hello_world_handle);

	//MonoString* omg = mono_array_get(array_, MonoString*, 0);
	//int k = mono_array_length(hello_world_handle);
	//MonoString* val;
	//for (int g = 0; g < k; g++)
	//{
	//	val = mono_array_get(hello_world_handle, MonoString*, g);
	//	tuputamadre = mono_string_to_utf8(val);
	//}
	//
	//
	//getchar();

}
