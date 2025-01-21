#pragma once

#include "bind_types.hpp"

namespace jrag::input
{
   auto print(binding_set const & set) -> void;
   auto print(binding_set_collection const & set_collection) -> void;
}