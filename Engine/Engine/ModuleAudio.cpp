#include "Globals.h"
#include "Application.h"
#include "ModuleAudio.h"
#include "FileSystem.h"

#pragma comment( lib, "Engine/SDL_mixer/libx86/SDL2_mixer.lib" )

// Constructors =================================
ModuleAudio::ModuleAudio(bool start_enabled) : Module(start_enabled), music(NULL)
{
	name = "Audio";
}

// Game Loop ====================================
// Called before render is available
bool ModuleAudio::Awake(const JSON_Object * data_root)
{
	//Load master volume
	master_volume = json_object_get_number(data_root, "master_volume");
	fx_on_input = json_object_get_boolean(data_root, "fx_on_input");

	config_menu = true;

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

	return true;
}

bool ModuleAudio::Start()
{
	//Apply the loaded master volume
	SetMasterVolume(master_volume);

	//Load all the engine fx
	std::string fx_dir = name + "/camera_fx.wav";
	LoadFx(fx_dir.c_str(), APPLY_FX);

	return true;
}

// Called before quitting
bool ModuleAudio::CleanUp()
{
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

	return true;
}

void ModuleAudio::BlitConfigInfo()
{
	//Master volume slice
	ImGui::SliderInt("Master Volume", &master_volume, 0, MAX_VOLUME);
	
	//Input FX check box
	ImGui::Checkbox("Input FX", &fx_on_input);

	ImGui::Separator();

	//Apply button
	if (ImGui::Button("Apply##audio_apply", ImVec2(50, 20)))
	{
		
		//Save values
		//Load config json file
		const JSON_Value *config_data = App->fs->LoadJSONFile("config.json");
		assert(config_data != NULL);

		//Save the new variables
		json_object_set_number(App->fs->AccessObject(config_data, 1, name.c_str()), "master_volume", master_volume);
		json_object_set_boolean(App->fs->AccessObject(config_data, 1, name.c_str()), "fx_on_input", fx_on_input);

		//Save the file
		App->fs->SaveJSONFile(config_data, "config.json");
		json_value_free((JSON_Value*)config_data);

		//Apply immediate effects
		SetMasterVolume(master_volume);

		//Play save fx
		App->audio->PlayFxForInput(FX_ID::APPLY_FX);
	}
}

// Functionality ================================
// Play a music file
bool ModuleAudio::PlayMusic(const char* path, float fade_time)
{
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
	return ret;
}

// Load WAV
unsigned int ModuleAudio::LoadFx(const char* path, FX_ID id)
{
	unsigned int ret = 0;

	Mix_Chunk* chunk = Mix_LoadWAV(path);

	if(chunk == NULL)
	{
		LOG("Cannot load wav %s. Mix_GetError(): %s", path, Mix_GetError());
	}
	else
	{
		FX_Chunk new_fx(chunk, id);
		fx.push_back(new_fx);
		ret = fx.size();
	}

	return ret;
}

// Play WAV
bool ModuleAudio::PlayFx(FX_ID id,int channel, int repeat)
{
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
	
	if (ret)Mix_PlayChannel(channel, (*it).data, repeat);
	
	return ret;
}

bool ModuleAudio::PlayFxForInput(FX_ID id)
{
	if (!fx_on_input)return false;

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

	if (ret)Mix_PlayChannel(-1, (*it).data, 0);

	return ret;
}

void ModuleAudio::SetMasterVolume(int volume)
{
	master_volume = volume;
	Mix_Volume(-1, master_volume);
	Mix_VolumeMusic(master_volume);
}
