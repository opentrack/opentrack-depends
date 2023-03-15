/**
 * Copyright and confidentiality notice
 *
 * This file is part of GazeSense SDK, which is proprietary and confidential
 * information of Eyeware Tech SA.
 *
 * Copyright (C) 2021 Eyeware Tech SA
 *
 * All rights reserved
 */

#ifndef EYEWARE_NETWORK_EXCEPTION_H_
#define EYEWARE_NETWORK_EXCEPTION_H_

#include <exception>
#include <string>

namespace eyeware {

enum class NetworkError { TIMEOUT = 0, UNKNOWN_ERROR = 1 };

struct NetworkException : public std::exception {
  public:
    NetworkException(const std::string &error_msg) : m_error_msg{error_msg} {}
    const char *what() const noexcept override { return m_error_msg.c_str(); }

  private:
    std::string m_error_msg;
};

} // namespace eyeware

#endif