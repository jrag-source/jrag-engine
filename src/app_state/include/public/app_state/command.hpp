#pragma once

#include <cstdint>

namespace jrag::app_state
{
   class state;

   class command final
   {
      public:
         [[nodiscard]] static auto none() -> command;
         [[nodiscard]] static auto push(state & push_state) -> command;
         [[nodiscard]] static auto pop() -> command;

      private:
         enum class type : std::uint8_t
         {
            None,
            Push,
            Pop,
         };

         command(type command_type, state * pushed_state);

         type m_command_type;
         state * m_pushed_state;

         friend class state_manager;
   };
}