#include "MaterialImporter.h"
#include "Glew/include/glew.h"
#include "Application.h"
#include "FileSystem.h"

#include "Devil/include/il.h"
#include "Devil/include/ilu.h"
#include "Devil/include/ilut.h"

#pragma comment (lib, "Engine/Devil/libx86/DevIL.lib")
#pragma comment (lib, "Engine/Devil/libx86/ILU.lib")
#pragma comment (lib, "Engine/Devil/libx86/ILUT.lib")

#include "Globals.h"

#include <fstream>
#include <iostream>

#include "ResourcesManager.h"
#include "Serializer.h"

// Constructors =================================
MaterialImporter::MaterialImporter()
{

}

// Functionality ================================
bool MaterialImporter::Load(const char * path, ComponentMaterial * target)
{
	Texture result;

	//Uint where the texture id will be saved
	ILuint textureID = 0;

	//Generate gl texture
	glGenTextures(1, &textureID);

	//Focus the generated texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	//Load the image
	ILboolean success = ilLoadImage(path);

	//If the image is correctly loaded
	if (success)
	{


		ILinfo ImageInfo;

		iluGetImageInfo(&ImageInfo);



		glBindTexture(GL_TEXTURE_2D, textureID);
		/*if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
		{
		iluFlipImage();
		}*/

		success = ilConvertImage(ilGetInteger(IL_IMAGE_FORMAT), IL_UNSIGNED_BYTE);

		// Quit out if we failed the conversion
		if (!success)
		{
			ILenum error = ilGetError();
			LOG("[error] Loading texture from: %s", path);
			LOG("[error] %s", iluErrorString(error));
			ilDeleteImages(1, &textureID);
			return NULL;
		}

		//Set the texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//Generate the image with the loaded data
		glTexImage2D(GL_TEXTURE_2D, 		// Type of texture
			0,								// Pyramid level (for mip-mapping) - 0 is the top level
			ilGetInteger(IL_IMAGE_FORMAT),	// Internal pixel format to use. Can be a generic type like GL_RGB or GL_RGBA, or a sized type
			ilGetInteger(IL_IMAGE_WIDTH),	// Image width
			ilGetInteger(IL_IMAGE_HEIGHT),	// Image height
			0,								// Border width in pixels (can either be 1 or 0)
			ilGetInteger(IL_IMAGE_FORMAT),	// Format of image pixel data
			GL_UNSIGNED_BYTE,				// Image data type
			ilGetData());					// The actual image data itself

		glGenerateMipmap(GL_TEXTURE_2D);

		//Texture sizes
		result.tex_height = ilGetInteger(IL_IMAGE_HEIGHT);
		result.tex_width = ilGetInteger(IL_IMAGE_WIDTH);

		LOG("%i x %i", ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT));
		LOG("Texture: %s correctly loaded!", path);
	}
	else
	{
		ILenum error = ilGetError();
		LOG("[error] Loading texture from: %s", path);
		LOG("[error] %s", iluErrorString(error));
		ilDeleteImages(1, &textureID);
		return NULL;
	}

	//Delete the image 
	ilDeleteImages(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, 0);

	//Texture id and path
	result.id = textureID;
	result.path = path;

	//Add the resulting texture to the component
	//if (textureID != 0)target->AddTexture(result);

	return (textureID != 0);
}

