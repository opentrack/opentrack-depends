/*
 * ActivatableBoardGame sample:
 * This is an example that demonstrates the use of Activatable interactors in the context of a board game.
 *
 * Copyright 2013-2014 Tobii Technology AB. All rights reserved.
 */

#include "stdafx.h"
#include "EyeXHost.h"
#include <objidl.h>
#include <gdiplus.h>
#include <cassert>
#include <cstdint>

#pragma comment (lib, "Tobii.EyeX.Client.lib")

#if INTPTR_MAX == INT64_MAX
#define WINDOW_HANDLE_FORMAT "%lld"
#else
#define WINDOW_HANDLE_FORMAT "%d"
#endif

EyeXHost::EyeXHost()
	: _state(Initializing), 
	_hWnd(nullptr), 
	_statusChangedMessage(0), _focusedRegionChangedMessage(0), _regionActivatedMessage(0), 
	_focusedRegionId(-1),
	_context(TX_EMPTY_HANDLE),
	_connectionStateChangedTicket(0), _queryHandlerTicket(0), _eventHandlerTicket(0)
{
	// initialize the EyeX Engine client library.
	txInitializeEyeX(TX_EYEXCOMPONENTOVERRIDEFLAG_NONE, nullptr, nullptr, nullptr, nullptr);

	// create a context and register event handlers, but don't enable the connection to the engine just yet.
	// we'll enable the connection in the Init method, when we're ready to handle the 
	// connection-status-changed notifications.
	bool success = txCreateContext(&_context, TX_FALSE) == TX_RESULT_OK;
	success &= RegisterConnectionStateChangedHandler();
	success &= RegisterQueryHandler();
	success &= RegisterEventHandler();

	if (!success)
	{
		SetState(Failed);
	}
}

EyeXHost::~EyeXHost()
{
	if (_context != TX_EMPTY_HANDLE)
	{
		// shut down, then release the context.
		txShutdownContext(_context, TX_CLEANUPTIMEOUT_DEFAULT, TX_FALSE);
		txReleaseContext(&_context);
	}
}

void EyeXHost::Init(HWND hWnd, UINT statusChangedMessage, UINT focusedRegionChangedMessage, UINT regionActivatedMessage)
{
	_hWnd = hWnd;
	_statusChangedMessage = statusChangedMessage;
	_focusedRegionChangedMessage = focusedRegionChangedMessage;
	_regionActivatedMessage = regionActivatedMessage;

	// connect to the engine.
	if (txEnableConnection(_context) != TX_RESULT_OK)
	{
		SetState(Failed);
	}
}

void EyeXHost::SetActivatableRegions(const std::vector<ActivatableRegion>& regions)
{
	std::lock_guard<std::mutex> lock(_mutex);

	_regions.assign(regions.begin(), regions.end());
	ResetFocusedRegionIdIfNonExistent();
}

int EyeXHost::GetFocusedRegionId() const
{
	std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(_mutex));

	return _focusedRegionId;
}

void EyeXHost::TriggerActivation()
{
	TX_HANDLE command(TX_EMPTY_HANDLE);
	txCreateActionCommand(_context, &command, TX_ACTIONTYPE_ACTIVATE);
	txExecuteCommandAsync(command, NULL, NULL);
	txReleaseObject(&command);
}

void EyeXHost::TriggerActivationModeOn()
{
    TX_HANDLE command(TX_EMPTY_HANDLE);
    txCreateActionCommand(_context, &command, TX_ACTIONTYPE_ACTIVATIONMODEON);
    txExecuteCommandAsync(command, NULL, NULL);
    txReleaseObject(&command);
}

void EyeXHost::SetFocusedRegionId(int regionId)
{
	std::lock_guard<std::mutex> lock(_mutex);

	_focusedRegionId = regionId;
	ResetFocusedRegionIdIfNonExistent();
	PostMessage(_hWnd, _focusedRegionChangedMessage, 0, 0);
}

// this method assumes that the mutex is held during the call.
void EyeXHost::ResetFocusedRegionIdIfNonExistent()
{
	for (auto region : _regions)
	{
		if (region.id == _focusedRegionId)
		{
			return;
		}
	}

	_focusedRegionId = -1;
}

bool EyeXHost::IsFunctional() const
{
	return _state == Initializing || 
		_state == Connected;
}

void EyeXHost::SetState(State state)
{
	std::lock_guard<std::mutex> lock(_mutex);

	if (_state != state)
	{
		_state = state;

		// note the use of the asynchronous PostMessage function to marshal the event to the main thread.
		// (this method is called from OnEngineConnectionStateChanged, which is typically invoked on a worker thread.)
		PostMessage(_hWnd, _statusChangedMessage, 0, 0);
	}
}

