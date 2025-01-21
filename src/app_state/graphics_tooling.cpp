#include <app_state/state.hpp>

using graphics_tooling = jrag::app_state::state::graphics_tooling;

graphics_tooling::graphics_tooling(sf::RenderWindow & window)
   : m_window{window}
{}

auto graphics_tooling::get_window() const -> sf::RenderWindow &
{
   return m_window;
}
