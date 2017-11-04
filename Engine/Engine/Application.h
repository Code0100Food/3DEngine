#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <list>
#include "Globals.h"
#include "Timer.h"
#include "PerfTimer.h"
#include "MathGeoLib/Algorithm/Random/LCG.h"

#define GRAPH_ARRAY_SIZE 30

using namespace std;

class Module;
class ModuleWindow;
class FileSystem;
class ModuleInput;
class InputManager;
class ModuleAudio;
class ModuleRenderer3D;
class ModuleCamera3D;
class ModuleImgui;
class ModuleConsole;
class ModuleHardware;
class ModuleTextures;
class GeometryManager;
class Profiler;
class Importer;
class Serializer;
class ModuleScene;
class DockContext;
class ImporterManager;
class TimeManager;

enum MODULE_ID;

enum APP_CONTEXT
{
	UNKNOWN_CONTEXT = 0,
	BUILD_CONTEXT,
	PLAY_CONTEXT,
	IN_GAME_CONTEXT,
	PAUSE_CONTEXT,
	MAIN_MENU_CONTEXT
};

class Application
{
public:

	Application();
	~Application();

public:

	bool			Awake();
	bool			Init();
	void			PrepareUpdate();
	update_status	Update();
	void			FinishUpdate();
	bool			CleanUp();

public:

	ModuleWindow*		window = nullptr;
	FileSystem*			fs = nullptr;
	ModuleInput*		input = nullptr;
	InputManager*		input_manager = nullptr;
	ModuleAudio*		audio = nullptr;
	ModuleRenderer3D*	renderer3D = nullptr;
	ModuleCamera3D*		camera = nullptr;
	ModuleImgui*		imgui = nullptr;
	ModuleConsole*		console = nullptr;
	ModuleHardware*		hard = nullptr;
	ModuleTextures*		textures = nullptr;
	GeometryManager*	geometry = nullptr;
	TimeManager*		time_manager = nullptr;
	ModuleScene*		scene = nullptr;

	//Tools 
	Profiler*			profiler = nullptr;
	ImporterManager*	importer = nullptr;
	Serializer*			serializer = nullptr;
	math::LCG*			randomizer = nullptr;

	APP_CONTEXT			app_context = UNKNOWN_CONTEXT;

private:

	PerfTimer		prof_timer;
	PerfTimer		m_prof_timer;

	list<Module*>	list_modules;
	bool			want_to_quit = false;

	float			fps_array[GRAPH_ARRAY_SIZE];
	float			miliseconds_array[GRAPH_ARRAY_SIZE];
	bool			config_opened = false;
	bool			show_config_window = true;

	DockContext*	config_dock = nullptr;

public:
	
	std::string		app_name;
	std::string		organization;

public:

	// Used in data loading
	APP_CONTEXT	StrToAppContext(const char* str) const;
	const char*	AppContextToStr(APP_CONTEXT app_context) const;
	MODULE_ID	StrToModuleID(const char* str)const;

	void SetQuit();
	void RequestBrowser(const char* link);
	void ShowConfiguration();
	bool GetConfigWindowState()const;
	void BlitConfigWindow();

private:

	void AddModule(Module* mod);

};

extern Application* App;

#endif // !_APPLICATION_H_


