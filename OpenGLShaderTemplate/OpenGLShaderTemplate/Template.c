#include "Template.h"

HWND hMainWindow;
HDC hdc;
HGLRC hrc;
RECT clientRect;
int startTime;
int time;
int currentTime;
float angle;
float speed = 0.18f;

float fov = 60.0f;
float zNear = 0.1f;
float zFar = 1000.0f;

int verticesCount;
Vertex *vertices;
Color *colors;
HANDLE hOpenGL;

PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLDETACHSHADERPROC glDetachShader;
PFNGLDELETESHADERPROC glDeleteShader;
PFNGLDELETEPROGRAMPROC glDeleteProgram;
PFNGLCREATESHADERPROC glCreateShader;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLATTACHSHADERPROC glAttachShader;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLGETPROGRAMIVPROC glGetProgramiv;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLUNIFORM1FPROC glUniform1f;
PFNGLUNIFORM2FPROC glUniform2f;

bool useShaders = true;
GLuint vertexShader;
GLuint fragmentShader;
GLint renderProgram = 0;

GLint timeLocation;
GLint screenSizeLocation;

GLint drawBuffers[1] =
{
	GL_COLOR_ATTACHMENT0
};

Color backColor =
{
	0.22f, 0.22f, 0.22f
};

Color c =
{
	0.5f, 0.5f, 0.5f
};

WNDCLASSEX wndClass =
{
	sizeof(WNDCLASSEX), 0, WindowProc, 0, 0, 0, 0, 0, 0, 0, CLASS_NAME, 0
};

PIXELFORMATDESCRIPTOR pfd =
{
	sizeof(PIXELFORMATDESCRIPTOR),
		1, PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW,
		PFD_TYPE_RGBA, COLOR_DEPTH, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		COLOR_DEPTH, 0, 0, PFD_MAIN_PLANE, 0, PFD_MAIN_PLANE, 0, 0
};

void main()
{
	MSG msg;

	RegisterClassEx(&wndClass);
	hMainWindow = CreateWindowEx(0, CLASS_NAME, 0, WS_CUSTOM, 0, 0, GetSystemMetrics(SM_CXSCREEN) / 2, GetSystemMetrics(SM_CYSCREEN) / 2, 0, 0, 0, 0);
	ShowWindow(hMainWindow, SW_SHOWNORMAL);

	Init();

	while (GetMessage(&msg, 0, 0, 0))
	{
		DispatchMessage(&msg);
	}

	ExitProcess(0);
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_SPACE)
		{
			useShaders = !useShaders;
		} else if (wParam == 'R')
		{
			InitShaders();
		}
		if (wParam != VK_ESCAPE)
		{
			break;
		}
	case WM_DESTROY:
		PostQuitMessage(0);
			
		return 0;
	case WM_PAINT:
		Draw();
			
		return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void Init()
{
	GetClientRect(hMainWindow, &clientRect);
	ShowCursor(false);

	hOpenGL = LoadLibrary("opengl32.dll");

	time = GetTickCount();
	startTime = time;

	hdc = GetDC(hMainWindow);
	SetPixelFormat(hdc, ChoosePixelFormat(hdc, &pfd), &pfd);

	hrc = wglCreateContext(hdc);
	wglMakeCurrent(hdc, hrc);
	glViewport(0, 0, clientRect.right, clientRect.bottom);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, (float)clientRect.right / clientRect.bottom, zNear, zFar);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
	InitExtensions();
	InitShaders();

	GenerateMesh(&cube);
}

void InitExtensions()
{
	glCreateShader = (PFNGLCREATESHADERPROC)GetExtension("glCreateShader");
	glDetachShader = (PFNGLDETACHSHADERPROC)GetExtension("glDetachShader");
	glDeleteShader = (PFNGLDELETESHADERPROC)GetExtension("glDeleteShader");
	glDeleteProgram = (PFNGLDELETEPROGRAMPROC)GetExtension("glDeleteProgram");
	glShaderSource = (PFNGLSHADERSOURCEPROC)GetExtension("glShaderSource");
	glCompileShader = (PFNGLCOMPILESHADERPROC)GetExtension("glCompileShader");
	glGetShaderiv = (PFNGLGETSHADERIVPROC)GetExtension("glGetShaderiv");
	glCreateProgram = (PFNGLCREATEPROGRAMPROC)GetExtension("glCreateProgram");
	glAttachShader = (PFNGLATTACHSHADERPROC)GetExtension("glAttachShader");
	glLinkProgram = (PFNGLLINKPROGRAMPROC)GetExtension("glLinkProgram");
	glGetProgramiv = (PFNGLGETPROGRAMIVPROC)GetExtension("glGetProgramiv");
	glUseProgram = (PFNGLUSEPROGRAMPROC)GetExtension("glUseProgram");
	glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)GetExtension("glGetUniformLocation");
	glUniform1f = (PFNGLUNIFORM1FPROC)GetExtension("glUniform1f");
	glUniform2f = (PFNGLUNIFORM2FPROC)GetExtension("glUniform2f");
}

