/*
 * MinimalUserProfiles sample:
 * This is an example that demontrates how to retrieve and listen to changes to 
 * the states Current Profile Name and Profiles, and how to change the Current
 * Profile.
 * 
 * (It is written in mostly very C-style C++, to make it similar to the other
 * minimal samples but still make use of std::vector and std::string. Using those
 * makes the implementation more straightforward when handling the Profile state 
 * which is very dynamic in nature: an unknown number of profile name entries of 
 * unknown size.) 
 *
 * Known limitation: this sample only works correctly for up to 10 profiles. It
 * will list all profiles, but the input will only read one digit, so 9 will be
 * the highest selectable index.
 *
 * Copyright 2015 Tobii AB (publ). All rights reserved.
 */

#include <Windows.h>
#include <stdio.h>
#include <conio.h>
#include "stdafx.h"
#include "eyex\EyeX.h"

#pragma comment (lib, "Tobii.EyeX.Client.lib")

static const UINT32 MIN_SIZE = 1; 
static const TX_STRING MIN_ENGINE_VERSION = "1.3.0";

// global variables
static TX_CONTEXTHANDLE g_hContext = TX_EMPTY_HANDLE;
static std::string g_currentProfileName = "";
static std::vector<std::string> g_profiles;

/*
 * Prints info about the current user profile, all available profiles and how 
 * to change to another profile.
 */
void PrintInfo()
{
    if (g_currentProfileName.length() < MIN_SIZE || g_profiles.size() < MIN_SIZE) {
        return;
    }

    printf("\n-------------------------------------------------------------------------\n");
    printf("\n  Current user profile: %s\n", g_currentProfileName.c_str());
    printf("\n  All user profiles:\n");
    int index = 0;
    for (std::string profile : g_profiles) {
        printf("   %i. %s\n", index++, profile.c_str());
    }
    printf("\n-------------------------------------------------------------------------\n");
    printf("\nEnter the number of the profile you want to change to (or 'q' to quit): ");
}

/*
 * Tries to extract an array of strings for the given state bag and state path. 
 * Returns TX_TRUE if successful, TX_FALSE otherwise.
 * If successful, the arrayOfStrings parameter will be populated with the strings.
 * 
 * This can be used to "unpack" a state bag with a TX_STATEPATH_EYETRACKINGPROFILES
 * state's multiple null-terminated string to a more manageable format.
 */
TX_BOOL GetStateValueAsArrayOfStrings(TX_CONSTHANDLE hStateBag, TX_CONSTSTRING valuePath, std::vector<std::string> *arrayOfStrings)
{
    TX_SIZE stringSize = 0;
    TX_BOOL success;
    
    success = (txGetStateValueAsString(hStateBag, valuePath, nullptr, &stringSize)) == TX_RESULT_OK;
    if (!success)
    {
        return TX_FALSE;
    }

    TX_STRING stringValue = new TX_CHAR[stringSize];
    success = (txGetStateValueAsString(hStateBag, valuePath, stringValue, &stringSize) == TX_RESULT_OK);
    if (!success)
    {
        delete[] stringValue;
        return TX_FALSE;
    }
    
    TX_STRING stringPtr = stringValue;
    while (*stringPtr != '\0')
    {
        TX_STRING startPoint = stringPtr;
        while (*stringPtr != '\0')
        {
            stringPtr++;
        }

        std::string str = std::string(startPoint, stringPtr - startPoint);
        arrayOfStrings->push_back(str);
        stringPtr++;
    }

    return TX_TRUE;
}

/*
 * Handles a state-changed notification, or the response from a get-state operation.
 */
void OnStateReceived(TX_HANDLE hStateBag)
{
	TX_BOOL success;
    TX_SIZE stringSize = 0;
    TX_STRING engineVersion;
    TX_STRING profileName;
    std::vector<std::string> profiles;

    success = (txGetStateValueAsString(hStateBag, TX_STATEPATH_ENGINEINFOVERSION, NULL, &stringSize) == TX_RESULT_OK);
    if (success) {
        engineVersion = (TX_STRING)malloc(stringSize*sizeof(char));
        if (txGetStateValueAsString(hStateBag, TX_STATEPATH_ENGINEINFOVERSION, engineVersion, &stringSize) == TX_RESULT_OK) {
            printf("\nEyeX Engine version: %s. This sample requires EyeX Engine %s or higher.\n", engineVersion, MIN_ENGINE_VERSION);
        }
        free(engineVersion);
    }

    success = (txGetStateValueAsString(hStateBag, TX_STATEPATH_EYETRACKINGCURRENTPROFILENAME, NULL, &stringSize) == TX_RESULT_OK);
    if (success) {
        profileName = (TX_STRING)malloc(stringSize*sizeof(char));
        if (txGetStateValueAsString(hStateBag, TX_STATEPATH_EYETRACKINGCURRENTPROFILENAME, profileName, &stringSize) == TX_RESULT_OK) {
            g_currentProfileName = profileName;
        }
        free(profileName);
    }

    success = GetStateValueAsArrayOfStrings(hStateBag, TX_STATEPATH_EYETRACKINGPROFILES, &profiles);
    if (success) {
        g_profiles = profiles;
    }

    PrintInfo();
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
		printf("Now that we're connected: get the user profiles...\n\n");

        // the two states below requires EyeX Engine 1.3.0, so check engine version as well
        txGetStateAsync(g_hContext, TX_STATEPATH_ENGINEINFOVERSION, OnEngineStateChanged, NULL);
		txGetStateAsync(g_hContext, TX_STATEPATH_EYETRACKINGCURRENTPROFILENAME, OnEngineStateChanged, NULL);
        txGetStateAsync(g_hContext, TX_STATEPATH_EYETRACKINGPROFILES, OnEngineStateChanged, NULL);
	}
}

