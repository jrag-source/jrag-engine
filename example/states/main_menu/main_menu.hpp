#pragma once

#include <app_state/base_state.hpp>

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>

namespace sf
{
   class RenderWindow;
}

namespace states
{   
   class main_menu final : public jrag::app_state::base_state
   {
      public:
         explicit main_menu();
         ~main_menu() override;

         void graphics_process(graphics_tooling tooling, float const interpolation) override;

         void on_enter() override;
         
         jrag::app_state::command eval_manager_command(logic_tooling tooling) override;

         enum class selection
         {
            play,
            quit,
         };

         selection get_selection() const;

      private:
         selection m_selection;

         sf::Font m_font;
         sf::Text m_title;

         sf::RectangleShape m_play_button_rect;
         sf::Text m_play_button_text;

         sf::RectangleShape m_quit_button_rect;
         sf::Text m_quit_button_text;
   };
}