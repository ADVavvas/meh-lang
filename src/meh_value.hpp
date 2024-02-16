#pragma once

#include "meh_expr.hpp"
#include "meh_util.hpp"
#include <variant>

class MehFunction;
using MehValue = std::variant<box<literal_t>, box<MehFunction>>;