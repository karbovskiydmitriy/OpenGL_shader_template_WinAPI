#pragma once

#ifndef __CUBE_H__
#define __CUBE_H__

Vertex cubeVertices[] =
{
	{ -100.0f, -100.0f, 100.0f },
	{ -100.0f, 100.0f, 100.0f },
	{ 100.0f, -100.0f, 100.0f },
	{ 100.0f, 100.0f, 100.0f },
	{ 100.0f, -100.0f, -100.0f },
	{ 100.0f, 100.0f, -100.0f },
	{ -100.0f, -100.0f, -100.0f },
	{ -100.0f, 100.0f, -100.0f }
};

Triangle cubeIndices[] =
{
	{ 2, 3, 1 },
	{ 2, 1, 0 },
	{ 4, 5, 3 },
	{ 4, 3, 2 },
	{ 6, 7, 5 },
	{ 6, 5, 4 },
	{ 0, 1, 7 },
	{ 0, 7, 6 },
	{ 3, 5, 7 },
	{ 3, 7, 1 },
	{ 4, 2, 0 },
	{ 4, 0, 6 }
};

Mesh cube =
{
	12, cubeVertices, (Color *)0, cubeIndices
};

#endif // __CUBE_H__
