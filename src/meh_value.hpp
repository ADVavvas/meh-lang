#pragma once

#include "meh_callable.hpp"
#include "meh_expr.hpp"
#include "meh_util.hpp"
#include <variant>

using MehValue = std::variant<box<literal_t>, box<MehFunction>>;