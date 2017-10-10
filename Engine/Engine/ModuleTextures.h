#ifndef _MODULE_TEXTURES_H_
#define _MODULE_TEXTURES_H_

#include "Module.h"

#define CHECKERS_HEIGHT 256
#define CHECKERS_WIDTH 256

class ModuleTextures : public Module
{
public:

	ModuleTextures(const char* _name, MODULE_ID _id, bool _config_menu, bool _enabled = true);
	~ModuleTextures();

public:

	bool Awake(const JSON_Object* data_root)final;
	bool Start()final;
	void BlitConfigInfo()final;

private:

	unsigned char checkImage[CHECKERS_HEIGHT][CHECKERS_WIDTH][4];

	bool check_mode = false;
	bool custom_mode = false;
	bool mesh_mode = true;
	
public:

	uint check_image = 0;
	uint custom_check_image = 0;

public:

	uint LoadTexture(const char* str, const char* folder);
	bool LoadCustomTexture(const char* str);

	bool GetCheckMode() const;
	bool GetCustomMode() const;
	bool GetMeshMode() const;

};
#endif