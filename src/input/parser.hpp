#pragma once

#include <input/commands.hpp>
#include <input/mouse_wheel.hpp>

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

#include <optional>
#include <string_view>

namespace jrag::input::parser
{
   [[nodiscard]] auto key_to_str(sf::Keyboard::Key key) -> std::string_view;
   [[nodiscard]] auto str_to_key(
      std::string_view str) -> std::optional<sf::Keyboard::Key>;

   [[nodiscard]] auto cmd_to_str(jrag::input::command cmd) -> std::string_view;
   [[nodiscard]] auto str_to_cmd(
      std::string_view str) -> std::optional<jrag::input::command>;

   [[nodiscard]] auto mb_to_str(sf::Mouse::Button button) -> std::string_view;
   [[nodiscard]] auto str_to_mb(
      std::string_view str) -> std::optional<sf::Mouse::Button>;

   [[nodiscard]] auto mw_to_str(
      jrag::input::mouse_wheel::scroll_direction dir) -> std::string_view;
   [[nodiscard]] auto str_to_mw(std::string_view str)
      -> std::optional<jrag::input::mouse_wheel::scroll_direction>;
}