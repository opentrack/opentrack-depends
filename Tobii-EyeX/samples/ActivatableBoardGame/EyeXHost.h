/*
 * EyeXHost class: Responsible for the gaze interaction within a window.
 * Holds the current set of activatable regions and acts as a simple interactor repository.
 * Sends notifications as Windows messages so that they are received on the main thread and can be handled there.
 *
 * Copyright 2013 Tobii Technology AB. All rights reserved.
 */

#pragma once

#include <windows.h>
#include <vector>
#include <mutex>
#include "eyex/EyeX.h"

class EyeXHost
{
public:
	// Represents an activatable region, that is, one particular kind of interactor.
	struct ActivatableRegion
	{
		int id;
		RECT bounds;

		ActivatableRegion(int paramId, RECT paramBounds) : id(paramId), bounds(paramBounds) { }
	};

	EyeXHost();
	virtual ~EyeXHost();

	// attaches to the window with the given handle.
	// the message parameters are custom windows messages sent to the window when an event has occurred.
	void Init(HWND hWnd, UINT statusChangedMessage, UINT focusedRegionChangedMessage, UINT regionActivatedMessage);

	// updates the collection (repository) of activatable regions.
	void SetActivatableRegions(const std::vector<ActivatableRegion>& regions);

	// gets the ID of the region that currently has the activation focus.
	int GetFocusedRegionId() const;

    // triggers an activation ("direct click").
	void TriggerActivation();

    // trigger a request to the EyeX Engine to switch activation mode on.
    // this will trigger Activation Focus Changed events to be raised for the
    // interactor the user is looking at/no longer looking at.
    // in this sample, the focus is used to draw a highlight on in the square
    // that is about to be "clicked" - where the "X" will be placed.
    void TriggerActivationModeOn();

	// indicates whether the connection to the EyeX Engine is working.
	bool IsFunctional() const;

private:
	enum State
	{
		Initializing,
		Connected,
		Disconnected,
		Failed
	};

	// registers handlers for notifications from the engine.
	bool RegisterConnectionStateChangedHandler();
	bool RegisterQueryHandler();
	bool RegisterEventHandler();

	// event handlers.
	void OnEngineConnectionStateChanged(TX_CONNECTIONSTATE connectionState);
	void HandleQuery(TX_CONSTHANDLE hAsyncData);
	void HandleEvent(TX_CONSTHANDLE hAsyncData);
	void HandleActivatableEvent(TX_HANDLE hEvent, int interactorId);
	void OnActivationFocusChanged(TX_HANDLE hBehavior, int interactorId);
	void OnActivated(TX_HANDLE hBehavior, int interactorId);

	// callback function invoked when a snapshot has been committed.
	static void TX_CALLCONVENTION OnSnapshotCommitted(TX_CONSTHANDLE hAsyncData, TX_USERPARAM param);

	// sets the internal state.
	void SetState(State state);

	// sets the ID of the region that currently has the activation focus.
	void SetFocusedRegionId(int regionId);

	// clears the focused region ID if there is no matching region in the repository.
	void ResetFocusedRegionIdIfNonExistent();

	static bool QueryIsForWindowId(TX_HANDLE hQuery, const TX_CHAR* windowId);

	// mutex protecting the state of the object from race conditions caused by multiple threads.
	// (for example, a call to SetActivatableRegions from the main thread while the HandleQuery 
	// method is iterating through the regions on a worker thread.)
	std::mutex _mutex;
	State _state;
	std::vector<ActivatableRegion> _regions;
	int _focusedRegionId;
	TX_CONTEXTHANDLE _context;
	TX_TICKET _connectionStateChangedTicket;
	TX_TICKET _queryHandlerTicket;
	TX_TICKET _eventHandlerTicket;

	// attached window and custom messages.
	HWND _hWnd;
	UINT _statusChangedMessage;
	UINT _focusedRegionChangedMessage;
	UINT _regionActivatedMessage;

	// private copy constructor and operator making the class non-copyable (declared but not implemented).
	EyeXHost(const EyeXHost&);
	EyeXHost& operator = (const EyeXHost&);
};
