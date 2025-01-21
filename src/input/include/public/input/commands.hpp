#pragma once

#include <cstdint>

namespace jrag::input
{
   enum class command : std::uint8_t
   {
      MoveRight = 0,
      MoveUp,
      MoveLeft,
      MoveDown,
      Shoot,
      Jump,
      ScrollUp,
      ScrollDown,

      Count,
   };
}