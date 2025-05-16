#include "game.hpp"
#include "math/vector2.hpp"
#include "pause/pause.hpp"

#include <app_state/command.hpp>
#include <input/dispatcher.hpp>
#include <path/paths.hpp>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Time.hpp>

#include <cmath>
#include <print>

#include <collision/collider.hpp>
#include <collision/map.hpp>

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

   jrag::collision::collider m_player{.position = {0.0F, 0.0F}, .size = {50.0F, 50.0F}};
   jrag::collision::collider m_medkit{.position = {200.0F, 200.0F}, .size = {50.0F, 50.0F}};
   jrag::collision::map m_world{};
}

game::game()
   : m_goon_texture{make_goon_texture()}
   , m_goon_sprite{make_goon_sprite(m_goon_texture)}
   , m_medkit_texture{make_medkit_texture()}
   , m_medkit_sprite{make_medkit_sprite(m_medkit_texture)}
   , m_medkit_x_dir{1}
   , m_pause_state{}
{
   m_world.add_collider(m_player);
   m_world.add_collider(m_medkit);

   m_goon_sprite.setOrigin(m_goon_sprite.getLocalBounds().getSize().x / 2, m_goon_sprite.getLocalBounds().getSize().y / 2);
   m_goon_sprite.setScale(m_player.size.x / m_goon_sprite.getLocalBounds().width, m_player.size.y / m_goon_sprite.getLocalBounds().height);

   m_medkit_sprite.setOrigin(m_medkit_sprite.getLocalBounds().getSize().x / 2, m_medkit_sprite.getLocalBounds().getSize().y / 2);
   m_medkit_sprite.setScale(m_medkit.size.x / m_medkit_sprite.getLocalBounds().width, m_medkit.size.y / m_medkit_sprite.getLocalBounds().height);
}

game::~game() = default;

void game::logic_process(logic_tooling tooling, std::chrono::duration<float> const dt)
{
   if (m_pause_state.is_active())
   {
      return;
   }
   
   jrag::math::vector2 move_dir {0.0F, 0.0F};

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

   constexpr auto move_speed {200.0F};

   auto const displacement { move_dir * move_speed * dt.count() };
   m_player.position = m_player.position + displacement;
   // std::println("world_pos: {}, {}", m_player.world_pos.x, m_player.world_pos.y);

   // constexpr auto medkit_speed { 200.0f };

   // auto const medkit_x_bounds { get_x_bounds(m_medkit) };

   // if (medkit_x_bounds.first < 0.0f)
   // {
   //    m_medkit.move_dir.x = 1;
   // }
   // else if (medkit_x_bounds.second > 200.0F)
   // {
   //    m_medkit.move_dir.x = -1;
   // }

   std::println("blah");

   m_world.handle_collisions();

   // jrag::math::vector2 const medkit_displacement { m_medkit.speed * m_medkit.move_dir.x * dt.count(), 0.0f };
   // m_medkit.world_pos = m_medkit.world_pos + medkit_displacement;

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

   m_goon_sprite.setPosition(m_player.position.x, m_player.position.y);
   m_medkit_sprite.setPosition(m_medkit.position.x, m_medkit.position.y);

   // sf::View playerView{m_goon_sprite.getPosition() + (m_goon_sprite.getGlobalBounds().getSize() / 2.0f), window.getDefaultView().getSize()};
   // playerView.zoom(0.5f);
   // window.setView(playerView);

   window.draw(m_goon_sprite);

   sf::RectangleShape goon_rect{{m_player.size.x, m_player.size.y}};
   goon_rect.setOutlineColor(sf::Color::Red);
   goon_rect.setOutlineThickness(-5.0F);
   goon_rect.setFillColor(sf::Color::Transparent);
   goon_rect.setOrigin(goon_rect.getLocalBounds().getSize().x / 2, goon_rect.getLocalBounds().getSize().y / 2);
   goon_rect.setPosition({m_player.position.x, m_player.position.y});
   window.draw(goon_rect);

   window.draw(m_medkit_sprite);

   sf::RectangleShape medkit_rect{{m_medkit.size.x, m_medkit.size.y}};
   medkit_rect.setOutlineColor(sf::Color::Red);
   medkit_rect.setOutlineThickness(-5.0F);
   medkit_rect.setFillColor(sf::Color::Transparent);
   medkit_rect.setOrigin(medkit_rect.getLocalBounds().getSize().x / 2, medkit_rect.getLocalBounds().getSize().y / 2);
   medkit_rect.setPosition({m_medkit.position.x, m_medkit.position.y});
   window.draw(medkit_rect);
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
