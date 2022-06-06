#ifndef TEXTURE_HPP
#define TEXTURE_HPP

struct textinfo
{
	unsigned char * data;
	int width;
	int height;
};
textinfo my_loadBMP(const char* imagepath);

// Load a .BMP file using our custom loader
GLuint loadBMP_custom(const char * imagepath);
unsigned char* loadBMP_custom_(const char* imagepath);

// Load a .TGA file using GLFW's own loader
GLuint loadTGA_glfw(const char * imagepath);

// Load a .DDS file using GLFW's own loader
GLuint loadDDS(const char * imagepath);


#endif