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

	bool			Awake(const JSON_Object* data_root)final;
	bool			Init() final;
	update_status	PreUpdate(float dt) final;
	update_status	PostUpdate(float dt) final;
	bool			CleanUp() final;
	void			BlitConfigInfo()final;

private:

	bool	vsync = true;
	bool	depth_test = true;
	bool	cull_face = true;
	bool	texture_2d = true;
	bool	lighting = true;
	bool	color_material = true;
	bool	dither = true;

	bool	fog = true;
	float	fog_density = 0.0f;


public:

	Light			lights[MAX_LIGHTS];
	SDL_GLContext	context;
	mat3x3			NormalMatrix;
	mat4x4			ModelMatrix, ViewMatrix, ProjectionMatrix;

public:

	void OnResize(int width, int height);
};


#endif // !_MODULE_RENDER_3D_H_


