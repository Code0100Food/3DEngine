#include "ScriptManager.h"

#include "Mono_Scripting/test.h"
#pragma comment(lib, "Engine/Mono_Scripting/MonoScripting.lib")

ScriptManager::ScriptManager()
{

	MonoScripting::InitMono();
	lol = MonoScripting::GetDLLPath();
}

ScriptManager::~ScriptManager()
{
}

const char* ScriptManager::Compile(const char * path, const char * output)
{
	return MonoScripting::CompileFile(path, output);
}
