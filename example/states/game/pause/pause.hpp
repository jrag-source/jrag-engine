#pragma once

#include <app_state/base_state.hpp>

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>

namespace sf
{
   class RenderWindow;
}

namespace states
{   
   class pause final : public jrag::app_state::base_state
   {
      public:
         explicit pause();

         [[nodiscard]] bool is_active() const;

         constexpr bool is_opaque() const override
         {
            return false;
         };
         
      private:
         void graphics_process(graphics_tooling tooling, float interpolationt) override;
         void on_enter() override;
         jrag::app_state::command eval_manager_command(logic_tooling tooling) override;

         bool m_is_active;

         sf::Font m_font;
         sf::Text m_pause_label;
   };
}