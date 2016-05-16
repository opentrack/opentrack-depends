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

	// triggers an activation ("direct click").
	void TriggerActivation();

private:
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

	static bool QueryIsForWindowId(TX_HANDLE hQuery, const TX_CHAR* windowId);

	// mutex protecting the state of the object from race conditions caused by multiple threads.
	// (for example, a call to SetActivatableRegions from the main thread while the HandleQuery 
	// method is iterating through the regions on a worker thread.)
	std::mutex _mutex;
	std::vector<ActivatableRegion> _regions;
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
