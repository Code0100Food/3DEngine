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

	bool Start()final;
	
private:

	unsigned char checkImage[CHECKERS_HEIGHT][CHECKERS_WIDTH][4];
	
public:

	uint check_image = 0;
	uint lenna_porn = 0;

public:

	uint LoadTexture(const char* str);

};
#endif