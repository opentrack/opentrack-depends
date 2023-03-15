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

#ifndef CPP_API_TRACKING_INFO_H_
#define CPP_API_TRACKING_INFO_H_

#include "defines.h"
#include "geometry.h"
#include "types.h"
#include <vector>

namespace eyeware {

/**
 * Realibility measure for obtained tracking results.
 */
enum class TrackingConfidence { UNRELIABLE = 0, LOW = 1, MEDIUM = 2, HIGH = 3 };

/**
 * Represents information of a person gaze intersection with a single screen, for a given time
 * instant. Screen gaze coordinates are expressed in pixels with respect to the top-left corner of
 * the screen.
 * \cond
 * For more information on the screen coordinate system, see @ref EwScreenConfig.
 * \endcond
 */
struct ScreenGazeInfo {
    /**
     * ID of the screen, to differentiate in case of a multiscreen setup.
     */
    uint32_t screen_id = 0;

    /**
     * The horizontal screen coordinate for the gaze intersection.
     */
    uint32_t x = 0;

    /**
     * The vertical screen coordinate for the gaze intersection.
     */
    uint32_t y = 0;

    /**
     * The confidence of the tracking result.
     */
    TrackingConfidence confidence = TrackingConfidence::UNRELIABLE;

    /**
     * Tracking status that tells if the other values are dependable.
     */
    bool is_lost = true;
};

/**
 * Represents information about gaze tracking within single frame corresponding to a particular
 * person.
 *
 * Note: left and right refer to the eyes from the point of view of the camera. This means that
 *       they are inverted compared to the anatomical left and right of the point of view of the
 *       person.
 */
struct GazeInfo {
    /**
     * Represents the left eye gaze ray, expressed in the World Coordinate System.
     */
    Ray3D left_eye_ray;

    /**
     * Represents the right eye gaze ray, expressed in the World Coordinate System.
     */
    Ray3D right_eye_ray;

    /**
     * Confidence of the left eye gaze ray prediction.
     */
    TrackingConfidence confidence_left = TrackingConfidence::UNRELIABLE;

    /**
     * Confidence of the right eye gaze ray prediction.
     */
    TrackingConfidence confidence_right = TrackingConfidence::UNRELIABLE;

    /**
     * Tracking status that tells if the other values are dependable
     */
    bool is_lost = true;
};

/**
 * Represents information about eyes blinks within a single frame corresponding to a particular
 * person.
 *
 * Note: left and right refer to the eyes from the point of view of the camera. This means that
 *       they are inverted compared to the anatomical left and right of the point of view of the
 *       person.
 */
struct BlinkInfo {
    /**
     * Represents the left eye closure.
     */
    bool left_eye_closed;

    /**
     * Represents the right eye closure.
     */
    bool right_eye_closed;

    /**
     * Confidence of the left eye closure prediction.
     */
    TrackingConfidence confidence_left = TrackingConfidence::UNRELIABLE;

    /**
     * Confidence of the right eye closure prediction.
     */
    TrackingConfidence confidence_right = TrackingConfidence::UNRELIABLE;

    /**
     * Whether person's blinks were tracked, meaning the values in this data structure are valid.
     */
    bool is_lost = true;
};

/**
 * Represents information of the head pose, for a given time instant.
 */
struct HeadPoseInfo {
    /**
     * Head pose, defined at the nose tip, with respect to the World Coordinate System (WCS).
     */
    AffineTransform3D transform;
    /**
     * Indicates if tracking of the head is lost, i.e., if false, the user is not being tracked.
     */
    bool is_lost = true;
    /**
     * Indicates the ID of the session of uninterrupted consecutive tracking.
     */
    uint64_t track_session_uid = 0;
};

struct TrackedPersonInfo {
    HeadPoseInfo head_pose;
    GazeInfo gaze;
    ScreenGazeInfo screen_gaze;
    BlinkInfo blink;
};

struct TrackedUser {
    TrackedUser(uint32_t id) : id{id} {}
    TrackedUser operator=(const TrackedUser &other) { return TrackedUser{other.id}; }
    const uint32_t id = 0;
    TrackedPersonInfo tracking_info;
};

struct TrackingEvent {
    std::vector<TrackedUser> people;
    Timestamp timestamp = 0.0;
};

} // namespace eyeware

#endif
