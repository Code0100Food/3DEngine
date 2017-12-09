#include "ScriptManager.h"

#include "Mono_Scripting/test.h"
#pragma comment(lib, "Engine/Mono_Scripting/MonoScripting.lib")

ScriptManager::ScriptManager()
{

	MonoScripting::InitMono();
	dll_path = MonoScripting::GetDLLPath();
}

ScriptManager::~ScriptManager()
{
	MonoScripting::CleanUpMono();
}

const char* ScriptManager::Compile(const char * path, const char * output)
{
	return MonoScripting::CompileFile(path, output);
}
