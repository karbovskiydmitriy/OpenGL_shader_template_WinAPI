#pragma once

#ifndef __TYPES_H__
#define __TYPES_H__

typedef unsigned char byte;
typedef byte bool;

#define true 1
#define false 0

typedef struct _Vertex
{
	float x, y, z;
} Vertex;

typedef struct _Color
{
	float r, g, b;
} Color;

typedef struct _Triangle
{
	byte vertices[3];
} Triangle;

#endif // __TYPES_H__