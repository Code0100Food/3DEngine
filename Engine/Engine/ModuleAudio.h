#ifndef __MODULE_AUDIO_H__
#define __MODULE_AUDIO_H__

#include "Module.h"
#include "SDL_mixer\include\SDL_mixer.h"

#define DEFAULT_MUSIC_FADE_TIME 2.0f
#define MAX_VOLUME 128

class ModuleAudio : public Module
{
public:

	ModuleAudio(bool start_enabled = true);

public:

	bool Awake(const JSON_Object* data_root)final;
	bool Init() final;
	bool CleanUp() final;
	void BlitConfigInfo()final;

private:

	Mix_Music*			music = nullptr;
	list<Mix_Chunk*>	fx;
	int				master_volume = 0;

public:

	// Play a music file
	bool			PlayMusic(const char* path, float fade_time = DEFAULT_MUSIC_FADE_TIME);
	// Load a WAV in memory
	unsigned int	LoadFx(const char* path);
	// Play a previously loaded WAV
	bool			PlayFx(unsigned int fx,int channel = -1, int repeat = 0);
	//Update master volume
	void			SetMasterVolume(int volume);

};

#endif // __MODULE_AUDIO_H__