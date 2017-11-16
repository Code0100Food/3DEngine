#ifndef _RESOURCE_MATERIAL_H_
#define _RESOURCE_MATERIAL_H_

#include "Resource.h"

enum COLOR_FORMAT
{
	UNKNOWN_COLOR = 0,
	INDEX_COLOR,
	RGB_COLOR,
	RGBA_COLOR,
	BGR_COLOR,
	BGRA_COLOR,
	LUMINANCE_COLOR
};

class ResourceMaterial : public Resource
{
public:

	ResourceMaterial();
	~ResourceMaterial();

private:

	uint width = 0;
	uint height = 0;
	uint depth = 0;
	uint bytes_per_pixel = 0;
	uint num_mip_maps = 0;
	uint num_layers = 0;
	uint bytes = 0;
	uint mat_id = 0;

	std::string mat_type = "undef_material";

	COLOR_FORMAT color_format = UNKNOWN_COLOR;

public:

	//Set Methods -----------
	void SetWidth(uint val);
	void SetHeight(uint val);
	void SetDepth(uint val);
	void SetBytesPerPixel(uint val);
	void SetNumMipMaps(uint val);
	void SetNumLayers(uint val);
	void SetBytes(uint val);
	void SetMaterialID(uint val);
	void SetMaterialType(const char* str);
	void SetColorFormat(COLOR_FORMAT fmt);
	
	//Get Methods -----------
	uint		GetMaterialID()const;
	const char* GetMaterialType()const;

	//Functionality ---------
	//Save/Load Methods
	bool Save(Serializer& file_root)const;
	bool Load(Serializer& data);

	//Load/Unload in memory
	void	LoadInMemory();
	void	UnloadInMemory();

	//UI
	void BlitUI()const;
};

const char*		ColorFormatToStr(COLOR_FORMAT fmt);
COLOR_FORMAT	StrToColorFormat(const char* str);

#endif // !_RESOURCE_MATERIAL_H_
