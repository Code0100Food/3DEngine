#include "ModuleTextures.h"

#include "Devil/include/il.h"
#include "Devil/include/ilu.h"
#include "Devil/include/ilut.h"

#pragma comment (lib, "Engine/Devil/libx86/DevIL.lib")
#pragma comment (lib, "Engine/Devil/libx86/ILU.lib")
#pragma comment (lib, "Engine/Devil/libx86/ILUT.lib")

// Constructors =================================
ModuleTextures::ModuleTextures(const char * _name, MODULE_ID _id, bool _config_menu, bool _enabled) :Module(_name, _id, _enabled)
{

}

// Destructors ==================================
ModuleTextures::~ModuleTextures()
{

}

bool ModuleTextures::Awake(const JSON_Object * data_root)
{
	//  ----- Initialise DevIL -----
	ilutRenderer(ILUT_OPENGL);
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);

	return true;
}

// Game Loop ====================================
bool ModuleTextures::Start()
{
	for (int i = 0; i < CHECKERS_HEIGHT; i++) 
	{
		for (int j = 0; j < CHECKERS_WIDTH; j++) 
		{
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			checkImage[i][j][3] = (GLubyte)255;
		}
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &check_image);
	glBindTexture(GL_TEXTURE_2D, check_image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);
	glEnable(GL_TEXTURE_2D);

	//Load lenna image
	lenna_porn = LoadTexture("texturacaja.jpg");

	return true;
}

// Functionality ================================
uint ModuleTextures::LoadTexture(const char * str)
{
	ILuint imageID;				// Create an image ID as a ULuint

	GLuint textureID;			// Create a texture ID as a GLuint

	ILboolean success;			// Create a flag to keep track of success/failure

	ILenum error;				// Create a flag to keep track of the IL error state

	ilGenImages(1, &imageID); 	// Generate the image ID

	ilBindImage(imageID); 		// Bind the image

	success = ilLoadImage(str); // Load the image file

								// If we managed to load the image, then we can start to do things with it...
	if (success)
	{
		// If the image is flipped (i.e. upside-down and mirrored, flip it the right way up!)
		ILinfo ImageInfo;
		iluGetImageInfo(&ImageInfo);
		
		if (ImageInfo.Origin != IL_ORIGIN_UPPER_LEFT)
		{
			iluFlipImage();
		}

		// Convert the image into a suitable format to work with
		// NOTE: If your image contains alpha channel you can replace IL_RGB with IL_RGBA
		//success = ilConvertImage(ilGetInteger(IL_IMAGE_FORMAT), IL_UNSIGNED_BYTE);

		// Quit out if we failed the conversion
		if (!success)
		{
			error = ilGetError();
			LOG("[error] Image conversion failed - IL reports error: ", iluErrorString(error));
			exit(-1);
		}

		glGenTextures(1, &textureID);

		// Bind the texture to a name
		glBindTexture(GL_TEXTURE_2D, textureID);


		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		// Set texture interpolation method to use linear interpolation (no MIPMAPS)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		// Specify the texture specification
		glTexImage2D(GL_TEXTURE_2D, 			// Type of texture
			0,									// Pyramid level (for mip-mapping) - 0 is the top level
			ilGetInteger(IL_IMAGE_FORMAT),		// Internal pixel format to use. Can be a generic type like GL_RGB or GL_RGBA, or a sized type
			ilGetInteger(IL_IMAGE_WIDTH),		// Image width
			ilGetInteger(IL_IMAGE_HEIGHT),		// Image height
			0,									// Border width in pixels (can either be 1 or 0)
			ilGetInteger(IL_IMAGE_FORMAT),		// Format of image pixel data
			GL_UNSIGNED_BYTE,					// Image data type
			ilGetData());						// The actual image data itself


												// Set texture clamping method

	}
	else // If we failed to open the image file in the first place...
	{
		error = ilGetError();
		LOG("[error] Image load failed - IL reports error: ", iluErrorString(error));
		exit(-1);
	}
	
	ilDeleteImages(1, &imageID); // Because we have already copied image data into texture data we can release memory used by image.

	LOG("Texture creation successful.");

	return textureID; // Return the GLuint to the texture so you can use
}
