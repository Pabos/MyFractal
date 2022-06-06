#ifndef SHADER_HPP
#define SHADER_HPP

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);
GLuint LoadComputes(const char * compute_file_path);
GLuint LoadGeometryShaders(const char * vertex_file_path, const char * fragment_file_path, const char * geometry_file_path);
#endif
