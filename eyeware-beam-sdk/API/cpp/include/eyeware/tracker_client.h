/**
 * Copyright and confidentiality notice
 *
 * This file is part of Eyeware SDK, which is proprietary and confidential information of
 * Eyeware Tech SA.
 *
 * Copyright (C) 2021 Eyeware Tech SA
 *
 * All rights reserved
 */

#ifndef EYEWARE_TRACKER_CLIENT_H_
#define EYEWARE_TRACKER_CLIENT_H_

#include <functional>
#include <memory>

#include "eyeware/defines.h"
#include "eyeware/network_exception.h"
#include "eyeware/tracking_info.h"

namespace eyeware {

/**
 * The default base port used in the Eyeware Beam application to broadcast tracking data.
 */
constexpr int DEFAULT_BASE_COMMUNICATION_PORT = 12010;

/**
 * The default timeouts used to detect network errors.
 */
constexpr int DEFAULT_NETWORK_TIMEOUTS_IN_MS = 2000;

/**
 * Class for connecting to the tracker server and retrieving its resulting tracking data.
 *
 * It establishes communication to the tracker server (Eyeware Beam application) and
 * provides synchronous access to the data. Those synchronous calls will return the last
 * tracking data results received from the server.
 *
 * A `network_error_handler` callback can be provided. If `nullptr` is used (default),
 * the network errors are ignored and the network connection is reestablished automatically
 * when possible by the @ref TrackerClient class instance. If a network handler is given,
 * it will be called in case of errors (e.g., timeout). In such case, the @ref TrackerClient
 * instance becomes invalid and needs to be recreated to reestablish connection.
 */
class EW_API TrackerClient {
  public:
    /**
     * @param network_error_handler             An optional callback function for managing
     *                                          connection to the server errors.
     * @param network_connection_timeout_ms     The time period (in ms) for an attempt to
     *                                          connect to the server, after which the
     *                                          network connection is treated as broken.
     * @param tracking_info_network_timeout_ms  The time period (in ms) for an attempt to
     *                                          obtain tracking info from the server, after which
     *                                          the network connection is treated as broken.
     * @param base_communication_port           Base connection port to the server. The instance
     *                                          may use base_communication_port+1 as well.
     * @param hostname                          The hostname of the server to obtain tracking
     *                                          results from. Typically the same PC, thus
     *                                          "127.0.0.1".
     *
     */
    TrackerClient(std::function<void(const NetworkError &)> network_error_handler = nullptr,
                  int network_connection_timeout_ms = DEFAULT_NETWORK_TIMEOUTS_IN_MS,
                  int tracking_info_network_timeout_ms = DEFAULT_NETWORK_TIMEOUTS_IN_MS,
                  int base_communication_port = DEFAULT_BASE_COMMUNICATION_PORT,
                  const char *hostname = "127.0.0.1");

    ~TrackerClient();

    /**
     * Retrieves the most recent screen gaze tracking result.
     */
    ScreenGazeInfo get_screen_gaze_info() const;

    /**
     * Retrieves the most recent head pose tracking result.
     */
    HeadPoseInfo get_head_pose_info() const;

    /**
     * Whether this client is currently connected to the tracker server or not.
     *
     * \since 1.1.0
     */
    bool connected() const;

  private:
    class Impl;
    std::unique_ptr<Impl> m_pimpl;
};

} // namespace eyeware

#ifdef __cplusplus
extern "C" {
#endif
EW_API eyeware::TrackerClient *create_tracker_instance(const char *hostname,
                                                       int communication_port);
EW_API void release_tracker_instance(eyeware::TrackerClient *p_instance);
EW_API eyeware::ScreenGazeInfo get_screen_gaze_info(eyeware::TrackerClient *p_instance);
EW_API eyeware::HeadPoseInfo get_head_pose_info(eyeware::TrackerClient *p_instance);

#include <stdbool.h>
EW_API bool connected(eyeware::TrackerClient *p_instance);
}

#endif // EYEWARE_TRACKER_CLIENT_H_