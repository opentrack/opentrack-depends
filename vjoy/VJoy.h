// VJoy.h header file.
#ifdef VJOY_EXPORTS
#define VJOY_API __declspec(dllexport)
#else
#define VJOY_API __declspec(dllimport)
#endif

#define VJOY_AXIS_MIN	-32768
#define VJOY_AXIS_NIL	0
#define VJOY_AXIS_MAX	32767

#define POV_UP			0
#define POV_RIGHT		1
#define POV_DOWN		2
#define POV_LEFT		3
#define POV_NIL			4

#include <pshpack1.h>

typedef struct _JOYSTICK_STATE
{
	UCHAR ReportId;				// Report Id
	SHORT XAxis;				// X Axis
	SHORT YAxis;				// Y Axis
	SHORT ZAxis;				// Z Axis
	SHORT XRotation;			// X Rotation
	SHORT YRotation;			// Y Rotation
	SHORT ZRotation;			// Z Rotation
	SHORT Slider;				// Slider
	SHORT Dial;					// Dial
	USHORT POV;					// POV
	UINT32 Buttons;				// 32 Buttons
} JOYSTICK_STATE, * PJOYSTICK_STATE;

#include <poppack.h>

VJOY_API BOOL __stdcall VJoy_Initialize(PCHAR name, PCHAR serial);
VJOY_API VOID __stdcall VJoy_Shutdown();
VJOY_API BOOL __stdcall VJoy_UpdateJoyState(int id, PJOYSTICK_STATE pJoyState);
