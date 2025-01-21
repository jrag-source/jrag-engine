#pragma once

#include "asset/sprite_sheet.hpp"
#include <app_state/base_state.hpp>

#include <asset/sprite_sheet.hpp>

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace sf
{
   class RenderWindow;
}

namespace states
{   
   class intro final : public jrag::app_state::base_state
   {
      public:
         explicit intro();
         ~intro() override;

         void logic_process(logic_tooling tooling, std::chrono::duration<float> dt) override;

         void graphics_process(graphics_tooling tooling, float interpolation) override;

         void on_enter() override;
         
         jrag::app_state::command eval_manager_command(logic_tooling tooling) override;

      private:
         sf::Font m_font;
         sf::Text m_title;

         sf::Texture m_sniper_texture;
         sf::Sprite m_sniper_sprite;
         
         sf::Text m_subtext;

         sf::Texture m_sprite_sheet;
         jrag::asset::sprite_sheet::animation m_animation;
   };
}