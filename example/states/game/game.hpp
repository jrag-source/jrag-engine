#pragma once

#include "pause/pause.hpp"

#include <app_state/base_state.hpp>

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <collision/collider.hpp>
#include <collision/map.hpp>

namespace sf
{
   class RenderWindow;
}

namespace states
{   
   class game final : public jrag::app_state::base_state
   {
      public:
         explicit game();
         ~game() override;

         void logic_process(logic_tooling tooling, std::chrono::duration<float> dt) override;
         void graphics_process(graphics_tooling tooling, float interpolation) override;

         void on_enter() override;
         
         jrag::app_state::command eval_manager_command(logic_tooling tooling) override;

      private:
         sf::Texture m_goon_texture;
         sf::Sprite m_goon_sprite;

         sf::Texture m_medkit_texture;
         sf::Sprite m_medkit_sprite;
         int m_medkit_x_dir;

         pause m_pause_state;

         std::vector<std::string> m_entities;
         jrag::collision::map m_world{};
   };
}