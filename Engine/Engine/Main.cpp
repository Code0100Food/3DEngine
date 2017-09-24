#include <stdlib.h>
#include "Application.h"
#include "Globals.h"

#include "SDL/include/SDL.h"

#pragma comment( lib, "Engine/SDL/libx86/SDL2.lib" )
#pragma comment( lib, "Engine/SDL/libx86/SDL2main.lib" )

#include <direct.h>
#define GetCurrentDir _getcwd

enum main_states
{
	MAIN_CREATION,
	MAIN_START,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};
enum LAUNCHER_STATE
{
	START_ENGINE = 0,
	CLOSE_ENGINE
};

int StartLauncher()
{
	int ret = -1;

	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi = { 0 };

	char myPath[_MAX_PATH + 1];
	GetCurrentDir(myPath, sizeof(myPath));

	string launcher_path = myPath;
	launcher_path += "/Launcher.exe";
	
	int res = CreateProcess(launcher_path.c_str(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

	if (!res)
	{
		LOG("Error in test call!");
	}
	else
	{
		DWORD exit;
		LOG("Process correctly started");
		while (WAIT_OBJECT_0 != WaitForSingleObject(pi.hProcess, INFINITE));

		GetExitCodeProcess(pi.hProcess, &exit);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		ret = (int)exit;
		
	}
	return ret;
}

Application* App = NULL;

int main(int argc, char ** argv)
{
	LOG("Starting the engine!");

	int main_return = EXIT_FAILURE;
	main_states state = MAIN_CREATION;

	LAUNCHER_STATE launch = (LAUNCHER_STATE)StartLauncher();

	if (launch == LAUNCHER_STATE::CLOSE_ENGINE)
		return main_return;

	while (state != MAIN_EXIT)
	{
		switch (state)
		{
		case MAIN_CREATION:

			LOG("-------------- Application Creation --------------");
			App = new Application();
			state = MAIN_START;
			break;

		case MAIN_START:

			LOG("-------------- Application Awake --------------");
			if (App->Awake() == false)
			{
				LOG("[error] Application Awake exits with ERROR");
				state = MAIN_EXIT;
			}

			LOG("-------------- Application Init --------------");
			if (App->Init() == false)
			{
				LOG("[error] Application Init exits with ERROR");
				state = MAIN_EXIT;
			}
			else
			{
				state = MAIN_UPDATE;
				LOG("-------------- Application Update --------------");
			}

			break;

		case MAIN_UPDATE:
		{
			update_status update_return = App->Update();

			if (update_return == UPDATE_ERROR)
			{
				LOG("[error] Application Update exits with ERROR");
				state = MAIN_EXIT;
			}

			if (update_return == UPDATE_STOP)
			{
				state = MAIN_FINISH;
			}
		}
			break;

		case MAIN_FINISH:

			LOG("-------------- Application CleanUp --------------");
			if (App->CleanUp() == false)
			{
				LOG("[error] Application CleanUp exits with ERROR");
			}
			else
			{
				main_return = EXIT_SUCCESS;
			}

			state = MAIN_EXIT;

			break;

		}
	}

	LOG("Exiting '%s'...\n", App->app_name.c_str());

	RELEASE(App);



	return main_return;
}