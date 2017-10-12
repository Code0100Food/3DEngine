#include "Globals.h"
#include "Application.h"
#include "ModuleAudio.h"
#include "FileSystem.h"

#pragma comment( lib, "Engine/SDL_mixer/libx86/SDL2_mixer.lib" )

// Constructors =================================
ModuleAudio::ModuleAudio(const char* _name, MODULE_ID _id, bool _config_menu, bool _enabled) : Module(_name, _id, _config_menu, _enabled), music(NULL)
{

}

// Destructors ==================================
ModuleAudio::~ModuleAudio()
{

}

// Game Loop ====================================
// Called before render is available
bool ModuleAudio::Awake(const JSON_Object * data_root)
{
	//Load master volume
	master_volume = json_object_get_number(data_root, "master_volume");
	fx_on_input = json_object_get_boolean(data_root, "fx_on_input");

	config_menu = true;

	num_audio_devices = SDL_GetNumAudioDrivers();

	for (int i = 0; i < num_audio_devices; i++)
		audio_drivers.push_back(SDL_GetAudioDriver(i));

	return true;
}

bool ModuleAudio::Init()
{
	LOG("Loading Audio Mixer");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
		LOG("[error] SDL_INIT_AUDIO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	// load support for the OGG format
	int flags = MIX_INIT_OGG;
	int init = Mix_Init(flags);

	if((init & flags) != flags)
	{
		LOG("[error] Could not initialize Mixer lib. Mix_Init: %s", Mix_GetError());
		ret = false;
	}

	//Initialize SDL_mixer
	if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		LOG("[error] SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
		ret = false;
	}

	if (!ret)this->enabled = false;

	return true;
}

bool ModuleAudio::Start()
{
	if (!enabled)return true;

	//Apply the loaded master volume
	SetMasterVolume(master_volume);

	//Load all the engine fx
	std::string fx_dir = App->fs->data_folder + "/" + name + "/apply_fx.wav";
	LoadFx(fx_dir.c_str(), APPLY_FX);
	fx_dir = App->fs->data_folder + "/" +  name + "/slice_tick_fx.wav";
	LoadFx(fx_dir.c_str(), SLICE_TICK_FX);
	fx_dir = App->fs->data_folder + "/" + name + "/checkbox_fx.wav";
	LoadFx(fx_dir.c_str(), CHECKBOX_FX);
	fx_dir = App->fs->data_folder + "/" + name + "/window_fx.wav";
	LoadFx(fx_dir.c_str(), WINDOW_FX);


	return true;
}

// Called before quitting
bool ModuleAudio::CleanUp()
{
	if (!enabled)
		return true;

	LOG("Freeing sound FX, closing Mixer and Audio subsystem");

	if(music != NULL)
	{
		Mix_FreeMusic(music);
	}

	for (std::list<FX_Chunk>::iterator it = fx.begin(); it != fx.end(); it++)
	{
		Mix_FreeChunk((*it).data);
	}

	fx.clear();
	Mix_CloseAudio();
	Mix_Quit();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);

	audio_logs.clear();

	return true;
}

void ModuleAudio::BlitConfigInfo()
{
	//Master volume slice
	if (ImGui::SliderInt("Master Volume", &master_volume, 0, MAX_VOLUME))
	{
		if (enabled)SetMasterVolume(master_volume);
		PlayFxForInput(SLICE_TICK_FX);
	}
	ImGui::SameLine(); ImGui::MyShowHelpMarker("(?)", "Change the master audio volume");

	//Input FX check box
	ImGui::Checkbox("Input FX", &fx_on_input);
	ImGui::SameLine(); ImGui::MyShowHelpMarker("(?)", "Turn ON/OFF all the input audio effects.");

	ImGui::Separator();
	ImGui::Separator();

	//Drivers
	ImGui::Text("Total Audio drivers:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "%i", num_audio_devices);

	for (std::vector<string>::iterator item = audio_drivers.begin(); item != audio_drivers.end(); item++)
	{
		ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "%s, ", (*item).c_str());
		ImGui::SameLine();
	}

	//Audio Callbacks
	ImGui::NewLine();
	ImGui::Separator();
	ImGui::Separator();
	ImGui::TextColored(ImVec4(0.2f, 0.2f, 0.8f, 1.0f), "History");

	ImGui::BeginGroup();
	ImGui::BeginChild(ImGui::GetID((void*)(intptr_t)1), ImVec2(400.0f, 200.0f), true);
	if (scroll_audio_calls)
	{
		ImGui::SetScrollFromPosY(ImGui::GetScrollMaxY(), 0.25f);
		scroll_audio_calls = false;
	}
	uint vec_size = audio_logs.size();
	for (uint k = 0; k < vec_size; k++)
	{
		ImGui::Text(audio_logs[k].c_str());
	}

	ImGui::EndChild();
	ImGui::EndGroup();
}

