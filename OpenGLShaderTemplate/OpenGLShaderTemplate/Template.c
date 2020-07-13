#include "Template.h"

const char *const vertexShader = "void main(void){vec4 a=gl_Vertex;a*=.5;gl_Position=gl_ModelViewProjectionMatrix*a;}";
const char *const fragmentShader = "void main(void){gl_FragColor=vec4(1.,0.,0.,1.);}";

HWND hMainWindow;
HDC hdc;
HGLRC hrc;
RECT rect;
int clock;
float angle;
float speed = 0.18f;

float fov = 60.0f;
float zNear = 0.1f;
float zFar = 1000.0f;

int verticesCount;
Vertex *vertices;
Color *colors;

bool useShaders = true;
GLuint vertexShaderObject, fragmentShaderObject;
GLint program;

Color backColor =
{
	0.0f, 0.0f, 1.0f
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
	hMainWindow = CreateWindowEx(0, CLASS_NAME, 0, WS_CUSTOM, 0, 0, 0, 0, 0, 0, 0, 0);

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
	GetClientRect(hMainWindow, &rect);
	ShowCursor(0);

	clock = GetTickCount();

	hdc = GetDC(hMainWindow);
	SetPixelFormat(hdc, ChoosePixelFormat(hdc, &pfd), &pfd);

	hrc = wglCreateContext(hdc);
	wglMakeCurrent(hdc, hrc);
	glViewport(0, 0, rect.right, rect.bottom);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, (float)rect.right / rect.bottom, zNear, zFar);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	if (useShaders)
	{
		InitShaders();
	}

	GenerateMesh(&cube);
}

void InitShaders()
{
	GLint shaderLength, compiled, linked;

	shaderLength = strlen(vertexShader);

	vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderObject, 1, &vertexShader, &shaderLength);
	glCompileShader(vertexShaderObject);

	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		ExitProcess(42);
	}

	shaderLength = strlen(fragmentShader);

	fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderObject, 1, &fragmentShader, &shaderLength);
	glCompileShader(fragmentShaderObject);

	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		ExitProcess(17);
	}

	program = glCreateProgram();
	glAttachShader(program, vertexShaderObject);
	glAttachShader(program, fragmentShaderObject);
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		ExitProcess(123);
	}
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
	int time;

	time = GetTickCount();
	if (time - clock > 10)
	{
		clock = time;
		angle += speed * PI;
	}

	glClearColor(backColor.r, backColor.g, backColor.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (useShaders)
	{
		glUseProgram(program);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, SIZE * 1.8f, SIZE * 3.6f, 0, 0, 0, 0, 1.0f, 0);
	glRotatef(angle, 0.0f, 1.0f, 0.0f);

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
