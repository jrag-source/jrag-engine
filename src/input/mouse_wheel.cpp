#include <input/mouse_wheel.hpp>

using jrag::input::mouse_wheel;

mouse_wheel::mouse_wheel(scroll_direction const direction)
   : m_direction{direction}
{}

auto mouse_wheel::get_direction() const -> scroll_direction
{
   return m_direction;
}

auto jrag::input::operator==(
   mouse_wheel const & lhs, mouse_wheel const & rhs) -> bool
{
   return lhs.get_direction() == rhs.get_direction();
}
