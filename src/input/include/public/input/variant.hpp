#pragma once

#include "types.hpp"
#include "keyboard.hpp"
#include "mouse.hpp"
#include "mouse_wheel.hpp"

#include <variant>

namespace jrag::input
{
   class variant
   {
      public:
         template<typename InputClass>
         explicit variant(const InputClass& input)
            : m_type { input.type }
            , m_variant { input }
         {

         }

         [[nodiscard]] auto get_type() const -> type;

         template<typename InputClass>
         [[nodiscard]] auto get() const -> InputClass const &
         {
            return std::get<InputClass>(m_variant);
         }

         friend auto operator==(const input::variant& lhs, const input::variant& rhs) -> bool;

         private:
            using input_variant = std::variant
            <
               keyboard,
               mouse,
               mouse_wheel
            >;

            type m_type;
            input_variant m_variant;
   };

   auto operator==(const input::variant& lhs, const input::variant& rhs) -> bool;
}