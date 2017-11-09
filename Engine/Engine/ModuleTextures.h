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
	bool CleanUp()final;

private:

	unsigned char check_image_array[CHECKERS_HEIGHT][CHECKERS_WIDTH][4];

	bool check_mode = false;
	bool custom_mode = false;
	bool mesh_mode = true;
	
public:

	uint check_image = 0;
	uint custom_check_image = 0;

	//Icons 
	uint play_icon = 0;
	uint play_click_icon = 0;
	uint pause_icon = 0;
	uint pause_click_icon = 0;
	uint next_icon = 0;
	uint next_click_icon = 0;

	uint garbage_icon = 0;
	uint up_arrow_icon = 0;
	uint down_arrow_icon = 0;

	uint transform_icon = 0;

public:

	//Set Methods -----------
	void SetCustomMode();

	//Get Methods -----------
	bool GetCheckMode() const;
	bool GetCustomMode() const;
	bool GetMeshMode() const;

	//Functionality ---------
	uint LoadTexture(const char* str, const char* folder);
	bool LoadCustomTexture(const char* str);

};
#endif