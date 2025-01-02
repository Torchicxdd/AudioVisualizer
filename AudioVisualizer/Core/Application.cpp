#include "Application.h"

Application::Application()
{
}

Application::Application(const Application& copy)
{
}

Application::~Application()
{
}

bool Application::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	return true;
}

void Application::Shutdown()
{
	return;
}

bool Application::Frame()
{
	return true;
}

bool Application::Render()
{
	return true;
}