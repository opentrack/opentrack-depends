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

#ifndef EYEWARE_SDK_GEOMETRY_H
#define EYEWARE_SDK_GEOMETRY_H

#include <cstdint>
#include <limits>
#include <memory>

#include "defines.h"

namespace eyeware {

/**
 * Matrix of 3x3, implemented as an array of arrays (row-major).
 *
 * \code{.cpp}
 * Matrix3x3 my_matrix; // Assume a Matrix3x3 instance is available
 * int row = 1;
 * int col = 2;
 * float coefficient = my_matrix[row][col];
 * \endcode
 *
 */
using Matrix3x3 = float[3][3];

/**
 * Representation of a 2D vector or 2D point.
 */
struct Vector2D {
    /**
     * x coordinate
     */
    float x = 0.0f;
    /**
     * y coordinate
     */
    float y = 0.0f;
};

/**
 * Representation of a 3D vector or 3D point.
 */
struct Vector3D {
    /**
     * x coordinate.
     */
    float x = 0.0f;
    /**
     * y coordinate.
     */
    float y = 0.0f;
    /**
     * z coordinate.
     */
    float z = 0.0f;
};

/**
 * Representation of a 3D affine transform, composed by a rotation matrix and a translation vector
 * as A = [R | t], where
 *
 * R = [c_00, c_01, c_02
 *      c_10, c_11, c_12
 *      c_20, c_21, c_22],
 *
 * t = [c_03,
 *      c_13,
 *      c_23].
 * */
struct AffineTransform3D {
    /**
     * Rotation matrix component.
     */
    Matrix3x3 rotation;
    /**
     * Translation vector component.
     */
    Vector3D translation;
};

struct Size2D {
    float width;
    float height;
};

struct Rectangle {
    uint32_t top_left_x;
    uint32_t top_left_y;
    uint32_t width;
    uint32_t height;
};

/**
 * Defines a ray which is a subset of a line, given by the following equation:
 *
 * x = o + d(t), for t in [0, +oo[ and ||d|| = 1.
 *
 * Where o denotes the origin, d denotes the direction, t is any given
 * constant between 0 and infinity that denotes the distance from the origin
 * to a point x.
 */
struct Ray3D {
    Vector3D origin;
    Vector3D direction;
};

enum class Orientation {
    UNKNOWN = 0,
    CLOCKWISE_0 = 1,
    CLOCKWISE_90 = 2,
    CLOCKWISE_180 = 3,
    CLOCKWISE_270 = 4
};

} // namespace eyeware

#endif // EYEWARE_SDK_GEOMETRY_H
