#ifndef __TEMPLATE_H__
#define __TEMPLATE_H__

#include <Windows.h>
#include <gl\GL.h>
#include <gl\GLU.h>

#define GL_GLEXT_PROTOTYPES
#include <gl\glext.h>

#include "Types.h"
#include "Cube.h"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glext.lib")

#define CLASS_NAME "OpenGLShaderTemplateClass"
#define WS_CUSTOM (WS_VISIBLE | WS_POPUP | WS_MAXIMIZE)
#define COLOR_DEPTH 24

#define SIZE 100
#define PI 3.14f

extern Mesh cube;

void main();
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void Init();
void InitShaders();
GenerateMesh(Mesh *mesh);
void Draw();

#endif // __TEMPLATE_H__