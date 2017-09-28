#ifndef __MODULE_AUDIO_H__
#define __MODULE_AUDIO_H__

#include "Module.h"
#include "SDL_mixer\include\SDL_mixer.h"

#define DEFAULT_MUSIC_FADE_TIME 2.0f
#define MAX_VOLUME 128

enum FX_ID
{
	UNDEF_FX= 0,
	APPLY_FX,
	SLICE_TICK_FX,
	CHECKBOX_FX,
	WINDOW_FX
};

/// FX_Chunk --------------------------
struct FX_Chunk
{
	FX_Chunk(Mix_Chunk* _data, FX_ID _id) :data(_data), id(_id) {}

	Mix_Chunk*	data = nullptr;
	FX_ID		id = UNDEF_FX;
};
/// -----------------------------------

/// ModuleAudio -----------------------
class ModuleAudio : public Module
{
public:

	ModuleAudio(const char* _name, MODULE_ID _id, bool _config_menu, bool _enabled = true);
	~ModuleAudio();

public:

	bool Awake(const JSON_Object* data_root)final;
	bool Init() final;
	bool Start()final;
	bool CleanUp() final;
	void BlitConfigInfo()final;
	void SaveConfigInfo(JSON_Object* data_root);

private:

	Mix_Music*			music = nullptr;
	list<FX_Chunk>		fx;
	int					master_volume = 0;
	bool				fx_on_input = true;

	//drivers
	vector<string>		audio_drivers;
	int					num_audio_devices = 0;

	//Audio Log Console(no input just output)
	std::vector<std::string>	audio_logs;

public:

	// Play a music file
	bool			PlayMusic(const char* path, float fade_time = DEFAULT_MUSIC_FADE_TIME);
	// Load a WAV in memory
	unsigned int	LoadFx(const char* path, FX_ID id);
	const char*		FxIdToStr(FX_ID id)const;
	// Play a previously loaded WAV
	bool			PlayFx(FX_ID id,int channel = -1, int repeat = 0);
	bool			PlayFxForInput(FX_ID id);

	//Update master volume
	void			SetMasterVolume(int volume);

	//Add console label
	void AddConsoleLabel(const char* action, const char* path);

};
/// -----------------------------------
#endif // __MODULE_AUDIO_H__