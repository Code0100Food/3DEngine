#include "MaterialImporter.h"

#include "Devil/include/il.h"
#include "Devil/include/ilu.h"
#include "Devil/include/ilut.h"

#pragma comment (lib, "Engine/Devil/libx86/DevIL.lib")
#pragma comment (lib, "Engine/Devil/libx86/ILU.lib")
#pragma comment (lib, "Engine/Devil/libx86/ILUT.lib")

#include "Globals.h"

#include <fstream>
#include <iostream>

MaterialImporter::MaterialImporter(const char * path, const void * buffer, unsigned int  _size)
{
	bool ret = false;

	if (buffer)
	{
		ILuint ImageName;
		ilGenImages(1, &ImageName);
		ilBindImage(ImageName);

		if (ilLoadL(IL_TYPE_UNKNOWN, (const void*)buffer, _size))
		{
			ilEnable(IL_FILE_OVERWRITE);

			ILuint   size;
			ILubyte *data;
			// To pick a specific DXT compression use 
			ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);
			size = ilSaveL(IL_DDS, NULL, 0); // Get the size of the data buffer
			if (size > 0)
			{
				data = new ILubyte[size]; // allocate data buffer
				if (ilSaveL(IL_DDS, data, size) > 0) // Save with the ilSaveIL function
					

				RELEASE_ARRAY(data);
			}
			ilDeleteImages(1, &ImageName);
		}
	}

	if (ret == false)
	{
		//LOG("Cannot load texture from buffer of size %u", size);
	}

	//return ret;
	
}
