#include <windows.h>  
#include <iostream>
using namespace std;

#include "InputEventAndControl.h"
#include "ConsoleControl.h"

int main(void)
{
	LARGE_INTEGER freq;
	LARGE_INTEGER start_t, record_t;
	int exe_time;

	ShowWindow(FindWindow("ConsoleWindowClass", NULL), SW_SHOWMAXIMIZED);															   
	SetConsoleOutputCP(936);																										   

	QueryPerformanceFrequency(&freq); //获得机器的时钟周期
	
	ConsoleControl console_control;
	console_control.hideCursor();

	InputEventAndControl input_event;
	input_event.PrintMessage(ALL);

	QueryPerformanceCounter(&start_t); //获得CPU从开机以来执行的时钟周期数
	
	while (1)
	{
		input_event.KeyTest();
		QueryPerformanceCounter(&record_t);
		exe_time = ( record_t.QuadPart - start_t.QuadPart ) / freq.QuadPart; //单位秒

		if (exe_time >= 180)
		{
			input_event.saveFile("_CacheFile", ".");
			start_t = record_t;
		}
	}

	return 0;
}