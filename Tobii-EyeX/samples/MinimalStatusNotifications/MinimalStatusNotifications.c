/*
 * This is an example that demonstrates two ways of getting status information from the EyeX Engine.
 *
 * Copyright 2013-2014 Tobii Technology AB. All rights reserved.
 */

#include <Windows.h>
#include <stdio.h>
#include <conio.h>
#include <assert.h>
#include "eyex/EyeX.h"

#pragma comment (lib, "Tobii.EyeX.Client.lib")

// global variables
static TX_CONTEXTHANDLE g_hContext = TX_EMPTY_HANDLE;

/*
 * Handles a state-changed notification, or the response from a get-state operation.
 */
void OnStateReceived(TX_HANDLE hStateBag)
{
	TX_BOOL success;
	TX_INTEGER eyeTrackingState;
	TX_SIZE2 displaySize;
	TX_SIZE2 screenBounds;
    TX_SIZE stringSize = 0;
    TX_STRING currentProfileName;
	TX_INTEGER presenceData;
	TX_INTEGER gazeTracking;

	success = (txGetStateValueAsInteger(hStateBag, TX_STATEPATH_EYETRACKINGSTATE, &eyeTrackingState) == TX_RESULT_OK);
	if (success) {
		switch (eyeTrackingState) {
		case TX_EYETRACKINGDEVICESTATUS_TRACKING:
			printf("Eye Tracking Device Status: 'TRACKING'.\n"
				"That means that the eye tracker is up and running and trying to track your eyes.\n");
			break;

		default:
			printf("The eye tracking device is not tracking.\n"
				"It could be a that the eye tracker is not connected, or that a screen setup or\n"
				"user calibration is missing. The status code is %d.\n", eyeTrackingState);
		}
	}

	success = (txGetStateValueAsSize2(hStateBag, TX_STATEPATH_EYETRACKINGDISPLAYSIZE, &displaySize) == TX_RESULT_OK);
	if (success) {
		printf("Display Size: %5.2f x %5.2f mm\n", displaySize.Width, displaySize.Height);
	}

	success = (txGetStateValueAsSize2(hStateBag, TX_STATEPATH_EYETRACKINGSCREENBOUNDS, &screenBounds) == TX_RESULT_OK);
	if (success) {
		printf("Screen Bounds: %5.0f x %5.0f pixels\n\n", screenBounds.Width, screenBounds.Height);
	}

	success = (txGetStateValueAsInteger(hStateBag, TX_STATEPATH_USERPRESENCE, &presenceData) == TX_RESULT_OK);
	if (success) {
        if (presenceData != TX_USERPRESENCE_UNKNOWN) {
		    printf("User is %s\n", presenceData == TX_USERPRESENCE_PRESENT ? "present" : "NOT present" );
        }
	}

    // The following state requires EyeX Engine 1.3.0 or later:
    success = (txGetStateValueAsString(hStateBag, TX_STATEPATH_EYETRACKINGCURRENTPROFILENAME, NULL, &stringSize) == TX_RESULT_OK);
    if (success) {
        currentProfileName = (TX_STRING)malloc(stringSize*sizeof(char));
        success = (txGetStateValueAsString(hStateBag, TX_STATEPATH_EYETRACKINGCURRENTPROFILENAME, currentProfileName, &stringSize) == TX_RESULT_OK);
        if (success) {
            printf("Current user profile name is: %s\n", currentProfileName);
        }
        free(currentProfileName);
    }

    // The following state requires EyeX Engine 1.4.0 or later:
	success = (txGetStateValueAsInteger(hStateBag, TX_STATEPATH_GAZETRACKING, &gazeTracking) == TX_RESULT_OK);
	if (success) {
		printf("User's eye-gaze is %s\n", gazeTracking == TX_GAZETRACKING_GAZETRACKED ? "tracked" : "NOT tracked");
	}
}

/*
 * Handles engine state change notifications.
 */
void TX_CALLCONVENTION OnEngineStateChanged(TX_CONSTHANDLE hAsyncData, TX_USERPARAM userParam)
{
	TX_RESULT result = TX_RESULT_UNKNOWN;
	TX_HANDLE hStateBag = TX_EMPTY_HANDLE;

	if (txGetAsyncDataResultCode(hAsyncData, &result) == TX_RESULT_OK && 
		txGetAsyncDataContent(hAsyncData, &hStateBag) == TX_RESULT_OK) {
		OnStateReceived(hStateBag);
		txReleaseObject(&hStateBag);
	}
}

/*
 * Callback function invoked when the status of the connection to the EyeX Engine has changed.
 */
