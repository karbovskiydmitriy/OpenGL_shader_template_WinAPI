#pragma once

#ifndef __TEMPLATE_H__
#define __TEMPLATE_H__

#include <Windows.h>
// #include <gl\GL.h>
#include <gl\GLU.h>
#include <gl\glext.h>
#include "Types.h"
#include "Cube.h"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#define CLASS_NAME "OpenGLShaderTemplateClass"
#define WS_CUSTOM (WS_VISIBLE | WS_POPUP)
#define COLOR_DEPTH 24

#ifdef DEBUG
#	define VERTEX_SHADER_FILE_NAME "../Release/vertex.glsl"
#	define FRAGMENT_SHADER_FILE_NAME "../Release/fragment.glsl"
#else
#	define VERTEX_SHADER_FILE_NAME "vertex.glsl"
#	define FRAGMENT_SHADER_FILE_NAME "fragment.glsl"
#endif // DEBUG

#define SIZE 100
#define PI 3.14f
#define null 0

extern Mesh cube;

void main();
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void Init();
void *GetExtension(char *functionName);
void InitExtensions();
void InitShaders();
GLint CreateProgram(GLuint vertexShader, GLuint fragmentShader);
GLuint LoadShader(char *fileName, GLenum type);
char *ReadFileContent(char *fileName);
void GenerateMesh(Mesh *mesh);
void Draw();

#endif // __TEMPLATE_H__