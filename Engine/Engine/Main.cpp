#include <stdlib.h>
#include "Application.h"
#include "Globals.h"

#include "SDL/include/SDL.h"

#pragma comment( lib, "Engine/SDL/libx86/SDL2.lib" )
#pragma comment( lib, "Engine/SDL/libx86/SDL2main.lib" )


enum main_states
{
	MAIN_CREATION,
	MAIN_START,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};

Application* App = NULL;

int main(int argc, char ** argv)
{
	LOG("Starting the engine!");

	int main_return = EXIT_FAILURE;
	main_states state = MAIN_CREATION;


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