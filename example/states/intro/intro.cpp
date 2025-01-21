#include "intro.hpp"
#include "asset/sprite_sheet.hpp"

#include <app_state/command.hpp>
#include <input/dispatcher.hpp>
#include <path/paths.hpp>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Time.hpp>

#include <cmath>
#include <filesystem>
#include <print>
#include <string_view>

using namespace states;
using namespace std::string_view_literals;

namespace
{
   sf::Font make_font()
   {
      sf::Font font {};
      font.loadFromFile(jrag::path::assets_dir() / "fonts/arial.ttf");
      return font;
   }

   sf::Texture make_texture(std::filesystem::path const & asset_path)
   {
      sf::Texture texture;
      texture.loadFromFile(jrag::path::assets_dir() / asset_path);
      return texture;
   }

   sf::Sprite make_sprite(const sf::Texture& texture)
   {
      sf::Sprite sprite { texture };
      sprite.setScale(20.0f, 20.0f);
      return sprite;
   }

   using namespace std::chrono_literals;
   constexpr std::initializer_list<jrag::asset::sprite_sheet::animation::frame_data> animation_data
   {
      { {3u, 2u}, 0.25s },
      { {4u, 2u}, 0.25s },
      { {5u, 2u}, 0.25s },
   };
}

intro::intro()
   : m_font { make_font() }
   , m_title { sf::String { "Intro State" }, m_font, 15u }
   , m_sniper_texture { make_texture(std::filesystem::path { "camper.png"sv }) }
   , m_sniper_sprite { make_sprite(m_sniper_texture) }
   , m_subtext { sf::String { "Press Space To Continue" }, m_font, 10u }
   , m_sprite_sheet{make_texture("smb_mario_sheet.png"sv)}
   , m_animation{jrag::asset::sprite_sheet::animation::make<{8u, 6u}>(m_sprite_sheet, animation_data)}
{
   auto const title_bounds { m_title.getLocalBounds() };
   m_title.setOrigin(title_bounds.width / 2.0f, (title_bounds.top + title_bounds.height)); // local bounds top is not always 0 for sf::Text
   m_title.setPosition(sf::Vector2{0.0f, 0.0f});

   m_sniper_sprite.setOrigin(m_sniper_sprite.getLocalBounds().width / 2.0f, 0.0f);
   m_sniper_sprite.setPosition(m_title.getPosition() + sf::Vector2{0.0f, 10.0f});

   auto const subtext_bounds { m_subtext.getLocalBounds() };
   m_subtext.setOrigin(subtext_bounds.width / 2.0f, subtext_bounds.top);
   m_subtext.setPosition(m_sniper_sprite.getPosition().x, m_sniper_sprite.getGlobalBounds().top + m_sniper_sprite.getGlobalBounds().height);
}

intro::~intro() = default;

void intro::logic_process(logic_tooling tooling, std::chrono::duration<float> dt)
{
   auto const title_pos { m_title.getPosition() };
   if (title_pos.y < tooling.get_window().getSize().y / 6.0f)
   {
      constexpr auto fall_speed { 80.0f }; // pixels per second;
      auto const fall_displacement { fall_speed * dt.count() };
      auto const window_center{tooling.get_window().getSize().x / 2.0f};
      m_title.setPosition(window_center, m_title.getPosition().y + fall_displacement);   
      m_sniper_sprite.setPosition(window_center, m_sniper_sprite.getPosition().y + fall_displacement);
      m_subtext.setPosition(window_center, m_subtext.getPosition().y + fall_displacement);
   }

   m_animation.advance(dt);
}

void intro::graphics_process(graphics_tooling tooling, float const)
{
   auto & window{tooling.get_window()};
   auto const title_pos { m_title.getPosition() };
   if (title_pos.y >= window.getSize().y / 6.0f)
   {
      window.draw(m_subtext);
   }

   window.draw(m_title);
   window.draw(m_sniper_sprite);

   auto mario_sprite{m_animation.get_frame()};
   mario_sprite.setScale({2u, 2u});
   window.draw(mario_sprite);

   auto crouch_sprite{jrag::asset::sprite_sheet::view::get_sprite<{8u, 6u}, {0u, 2u}>(m_sprite_sheet)};
   crouch_sprite.setOrigin(crouch_sprite.getLocalBounds().width, crouch_sprite.getLocalBounds().height);
   crouch_sprite.setPosition(window.getView().getSize());
   crouch_sprite.setScale({2u, 2u});
   window.draw(crouch_sprite);
}

void intro::on_enter()
{
   std::print("{}\n", __PRETTY_FUNCTION__);
}

jrag::app_state::command intro::eval_manager_command(logic_tooling tooling)
{
   using input_command = jrag::input::command;

   if (tooling.get_input_dispatcher().is_command_activating(input_command::Jump))
   {
      std::print("intro exit\n");
      return jrag::app_state::command::pop();
   }

   return jrag::app_state::command::none();
}
