#include "AudioVisualizer.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int ICmdshow)
{
	AudioVisualizer* System;
	bool result;

	// Create the system object
	auto system = new AudioVisualizer;

	// Initialize and run the system object
	result = system->Initialize();
	if (result)
	{
		system->Run();
	}

	// Shutdown and release the system object
	system->Shutdown();
	delete system;
	system = 0;

	return 0;
}