#include "main_menu.hpp"

#include <app_state/command.hpp>
#include <input/dispatcher.hpp>
#include <path/paths.hpp>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Mouse.hpp>

#include <cmath>
#include <print>

using namespace states;
using namespace std::string_view_literals;
namespace
{
   sf::Font make_font()
   {
      // TODO: font should be stored in an asset resource manager
      sf::Font font {};
      font.loadFromFile(jrag::path::assets_dir() / "fonts/arial.ttf");
      return font;
   }

   sf::Text make_label(std::string_view const text, sf::Font const & font, unsigned int const size)
   {
      sf::Text label { std::string { text }, font, size };
      auto const bounds { label.getLocalBounds() };
      label.setOrigin(bounds.width / 2.0f, (bounds.top + bounds.height) / 2.0f); // local bounds top is not always 0 for sf::Text
      return label;
   }

   sf::RectangleShape make_rect(sf::Vector2f const & size, sf::Color const & color)
   {
      sf::RectangleShape rect { size };
      rect.setOrigin(size.x / 2, size.y / 2);
      rect.setFillColor(color);
      return rect;
   }

   namespace title
   {
      constexpr auto font_size { 18u };
      constexpr auto padding { 6.0f };
   }
   
   namespace button
   {
      sf::Vector2 const size { 300.0f, 32.0f };
      sf::Color const color { sf::Color::Red };
      constexpr auto font_size { 12u };
      constexpr auto padding { 6.0f };
   }
}

main_menu::main_menu()
   : m_selection { selection::play }
   , m_font { make_font() }
   , m_title { make_label("Main Menu"sv, m_font, title::font_size) }
   , m_play_button_rect { make_rect(button::size, button::color) }
   , m_play_button_text { make_label("Play"sv, m_font, button::font_size) }
   , m_quit_button_rect { make_rect(button::size, button::color) }
   , m_quit_button_text { make_label("Quit"sv, m_font, button::font_size) }
{}

main_menu::~main_menu() = default;

void main_menu::graphics_process(graphics_tooling tooling, float const)
{
   auto & window{tooling.get_window()};
   
   m_title.setPosition(window.getSize().x / 2.0f, (m_title.getGlobalBounds().top + m_title.getGlobalBounds().height) / 2 + title::padding);
   window.draw(m_title);

   m_play_button_rect.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f);
   window.draw(m_play_button_rect);
   m_play_button_text.setPosition(m_play_button_rect.getPosition());
   window.draw(m_play_button_text);

   m_quit_button_rect.setPosition(m_play_button_rect.getPosition() + sf::Vector2 { 0.0f, button::size.y + button::padding });
   window.draw(m_quit_button_rect);
   m_quit_button_text.setPosition(m_quit_button_rect.getPosition());
   window.draw(m_quit_button_text);
}

void main_menu::on_enter()
{
   std::print("{}\n", __PRETTY_FUNCTION__);
}

jrag::app_state::command main_menu::eval_manager_command(logic_tooling tooling)
{
   auto const & window{tooling.get_window()};
   
   using input_command = jrag::input::command;

   if (m_play_button_rect.getGlobalBounds().contains(sf::Vector2f { sf::Mouse::getPosition(window) }))
   {
      m_play_button_rect.setFillColor(sf::Color::Blue);
      // TODO: Make each state be able to set their own commands
      if (tooling.get_input_dispatcher().is_command_activating(input_command::Shoot))
      {
         m_selection = selection::play;
         return jrag::app_state::command::pop();
      }
   }
   else
   {
      m_play_button_rect.setFillColor(sf::Color::Red);
   }

   if (m_quit_button_rect.getGlobalBounds().contains(sf::Vector2f { sf::Mouse::getPosition(window) }))
   {
      m_quit_button_rect.setFillColor(sf::Color::Blue);
      // TODO: Make each state be able to set their own commands
      if (tooling.get_input_dispatcher().is_command_activating(input_command::Shoot))
      {
         m_selection = selection::quit;
         return jrag::app_state::command::pop();
      }
   }
   else
   {
      m_quit_button_rect.setFillColor(sf::Color::Red);
   }

   if (tooling.get_input_dispatcher().is_command_activating(input_command::Jump))
   {
      std::print("main_menu exit\n");
      return jrag::app_state::command::pop();
   }

   return jrag::app_state::command::none();
}

main_menu::selection main_menu::get_selection() const
{
   return m_selection;
}
