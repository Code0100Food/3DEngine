#include "ModuleTextures.h"
#include "Glew/include/glew.h"

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
	//  ----- Initialize DevIL -----
	ilutRenderer(ILUT_OPENGL);
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);
	LOG("DevIL lib initialized!");

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
			check_image_array[i][j][0] = (GLubyte)c;
			check_image_array[i][j][1] = (GLubyte)c;
			check_image_array[i][j][2] = (GLubyte)c;
			check_image_array[i][j][3] = (GLubyte)255;
		}
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &check_image);
	glBindTexture(GL_TEXTURE_2D, check_image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, check_image_array);
	glBindTexture(GL_TEXTURE_2D, 0);
	glEnable(GL_TEXTURE_2D);

	//Load lenna image
	custom_check_image = LoadTexture("Lenna.png", "DATA/Textures/");

	play_icon =	LoadTexture("play_icon.png", "DATA/Textures/");
	play_click_icon = LoadTexture("play_click_icon.png", "DATA/Textures/");
	pause_icon = LoadTexture("pause_icon.png", "DATA/Textures/");
	pause_click_icon = LoadTexture("pause_click_icon.png", "DATA/Textures/");
	next_icon = LoadTexture("next_icon.png", "DATA/Textures/");
	next_click_icon = LoadTexture("next_click_icon.png", "DATA/Textures/");

	garbage_icon = LoadTexture("garbage_icon.png", "DATA/Textures/");
	up_arrow_icon = LoadTexture("up_arrow_icon.png", "DATA/Textures/");
	down_arrow_icon = LoadTexture("down_arrow_icon.png", "DATA/Textures/");

	transform_icon = LoadTexture("transform_icon.png", "DATA/Textures/");

	play_icon_id = play_icon;
	pause_icon_id = pause_icon;
	next_icon_id = next_icon;

	return true;
}

void ModuleTextures::BlitConfigInfo()
{
	ImGui::Text("DevIL Version: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "%i", ilGetInteger(IL_VERSION_NUM));
	ImGui::Separator();
	
	ImGui::Text("Mode: ");
	if (ImGui::Checkbox("Check Texture", &check_mode))	 { custom_mode = false; mesh_mode = false; }
	ImGui::SameLine();
	if (ImGui::Checkbox("Custom Texture", &custom_mode)) { check_mode = false; mesh_mode = false; }
	ImGui::SameLine();
	if (ImGui::Checkbox("Mesh Texture", &mesh_mode))	 { check_mode = false; custom_mode = false; }
	ImGui::Separator();

	//Show checker texture
	ImGui::Image((GLuint*)check_image, ImVec2(50, 50));
	ImGui::Text("Size: %ix%i", CHECKERS_WIDTH, CHECKERS_HEIGHT);
	ImGui::Image((GLuint*)custom_check_image, ImVec2(50, 50));
	
	//Show custom texture
	int width = 0;
	int height = 0;
	glBindTexture(GL_TEXTURE_2D, (GLuint)custom_check_image);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
	ImGui::Text("Size: %ix%i", width, height);
	glBindTexture(GL_TEXTURE_2D, 0);
}

bool ModuleTextures::CleanUp()
{
	if (check_image != NULL)glDeleteBuffers(1, (GLuint*)&check_image);
	if (custom_check_image != NULL)glDeleteBuffers(1, (GLuint*)&custom_check_image);

	return true;
}

// Set Methods ==================================
void ModuleTextures::SetCustomMode()
{
	custom_mode = true;
	check_mode = mesh_mode = false;
}

// Get Methods ==================================
bool ModuleTextures::GetCheckMode() const
{
	return check_mode;
}

bool ModuleTextures::GetCustomMode() const
{
	return custom_mode;
}

bool ModuleTextures::GetMeshMode() const
{
	return mesh_mode;
}

// Functionality ================================
uint ModuleTextures::LoadTexture(const char * str, const char* folder)
{
	std::string filename = std::string(str);

	if (folder != nullptr)
	{
		filename = folder + filename;
	}

	//Uint where the texture id will be saved
	ILuint textureID = 0;
	
	//Generate gl texture
	glGenTextures(1, &textureID);
	
	//Focus the generated texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	//Load the image
	ILboolean success = ilLoadImage(filename.c_str());
	
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
			LOG("[error] Loading texture from: %s", filename.c_str());
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
	
		LOG("%i x %i", ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT));
		LOG("Texture: %s correctly loaded!", filename.c_str());
	}
	else
	{
		ILenum error = ilGetError();
		LOG("[error] Loading texture from: %s", filename.c_str());
		LOG("[error] %s", iluErrorString(error));
		ilDeleteImages(1, &textureID);
		return NULL;
	}
	
	//Delete the image 
	ilDeleteImages(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;
}

bool ModuleTextures::LoadCustomTexture(const char * str)
{

	if (custom_check_image != 0)
	{
		glDeleteTextures(1, &custom_check_image);
		custom_check_image = 0;
	}

	custom_check_image = LoadTexture(str, nullptr);
	return custom_check_image;
}
