#pragma once
#include <Windows.h>

namespace GameCore
{
	class IGameApp
	{
	public:
		virtual void Startup() = 0;
		virtual void Cleanup() = 0;

		virtual void Update(float deltaT) = 0;
		virtual void RenderScene() = 0;
	};
}
