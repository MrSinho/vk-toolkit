#include "scene.h"

Vertex quad[QUAD_VERTEX_COUNT] = {
	{ .position = { -0.5f, -0.5f, 0.0f } },
	{ .position = {  0.5f, -0.5f, 0.0f } },
	{ .position = {  0.5f,  0.5f, 0.0f } },
	{ .position = { -0.5f,  0.5f, 0.0f } }
};

Vertex triangle[TRIANGLE_VERTEX_COUNT] = {
	{ .position = { -1.0f, 1.0f, 0.0f } },
	{ .position = {  0.0f, 0.0f, 0.0f } },
	{ .position = {  1.0f, 1.0f, 0.0f } }
};

float models[48] = {
	0.2f, 0.0f, 0.0f, -0.4f,
	0.0f, 1.3f, 0.0f, -0.2f,
	0.0f, 0.0f, 1.0f,  0.3f,
	0.0f, 0.0f, 0.0f,  1.0f,

	0.2f, 0.0f, 0.0f,  0.4f,
	0.0f, 1.3f, 0.0f, -0.2f,
	0.0f, 0.0f, 1.0f,  0.2f,
	0.0f, 0.0f, 0.0f,  1.0f,

	0.7f, 0.0f, 0.0f,  0.0f,
	0.0f, 0.5f, 0.0f,  0.3f,
	0.0f, 0.0f, 1.0f,  0.1f,
	0.0f, 0.0f, 0.0f,  1.0f
};

uint32_t indices[QUAD_INDEX_COUNT] = {
	0, 1, 2,
	2, 3, 0
};

float light[8] = {
	0.0f,  2.0f, 0.0f, 1.0f,
	0.0f, 0.45f, 0.9f, 1.0f
};

float projectionView[32] = {
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f,

	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
};