bool MaterialImporter::Load(ResourceMaterial* to_load)
{
	if (to_load->GetMaterialID() != 0)
	{
		LOG("Material already loaded!");
		return true;
	}

	//Uint where the texture id will be saved
	ILuint textureID = 0;

	//Generate gl texture
	glGenTextures(1, &textureID);

	//Focus the generated texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	//Load the image
	char path[250];
	sprintf(path, "%s%s", LIBRARY_TEXTURES_FOLDER, to_load->GetOwnFile());
	ILboolean success = ilLoadImage(path);

	//If the image is correctly loaded
	if (success)
	{
		//Get the information of the original file
		ILinfo info;
		iluGetImageInfo(&info);
		to_load->SetWidth(info.Width);
		to_load->SetHeight(info.Height);
		to_load->SetBytesPerPixel(info.Bpp);
		to_load->SetDepth(info.Depth);
		to_load->SetNumMipMaps(info.NumMips);
		to_load->SetNumLayers(info.NumLayers);
		to_load->SetBytes(info.SizeOfData);

		switch (info.Format)
		{
		case IL_COLOUR_INDEX:	to_load->SetColorFormat(COLOR_FORMAT::INDEX_COLOR);		break;
		case IL_RGB:			to_load->SetColorFormat(COLOR_FORMAT::RGB_COLOR);			break;
		case IL_RGBA:			to_load->SetColorFormat(COLOR_FORMAT::RGBA_COLOR);			break;
		case IL_BGR:			to_load->SetColorFormat(COLOR_FORMAT::BGR_COLOR);			break;
		case IL_BGRA:			to_load->SetColorFormat(COLOR_FORMAT::BGRA_COLOR);			break;
		case IL_LUMINANCE:		to_load->SetColorFormat(COLOR_FORMAT::LUMINANCE_COLOR);	break;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		success = ilConvertImage(ilGetInteger(IL_IMAGE_FORMAT), IL_UNSIGNED_BYTE);

		// Quit out if we failed the conversion
		if (!success)
		{
			ILenum error = ilGetError();
			LOG("[error] Loading texture from: %s", to_load->GetOwnFile());
			LOG("[error] %s", iluErrorString(error));
			ilDeleteImages(1, &textureID);
			return NULL;
		}

		//Set the texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//Generate the image with the loaded data
		glTexImage2D(GL_TEXTURE_2D, 		// Type of texture
			0,								// Pyramid level (for mip-mapping) - 0 is the top level
			ilGetInteger(IL_IMAGE_FORMAT),	// Internal pixel format to use. Can be a generic type like GL_RGB or GL_RGBA, or a sized type
			ilGetInteger(IL_IMAGE_WIDTH),	// Image width
			ilGetInteger(IL_IMAGE_HEIGHT),	// Image height
			0,								// Border width in pixels (can either be 1 or 0)
			ilGetInteger(IL_IMAGE_FORMAT),	// Format of image pixel data
			GL_UNSIGNED_BYTE,				// Image data type
			ilGetData());					// The actual image data itself

		glGenerateMipmap(GL_TEXTURE_2D);

		LOG("MATERIAL Resource: %s correctly loaded!", to_load->GetOwnFile());
	}
	else
	{
		ILenum error = ilGetError();
		LOG("[error] Loading texture from: %s", to_load->GetOwnFile());
		LOG("[error] %s", iluErrorString(error));
		ilDeleteImages(1, &textureID);
		return NULL;
	}

	//Delete the image 
	ilDeleteImages(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, 0);

	//Texture id and path
	to_load->SetMaterialID(textureID);
	
	return (textureID != 0);
}

bool MaterialImporter::Import(const char* path)
{
	//Texture buffer
	char* buffer = nullptr;
	int	  lenght = App->fs->LoadFile(path, &buffer);

	if (buffer && lenght)
	{
		ResourceMaterial* resource = (ResourceMaterial*)App->res_manager->CreateResource(RESOURCE_TYPE::MATERIAL_RESOURCE);

		ILuint image_name;
		ilGenImages(1, &image_name);
		ilBindImage(image_name);
		
		if (ilLoadL(IL_TYPE_UNKNOWN, (const void*)buffer, lenght))
		{
			//Import a dds file of the original file in the correct folder
			ilEnable(IL_FILE_OVERWRITE);

			ILuint   size = 0;
			ILubyte* data = nullptr;

			// To pick a specific DXT compression use 
			ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);
			// Get the size of the data buffer
			size = ilSaveL(IL_DDS, NULL, 0);
			if (size > 0)
			{
				// allocate data buffer
				data = new ILubyte[size]; 
				// Save with the ilSaveIL function
				if (ilSaveL(IL_DDS, data, size) > 0) 
				{
					std::string name;
					App->fs->GetFileNameFromPath(path, &name);
					App->fs->ChangeFileFormat(name.c_str(), "dds", &name);
					resource->SetOwnFile(name.c_str());
					resource->SetOriginalFile(path);
					App->fs->SaveFile(name.c_str(), (char*)data, size, LIBRARY_TEXTURES_FOLDER);
				}

				RELEASE_ARRAY(data);
			}
			ilDeleteImages(1, &image_name);
		}

		char meta_name[200];
		sprintf(meta_name, "%s.meta", resource->GetOwnFile());

		//Generate a meta file to link the generated resource with the file data
		Serializer meta_file;

		bool ret = resource->Save(meta_file);

		if (ret)
		{
			//Save the generated meta file
			char* buffer = nullptr;
			uint size = meta_file.Save(&buffer);
			App->fs->SaveFile(meta_name, buffer, size - 1, LIBRARY_META_FOLDER);

			RELEASE_ARRAY(buffer);
		}		
	}
	else
	{
		LOG("Cannot load texture from path: %s", path);
		return false;
	}

	return true;
}
