#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <stdexcept>

void Image::Load(const std::string& texture)
{
	images[texture] = imageCount;
	++imageCount;
	pixels.resize(imageCount);
	textureDetails.resize(imageCount);
	imageSize.resize(imageCount);

	std::string filepath = "textures/"+ texture +".jpg";
	int texWidth, texHeight, texChannels;
	pixels[images[texture]] = stbi_load(filepath.c_str(),
		&texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

	textureDetails[images[texture]].x = texWidth;
	textureDetails[images[texture]].y = texHeight;
	textureDetails[images[texture]].z = texChannels;

	imageSize[images[texture]] = textureDetails[images[texture]].x * textureDetails[images[texture]].y * 4;

	if (!pixels[images[texture]]) {
		throw std::runtime_error("failed to load texture image!");
	}
}