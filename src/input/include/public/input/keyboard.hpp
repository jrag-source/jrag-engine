#pragma once

#include "types.hpp"

#include <SFML/Window/Keyboard.hpp>

namespace jrag::input
{
   class keyboard
   {
      public:
         static constexpr auto type { input::type::Keyboard };
         using key_type = sf::Keyboard::Key;
      
         explicit keyboard(key_type key);

         [[nodiscard]] auto get_key() const -> key_type;
      
      private:
         key_type m_key;
   };

   auto operator==(keyboard const & lhs, keyboard const & rhs) -> bool;
}
