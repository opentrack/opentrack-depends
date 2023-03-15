/**
 * Copyright and confidentiality notice
 *
 * This file is part of GazeSense SDK, which is proprietary and confidential
 * information of Eyeware Tech SA.
 *
 * Copyright (C) 2020 Eyeware Tech SA
 *
 * All rights reserved
 */

#ifndef CPP_API_TRACKER_LISTENER_H
#define CPP_API_TRACKER_LISTENER_H

#include "tracking_info.h"

namespace eyeware {

/**
 * Class which the client needs to inherit in their own type to receive the real time tracking data
 */
class TrackerListener {
  public:
    /**
     * Reimplement this function in a child class to receive TrackingEvents after registering it to
     * a tracker
     */
    virtual void on_track_ready(TrackingEvent event) = 0;
};

} // namespace eyeware

#endif