bool EyeXHost::RegisterConnectionStateChangedHandler()
{
	// we pass the "this" pointer as the user parameter when registering the event handler,
	// so that we can access it in the callback function.

	auto connectionStateChangedTrampoline = [](TX_CONNECTIONSTATE connectionState, TX_USERPARAM userParam)
	{
		static_cast<EyeXHost*>(userParam)->OnEngineConnectionStateChanged(connectionState);
	};

	bool success = txRegisterConnectionStateChangedHandler(
		_context, 
		&_connectionStateChangedTicket, 
		connectionStateChangedTrampoline, 
		this) == TX_RESULT_OK;

	return success;
}

bool EyeXHost::RegisterQueryHandler()
{
	auto queryHandlerTrampoline = [](TX_CONSTHANDLE hObject, TX_USERPARAM userParam)
	{
		static_cast<EyeXHost*>(userParam)->HandleQuery(hObject);
	};

	bool success = txRegisterQueryHandler(
		_context, 
		&_queryHandlerTicket, 
		queryHandlerTrampoline, 
		this) == TX_RESULT_OK;

	return success;
}

bool EyeXHost::RegisterEventHandler()
{
	auto eventHandlerTrampoline = [](TX_CONSTHANDLE hObject, TX_USERPARAM userParam)
	{
		static_cast<EyeXHost*>(userParam)->HandleEvent(hObject);
	};

	bool success = txRegisterEventHandler(_context, 
		&_eventHandlerTicket, 
		eventHandlerTrampoline, 
		this) == TX_RESULT_OK;

	return success;
}

void EyeXHost::OnEngineConnectionStateChanged(TX_CONNECTIONSTATE connectionState)
{
	switch (connectionState)
	{
	case TX_CONNECTIONSTATE::TX_CONNECTIONSTATE_CONNECTED:
		SetState(Connected);
		break;

	case TX_CONNECTIONSTATE::TX_CONNECTIONSTATE_DISCONNECTED:
	case TX_CONNECTIONSTATE::TX_CONNECTIONSTATE_TRYINGTOCONNECT:
	case TX_CONNECTIONSTATE::TX_CONNECTIONSTATE_SERVERVERSIONTOOLOW:
	case TX_CONNECTIONSTATE::TX_CONNECTIONSTATE_SERVERVERSIONTOOHIGH:
		SetState(Disconnected);
		break;

	default:
		break;
	}
}

void EyeXHost::HandleQuery(TX_CONSTHANDLE hAsyncData)
{
	std::lock_guard<std::mutex> lock(_mutex);

	// NOTE. This method will fail silently if, for example, the connection is lost before the snapshot has been committed, 
	// or if we run out of memory. This is by design, because there is nothing we can do to recover from these errors anyway.

	TX_HANDLE hQuery(TX_EMPTY_HANDLE);
	txGetAsyncDataContent(hAsyncData, &hQuery);

	const int bufferSize = 20;
	TX_CHAR stringBuffer[bufferSize];

	// read the query bounds from the query, that is, the area on the screen that the query concerns.
	// the query region is always rectangular.
	TX_HANDLE hBounds(TX_EMPTY_HANDLE);
	txGetQueryBounds(hQuery, &hBounds);
	TX_REAL pX, pY, pWidth, pHeight;
	txGetRectangularBoundsData(hBounds, &pX, &pY, &pWidth, &pHeight);
	txReleaseObject(&hBounds);
	Gdiplus::Rect queryBounds((INT)pX, (INT)pY, (INT)pWidth, (INT)pHeight);

	// create a new snapshot with the same window id and bounds as the query.
	TX_HANDLE hSnapshot(TX_EMPTY_HANDLE);
	txCreateSnapshotForQuery(hQuery, &hSnapshot);

	TX_CHAR windowIdString[bufferSize];
	sprintf_s(windowIdString, bufferSize, WINDOW_HANDLE_FORMAT, _hWnd);

	if (QueryIsForWindowId(hQuery, windowIdString))
	{
		// define options for our activatable regions: no, we don't want tentative focus events.
		TX_ACTIVATABLEPARAMS params = { TX_FALSE };

		// iterate through all regions and create interactors for those that overlap with the query bounds.
		for (auto region : _regions)
		{
			Gdiplus::Rect regionBounds((INT)region.bounds.left, (INT)region.bounds.top, 
				(INT)(region.bounds.right - region.bounds.left), (INT)(region.bounds.bottom - region.bounds.top));

			if (queryBounds.IntersectsWith(regionBounds))
			{
				TX_HANDLE hInteractor(TX_EMPTY_HANDLE);
				sprintf_s(stringBuffer, bufferSize, "%d", region.id);

				TX_RECT bounds;
				bounds.X = region.bounds.left; 
				bounds.Y = region.bounds.top;
				bounds.Width = region.bounds.right - region.bounds.left;
				bounds.Height = region.bounds.bottom - region.bounds.top;

				txCreateRectangularInteractor(hSnapshot, &hInteractor, stringBuffer, &bounds, TX_LITERAL_ROOTID, windowIdString);
				txCreateActivatableBehavior(hInteractor, &params);

				txReleaseObject(&hInteractor);
			}
		}
	}

	txCommitSnapshotAsync(hSnapshot, OnSnapshotCommitted, nullptr);
	txReleaseObject(&hSnapshot);
	txReleaseObject(&hQuery);
}

