#pragma once

#include <chrono>
#include <cstdint>
#include <vector>

namespace jrag::input
{
   class dispatcher;
}

namespace sf
{
   class RenderWindow;
}

namespace jrag::app_state
{
   class state;

   class state_manager final
   {
      public:
         explicit state_manager(state & root_state);
         ~state_manager();

         state_manager(const state_manager &) = delete;
         auto operator=(const state_manager &) -> state_manager & = delete;

         enum class process_status : std::uint8_t
         {
            inactive,
            active,
         };

         [[nodiscard]] auto logic_process(sf::RenderWindow const & window,
            jrag::input::dispatcher const & input_dispatcher,
            std::chrono::duration<float> delta_time) -> process_status;
         auto graphics_process(
            sf::RenderWindow & window, float interpolation) -> void;

      private:
         std::vector<state *> m_state_stack;
   };
}