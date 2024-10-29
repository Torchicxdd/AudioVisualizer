#ifndef WINDOW_H
#define WINDOW_H

#include <Windows.h>

class Window
{
public:
	Window(const int width, const int height) : rc({ 0, 0, width, height }) {}

	virtual ~Window() = default;

	
private:
	RECT rc;
};


#endif // !WINDOW_H

