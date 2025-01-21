#include "game.hpp"
#include "pause/pause.hpp"

#include <app_state/command.hpp>
#include <input/dispatcher.hpp>
#include <path/paths.hpp>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Time.hpp>

#include <cmath>
#include <print>

using namespace states;

namespace
{
   sf::Texture make_goon_texture()
   {
      sf::Texture texture;
      texture.loadFromFile(jrag::path::assets_dir() / "goon.png", sf::IntRect { 0, 1, 16 , 14 });
      return texture;
   }

   sf::Sprite make_goon_sprite(const sf::Texture& texture)
   {
      sf::Sprite sprite { texture };
      sprite.setScale(10.0f, 10.0f);
      return sprite;
   }

   sf::Texture make_medkit_texture()
   {
      sf::Texture texture;
      texture.loadFromFile(jrag::path::assets_dir() / "medkit.png");
      return texture;
   }

   sf::Sprite make_medkit_sprite(const sf::Texture& texture)
   {
      sf::Sprite sprite { texture };
      sprite.setScale(10.0f, 10.0f);
      return sprite;
   }
}

game::game()
   : m_goon_texture{make_goon_texture()}
   , m_goon_sprite{make_goon_sprite(m_goon_texture)}
   , m_medkit_texture{make_medkit_texture()}
   , m_medkit_sprite{make_medkit_sprite(m_medkit_texture)}
   , m_medkit_x_dir{1}
   , m_pause_state{}
{}

game::~game() = default;

void game::logic_process(logic_tooling tooling, std::chrono::duration<float> const dt)
{
   if (m_pause_state.is_active())
   {
      return;
   }
   
   sf::Vector2f move_dir { 0.0f, 0.0f };

   using Cmd = jrag::input::command;

   if (tooling.get_input_dispatcher().is_command_active(Cmd::MoveRight))
   {
      move_dir.x += 1.0f;
   }

   if (tooling.get_input_dispatcher().is_command_active(Cmd::MoveUp))
   {
      move_dir.y -= 1.0f;
   }

   if (tooling.get_input_dispatcher().is_command_active(Cmd::MoveLeft))
   {
      move_dir.x -= 1.0f;
   }

   if (tooling.get_input_dispatcher().is_command_active(Cmd::MoveDown))
   {
      move_dir.y += 1.0f;
   }

   const float magnitude { std::sqrt( (move_dir.x * move_dir.x) + (move_dir.y * move_dir.y) ) };
   
   if (magnitude != 0.0f) // TODO: Potentially bad comparison with 0. OK as long as there is an exact representation for 0.0f in the hardware. Fine for now...
   {
      move_dir.x /= magnitude;
      move_dir.y /= magnitude;
   }

   constexpr float GOON_SPEED { 200.0f }; // Pixels per second
   const sf::Vector2f displacement { move_dir * GOON_SPEED * dt.count() };
   m_goon_sprite.setPosition(m_goon_sprite.getPosition() + displacement);

   constexpr auto medkit_speed { 200.0f };

   auto const medkit_bounds { m_medkit_sprite.getGlobalBounds() };
   if (medkit_bounds.left < 0.0f)
   {
      m_medkit_x_dir = 1;
   }
   else if (medkit_bounds.left + medkit_bounds.width > tooling.get_window().getSize().x)
   {
      m_medkit_x_dir = -1;
   }

   sf::Vector2 const medkit_displacement { medkit_speed * m_medkit_x_dir * dt.count(), 0.0f };
   m_medkit_sprite.setPosition(m_medkit_sprite.getPosition() + medkit_displacement);

   if (tooling.get_input_dispatcher().is_command_activating(Cmd::Shoot))
   {
      std::print("Shoot active\n");
   }

   if (tooling.get_input_dispatcher().is_command_active(Cmd::ScrollUp))
   {
      std::print("ScrollUp active\n");
   }

   if (tooling.get_input_dispatcher().is_command_active(Cmd::ScrollDown))
   {
      std::print("ScrollDown active\n");
   }
}

void game::graphics_process(graphics_tooling tooling, float const)
{
   auto & window{tooling.get_window()};
   sf::View playerView{m_goon_sprite.getPosition() + (m_goon_sprite.getGlobalBounds().getSize() / 2.0f), window.getDefaultView().getSize()};
   playerView.zoom(0.5f);
   window.setView(playerView);
   window.draw(m_goon_sprite);
   window.draw(m_medkit_sprite);
}

void game::on_enter()
{
   std::print("{}\n", __PRETTY_FUNCTION__);
}

jrag::app_state::command game::eval_manager_command(logic_tooling tooling)
{
   using input_command = jrag::input::command;

   if (tooling.get_input_dispatcher().is_command_activating(input_command::Jump))
   {
      std::print("game_state exit\n");
      return jrag::app_state::command::pop();
   }
   else if (tooling.get_input_dispatcher().is_command_activating(input_command::ScrollUp))
   {
      std::print("game state push pause\n");
      return jrag::app_state::command::push(m_pause_state);
   }

   return jrag::app_state::command::none();
}
