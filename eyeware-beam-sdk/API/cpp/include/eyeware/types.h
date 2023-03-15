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

#ifndef EYEWARE_SDK_TYPES_H
#define EYEWARE_SDK_TYPES_H

#include <cstdint>
#include <limits>

#include "defines.h"

namespace eyeware {

using Timestamp = double;

/*< Person ID numeric type */
using person_profile_id_t = int64_t;

/*< Object ID numeric type */
using object_id_t = uint32_t;

/*< Screen ID numeric type */
using screen_id_t = uint32_t;

/*< Sensor ID numeric type */
using sensor_id_t = uint32_t;

} // namespace eyeware

#endif
