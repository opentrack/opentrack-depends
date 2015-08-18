// VJoy.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"
#include "VJoy.h"

JOYSTICK_STATE m_joyState[2] = { 0 };

int main(int argc, char* argv[])
{
	VJoy_Initialize("", "");

	m_joyState[0].XAxis = 32767;
	m_joyState[0].YAxis = 32767;
	m_joyState[0].ZAxis = 32767;
	m_joyState[0].Buttons = 0xAAAAAAAA;
	m_joyState[0].POV = (4 << 12) | (4 << 8) | (4 << 4) | 4;

	VJoy_UpdateJoyState(0, &m_joyState[0]);

	VJoy_Shutdown();

	return 0;
}