void EyeXHost::HandleEvent(TX_CONSTHANDLE hAsyncData)
{
	TX_HANDLE hEvent(TX_EMPTY_HANDLE);
	txGetAsyncDataContent(hAsyncData, &hEvent);

	// NOTE. Uncomment the following line of code to view the event object. The same function can be used with any interaction object.
	//OutputDebugStringA(txDebugObject(hEvent));

	// read the interactor ID from the event.
	const int bufferSize = 20;
	TX_CHAR stringBuffer[bufferSize];
	TX_SIZE idLength(bufferSize);
	if (txGetEventInteractorId(hEvent, stringBuffer, &idLength) == TX_RESULT_OK)
	{
		int interactorId = atoi(stringBuffer);

		HandleActivatableEvent(hEvent, interactorId);
	}

	txReleaseObject(&hEvent);
}

void EyeXHost::HandleActivatableEvent(TX_HANDLE hEvent, int interactorId)
{
	TX_HANDLE hActivatable(TX_EMPTY_HANDLE);
	if (txGetEventBehavior(hEvent, &hActivatable, TX_BEHAVIORTYPE_ACTIVATABLE) == TX_RESULT_OK)
	{
		TX_ACTIVATABLEEVENTTYPE eventType;
		if (txGetActivatableEventType(hActivatable, &eventType) == TX_RESULT_OK)
		{
			if (eventType == TX_ACTIVATABLEEVENTTYPE_ACTIVATED)
			{
				OnActivated(hActivatable, interactorId);
			}
			else if (eventType == TX_ACTIVATABLEEVENTTYPE_ACTIVATIONFOCUSCHANGED)
			{
				OnActivationFocusChanged(hActivatable, interactorId);
			}
		}

		txReleaseObject(&hActivatable);
	}
}

void EyeXHost::OnActivationFocusChanged(TX_HANDLE hBehavior, int interactorId)
{
	TX_ACTIVATIONFOCUSCHANGEDEVENTPARAMS eventData;
	if (txGetActivationFocusChangedEventParams(hBehavior, &eventData) == TX_RESULT_OK)
	{
		if (eventData.HasActivationFocus)
		{
			SetFocusedRegionId(interactorId);
		}
		else
		{
			SetFocusedRegionId(-1);
		}
	}
}

void EyeXHost::OnActivated(TX_HANDLE hBehavior, int interactorId)
{
	PostMessage(_hWnd, _regionActivatedMessage, interactorId, 0);
}

void TX_CALLCONVENTION EyeXHost::OnSnapshotCommitted(TX_CONSTHANDLE hAsyncData, TX_USERPARAM param) 
{
	// check the result code using an assertion.
	// this will catch validation errors and runtime errors in debug builds. in release builds it won't do anything.

	TX_RESULT result = TX_RESULT_UNKNOWN;
	txGetAsyncDataResultCode(hAsyncData, &result);
	assert(result == TX_RESULT_OK || result == TX_RESULT_CANCELLED);
}

bool EyeXHost::QueryIsForWindowId(TX_HANDLE hQuery, const TX_CHAR* windowId)
{
	const int bufferSize = 20;
	TX_CHAR buffer[bufferSize];

	TX_SIZE count;
	if (TX_RESULT_OK == txGetQueryWindowIdCount(hQuery, &count))
	{
		for (int i = 0; i < count; i++)
		{
			TX_SIZE size = bufferSize;
			if (TX_RESULT_OK == txGetQueryWindowId(hQuery, i, buffer, &size))
			{
				if (0 == strcmp(windowId, buffer))
				{
					return true;
				}
			}
		}
	}

	return false;
}
