#ifndef _MODULE_RENDER_3D_H_
#define _MODULE_RENDER_3D_H_

#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"

#define MAX_LIGHTS 8

class ModuleRenderer3D : public Module
{
public:

	ModuleRenderer3D(bool start_enabled = true);

public:

	bool			Init() final;
	update_status	PreUpdate(float dt) final;
	update_status	PostUpdate(float dt) final;
	bool			CleanUp() final;

public:

	Light			lights[MAX_LIGHTS];
	SDL_GLContext	context;
	mat3x3			NormalMatrix;
	mat4x4			ModelMatrix, ViewMatrix, ProjectionMatrix;

public:

	void OnResize(int width, int height);
};


#endif // !_MODULE_RENDER_3D_H_


