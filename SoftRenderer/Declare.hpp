#pragma once

#include <cassert>
#include <limits>
#include <numeric>
#include <vector>
#include <array>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <algorithm>
#include <memory>
#include <functional>

namespace X
{
	class Context;
	class MainWindow;

	class Scene;
	class Entity;
	class Component;

	class Transformation;
	class Renderable;
	class Camera;
	class Light;

	class InputManager;
	class InputHandler;

	class Renderer;

	class Pipeline;

	class VertexShader;
	class FragmentShader;
	class ComputeShader;
	class SurfaceShader;

	class ResourceLoader;
	class Mesh;
	class Material;
	class Sampler;
	class Texture2D;
	class Buffer;
	class GeometryLayout;

	class PerformanceCounter;
}

#define MEMORY_LEAK_CHECK

#ifdef MEMORY_LEAK_CHECK
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif