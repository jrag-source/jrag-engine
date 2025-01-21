#include <input/mouse.hpp>

using jrag::input::mouse;

mouse::mouse(sf::Mouse::Button const button)
   : m_button{button}
{}

auto mouse::get_button() const -> sf::Mouse::Button { return m_button; }

auto jrag::input::operator==(mouse const & lhs, mouse const & rhs) -> bool
{
   return lhs.get_button() == rhs.get_button();
}