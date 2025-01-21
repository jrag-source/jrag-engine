#pragma once

#include <input/variant.hpp>
#include <input/commands.hpp>

#include <array>
#include <string>
#include <vector>

namespace jrag::input
{
   using input_collection = std::vector<jrag::input::variant>;
   using input_alternatives = std::vector<input_collection>;
   using binding_collection = std::array<input_alternatives, static_cast<std::size_t>(command::Count)>;
   using binding_set = std::pair<std::string, binding_collection>;
   using binding_set_collection = std::vector<binding_set>;
}