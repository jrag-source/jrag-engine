#pragma once

#include "types.hpp"

#include <SFML/Window/Mouse.hpp>

#include <cstdint>

namespace jrag::input
{
   class mouse_wheel
   {
      public:
         static constexpr auto type { input::type::MouseWheel };

         enum class scroll_direction : std::uint8_t
         {
            Up,
            Down,
         };
      
         explicit mouse_wheel(scroll_direction direction);

         [[nodiscard]] auto get_direction() const -> scroll_direction;
      
      private:
         scroll_direction m_direction;
   };

   auto operator==(mouse_wheel const & lhs, mouse_wheel const & rhs) -> bool;
}