void ModuleAudio::SaveConfigInfo(JSON_Object * data_root)
{
	json_object_set_number(data_root, "master_volume", master_volume);
	json_object_set_boolean(data_root, "fx_on_input", fx_on_input);
}

// Functionality ================================
// Play a music file
bool ModuleAudio::PlayMusic(const char* path, float fade_time)
{
	if (!enabled)return false;

	bool ret = true;
	
	if(music != NULL)
	{
		if(fade_time > 0.0f)
		{
			Mix_FadeOutMusic((int) (fade_time * 1000.0f));
		}
		else
		{
			Mix_HaltMusic();
		}

		// this call blocks until fade out is done
		Mix_FreeMusic(music);
	}

	music = Mix_LoadMUS(path);

	if(music == NULL)
	{
		LOG("[error] Cannot load music %s. Mix_GetError(): %s\n", path, Mix_GetError());
		ret = false;
	}
	else
	{
		if(fade_time > 0.0f)
		{
			if(Mix_FadeInMusic(music, -1, (int) (fade_time * 1000.0f)) < 0)
			{
				LOG("[error] Cannot fade in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
		else
		{
			if(Mix_PlayMusic(music, -1) < 0)
			{
				LOG("[error] Cannot play in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
	}

	LOG("Successfully playing %s", path);
	
	//Console call
	if(!ret)AddConsoleLabel("Error Playing Music: %s", path);
	else AddConsoleLabel("Playing Music: %s", path);

	return ret;
}

// Load WAV
unsigned int ModuleAudio::LoadFx(const char* path, FX_ID id)
{
	if (!enabled)return 0;

	unsigned int ret = 0;

	Mix_Chunk* chunk = Mix_LoadWAV(path);

	if(chunk == NULL)
	{
		LOG("Cannot load wav %s. Mix_GetError(): %s", path, Mix_GetError());
		AddConsoleLabel("Error loading FX: %s", path);
	}
	else
	{
		FX_Chunk new_fx(chunk, id);
		fx.push_back(new_fx);
		ret = fx.size();
		AddConsoleLabel("FX Loaded: %s", path);
	}

	return ret;
}

const char* ModuleAudio::FxIdToStr(FX_ID id) const 
{
	switch (id)
	{
	case UNDEF_FX: return "Undef";
		break;
	case APPLY_FX:	return "Apply FX";
		break;
	case SLICE_TICK_FX:	return "Slice FX";
		break;
	case CHECKBOX_FX:	return "CheckBox FX";
		break;
	case WINDOW_FX:	return "Window FX";
		break;
	}
}

// Play WAV
bool ModuleAudio::PlayFx(FX_ID id,int channel, int repeat)
{
	if (!enabled)return false;

	bool ret = false;

	std::list<FX_Chunk>::iterator it = fx.begin();
	while (it != fx.end())
	{
		if (it._Ptr->_Myval.id == id)
		{
			ret = true;
			break;
		}
		it++;
	}
	
	if (ret)
	{
		Mix_PlayChannel(channel, (*it).data, repeat);
		//Console call
		AddConsoleLabel("Playing FX: %s", FxIdToStr(id));
	}
	
	//Console call
	else AddConsoleLabel("Error playing FX: %s", FxIdToStr(id));

	return ret;
}

bool ModuleAudio::PlayFxForInput(FX_ID id)
{
	if (!fx_on_input || !enabled)return false;

	bool ret = false;

	std::list<FX_Chunk>::iterator it = fx.begin();
	while (it != fx.end())
	{
		if (it._Ptr->_Myval.id == id)
		{
			ret = true;
			break;
		}
		it++;
	}

	if (ret)
	{
		Mix_PlayChannel(-1, (*it).data, 0);
		//Console call
		AddConsoleLabel("Playing FX: %s", FxIdToStr(id));
	}

	//Console call
	else AddConsoleLabel("Error playing FX: %s", FxIdToStr(id));

	return ret;
}

void ModuleAudio::SetMasterVolume(int volume)
{
	master_volume = volume;
	Mix_Volume(-1, master_volume);
	Mix_VolumeMusic(master_volume);
}

void ModuleAudio::AddConsoleLabel(const char* action,...)
{
	//Build and push a console label
	char buf[1024];
	va_list args;
	va_start(args, action);
	vsnprintf(buf, IM_ARRAYSIZE(buf), action, args);
	buf[IM_ARRAYSIZE(buf) - 1] = 0;
	va_end(args);

	audio_logs.push_back(buf);
	scroll_audio_calls = true;
}