/*
 * Callback function invoked when the async call to set current profile is completed.
 */
void TX_CALLCONVENTION OnSetCurrentProfileCompleted(TX_CONSTHANDLE hAsyncData, TX_USERPARAM userParam)
{
    TX_BOOL success;
    TX_RESULT result = TX_RESULT_UNKNOWN;
    success = (txGetAsyncDataResultCode(hAsyncData, &result) == TX_RESULT_OK && result == TX_RESULT_OK);
    if (success == TX_FALSE) {
        printf("\nSomething went wrong when trying to change the current profile.\n");
    }
}

/*
 * Change profile to the profile with index profileIndex as listed by the application's PrintInfo function.
 */
void ChangeProfileTo(int profileIndex)
{
    TX_CONSTSTRING profileName = (TX_CONSTSTRING)g_profiles[profileIndex].c_str();
    if (profileName == g_currentProfileName) {
        printf("\nYou selected the current profile. Try again: ");
    } else {
        printf("\nChanging user profile to '%s'.\n", profileName);
        txSetCurrentProfile(g_hContext, profileName, OnSetCurrentProfileCompleted, NULL);
    }
}

int _tmain(int argc, _TCHAR* argv[])
{
    printf("Press 'q' to quit the application.\n\n");

    TX_TICKET hConnectionStateChangedTicket = TX_INVALID_TICKET;
    TX_TICKET hCurrentProfileNameChangedTicket = TX_INVALID_TICKET;
    TX_TICKET hEyeTrackingProfilesChangedTicket = TX_INVALID_TICKET;
    TX_BOOL success;
    TX_EYEXAVAILABILITY availability;

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
	// register observers on the connection state, current profile name state, and profiles state.
	success = txInitializeEyeX(TX_EYEXCOMPONENTOVERRIDEFLAG_NONE, NULL, NULL, NULL, NULL) == TX_RESULT_OK;
	success &= txCreateContext(&g_hContext, TX_FALSE) == TX_RESULT_OK;
	success &= txRegisterConnectionStateChangedHandler(g_hContext, &hConnectionStateChangedTicket, OnEngineConnectionStateChanged, NULL) == TX_RESULT_OK;
    success &= txRegisterStateChangedHandler(g_hContext, &hCurrentProfileNameChangedTicket, TX_STATEPATH_EYETRACKINGCURRENTPROFILENAME, OnEngineStateChanged, NULL) == TX_RESULT_OK;
    success &= txRegisterStateChangedHandler(g_hContext, &hEyeTrackingProfilesChangedTicket, TX_STATEPATH_EYETRACKINGPROFILES, OnEngineStateChanged, NULL) == TX_RESULT_OK;
    success &= txEnableConnection(g_hContext) == TX_RESULT_OK;

	if (success) {
		printf("Initialization was successful.\n");
		printf("We are waiting for a connection to the EyeX Engine...\n\n");
	} else {
		printf("Initialization failed.\n\n");
	}

    // read profile index entered by user, if valid: change profile.
    // keep reading input until 'q' is read and application should exit.
    // known limitation: only reads one character, meaning 9 will be the highest 
    // selectable profile index.
    auto keepRunning = true;
    while(keepRunning) {
	    int input = _getch();
        char inputChar = input;
        printf("%c\n", inputChar);
        int profileIndex = input - '0';
        if (input == 'q') {
	        printf("\nExiting.\n");
            keepRunning = false;
        } else if (isdigit(input) && ((unsigned int)profileIndex < g_profiles.size())) {
            ChangeProfileTo(profileIndex);
        } else {
            printf("\nIllegal profile index: '%c'. Try again: ", inputChar);
        }
    }

    // unregister handlers and delete the context.
	txUnregisterConnectionStateChangedHandler(g_hContext, hConnectionStateChangedTicket);
    txUnregisterStateChangedHandler(g_hContext, hCurrentProfileNameChangedTicket);
    txUnregisterStateChangedHandler(g_hContext, hEyeTrackingProfilesChangedTicket);
    success = txShutdownContext(g_hContext, TX_CLEANUPTIMEOUT_DEFAULT, TX_FALSE) == TX_RESULT_OK;
	success &= txReleaseContext(&g_hContext) == TX_RESULT_OK;
	success &= txUninitializeEyeX() == TX_RESULT_OK;
	if (!success) {
		printf("EyeX could not be shut down cleanly. Did you remember to release all handles?\n");
	}

	return 0;
}

