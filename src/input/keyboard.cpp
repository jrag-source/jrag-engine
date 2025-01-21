#include <input/keyboard.hpp>

using jrag::input::keyboard;

keyboard::keyboard(key_type const key)
   : m_key{key}
{}

auto keyboard::get_key() const -> key_type { return m_key; }

auto jrag::input::operator==(keyboard const & lhs, keyboard const & rhs) -> bool
{
   return lhs.get_key() == rhs.get_key();
}