#ifndef __INCLUDE_HPP__
#define __INCLUDE_HPP__

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <iomanip>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include "spdlog/spdlog.h"

class segmentation_fault_exception : public std::exception {
public:
    segmentation_fault_exception();

    explicit segmentation_fault_exception(const std::string &reason);

public:
    const char *what() const noexcept override;

private:
    const std::string &reason;
};

class unknown_instruction_exception : public std::exception {
public:
    unknown_instruction_exception();

    explicit unknown_instruction_exception(const std::string &reason);

public:
    const char *what() const noexcept override;

private:
    const std::string &reason;
};

namespace log = spdlog;
namespace po = boost::program_options;

using log::debug;
using log::info;
using log::warn;
using log::error;
using log::critical;

void error_exit(const char *const message, int code = 255);

#endif