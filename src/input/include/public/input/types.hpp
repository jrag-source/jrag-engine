#pragma once

#include <cstdint>

namespace jrag::input
{
   enum class type : std::uint8_t
   {
      Keyboard,
      Mouse,
      MouseWheel,
   };
}