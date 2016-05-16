/*
 * Observer interface: Standard Observer interface used for notifying an object of a change in another object.
 *
 * Copyright 2013 Tobii Technology AB. All rights reserved.
 */

#pragma once

class Observer
{
public:
	virtual void SubjectChanged() = 0;
};