void TX_CALLCONVENTION OnEngineConnectionStateChanged(TX_CONNECTIONSTATE connectionState, TX_USERPARAM userParam)
{
	if (connectionState == TX_CONNECTIONSTATE_CONNECTED) {
		printf("We're now connected to the EyeX Engine!\n");
		printf("Now that we're connected: get the current eye tracking device status, display size and screen bounds...\n\n");

		// NOTE. A request for the TX_STATEPATH_EYETRACKING state path (which expands to 
		// "eyeTracking") will also query for all sub-paths ("eyeTracking.whatever") -- which 
		// explains why this request provides data for the eye tracking device status etc.
		txGetStateAsync(g_hContext, TX_STATEPATH_EYETRACKING, OnEngineStateChanged, NULL);
	}
}

/*
 * Application entry point.
 */
int main(int argc, char* argv[])
{
	TX_TICKET hConnectionStateChangedTicket = TX_INVALID_TICKET;
	TX_TICKET hPresenceStateChangedTicket = TX_INVALID_TICKET;
	TX_TICKET hGazeTrackingStateChangedTicket = TX_INVALID_TICKET;
	TX_BOOL success;
	TX_EYEXAVAILABILITY availability;

	printf(
		"===============================================================================\n\n"
		"This sample illustrates two different ways of getting status information from\n"
		"the EyeX Engine.\n\n"
		"- First we try and connect to the EyeX Engine. When we get a connection we read\n"
		"  and print the current values of the eye tracking device status, the display\n"
		"  size and the screen bounds. This is done once per connection to the EyeX\n"
		"  Engine.\n\n"
		"- When we have a connection to the Engine, we set up a listener for changes of\n"
		"  the user presence state. When the user's eyes are found the user is\n"
		"  considered present, when the eyes cannot be found, the user is considered not\n"
		"  present. Try blocking and unblocking the eye tracker's view of your eyes to\n"
		"  see the changes in the user presence state.\n\n"
		"===============================================================================\n\n");

	if (txGetEyeXAvailability(&availability) == TX_RESULT_OK) {
		if (availability == TX_EYEXAVAILABILITY_NOTAVAILABLE) {
			printf(
				"This sample requires the EyeX Engine, but it isn't available.\n"
				"Please install the EyeX Engine and try again.\n");
			exit(-1);
		}
		else if (availability == TX_EYEXAVAILABILITY_NOTRUNNING) {
			printf(
				"This sample requires the EyeX Engine, but it isn't running.\n"
				"Please make sure that the EyeX Engine is started.\n");
		}
	}

	// initialize and enable the context that is our link to the EyeX Engine.
	// register observers on the connection state, TX_STATEPATH_USERPRESENCE and the TX_STATEPATH_GAZETRACKING engine state.
	success = txInitializeEyeX(TX_EYEXCOMPONENTOVERRIDEFLAG_NONE, NULL, NULL, NULL, NULL) == TX_RESULT_OK;
	success &= txCreateContext(&g_hContext, TX_FALSE) == TX_RESULT_OK;
	success &= txRegisterConnectionStateChangedHandler(g_hContext, &hConnectionStateChangedTicket, OnEngineConnectionStateChanged, NULL) == TX_RESULT_OK;
	success &= txRegisterStateChangedHandler(g_hContext, &hPresenceStateChangedTicket, TX_STATEPATH_USERPRESENCE, OnEngineStateChanged, NULL) == TX_RESULT_OK;
	success &= txRegisterStateChangedHandler(g_hContext, &hGazeTrackingStateChangedTicket, TX_STATEPATH_GAZETRACKING, OnEngineStateChanged, NULL) == TX_RESULT_OK;
	success &= txEnableConnection(g_hContext) == TX_RESULT_OK;

	// let the events flow until a key is pressed.
	if (success) {
		printf("Initialization was successful.\n");
		printf("We are waiting for a connection to the EyeX Engine...\n\n");
	} else {
		printf("Initialization failed.\n\n");
	}

	printf("Press any key to exit...\n\n");
	_getch();
	printf("Exiting.\n");

	// unregister handlers and delete the context.
	txUnregisterConnectionStateChangedHandler(g_hContext, hConnectionStateChangedTicket);
	txUnregisterStateChangedHandler(g_hContext, hPresenceStateChangedTicket);
	txUnregisterStateChangedHandler(g_hContext, hGazeTrackingStateChangedTicket);
	success = txShutdownContext(g_hContext, TX_CLEANUPTIMEOUT_DEFAULT, TX_FALSE) == TX_RESULT_OK;
	success &= txReleaseContext(&g_hContext) == TX_RESULT_OK;
	success &= txUninitializeEyeX() == TX_RESULT_OK;
	if (!success) {
		printf("EyeX could not be shut down cleanly. Did you remember to release all handles?\n");
	}

	return 0;
}
