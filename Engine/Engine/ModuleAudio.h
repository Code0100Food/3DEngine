#ifndef __MODULE_AUDIO_H__
#define __MODULE_AUDIO_H__

#include "Module.h"
#include "SDL_mixer\include\SDL_mixer.h"

#define DEFAULT_MUSIC_FADE_TIME 2.0f
#define MAX_VOLUME 128

enum FX_ID
{
	UNDEF_FX= 0,
	APPLY_FX
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

	ModuleAudio(bool start_enabled = true);

public:

	bool Awake(const JSON_Object* data_root)final;
	bool Init() final;
	bool Start()final;
	bool CleanUp() final;
	void BlitConfigInfo()final;

private:

	Mix_Music*			music = nullptr;
	list<FX_Chunk>		fx;
	int					master_volume = 0;
	bool				fx_on_input = true;

public:

	// Play a music file
	bool			PlayMusic(const char* path, float fade_time = DEFAULT_MUSIC_FADE_TIME);
	// Load a WAV in memory
	unsigned int	LoadFx(const char* path, FX_ID id);
	// Play a previously loaded WAV
	bool			PlayFx(FX_ID id,int channel = -1, int repeat = 0);
	bool			PlayFxForInput(FX_ID id);

	//Update master volume
	void			SetMasterVolume(int volume);

};
/// -----------------------------------
#endif // __MODULE_AUDIO_H__