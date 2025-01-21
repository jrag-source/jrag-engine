#include "pause.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <app_state/command.hpp>
#include <input/dispatcher.hpp>
#include <path/paths.hpp>

#include <SFML/Graphics/RenderWindow.hpp>

#include <print>
#include <string_view>

using namespace states;
using namespace std::string_view_literals;

namespace
{
   sf::Font make_font()
   {
      sf::Font font {};
      font.loadFromFile(jrag::path::assets_dir() / "fonts/arial.ttf"sv);
      return font;
   }
}

pause::pause()
   : m_is_active { false }
   , m_font { make_font() }
   , m_pause_label { sf::String { "Paused" }, m_font, 16u }
{
   auto const bounds { m_pause_label.getLocalBounds() };
   m_pause_label.setOrigin(bounds.width / 2.0f, (bounds.top + bounds.height) / 2.0f);
}

void pause::graphics_process(graphics_tooling tooling, float const)
{
   auto & window{tooling.get_window()};
   sf::RectangleShape background{static_cast<sf::Vector2f>(window.getSize())};
   background.setFillColor(sf::Color{0, 0, 0, 150});
   window.draw(background);

   m_pause_label.setPosition(sf::Vector2f{window.getSize()} / 2.0f);
   window.draw(m_pause_label);
}

void pause::on_enter()
{
   m_is_active = true;
   std::print("{}\n", __PRETTY_FUNCTION__);
}

jrag::app_state::command pause::eval_manager_command(logic_tooling tooling)
{
   if (tooling.get_input_dispatcher().is_command_activating(jrag::input::command::Jump))
   {
      m_is_active = false;
      return jrag::app_state::command::pop();
   }
   
   return jrag::app_state::command::none();
}

bool pause::is_active() const
{
   return m_is_active;
}
