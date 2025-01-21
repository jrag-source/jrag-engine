#pragma once

#include "bind_types.hpp"

namespace jrag::input::loader
{
   [[nodiscard]] auto load() -> binding_set_collection;
}