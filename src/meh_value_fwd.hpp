#include "meh_token.hpp"
#include "meh_util.hpp"
#include <variant>

class MehNativeFunction;
class MehFunction;
class MehClass;
class MehInstance;

using MehValue =
    std::variant<box<literal_t>, box<MehNativeFunction>, box<MehFunction>,
                 box<MehClass>, box<MehInstance>>;