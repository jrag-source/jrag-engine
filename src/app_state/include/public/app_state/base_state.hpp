#pragma once

#include "state.hpp"

namespace jrag::app_state
{
   class base_state : public state
   {
      public:
         explicit base_state() = default;
         ~base_state() override = default;

         [[nodiscard]] constexpr auto is_opaque() const -> bool override
         {
            return true;
         }

      private:
         auto logic_process(logic_tooling tooling,
            std::chrono::duration<float> delta_time) -> void override;

         auto graphics_process(
            graphics_tooling tooling, float interpolation) -> void override;

         auto on_enter() -> void override;

         auto eval_manager_command(logic_tooling tooling) -> command override;
   };
}