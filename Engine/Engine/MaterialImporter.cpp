#include "MaterialImporter.h"
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


// Constructors =================================
MaterialImporter::MaterialImporter()
{

}

// Functionality ================================
void MaterialImporter::Import(const char* path)
{
	//Texture buffer
	char* buffer = nullptr;
	int	  lenght = App->fs->LoadFile(path, &buffer);

	if (buffer && lenght)
	{
		ILuint image_name;
		ilGenImages(1, &image_name);
		ilBindImage(image_name);

		if (ilLoadL(IL_TYPE_UNKNOWN, (const void*)buffer, lenght))
		{
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
					App->fs->SaveFile(name.c_str(), (char*)data, size, LIBRARY_TEXTURES_FOLDER);
				}

				RELEASE_ARRAY(data);
			}
			ilDeleteImages(1, &image_name);
		}
	}
	else
	{
		LOG("Cannot load texture from path: %s", path);
	}
}
