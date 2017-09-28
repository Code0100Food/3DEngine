#define _CRT_SECURE_NO_WARNINGS

#include <string>
#include "SDL/include/SDL.h"
#include "SDL/include/SDL_opengl.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "Parson/parson.h"
#include "cURL\include\curl\curl.h"
#include <vector>
#include <shellapi.h>

#pragma comment( lib, "SDL/libx86/SDL2.lib" )
#pragma comment( lib, "SDL/libx86/SDL2main.lib" )
#pragma comment (lib, "glut/glut32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib")

#ifdef _DEBUG
#pragma comment (lib, "cURL/lib/libcurl_debug.lib")
#else
#pragma comment (lib, "cURL/lib/libcurl.lib")
#endif // _DEBUG

enum LAUNCHER_FINISH
{
	START_ENGINE,
	CLOSE_ENGINE
};

using namespace std;

bool WindowInit(SDL_Window** window, SDL_Surface** screen_surface)
{
	bool ret = true;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		ret = false;
	}
	else
	{
		//Create window
		int w_width = 500;
		int w_height = 500;
		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		//Use OpenGL 2.1
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

		*window = SDL_CreateWindow("Fiesta Engine Launcher", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w_width, w_height, flags);

		

		if (window == NULL)
		{
			//LOG("[error] Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface
			*screen_surface = SDL_GetWindowSurface(*window);

			//Setup ImGui binding
			ImGui_ImplSdl_Init(*window);
		}
	}

	return ret;
}
bool RenderInit(SDL_GLContext* context, SDL_Window* window)
{
	bool ret = true;

	//Create context
	*context = SDL_GL_CreateContext(window);
	if (context == NULL)
	{
		ret = false;
	}

	if (ret == true)
	{
		//Use Vsync
		if (SDL_GL_SetSwapInterval(1) < 0)
			//LOG("[error] Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			//LOG("[error] Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			//LOG("[error] Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);

		//Initialize clear color
		glClearColor(0.4f, 0.0f, 0.4f, 1.f);

		//Check for error
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			//LOG("[error] Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		GLfloat MaterialAmbient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_COLOR_MATERIAL);
	}

	return ret;
}
bool InputInit()
{
	bool ret = true;
	SDL_Init(0);

	if (SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		//LOG("[error] SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}
bool GetInput()
{
	SDL_PumpEvents();

	bool quit = false;
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		ImGui_ImplSdl_ProcessEvent(&e);

		switch (e.type)
		{
		case SDL_KEYDOWN:
			break;
		case SDL_MOUSEWHEEL:
			break;

		case SDL_MOUSEMOTION:
			break;

		case SDL_QUIT:
			quit = true;
			break;

		case SDL_WINDOWEVENT:
			break;
		}
	}

	if (quit == true)
		return false;
	
	return true;
}

//curl functions
string curl_data;

size_t writeCallback(char* buf, size_t size, size_t nmemb, void* up)
{ //callback must have this declaration
  //buf is a pointer to the data that curl has for us
  //size*nmemb is the size of the buffer

	for (int c = 0; c<size*nmemb; c++)
	{
		curl_data.push_back(buf[c]);
	}
	return size*nmemb; //tell curl how many bytes we handled
}
size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
	size_t written;
	written = fwrite(ptr, size, nmemb, stream);
	return written;
}
SDL_version GetVersion(string str)
{
	SDL_version ret;

	int rounds = 0;
	for (int pos = 0; rounds < str.length(); rounds++)
	{
		pos = str.find_first_not_of('.', pos);
		if (pos == -1) break;

		if (rounds == 0) ret.major = atoi(&str.at(pos));
		
		if (rounds == 1) ret.minor = atoi(&str.at(pos));

		if (rounds == 2)
		{
			ret.patch = atoi(&str.at(pos));
			break;
		}
		pos++;
	}

	return ret;

}

