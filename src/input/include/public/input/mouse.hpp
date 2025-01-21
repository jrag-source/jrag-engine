#pragma once

#include "types.hpp"

#include <SFML/Window/Mouse.hpp>

namespace jrag::input
{
   class mouse
   {
      public:
         static constexpr auto type { input::type::Mouse };
         using button_type = sf::Mouse::Button;
      
         explicit mouse(button_type button);

         [[nodiscard]] auto get_button() const -> button_type;
      
      private:
         button_type m_button;
   };

   auto operator==(mouse const & lhs, mouse const & rhs) -> bool;
}
