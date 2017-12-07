#include "ResourceScript.h"

#include "imgui/imgui.h"

ResourceScript::ResourceScript():Resource(SCRIPT_RESOURCE)
{

}

ResourceScript::~ResourceScript()
{

}

void ResourceScript::SetBuffer(const char * bf)
{
	if (buffer != nullptr)RELEASE(buffer);
	buffer = (char*)bf;
}

char * ResourceScript::GetBuffer() const
{
	return buffer;
}

void ResourceScript::BlitUI() const
{

}

void ResourceScript::BlitComplexUI()
{
	ImGui::TextWrapped(buffer);
}