int main(int argc, char ** argv)
{
	bool ret = 0;

	//Launcher logic
	string download_url;
	string json_url = "http://fiestaengine.dx.am/updates.json";
	JSON_Value* reader = json_parse_file("DATA/update.json");
	JSON_Object* root_object = json_value_get_object(reader);
	JSON_Object* update_data = json_object_dotget_object(root_object, "Fiesta Engine");

	//Save the actual version
	string str_version = json_object_get_string(update_data, "version");
	SDL_version current_version = GetVersion(str_version);
	SDL_version new_version;
	bool outdated = false;

	CURL* curl = nullptr;
	CURLcode res;
	FILE* fp = nullptr;
	char filename[FILENAME_MAX] = "new_update.json";

	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();

	if (curl)
	{
		fp = fopen(filename, "w+");
		curl_easy_setopt(curl, CURLOPT_URL, json_url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

		/* get it! */
		res = curl_easy_perform(curl);
		fclose(fp);

		//Look new update
		JSON_Value* new_reader = json_parse_file("new_update.json");
		const JSON_Object* new_root_object = json_value_get_object(new_reader);
		const JSON_Object* new_update_data = json_object_dotget_object(new_root_object, "Fiesta Engine");
		string str_new_version = json_object_get_string(new_update_data, "version");
		new_version = GetVersion(str_new_version);

		if ((new_version.major != current_version.major) || (new_version.minor != current_version.minor) || (new_version.patch != current_version.patch))
		{
			outdated = true;
			download_url = json_object_get_string(new_update_data, "url");
		}

		remove("new_update.json");
		json_value_free(new_reader);
		curl_easy_cleanup(curl);
	}
	
	//Launcher display
	//window rendering
	SDL_Window* window = nullptr;
	//The surface contained by the window
	SDL_Surface* screen_surface = nullptr;

	SDL_GLContext context;
	bool input = InputInit();
	bool loop = WindowInit(&window, &screen_surface);
	bool loop2 = RenderInit(&context, window);

	while (loop)
	{
		//Starts Frame
		ImGui_ImplSdl_NewFrame(window);

		loop = GetInput();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		//Launcher UI
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::Begin("Welcome to the Fiesta Engine Launcher",0, ImVec2(500, 500),0.0f,ImGuiWindowFlags_::ImGuiWindowFlags_NoMove| ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar);
		ImGui::TextColored(ImVec4(0.2, 0.5, 0.8, 1.0), "Welcome to the Fiesta Engine Launcher!");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.0, 0.8, 0.8, 1.0), "v%i.%i.%i", current_version.major, current_version.minor, current_version.patch);
		if (outdated)
		{
			ImGui::Separator();
			ImGui::Text("There's a new version available");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(0.0, 0.8, 0.8, 1.0), "v%i.%i.%i", new_version.major, new_version.minor, new_version.patch);
			ImGui::SameLine();
			if (ImGui::Button("Download!"))
			{
				ShellExecute(NULL, "open", download_url.c_str(), NULL, NULL, SW_SHOWNORMAL);
			}
		}
		else
		{
			ImGui::Text("There isn't a new version available");
		}
		
		ImGui::Separator();
		ImGui::Separator();
		ImGui::Separator();

		if (ImGui::Button("Start", ImVec2(238, 50)))
		{
			ret = 1;
			loop = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("Close", ImVec2(238, 50)))
		{
			loop = false;
		}
		
		ImGui::Separator();
		ImGui::Separator();
		ImGui::Separator();

		ImGui::BulletText("A 3D engine developed by two students of video games development.");
		ImGui::BulletText("This code is under the MIT License.");
		ImGui::Separator();
		ImGui::BulletText("Ferran Martin Vila");
		ImGui::SameLine();
		if (ImGui::Button("Info##f"))
		{
			ShellExecute(NULL, "open", "https://github.com/ferranmartinvila", NULL, NULL, SW_SHOWNORMAL);
		}
		ImGui::BulletText("Eric Sola Vila");
		ImGui::SameLine();
		if (ImGui::Button("Info##e"))
		{
			ShellExecute(NULL, "open", "https://github.com/HeladodePistacho", NULL, NULL, SW_SHOWNORMAL);
		}
		ImGui::End();


		ImGui::Render();

		//Finish Frame
		SDL_GL_SwapWindow(window);
	}
	
	//Close everything
	ImGui::Shutdown();
	if (window != NULL)
		SDL_DestroyWindow(window);
	SDL_GL_DeleteContext(context);
	SDL_Quit();
	json_value_free(reader);
	curl_global_cleanup();

	
	if (ret == 0) exit(LAUNCHER_FINISH::CLOSE_ENGINE);
	else exit(LAUNCHER_FINISH::START_ENGINE);

	return ret;
}