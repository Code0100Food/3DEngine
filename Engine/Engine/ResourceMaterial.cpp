#include "ResourceMaterial.h"
#include "Serializer.h"
#include "Application.h"
#include "ImporterManager.h"
#include "imgui/imgui.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "FileSystem.h"

// Constructors =================================
ResourceMaterial::ResourceMaterial() :Resource(MATERIAL_RESOURCE)
{

}

// Destructors ==================================
ResourceMaterial::~ResourceMaterial()
{

}

// Set Methods ==================================
void ResourceMaterial::SetWidth(uint val)
{
	width = val;
}

void ResourceMaterial::SetHeight(uint val)
{
	height = val;
}

void ResourceMaterial::SetDepth(uint val)
{
	depth = val;
}

void ResourceMaterial::SetBytesPerPixel(uint val)
{
	bytes_per_pixel = val;
}

void ResourceMaterial::SetNumMipMaps(uint val)
{
	num_mip_maps = val;
}

void ResourceMaterial::SetNumLayers(uint val)
{
	num_layers = val;
}

void ResourceMaterial::SetBytes(uint val)
{
	bytes = val;
}

void ResourceMaterial::SetMaterialID(uint val)
{
	mat_id = val;
}

void ResourceMaterial::SetMaterialType(const char * str)
{
	mat_type = str;
}

void ResourceMaterial::SetColorFormat(COLOR_FORMAT fmt)
{
	color_format = fmt;
}

// Get Methods ==================================
uint ResourceMaterial::GetMaterialID() const
{
	return mat_id;
}

const char * ResourceMaterial::GetMaterialType() const
{
	return mat_type.c_str();
}

// Functionality ================================
bool ResourceMaterial::Save()
{
	bool ret = false;

	//Save the meta file
	Serializer meta_file;

	//Save all the standard resource data
	meta_file.InsertInt("id", id);
	meta_file.InsertString("res_type", ResourceTypeToStr(type));
	meta_file.InsertString("original_file", original_file.c_str());
	meta_file.InsertString("own_file", own_file.c_str());
	meta_file.InsertString("mat_type", mat_type.c_str());

	char* buffer = nullptr;
	uint size = meta_file.Save(&buffer);
	if (buffer != nullptr)
	{
		char meta_name[200];
		sprintf(meta_name, "%s.meta", own_file.c_str());
		App->fs->SaveFile(meta_name, buffer, size - 1, LIBRARY_META_FOLDER);

		RELEASE_ARRAY(buffer);
		ret = true;
	}

	return ret;
}

bool ResourceMaterial::Load(Serializer & data)
{
	//Load all the standard resource data
	id = data.GetInt("id");
	original_file = data.GetString("original_file");
	own_file = data.GetString("own_file");

	mat_type = data.GetString("mat_type");

	return true;
}

void ResourceMaterial::LoadInMemory()
{
	App->importer->material_importer.Load(this);
}

void ResourceMaterial::UnloadInMemory()
{
	glDeleteTextures(1, &mat_id);

	width = 0;
	height = 0;
	depth = 0;
	bytes_per_pixel = 0;
	num_mip_maps = 0;
	num_layers = 0;
	bytes = 0;
	mat_id = 0;
	color_format = UNKNOWN_COLOR;
}

void ResourceMaterial::BlitUI() const
{
	ImGui::Text("%s", mat_type.c_str());
	ImGui::Image((void*)mat_id, ImVec2(100, 100));
	ImGui::Text("Size: %ix%i", width, height);
}

const char * ColorFormatToStr(COLOR_FORMAT fmt)
{
	switch (fmt)
	{
	case INDEX_COLOR:		return "index_color";
	case RGB_COLOR:			return "rgb_color";
	case RGBA_COLOR:		return "rgba_color";
	case BGR_COLOR:			return "bgr_color";
	case BGRA_COLOR:		return "bgra_color";
	case LUMINANCE_COLOR:	return "luminance_color";
	}
	return "invalid_color_format";
}

COLOR_FORMAT StrToColorFormat(const char * str)
{
	if (strcmp(str, "index_color") == 0)	return INDEX_COLOR;
	if (strcmp(str, "rgb_color") == 0)		return RGB_COLOR;
	if (strcmp(str, "rgba_color") == 0)		return RGBA_COLOR;
	if (strcmp(str, "bgr_color") == 0)		return BGR_COLOR;
	if (strcmp(str, "bgra_color") == 0)		return BGRA_COLOR;
	if (strcmp(str, "luminance_color") == 0)return LUMINANCE_COLOR;
	return COLOR_FORMAT::UNKNOWN_COLOR;
}