void *GetExtension(char *functionName)
{
	void *pointer;

	pointer = wglGetProcAddress(functionName);
	if (pointer != (void *)-1 && pointer != (void *)0 && pointer != (void *)1 && pointer != (void *)2 && pointer != (void *)3)
	{
		return pointer;
	}
	else
	{
		pointer = GetProcAddress(hOpenGL, functionName);
		if (pointer)
		{
			return pointer;
		}
	}

	return 0;
}

void InitShaders()
{
	glUseProgram(0);

	if (renderProgram)
	{
		glDetachShader(renderProgram, vertexShader);
		glDetachShader(renderProgram, fragmentShader);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		glDeleteProgram(renderProgram);
	}

	vertexShader = LoadShader(VERTEX_SHADER_FILE_NAME, GL_VERTEX_SHADER);
	if (!vertexShader)
	{
		renderProgram = 0;

		return;
	}

	fragmentShader = LoadShader(FRAGMENT_SHADER_FILE_NAME, GL_FRAGMENT_SHADER);
	if (!fragmentShader)
	{
		renderProgram = 0;

		return;
	}

	renderProgram = CreateProgram(vertexShader, fragmentShader);
	if (renderProgram)
	{
		glUseProgram(renderProgram);

		timeLocation = glGetUniformLocation(renderProgram, "time");
		screenSizeLocation = glGetUniformLocation(renderProgram, "screenSize");
	}
	else
	{
		return;
	}
}

GLint CreateProgram(GLuint vertexShader, GLuint fragmentShader)
{
	GLint program;
	GLint linked;

	program = glCreateProgram();
	if (vertexShader)
	{
		glAttachShader(program, vertexShader);
	}
	if (fragmentShader)
	{
		glAttachShader(program, fragmentShader);
	}
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (linked)
	{
		return program;
	}
	else
	{
		if (vertexShader)
		{
			glDetachShader(program, vertexShader);
			glDeleteShader(vertexShader);
		}
		if (fragmentShader)
		{
			glDetachShader(program, fragmentShader);
			glDeleteShader(fragmentShader);
		}
		glDeleteProgram(program);

		return 0;
	}
}

GLuint LoadShader(char *fileName, GLenum type)
{
	char *buffer;
	GLint shaderLength;
	GLuint shader;
	GLint compiled;

	buffer = ReadFileContent(fileName);
	shaderLength = strlen(buffer);

	shader = glCreateShader(type);
	glShaderSource(shader, 1, &buffer, &shaderLength);
	glCompileShader(shader);

	free(buffer);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (compiled)
	{
		return shader;
	}
	else
	{
		return null;
	}
}

char *ReadFileContent(char *fileName)
{
	HANDLE hFile;
	int length;
	int written;
	char *buffer;

 	hFile = CreateFile(fileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	length = GetFileSize(hFile, 0);
	buffer = (char *)calloc(length + 1, sizeof(char));
	ReadFile(hFile, buffer, length, &written, 0);

	CloseHandle(hFile);

	return buffer;
}

void GenerateMesh(Mesh *mesh)
{
	int i, j;
	int index;
	int vertexIndex;
	Triangle *t;

	index = 0;
	verticesCount = mesh->trianglesCount * 3;
	vertices = (Vertex *)calloc(verticesCount, sizeof(Vertex));
	colors = (Color *)calloc(verticesCount, sizeof(Vertex));

	for (i = 0; i < mesh->trianglesCount; i++)
	{
		t = &mesh->indices[i];
		for (j = 0; j < 3; j++)
		{
			vertexIndex = t->vertices[j];

			vertices[index] = mesh->vertices[vertexIndex];
			colors[index] = c;

			index++;
		}
	}
}

void Draw()
{
	currentTime = GetTickCount();
	if (currentTime - time > 10)
	{
		time = currentTime;
		angle += speed * PI;
	}

	glClearColor(backColor.r, backColor.g, backColor.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, SIZE * 1.8f, SIZE * 3.6f, 0, 0, 0, 0, 1.0f, 0);
	glRotatef(angle, 0.0f, 1.0f, 0.0f);

	if (useShaders && renderProgram)
	{
		glUseProgram(renderProgram);
		glUniform1f(timeLocation, (float)(currentTime - startTime));
		glUniform2f(screenSizeLocation, (float)clientRect.right, (float)clientRect.bottom);
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, vertices);
	glColorPointer(3, GL_FLOAT, 0, colors);
	glDrawArrays(GL_TRIANGLES, 0, verticesCount);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	if (useShaders)
	{
		glUseProgram(0);
	}
	
	SwapBuffers(hdc);
}
