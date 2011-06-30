//**********************************************
//Singleton Texture Manager class
//Written by Ben English
//benjamin.english@oit.edu
//
//For use with OpenGL and the FreeImage library
//**********************************************

#ifndef TextureManager_H
#define TextureManager_H

#include <OpenGL/GL.h>
#include "FreeImage.h"
#include <map>

class TextureManager
{
public:
	static TextureManager* Inst();
	virtual ~TextureManager();

	//load a texture an make it the current texture
	//if texID is already in use, it will be unloaded and replaced with this texture
	bool LoadTexture(const char* filename,	//where to load the file from
		const unsigned int texID,			//arbitrary id you will reference the texture by
											//does not have to be generated with glGenTextures
		GLenum image_format = GL_RGB,		//format the image is in
		GLint internal_format = GL_RGB,		//format to store the image in
		GLint level = 0,					//mipmapping level
		GLint border = 0);					//border size

	//free the memory for a texture
	bool UnloadTexture(const unsigned int texID);

	//set the current texture
	bool BindTexture(const unsigned int texID);

	//free all texture memory
	void UnloadAllTextures();

protected:
	TextureManager();
	TextureManager(const TextureManager& tm);
	TextureManager& operator=(const TextureManager& tm);

	static TextureManager* m_inst;
	std::map<unsigned int, GLuint> m_texID;
};

TextureManager* TextureManager::m_inst(0);

TextureManager* TextureManager::Inst()
{
	if(!m_inst)
		m_inst = new TextureManager();

	return m_inst;
}

TextureManager::TextureManager()
{
	// call this ONLY when linking with FreeImage as a static library
	#ifdef FREEIMAGE_LIB
		FreeImage_Initialise();
	#endif
}

//these should never be called
//TextureManager::TextureManager(const TextureManager& tm){}
//TextureManager& TextureManager::operator=(const TextureManager& tm){}
	
TextureManager::~TextureManager()
{
	// call this ONLY when linking with FreeImage as a static library
	#ifdef FREEIMAGE_LIB
		FreeImage_DeInitialise();
	#endif

	UnloadAllTextures();
	m_inst = 0;
}

bool TextureManager::LoadTexture(const char* filename, const unsigned int texID, GLenum image_format, GLint internal_format, GLint level, GLint border)
{
	//image format
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	//pointer to the image, once loaded
	FIBITMAP *dib(0);
	//pointer to the image data
	BYTE* bits(0);
	//image width and height
	unsigned int width(0), height(0);
	//OpenGL's image ID to map to
	GLuint gl_texID;
	
	//check the file signature and deduce its format
	fif = FreeImage_GetFileType(filename, 0);
	//if still unknown, try to guess the file format from the file extension
	if(fif == FIF_UNKNOWN) 
		fif = FreeImage_GetFIFFromFilename(filename);
	//if still unkown, return failure
	if(fif == FIF_UNKNOWN)
		return false;

	//check that the plugin has reading capabilities and load the file
	if(FreeImage_FIFSupportsReading(fif))
		dib = FreeImage_Load(fif, filename);
	//if the image failed to load, return failure
	if(!dib)
		return false;

	//retrieve the image data
	bits = FreeImage_GetBits(dib);
	//get the image width and height
	width = FreeImage_GetWidth(dib);
	height = FreeImage_GetHeight(dib);
	//if this somehow one of these failed (they shouldn't), return failure
	if((bits == 0) || (width == 0) || (height == 0))
		return false;
	
	//if this texture ID is in use, unload the current texture
	if(m_texID.find(texID) != m_texID.end())
		glDeleteTextures(1, &(m_texID[texID]));

	glGenTextures(1, &gl_texID);
	m_texID[texID] = gl_texID;
	glBindTexture(GL_TEXTURE_2D, gl_texID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
			GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
			GL_NEAREST);*/

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, bits);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE, bits);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//Free FreeImage's copy of the data
	FreeImage_Unload(dib);

	//return success
	return true;
}

bool TextureManager::UnloadTexture(const unsigned int texID)
{
	bool result(true);
	//if this texture ID mapped, unload it's texture, and remove it from the map
	if(m_texID.find(texID) != m_texID.end())
	{
		glDeleteTextures(1, &(m_texID[texID]));
		m_texID.erase(texID);
	}
	//otherwise, unload failed
	else
	{
		result = false;
	}

	return result;
}

bool TextureManager::BindTexture(const unsigned int texID)
{
	bool result(true);
	//if this texture ID mapped, bind it's texture as current
	if(m_texID.find(texID) != m_texID.end())
		glBindTexture(GL_TEXTURE_2D, m_texID[texID]);
	//otherwise, binding failed
	else
		result = false;

	return result;
}

void TextureManager::UnloadAllTextures()
{
	//start at the begginning of the texture map
	std::map<unsigned int, GLuint>::iterator i = m_texID.begin();

	//Unload the textures untill the end of the texture map is found
	while(i != m_texID.end())
		UnloadTexture(i->first);

	//clear the texture map
	m_texID.clear();
}

#endif
