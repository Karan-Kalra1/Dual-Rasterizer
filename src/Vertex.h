#pragma once
#include "Math.h"

namespace dae
{
	
	struct Vertex_PosCol
	{
		dae::Vector3 position;
		dae::Vector2 uv;
		dae::Vector3 normal;
		dae::Vector3 tangent;
		dae::Vector3 color;
	};


	struct Vertex_Out
	{
		Vector4 position{};
		dae::Vector3 color{};
		Vector2 uv{};
		Vector3 normal{};
		Vector3 tangent{};
		Vector3 viewDirection{};
	};

	struct Vertex_Fire
	{
		dae::Vector3 position;
		dae::Vector2 uv;
	};

}